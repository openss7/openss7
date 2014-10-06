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

sub dodatacsv {
	my $dbh = shift;
	$dbh->begin_work;
	my $fn = "$progdir/data.csv";
	print STDERR "I: processing $fn\n";
	open(INFILE,"<:utf8",$fn) or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $header = 1;
	my @fields = ();
	my $lineno = 0;
	while (<INFILE>) { chomp; $lineno++;
		if (/^Sheet[ =]+(.*?)$/) {
			print STDERR "I: processing sheet $1\n";
			next;
		}
		if (/^---/) {
			$header = 1;
			@fields = ();
			next;
		}
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		if (scalar @tokens != scalar @fields) {
			print STDERR "E: bad line $lineno: ",scalar(@tokens)," tokens instead of ",scalar(@fields),"\n";
			next;
		}
		my $sect = "VZ-$lineno";
		my %data = (sect=>$sect,fdate=>$fdate);
		for (my $i=0;$i<@fields;$i++) {
			$tokens[$i] =~ s/^\s+//;
			$tokens[$i] =~ s/\s+$//;
			$tokens[$i] = '' if $tokens[$i] eq '-';
			$data{"\L$fields[$i]\E"} = $tokens[$i] if length($tokens[$i]);
		}
		delete $data{tdmlcl} if $data{tdmlcl} eq 'No Local Tandem';
		delete $data{tdm911} if $data{tdm911} eq 'NO E-911';
		$data{switch}	    = delete $data{swclli} if $data{swclli};
		$data{switchname}   = delete $data{swname} if $data{swname};
		$data{switchtype}   = delete $data{swtype} if $data{swtype};
		$data{wc}	    = delete $data{wcclli} if $data{wcclli};
		$data{wc}	    = substr($data{switch},0,8) unless $data{wc};
		$data{swocn}	    = $data{ocn} if $data{ocn};
		$data{swoocn}	    = $data{oocn} if $data{oocn};
		$data{swlata}	    = $data{lata} if $data{lata};
		my $f = 'switch';
		unless (not $data{$f} or length($data{$f}) == 11) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		my $f = 'wc';
		unless (not $data{$f} or length($data{$f}) == 8) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		for $f (@makedb::allcllis) {
			if (length($data{$f})) {
				$data{$f} =~ s/[-_]/ /g;
				foreach my $pc (split(/,/,$data{$f})) {
					if (length($pc) and length($pc) != 11) {
						print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
						next;
					}
				}
			}
		}
		$f = 'npa';
		if (length($data{$f})) {
			foreach my $npa (split(/,/,$data{$f})) {
				if (length($npa) and $npa !~ /^[0-9]{3}$/) {
					print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
					next;
				}
			}
		}
		for $f (qw/spc apc/) {
			if (length($data{$f}) and $data{$f} !~ /^[0-9]{3}-[0-9]{3}-[0-9]{3}$/) {
				print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
				next;
			}
		}
		$f = 'swlata';
		if (length($data{$f}) and $data{$f} !~ /^[0-9]{3}$/ and $data{$f} !~ /^[0-9]{5}$/) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		$f = 'wczip';
		if (length($data{$f}) and $data{$f} !~ /^[0-9]{5}$/) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		$f = 'wcst';
		if (length($data{$f}) and $data{$f} !~ /^[A-Z]{2}$/) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		$f = 'wcvh';
		if (length($data{$f}) and $data{$f} !~ /^0[0-9]{4},0[0-9]{4}$/) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		if ($data{nxx} and $data{nxx} =~ /-/) {
			($data{npa},$data{nxx}) = split(/-/,$data{nxx});
		}
		makedb::updatedata(\%data,$fdate,$sect);
	}
	close(INFILE);
	$dbh->commit;
}

sub doe911data {
	my $dbh = shift;
	$dbh->begin_work;
	my $fn = "$progdir/E911_ActGuide_March2007_X.pdf";
	print STDERR "I: processing $fn\n";
	open(INFILE,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $sect = 'E911';
	while (<INFILE>) { chomp;
		while (/\s*(.*?)\s*([A-Z0-9]{11})\s+([0-9]{3})\s+([0-9]{3})\s+([0-9]{3})\b/g) {
			my %data = (sect=>$sect,fdate=>$fdate);
			$data{switchtype} = $1;
			$data{switch} = $2;
			$data{spc} = sprintf('%03d-%03d-%03d',$3,$4,$5);
			$data{swfunc} = 'TDM911';
			if ($data{switchtype} =~ /^(.*?)\s*\/\s*(.*)$/) {
				$data{switchname} = $1;
				$data{switchtype} = $2;
			} elsif ($data{switchtype} =~ /^(.*?)\s*(DMS\s*(100)?)$/) {
				$data{switchname} = $1;
				$data{switchtype} = $2;
			} elsif ($data{switchtype} =~ /^(.*?)\s*(5E(SS)?)$/) {
				$data{switchname} = $1;
				$data{switchtype} = $2;
			} elsif ($data{switchtype} =~ /^(.*?)\s*\((.*)\)$/) {
				$data{switchname} = $2;
				$data{switchtype} = $1;
			}
			$data{switchtype} =~ s/^5E(\s?ESS|SS)$/5ES/;
			$data{switchtype} =~ s/^DMS(\s?100)$/DMH/;
			makedb::updatedata(\%data,$fdate,$sect);
		}
	}
	close(INFILE);
	$dbh->commit;
}

sub dopchangedata {
	my $dbh = shift;
	$dbh->begin_work;
	my $fn = "$progdir/ND10-0003.pdf";
	print STDERR "I: processing $fn\n";
	open(INFILE,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $sect = 'ND10';
	while (<INFILE>) { chomp;
		if (/^([A-Z]{2})\s+(.*?)\s+([A-Z0-9]{11})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})/) {
			my %data = (sect=>$sect,fdate=>$fdate);
			$data{st} = $1;
			$data{switchname} = $2;
			$data{switch} = $3;
			$data{oldspc} = $4;
			$data{spc} = $5;
			makedb::updatedata(\%data,$fdate,$sect);
		}
	}
	close(INFILE);
	$dbh->commit;
}

sub dodata {
	my $dbh = shift;
	dodatacsv($dbh);
	dopchangedata($dbh);
	doe911data($dbh);
}

makedb::makedb('veridata',\&dodata);

exit;

1;

__END__
