#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $geondir = "$progdir/../geonames";
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;
use File::stat;
use Time::gmtime;
use Date::Parse;
use DBI;

require "$progdir/../makedb.pm";

binmode(STDERR,':utf8');

sub doprefixdata {
	my $dbh = shift;
	$dbh->begin_work;
	my @files = `find results -name '*.A.xml.xz' | sort`;
	my $inrecord = 0;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzcat $fn |") or die "can't process $fn";
		my $fdate = stat($fn)->mtime;
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.A\.xml\.xz//;
		my %data = (sect=>$sect,fdate=>$fdate);
		while (<INFILE>) { chomp; s/\r//g;
			if (/^<\/prefixdata>/) {
				foreach my $k (qw/udate effdate discdate/) {
					$data{$k} = str2time($data{$k}) if $data{$k};
				}
				delete $data{switch} if $data{switch} eq '99999999999' or $data{switch} eq 'XXXXXXXXXXX';
				$data{ex}      = delete $data{rc}      if $data{rc};
				$data{exshort} = delete $data{rcshort} if $data{rcshort};
				$data{seeex}   = delete $data{seerc}   if $data{seerc};
				$data{exlata}  = $data{lata} unless $data{exlata} or not $data{lata};
				makedb::updatedata(\%data,$fdate,$sect);
			} elsif (/^<prefixdata>/) {
				%data = (sect=>$sect,fdate=>$fdate);
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				my ($fld,$val) = ($1,$2);
				$fld =~ s/-//g;
				$fld = "\L$fld\E";
				$data{$fld} = $val if length($val);
			}
		}
		close(INFILE);
	}
	$dbh->commit;
}

sub dorcdata {
	my $dbh = shift;
	$dbh->begin_work;
	my @files = `find ratecenters -name '*.xml.xz' | sort`;
	my $inrecord = 0;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzcat $fn |") or die "can't process $fn";
		my $fdate = stat($fn)->mtime;
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.xml\.xz//; $sect = "RC-$sect";
		my %data = (sect=>$sect,fdate=>$fdate);
		while (<INFILE>) { chomp;
			if (/^<\/rcdata>/) {
				foreach my $k (qw/udate effdate discdate/) {
					$data{$k} = str2time($data{$k}) if $data{$k};
				}
				$data{ex}      = delete $data{rc}      if $data{rc};
				$data{exshort} = delete $data{rcshort} if $data{rcshort};
				$data{seeex}   = delete $data{seerc}   if $data{seerc};
				$data{exlata}  = $data{lata} unless $data{exlata} or not $data{lata};
				makedb::updatedata(\%data,$fdate,$sect);
			} elsif (/^<rcdata>/) {
				%data = (sect=>$sect,fdate=>$fdate);
				$inrecord = 1;
			} elsif ($inrecord and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				my ($fld,$val) = ($1,$2);
				$fld =~ s/-//g;
				$fld = "\L$fld\E";
				$data{$fld} = $val if length($val);
			}
		}
		close(INFILE);
	}
	$dbh->commit;
}

sub doswitchdata {
	my $dbh = shift;
	$dbh->begin_work;
	my @files = `find switches -name 'data.html.xz' | sort`;
	foreach my $fn (@files) { chomp $fn;
		next unless $fn =~ /switches\/([^\/]+)\/([^\/]+)\/([^\/]+)\/([^\/]+)\/data.html.xz/;
		next unless $4;
		my $sw = "$2$1$3$4";
		$sw =~ s/-/ /g;
		my $inrecord = 0;
		my $f2 = $fn; $f2 =~ s/ /\\ /g;
		print STDERR "I: processing $f2\n";
		open(INFILE,"xzcat $f2 |") or die "can't process $f2";
		my $fdate = stat($fn)->mtime;
		my $sect = $sw;
		my %data = (sect=>$sect,fdate=>$fdate);
		while (<INFILE>) { chomp;
			if (/<p>Last updated: <strong>([^<]+)<\/strong><\/p>/) {
				$data{udate} = $1;
				$inrecord = 1;
			} elsif (/<p>Switch: <span class="srch">([^<]+)<\/span><\/p>/) {
				$data{switch} = $1;
				$data{switch} =~ s/-/ /g;
				$inrecord = 1;
			} elsif ($inrecord == 1 and /<ul>/) {
				$inrecord = 2;
			} elsif ($inrecord == 2 and /<li>([^<:]+):\s+(.*?)<\/li>/) {
				my ($k,$v) = ($1,$2);
				$v =~ s/<[^>]+>//g;
				$k =~ s/\s//g;
				$k =~ s/-//g;
				$k = "\L$k\E";
				if (length($k) and length($v)) {
					$v =~ s/-/ /g if $k =~ /^(host|remotes)$/;
					if ($k eq 'remotes') {
						$data{$k}{$v}++;
					} else {
						$data{$k} = $v;
					}
				}
			} elsif ($inrecord == 2 and /<\/ul>/) {
				$inrecord = 0;
				foreach my $k (qw/udate effdate discdate/) {
					$data{$k} = str2time($data{$k}) if $data{$k};
				}
				delete $data{switch} if $data{switch} eq '99999999999' or $data{switch} eq 'XXXXXXXXXXX';
				if (length($data{switch})) {
					if ($data{remotes}) {
						foreach my $remote (sort keys %{$data{remotes}}) {
							next if $remote eq '99999999999' or $remote eq 'XXXXXXXXXXX';
							my $rem = {
								switch=>$remote,
								swfunc=>'REMOTE',
								host=>$data{switch},
								udate=>$data{udate},
								sect=>$sect,
								fdate=>$fdate,
							};
							makedb::updatedata($rem,$fdate,$sect);
						}
						$data{swfunc} = 'HOST';
					}
					$data{ex}      = delete $data{rc}      if $data{rc};
					$data{exshort} = delete $data{rcshort} if $data{rcshort};
					$data{seeex}   = delete $data{seerc}   if $data{seerc};
					$data{exlata}  = $data{lata} unless $data{exlata} or not $data{lata};
					makedb::updatedata(\%data,$fdate,$sect);
				}
			}
		}
		close(INFILE);
	}
	$dbh->commit;
}

sub dolcadata {
	my $dbh = shift;
	$dbh->begin_work;
	my @files = `find exchs -name '*.xml.xz' | sort`;
	my $inrecord = 0;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzcat $fn |") or die "can't process $fn";
		my $fdate = stat($fn)->mtime;
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.xml\.xz//; $sect = "EX-$sect";
		my $data = {sect=>$sect,fdate=>$fdate};
		my %inpt = (fdate=>$fdate);
		my @prefixes = ();
		while (<INFILE>) { chomp;
			if (/^<\/inputdata>/) {
				$inrecord = 0;
			} elsif (/^<inputdata>/) {
				$inrecord = 1;
			} elsif (/^<\/lca-data>/) {
				$inrecord = 0;
				if (length($inpt{exch}) and scalar @prefixes) {
					my %done = ();
					foreach my $rec (@prefixes) {
						next unless length($rec->{exch});
						my $exchob = $inpt{exch};
						my $exchib = $rec->{exch};
						if ($exchob ne $exchib) {
							if ($inpt{dir} eq 'Inbound') {
								($exchob,$exchib) =
								($exchib,$exchob);
							}
							unless (exists $done{$exchib}{$exchob}) {
								$rec->{exchib} = $exchib;
								$rec->{exchob} = $exchob;
								delete $rec->{npa};
								delete $rec->{nxx};
								makedb::updatedata($rec,$fdate,$sect);
								$done{$exchib}{$exchob}++;
							}
						}
					}
				}
				last;
			} elsif (/^<lca-data>/) {
				$inrecord = 2;
			} elsif (/^<\/prefix>/) {
				$inrecord = 2;
				foreach my $k (qw/udate effdate discdate/) {
					$data->{$k} = str2time($data->{$k}) if $data->{$k};
				}
				push @prefixes, $data;
				$data = {sect=>$sect,fdate=>$fdate};
			} elsif (/^<prefix>/) {
				$inrecord = 3;
			} elsif ($inrecord == 1 and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				my ($k,$v) = ($1,$2);
				$k =~ s/-//g;
				$k = "\L$k\E";
				$inpt{$k} = $v if length($v);
			} elsif ($inrecord == 3 and /^<([-a-z]+)>\s*([^<]*?)\s*</) {
				my ($k,$v) = ($1,$2);
				$k =~ s/-//g;
				$k = "\L$k\E";
				$data->{$k} = $v if length($v);
			}
		}
		close(INFILE);
	}
	$dbh->commit;
}

sub dodata {
	my $dbh = shift;
	dorcdata($dbh);
	dolcadata($dbh);
	doswitchdata($dbh);
	doprefixdata($dbh);
}

makedb::makedb('lcgdata',\&dodata);

exit;

1;

__END__
