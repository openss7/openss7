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

my %ccst2rg = ();
my %ccst2pc = ();
my %ccst2ps = ();
my %pcps2rg = ();

sub dolergst {
	my $fn = "$codedir/lergst.txt";
	print STDERR "I: reading $fn\n";
	open(INFILE,"<:utf8",$fn) or die "can't read $fn";
	while (<INFILE>) { chomp;
		next unless /^[A-Z][A-Z]\t/;
		my @tokens = split(/\t/,$_);
		$ccst2rg{$tokens[0]}{$tokens[1]} = $tokens[2];
		$ccst2pc{$tokens[0]}{$tokens[1]} = $tokens[4] if $tokens[4] ne 'ZZ';
		$ccst2ps{$tokens[0]}{$tokens[1]} = $tokens[5] if $tokens[5] ne 'XX';
		$pcps2rg{$tokens[4]}{$tokens[5]} = $tokens[2] if $tokens[2] ne 'XX' and $tokens[2] ne 'ZZ';
	}
	close(INFILE);
}

sub donpanxx {
	my $dbh = shift;
	$dbh->begin_work;
	my %mapping = (
		'NPA NXX'=>{
			'NPA NXX'=>sub{
				my ($dat,$fld,$val) = @_;
				return unless $val;
				if ($val =~ /([0-9]{3})-([0-9]{3})(-([0-9A]))?/) {
					$dat->{npa} = $1;
					$dat->{nxx} = $2;
					$dat->{x} = $4 if length($4);
				}
			},
			'NPA NXX Record'=>\&makedb::simplefield,
			'Line From'=>sub{
				my ($dat,$fld,$val) = @_;
				#$dat->{$fld} = $val if length($val);
				$dat->{xxxx} = substr($val,8,4) if length($val) >= 12;
			},
			'Line To'=>sub{
				my ($dat,$fld,$val) = @_;
				#$dat->{$fld} = $val if length($val);
				$dat->{yyyy} = substr($val,8,4) if length($val) >= 12;
				$dat->{x} = substr($dat->{xxxx},0,1) unless length($dat->{x});
				if ($dat->{xxxx} eq '0000' and $dat->{yyyy} eq '9999') {
					delete $dat->{x};
					delete $dat->{xxxx};
					delete $dat->{yyyy};
				} elsif ($dat->{xxxx} eq "$dat->{x}000" and $dat->{yyyy} eq "$dat->{x}999") {
					delete $dat->{xxxx};
					delete $dat->{yyyy};
				}
			},
			'Numbers From'=>sub{
				my ($dat,$fld,$val) = @_;
				#$dat->{$fld} = $val if length($val);
				$dat->{xxxx} = substr($val,8,4) if length($val) >= 12;
			},
			'Numbers To'=>sub{
				my ($dat,$fld,$val) = @_;
				#$dat->{$fld} = $val if length($val);
				$dat->{yyyy} = substr($val,8,4) if length($val) >= 12;
				$dat->{x} = substr($dat->{xxxx},0,1) unless length($dat->{x});
				if ($dat->{xxxx} eq '0000' and $dat->{yyyy} eq '9999') {
					delete $dat->{x};
					delete $dat->{xxxx};
					delete $dat->{yyyy};
				} elsif ($dat->{xxxx} eq "$dat->{x}000" and $dat->{yyyy} eq "$dat->{x}999") {
					delete $dat->{xxxx};
					delete $dat->{yyyy};
				}
			},
			'Wireless Block'=>\&makedb::booleanfield,
			'NXX Type'=>\&makedb::simplefield,
			'Portable Block'=>\&makedb::booleanfield,
			'1,000 Block Pooling'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::booleanfield($dat,'tbpooling',$val);
			},
			'Block Contaminated'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::booleanfield($dat,'contaminated',$val);
			},
			'Block Retained'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::booleanfield($dat,'retained',$val);
			},
			'Date Assigned'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::datefield($dat,'assigndate',$val);
			},
			'Effective Date'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::datefield($dat,'effdate',$val);
			},
			'Date Activated'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::datefield($dat,'actdate',$val);
			},
			'Last Modified'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::datefield($dat,'udate',$val);
			},
		},
		'Carrier'=>{
			'Common Name'=>\&makedb::simplefield,
			'OCN'=>\&makedb::simplefield,
			'OCN Type'=>\&makedb::simplefield,
			'Name'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'carrier',$val);
			},
			'Abbreviation'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'abbv',$val);
			},
			'DBA'=>\&makedb::simplefield,
			'FKA'=>\&makedb::simplefield,
			'Address'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'addr',$val);
			},
			'City'=>\&makedb::simplefield,
			'State'=>\&makedb::simplefield,
			'Zip'=>\&makedb::simplefield,
			'Country'=>\&makedb::simplefield,
		},
		'Wire Center'=>{
			'LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wclata',$val);
			},
			'Other switches in WC'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'other',$val);
			},
			'Address'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wcaddr',$val);
			},
			'County'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wccounty',$val);
			},
			'City'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wccity',$val);
			},
			'Operator Services'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'tdmops',$val);
			},
			'Class 4/5 Switch'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'cls45sw',$val);
			},
			'State'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wcst',$val);
			},
			'Trunk Gateway'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'tgclli',$val);
			},
			'Point Code'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'spc',$val);
			},
			'Switch type'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'switchtype',$val);
			},
			'Tandems'=>{
				'IntraLATA'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmilt',$val);
				},
				'Feature Group C'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmfgc',$val);
				},
				'Feature Group D'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmfgd',$val);
				},
				'Local'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmlcl',$val);
				},
				'Feature Group B'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmfgb',$val);
				},
				'Operator Services'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'tdmops',$val);
				},
			},
			'Zip'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wczip',$val);
			},
			'SS7 STP 1'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'stp1',$val);
			},
			'Coordinates'=>{
				'Google maps'=>sub{
					my ($dat,$fld,$val) = @_;
				},
				'Latitude/Longitude'=>sub{
					my ($dat,$fld,$val) = @_;
					if (length($val)) {
						my ($wclat,$wclon) = split(/,/,$val);
						$dat->{wclat} = sprintf('%.8f',$wclat) if $wclat;
						$dat->{wclon} = sprintf('%.8f',$wclon) if $wclon;
					}
				},
				'V&H'=>sub{
					my ($dat,$fld,$val) = @_;
					if (length($val)) {
						my ($wcv,$wch) = split(/,/,$val);
						$dat->{wcv} = sprintf('%05d',$wcv) if $wcv;
						$dat->{wch} = sprintf('%05d',$wch) if $wch;
					}
				},
			},
			'Wire Center CLLI'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wc',$val);
			},
			'Switch'=>\&makedb::simplefield,
			'SS7 STP 2'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'stp2',$val);
			},
			'Actual Switch'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'actual',$val);
			},
			'Country'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wccc',$val);
			},
			'Call Agent'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'agent',$val);
			},
			'Host'=>\&makedb::simplefield,
			'Wire Center Name'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'wcname',$val);
			},
		},
		'LATA'=>{
			'LATA'=>\&makedb::simplefield,
			'Name'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'ltname',$val);
			},
			'Historical Region'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'lthist',$val);
			},
			'Area Codes in LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'ltnpa',$val);
			},
			'Remark'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'ltremark',$val);
			},
		},
		'Rate Center - Locality'=>{
			'LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rclata',$val);
			},
			'Rate Center Name'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rc',$val);
			},
			'State'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rcst',$val);
			},
			'Country'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rccc',$val);
				if (exists $pcps2rg{$dat->{rccc}}{$dat->{rcst}}) {
					$dat->{region} = $pcps2rg{$dat->{rccc}}{$dat->{rcst}};
				} else {
					print STDERR "E: no region for $dat->{rccc}-$dat->{rcst}\n";
				}
			},
			'LERG Abbreviation'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rcshort',$val);
			},
			'Zone'=>\&makedb::simplefield,
			'Zone Type'=>\&makedb::simplefield,
			'Number Pooling'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'nbpooling',$val);
			},
			'Point Identifier'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'pointid',$val);
			}, # Canada only
			'Rate Step'=>\&makedb::simplefield,
			'Area Codes in Rate Center'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rcnpa',$val);
			},
			'Embedded Overlays'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'overlays',$val);
			},
			'Coordinates'=>{
				'Major V&H'=>sub{
					my ($dat,$fld,$val) = @_;
					if (length($val)) {
						my ($rcv,$rch) = split(/,/,$val);
						$dat->{rcv} = sprintf('%05d',$rcv) if $rcv;
						$dat->{rch} = sprintf('%05d',$rch) if $rch;
					}
				},
				'Minor V&H'=>sub{
					my ($dat,$fld,$val) = @_;
				},
				'Latitude/Longitude'=>sub{
					my ($dat,$fld,$val) = @_;
					if (length($val)) {
						my ($rclat,$rclon) = split(/,/,$val);
						$dat->{rclat} = sprintf('%.8f',$rclat) if $rclat;
						$dat->{rclon} = sprintf('%.8f',$rclon) if $rclon;
					}
				},
				'Google maps'=>sub{
					my ($dat,$fld,$val) = @_;
				},
			},
			'Time Zone'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'tzone',$val);
			},
			'DST Recognized'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::booleanfield($dat,'dst',$val);
			},
			'Location within Rate Center'=>{
				'Type'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'loctype',$val);
				},
				'Name'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'locname',$val);
				},
			},
			'County or Equivalent'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rccounty',$val);
			},
			'Federal Feature ID'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'fedfeatid',$val);
			},
			'FIPS County Code'=>\&makedb::simplefield,
			'FIPS Place Code'=>\&makedb::simplefield,
			'Land Area'=>\&makedb::simplefield,
			'Water Area'=>\&makedb::simplefield,
			'Latitude/Longitude'=>sub{
				my ($dat,$fld,$val) = @_;
				if (length($val)) {
					my ($lclat,$lclon) = split(/,/,$val);
					$dat->{lclat} = sprintf('%.8f',$lclat) if $lclat;
					$dat->{lclon} = sprintf('%.8f',$lclon) if $lclon;
				}
			},
			'Population 2000'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'pop2000',$val);
			},
			'Population 2009'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'pop2009',$val);
			},
			'Rural-Urban Continuum Code'=>sub{
				my ($dat,$fld,$val) = @_;
				makedb::simplefield($dat,'rucc',$val);
			},
			'Core Based Statistical Area'=>{
				'CBSA'=>\&makedb::simplefield,
				'CBSA Level'=>\&makedb::simplefield,
				'County Relation to CBSA'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'cbsarel',$val);
				},
				'Metro Division'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'metrodiv',$val);
				},
				'Part of CSA'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'csa',$val);
				},
			},
			'Market Area'=>{
				'Cellular Market'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'cellmarket',$val);
				},
				'Major Trading Area'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'mta',$val);
				},
				'Basic Trading Area'=>sub{
					my ($dat,$fld,$val) = @_;
					makedb::simplefield($dat,'bta',$val);
				},
			},
		},
	);
	print STDERR "I: locating files...\n";
	my @files = `find $datadir -name '*.html.xz' | sort`;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzcat $fn |") or die "can't process $fn";
		my $fdate = stat($fn)->mtime;
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.html\.xz//;
		my $entry='';
		while (<INFILE>) { chomp; s/\r//g; chomp;
			$entry .= $_;
		}
		close(INFILE);
		my %data = (sect=>$sect,fdate=>$fdate);
		my ($hdr,$key,$val);
		while ($entry =~ /(<h3>(.*?)<\/h3>|<div class="n30">(.*?)<\/div>[[:space:]]*<div class="n70">(.*?)<\/div>|<div class="n100">(&nbsp;)<\/div>)/g) {
			if ($2) {
				my $tit = $2;
				my $npa;
				if ($tit =~ /^NPA NXX ([0-9]+-[0-9]+)$/) {
					$tit = 'NPA NXX';
					$npa = $1;
				}
				$key = $tit;
				$hdr = undef;
				if ($npa) {
					if (exists $mapping{$key}{$tit}) {
						#print STDERR "D:(699) mapping '$key', '$tit', '$npa'\n";
						&{$mapping{$key}{$tit}}(\%data,$tit,$npa);
					} else {
						print STDERR "E: no mapping for '$key', '$tit'\n";
					}
				}
			} elsif ($5) {
				$hdr = undef;
			} else {
				my $fld = $3;
				my $val = $4;
				my $sub;
				$fld =~ s/\&amp;/\&/g;
				$val =~ s/\&amp;/\&/g;
				if ($fld =~ /^CBSA ([0-9]+)$/) {
					$fld = 'CBSA';
					$sub = $1;
				} elsif ($fld =~ /^Part of CSA ([0-9]+)$/) {
					$fld = 'Part of CSA';
					$sub = $1;
				} elsif ($fld =~ /^Metro Division ([0-9]+)$/) {
					$fld = 'Metro Division';
					$sub = $1;
				} elsif ($fld =~ /^Rural-Urban Continuum Code ([0-9]+)$/) {
					$fld = 'Rural-Urban Continuum Code';
					$sub = $1;
				}
				if ($fld eq '&nbsp;') {
					if ($val =~ /<em>(.*?)<\/em>/) {
						$hdr = $1;
					} elsif ($val =~ /<a href/) {
						$fld = 'Google maps';
					} else {
						$fld = 'Remark';
					}
				}
				unless ($fld eq '&nbsp;') {
					$val = "$sub $val" if $sub;
					if ($hdr) {
						if (exists $mapping{$key}{$hdr}{$fld}) {
							#print STDERR "D:(742) mapping '$key', '$hdr', '$fld', '$val'\n";
							&{$mapping{$key}{$hdr}{$fld}}(\%data,$fld,$val);
						} else {
							print STDERR "E: no mapping for '$key', '$hdr', '$fld', val='$val'\n";
						}
					} else {
						if (exists $mapping{$key}{$fld}) {
							#print STDERR "D:(749) mapping '$key', '$fld', '$val'\n";
							&{$mapping{$key}{$fld}}(\%data,$fld,$val);
						} else {
							print STDERR "E: no mapping for '$key', '$fld' val='$val'\n";
						}
					}
				}
			}
		}
		makedb::updatedata(\%data,$fdate,$sect);
	}
	$dbh->commit;
}

sub dodata {
	my $dbh = shift;
	dolergst($dbh);
	donpanxx($dbh);
}

makedb::makedb('nnxsdata',\&dodata);

exit;

1;

__END__
