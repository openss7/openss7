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

sub doocndata {
	my $dbh = shift;
	$dbh->begin_work;
	my $fn = "$datadir/NECA-4-1.pdf";
	print STDERR "I: processing $fn\n";
	open(INFILE,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	binmode(INFILE,':utf8');
	my $start = 0;
	my $frag = '';
	my $fdate = stat($fn)->mtime;
	while (<INFILE>) { chomp;
		my %data = (fdate=>$fdate);
		unless ($start) {
			next unless /^\s+COMPANY CODES FOR/;
			$start = 1;
			next;
		}
		if ($frag) {
			$_ = join(' ',$frag,$_);
			$frag = '';
		}
		last if /^\s+END OF SECTION/;
		next unless /^\s/ or /^[0-9][0-9][0-9][A-Z0-9]\s/;
		next if /^\s*-\s*THE\s*COMPANY HAS/;
		next if /^\s*-\s*THE\s*LEGAL NAME/;
		next if /^\s*-\s*THE\s*ACQUIRED CODE/;
		next if /PRINTED IN U.S.A./;
		next if /^====/;
		next if /^\f/;
		next if /^DIRECTOR - ACCESS TARIFFS/;
		next if /^80 S. JEFFERSON/;
		next if /^WHIPPANY, N.J./;
		next if /^ISSUED: /;
		next if /WIRE CENTER AND INTERCONNECTION INFORMATION/;
		next if /COMPANY CODES FOR/;
		next if /^\s*CODE\s*$/;
		next if /^\s*$/;
		s/\([CDSN]\)//g; s/^\s+//; s/\s+$//;
		next if $_ eq '';
		if (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+(.*?)\s+\(OVERALL\)$/) {
			my ($trgt,$ocn,$company) = ($1,$2,$3);
			$data{trgt} = $trgt if $trgt;
			$data{ocn} = $ocn;
			$data{companyname} = $company;
			$data{overall} = 1;
			$data{trgt} =~ s/\s+$//;
			if ($data{companyname} =~ /^\(FORMERLY\)/) {
				$data{fka} = delete $data{companyname};
				$data{fka} =~ s/^\(FORMERLY\)//;
			}
			makedb::updatedata(\%data,$fdate);
			%data = (fdate=>$fdate);
		} elsif (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])\s+([YN])\s+(CAP|[A-Z]+ )\s*(.+)$/) {
			my ($trgt,$ocn,$neca,$type,$company) = ($1,$2,$3,$4,$5);
			$data{trgt} = $trgt if $trgt;
			$data{ocn} = $ocn;
			$data{neca} = $neca eq 'Y' ? 1 : 0;
			$data{companytype} = $type;
			$data{companyname} = $company;
			$data{trgt} =~ s/\s+$//;
			$data{companytype} =~ s/\s+$//;
			if ($data{companyname} =~ /^\(FORMERLY\)/) {
				$data{fka} = delete $data{companyname};
				$data{fka} =~ s/^\(FORMERLY\)//;
			}
			makedb::updatedata(\%data,$fdate);
			%data = (fdate=>$fdate);
		} elsif (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])(\s+([YN])\s+([A-Z]+))?$/) {
			$frag = $_;
		} else {
			warn "cannot parse line '$_'";
		}
	}
	close(INFILE);
	$dbh->commit;
}

my %states = (
	'GUAM'=>'GU',
	'VIRGIN ISLANDS'=>'VI',
	'AMERICAN SAMOA'=>'AS',
	'ALABAMA'=>'AL',
	'ALASKA'=>'AK',
	'ARIZONA'=>'AZ',
	'ARKANSAS'=>'AR',
	'CALIFORNIA'=>'CA',
	'COLORADO'=>'CO',
	'CONNECTICUT'=>'CT',
	'DELAWARE'=>'DE',
	'DISTRICT OF COLUMBIA'=>'DC',
	'FLORIDA'=>'FL',
	'GEORGIA'=>'GA',
	'HAWAII'=>'HI',
	'IDAHO'=>'ID',
	'ILLINOIS'=>'IL',
	'INDIANA'=>'IN',
	'IOWA'=>'IA',
	'KANSAS'=>'KS',
	'KENTUCKY'=>'KY',
	'LOUISIANA'=>'LA',
	'MAINE'=>'ME',
	'MARYLAND'=>'MD',
	'MASSACHUSETTS'=>'MA',
	'MICHIGAN'=>'MI',
	'MINNESOTA'=>'MN',
	'MISSISSIPPI'=>'MS',
	'MISSOURI'=>'MO',
	'MONTANA'=>'MT',
	'NEBRASKA'=>'NE',
	'NEVADA'=>'NV',
	'NEW HAMPSHIRE'=>'NH',
	'NEW JERSEY'=>'NJ',
	'NEW MEXICO'=>'NM',
	'NEW YORK'=>'NY',
	'NORTH CAROLINA'=>'NC',
	'NORTH DAKOTA'=>'ND',
	'OHIO'=>'OH',
	'OKLAHOMA'=>'OK',
	'OREGON'=>'OR',
	'PENNSYLVANIA'=>'PA',
	'PUERTO RICO'=>'PR',
	'RHODE ISLAND'=>'RI',
	'SOUTH CAROLINA'=>'SC',
	'SOUTH DAKOTA'=>'SD',
	'TENNESSEE'=>'TN',
	'TEXAS'=>'TX',
	'UTAH'=>'UT',
	'VERMONT'=>'VT',
	'VIRGINIA'=>'VA',
	'WASHINGTON'=>'WA',
	'WEST VIRGINIA'=>'WV',
	'WISCONSIN'=>'WI',
	'WYOMING'=>'WY',
);

sub docontactdata {
	my $dbh = shift;
	$dbh->begin_work;
	my $fn = "$datadir/NECA-4-1.pdf";
	print STDERR "I: processing $fn\n";
	open(INFILE,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	binmode(INFILE,':utf8');
	my $fdate = stat($fn)->mtime;
	my @lines1 = (); my $wasblank1 = 1;
	my @lines2 = (); my $wasblank2 = 1;
	my @holdln = ();
	my $start = 0;
	my $holding = 0;
	while (<INFILE>) { chomp;
		unless ($start) {
			next unless /^\s+ISSUING CARRIERS/;
			$start = 1;
			next;
		}
		last if /^\s+END OF SECTION/;
		next if /THE CARRIERS ON WHOSE/;
		next if /THERE ARE ANY CONCURRING/;
		next if /TARIFF, THEY ARE NOT/;
		next if /PRINTED IN U.S.A./;
		next if /^====/;
		next if /^\f/;
		next if /TARIFF F\.C\.C\. NO\.\s+4/;
		next if /^NATIONAL EXCHANGE CARRIER ASSOCIATION, INC\./;
		next if /^DIRECTOR - ACCESS TARIFFS/;
		next if /^80 S. JEFFERSON/;
		next if /^WHIPPANY, N.J./;
		next if /^ISSUED: /;
		next if /WIRE CENTER AND INTERCONNECTION INFORMATION/;
		next if /ISSUING CARRIERS/;
		my $line1 = substr($_,0,40);
		$line1 =~ s/\([CDSN]\)//g;
		$line1 =~ s/^\s+//; $line1 =~ s/\s+$//;
		$line1 =~ s/  +/ /g;
		my $line2 = substr($_,41);
		$line2 =~ s/\([CDSN]\)//g;
		$line2 =~ s/^\s+//; $line2 =~ s/\s+$//;
		$line2 =~ s/  +/ /g;
		if ($line1 eq '') {
			unless ($wasblank1) {
				$wasblank1 = 1;
				if (@holdln) {
					push @lines1, @holdln;
					@holdln = ();
				}
				push @lines1, $line1;
			}
		} else {
			$wasblank1 = 0;
			push @lines1, $line1;
		}
		if ($line2 eq '') {
			if ($holding) {
				$holding = 0;
				next;
			}
			unless ($wasblank2) {
				$wasblank2 = 2;
				push @lines2, $line2;
			}
		} elsif ($line2 eq "-cont'd-") {
			$holding = 1;
			next;
		} else {
			if ($holding) {
				push @holdln, $line2;
				next;
			}
			$wasblank2 = 0;
			push @lines2, $line2;
		}
	}
	close(INFILE);
	my %data = (fdate=>$fdate);
	my $lno = 0;
	my $continue = 0;
	foreach (@lines1,'',@lines2,'') {
		#printf STDERR "line %d: %s\n", $lno, $_;
		if (/^$/) {
			if ($lno) {
				makedb::updatedata(\%data,$fdate);
				#print STDERR "I: done processing $data{ocn}\n";
				%data = (fdate=>$fdate);
				$lno = 0;
			}
			next;
		}
		if ($lno == 0) {
			if (/^([0-9][0-9][0-9][A-Z0-9])$/) {
				$data{ocn} = $1;
				$lno++;
				#print STDERR "I: processing $data{ocn}\n";
				next;
			} elsif (/^\(([0-9][0-9][0-9][A-Z0-9])\)$/) {
				$data{ocn} = $1;
				$data{overall} = 1;
				$lno++;
				#print STDERR "I: processing ($data{ocn})\n";
				next;
			}
			die "cannot find ocn in line '$_'";
		} elsif ($lno == 1) {
			$data{name} = $_;
			$lno++;
			next;
		} elsif ($lno == 2) {
			$data{title} = $_;
			$lno++;
			next;
		} elsif ($lno == 3) {
			if (/PO BOX ADDRESS/) {
				# missing title
				$data{org} = delete $data{title};
				$lno = 8;
				next;
			} elsif (/^%/) {
				# missing title
				$data{org} = delete $data{title};
				$data{careof} = $_;
				$data{careof} =~ s/^%//;
				$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
				$lno += 2;
				next;
			} else {
				$data{org} = $_;
				$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
				$lno++;
				next;
			}
		} elsif ($lno == 4) {
			if (/^%/) {
				$data{careof} = $_;
				$data{careof} =~ s/^%//;
				$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
				next;
			}
			if ($continue
			    or /^Overall$/
			    or /Inc/
			    or /^Communications/
			    or /^Adak/
			    or /Cooperative/
			    or /Authority/
			    or /Telecommunications/
			    or /dba/
			    or /^- /
			    or /LLC/
			    or /L\.P\./
			    or /\bCo\b/
			    or /COTC Connections/
			    or /Corp/
			    or /Company/
			    or /Services/
			    or /Telecom/
			    or /Telcom/
			    or /Fulton/
			    or /Coop/
			    or /Dept/
			    or /Communications/
			    or /\(Contel\)/
			    or /\(Gte\)/
			    or /\(Quaker St\)/
			    or /^Springs$/
			    or /^Jamestown$/
			    or /^Red Jacket$/
			    or /^[A-Z][A-Z]$/
			    or /^- *[A-Z][A-Z]$/
			)
			{
				if (exists $data{careof}) {
					$data{careof} .= " $_";
					$data{careof} =~ s/^%//;
				} else {
					$data{org} .= " $_";
				}
				$continue = /\b(d\/b\/a|dba|Of|of|,)$/;
				next;
			}
			if (/PO BOX ADDRESS/) {
				$lno = 8;
				next;
			}
			$data{street} = $_;
			$lno++;
			next;
		} elsif ($lno == 5) {
			if (/^USA$/) {
				# missing {title}
				$data{country} = $_;
				if (exists $data{careof}) {
					$data{city} = delete $data{careof};
					$data{careof} = delete $data{org};
					$data{org}  = delete $data{title};
				} else {
					$data{city} = delete $data{org};
					$data{org}  = delete $data{title};
				}
				$lno += 2;
				next;
			}
			if (/\b(Mailstop|Route|Suite|Ste|Box|Floor)\b/) {
				$data{suite} = $_;
				next;
			}
			$data{city} = $_;
			$lno++;
			next;
		} elsif ($lno == 6) {
			$data{country} = $_;
			$lno++;
			next;
		} elsif ($lno == 7) {
			if (/PO BOX ADDRESS/) {
				$lno++;
				next;
			}
			if (/FOR THE STATES OF:/) {
				$lno = 12;
				next;
			}
			$lno = 11;
			next;
		} elsif ($lno == 8) {
			$data{pobox} = $_;
			$lno++;
			next;
		} elsif ($lno == 9) {
			$data{pocity} = $_;
			$lno++;
			next;
		} elsif ($lno == 10) {
			$data{pocountry} = $_;
			$lno++;
			next;
		} elsif ($lno == 11) {
			if (/FOR THE STATES OF:/) {
				$lno++;
				next;
			}
			warn "excess lines '$_'";
			next;
		} elsif ($lno == 12) {
			if (/^([A-Z].*?)\(([0-9][0-9][0-9][A-Z0-9])\)$/) {
				my ($stat,$ocn) = ($1,$2);
				my $st = $states{$stat} if exists $states{$stat};
				if ($stat and not $st) {
					print STDERR "E: no mapping for state '$stat'\n";
				}
				push @{$data{states}}, {st=>$st,ocn=>$ocn};
				my $ocn = {ocn=>$ocn,oocn=>$data{ocn},st=>$st,fdate=>$fdate};
				makedb::updatedata($ocn,$fdate);
				next;
			}
			warn "cannot parse state '$_'";
			next;
		}
	}
	$dbh->commit;
}

sub mydoneit {
	my ($dat,$fdate,$sect) = @_;
	makedb::updatedata($dat,$fdate,$sect);
	if ($dat->{blks}) {
		my %data = %$dat;
		for (my $i=0;$i<@{$data{blks}};$i++) {
			$data{npa}   = $data{npas}[$i];
			$data{nxx}   = $data{nxxs}[$i];
			$data{x}     = $data{blks}[$i];
			$data{rng}   = $data{rngs}[$i];
			$data{lines} = $data{lins}[$i];
			($data{xxxx},$data{yyyy}) = split(/-/,$data{rng})
			if length($data{x}) and length($data{rng});
			if ($data{xxxx} eq '0000' and $data{yyyy} eq '9999') {
				delete $data{x};
				delete $data{xxxx};
				delete $data{yyyy};
			} elsif ($data{xxxx} eq "$data{x}000" and $data{yyyy} eq "$data{x}999") {
				delete $data{xxxx};
				delete $data{yyyy};
			}
			makedb::updatedata(\%data,$fdate,$sect);
		}
	}
}

sub donecadata {
	my $dbh = shift;
	$dbh->begin_work;
	foreach my $fname (qw/NECA-4-12.pdf NECA-4-44.pdf NECA-4-400.pdf/) {
		my $fn = "$datadir/$fname";
		print STDERR "I: processing $fn\n";
		open(INFILE,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
		binmode(INFILE,':utf8');
		my $fdate = stat($fn)->mtime;
		my $sect = '0-0';
		my $flineno = 0;
		my $inheader = 1;
		my $infooter = 0;
		my $lineno = 0;
		my %data = (sect=>$sect,fdate=>$fdate);
		my $changing = 0;
		my $willchange = 0;
		my $couldchange = 0;
		my $changenxx = 0;
		my $pending = 0;
		my ($state,$section,$page);
		while (<INFILE>) { chomp; $flineno++;
			if ($inheader) {
				if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
					if ($state ne $1) {
						$state = $1;
						$dbh->commit;
						$dbh->begin_work;
					}
				}
				if (/^\s+SECTION\s+([0-9]+)/) {
					$section = $1;
					$page = 1;
					$sect = "$section-$page";
				}
				if (/\sPAGE\s+([0-9]+)/) {
					$page = $1;
					$sect = "$section-$page";
				}
				if (/PENDING CHANGES/) {
					$pending = 1;
				}
				next unless /^\s?-----/;
				$inheader = 0;
				next;
			}
			if (/^\s?\.\.\.\.\./) {
				my $line = $_;
				mydoneit(\%data,$fdate,$sect);
				$_ = $line;
				%data = (sect=>$sect,fdate=>$fdate);
				$lineno = 0;
				$changing = 0;
				$couldchange = 0;
				$willchange = 0;
				$changenxx = 0;
				next;
			}
			if (/^\s*\(x\) / or /\=====/ or /PRINTED IN U.S.A./ or /END OF SECTION/) {
				$inheader = 1;
				next;
			}
			if (/END OF REPORT/) {
				if ($pending) {
					$pending = 0;
					next;
				}
			}
			if ($pending) {
				next;
			}
			next if /^\s*$/;
			next if /--- Continued/;
			if (/^\s?[A-Z]/) {
				if ($lineno) {
					$lineno = 0;
					$changing = 0;
					$couldchange = 0;
					$willchange = 0;
					$changenxx = 0;
					mydoneit(\%data,$fdate,$sect);
					%data = (sect=>$sect,fdate=>$fdate);
				}
			} else {
				unless ($lineno or /\(D\)/) {
					if (/SERVING WIRE CENTER V AND H COORDINATE INFORMATION - (.*)$/) {
						if ($state ne $1) {
							$state = $1;
							$dbh->commit;
							$dbh->begin_work;
						}
					}
					print STDERR "SKIPPING: $fname($flineno) $_\n";
					next;
				}
			}
			if (/\(D\)/) {
				next;
			}
			$lineno++;
			my $ugly = 1;
			if (/\(S\)/) {
				$willchange = 0;
				$couldchange = 1;
				$changing = 0;
				$changenxx = 0;
			} elsif (/\(C\)/) {
				$willchange = 1;
				$couldchange = 0;
				$changing = 0;
				$changenxx = 0;
			} elsif ($willchange) {
				$willchange = 0;
				$couldchange = 0;
				$changing = 1;
				$changenxx = 0;
				$lineno = 1;
				if (/\b[A-Z][A-Z0-9]\b/) {
					# feature codes are changing too, clear old codes
					delete $data{feat};
				}
			} elsif ($couldchange) {
				if (/"/) {
					$willchange = 0;
					$couldchange = 0;
					$changing = 1;
					$changenxx = 0;
					$lineno = 1;
					if (/\b[A-Z][A-Z0-9]\b/) {
						# feature codes are changing too, clear old codes
						delete $data{feat};
					}
				} else {
					$willchange = 0;
					$couldchange = 0;
					$changing = 0;
					$changenxx = 0;
				}
			}
			if ($lineno >= 1) {
				while (/\b([A-Z][A-Z0-9])\b/g) {
					$data{feat}{$1} = 1;
					$ugly = undef;
				}
			}
			if ($lineno == 1) {
				if ($changing) {
					if (/^\s?([A-Z]+.*?|\s*")\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7}|")\s+([-0-9][0-9]{4}|")\s+([-0-9][0-9]{4}|")\s+([0-9]{3}[A-Z0-9]|")\s+([0-9]{3,}|")/) {
						my ($loc,$clli,$wcv,$wch,$ocn,$lata) = ($1,$2,$3,$4,$5,$6);
						#$data{state} = $state if $state;
						$data{st} = $states{$state} if exists $states{$state};
						if ($state and not $data{st}) {
							print STDERR "E: no mapping for state '$state'\n";
						}
						$data{loc} = $loc unless $loc =~ /"/;
						$data{stloc} = "$state: $loc" if $state and $loc !~ /"/;
						$data{switch} = $clli unless $clli eq '"';
						$data{wcv} = $wcv unless $wcv eq '"';
						$data{wch} = $wch unless $wch eq '"';
						$data{wcvh} = "$data{wcv},$data{wch}" if $data{wcv} and $data{wch};
						$data{ocn} = $ocn unless $ocn eq '"';
						$data{swocn} = $ocn unless $ocn eq '"';
						$data{lata} = $lata unless $lata eq '"';
						$data{swlata} = $lata unless $lata eq '"';
					} elsif (/--- Continued/) {
						# skip these lines
					} else {
						print STDERR "E: $fname($flineno) can't grok line '$_'\n";
					}
				} else {
					if (/^\s?([A-Z]+.*?)\s+([A-Z][A-Z0-9]{2}[A-Z0-9 ][A-Z0-9]{7})\s+([-0-9][0-9]{4})\s+([-0-9][0-9]{4})\s+([0-9]{3}[A-Z0-9])\s+([0-9]{3,})/) {
						my ($loc,$clli,$wcv,$wch,$ocn,$lata) = ($1,$2,$3,$4,$5,$6);
						#$data{state} = $state if $state;
						$data{st} = $states{$state} if exists $states{$state};
						if ($state and not $data{st}) {
							print STDERR "E: no mapping for state '$state'\n";
						}
						$data{loc} = $loc;
						$data{stloc} = "$state: $loc" if $state;
						$data{switch} = $clli;
						$data{wcv} = $wcv;
						$data{wch} = $wch;
						$data{wcvh} = "$wcv,$wch";
						$data{ocn} = $ocn;
						$data{swocn} = $ocn;
						$data{lata} = $lata;
						$data{swlata} = $lata;
						$data{sect} = $sect;
					} elsif (/--- Continued/) {
						# skip these lines
					} else {
						print STDERR "E: $fname($flineno) can't grok line '$_'\n";
					}
				}
			} elsif ($lineno > 1) {
				if (/^\s*([2-9][0-9][0-9])\//) {
					my $npa = $1;
					if ($changing) {
						unless ($changenxx) {
							# changing NXXs, must clear old data
							$data{npas} = [];
							$data{nxxs} = [];
							$data{blks} = [];
							$data{rngs} = [];
							$data{lins} = [];
							$data{total} = 0;
							$changenxx = 1;
						}
					}
					s/^\s*[2-9][0-9]{2}\///;
					s/\s+[A-Z][A-Z0-9]\b//g;
					s/\s*\([A-Za-z]\)\b//g;
					s/\s+.*$//;
					my @nxxs = split(/,/,$_);
					foreach my $nxx (@nxxs) {
						if ($nxx =~ /^[1-9][0-9][0-9]$/) {
							push @{$data{npas}}, $npa;
							push @{$data{nxxs}}, $nxx;
							push @{$data{blks}}, '';
							push @{$data{rngs}}, '0000-9999';
							push @{$data{lins}}, 10000;
							$data{total} += 10000;
						} elsif ($nxx =~ /^([2-9][0-9]{2})\((([0-9])[0-9]{3})-(([0-9])[0-9]{3})\)$/) {
							$nxx = $1;
							my ($s,$e) = ($2,$4);
							my ($f,$l) = ($3,$5);
							for (my $i=$f;$i<=$l;$i++) {
								push @{$data{npas}}, $npa;
								push @{$data{nxxs}}, $nxx;
								push @{$data{blks}}, $i;
								my $bs = $i * 1000;
								my $be = $bs + 999;
								my ($ns,$ne) = ($s,$e);
								$ns = $bs if $s < $bs;
								$ne = $be if $e > $be;
								push @{$data{rngs}}, sprintf("%04d-%04d",$ns,$ne);
								push @{$data{lins}}, $ne - $ns + 1;
								$data{total} += $ne - $ns + 1;
							}
						} else {
							print STDERR "E: $fname($flineno) BAD NXX: $nxx\n";
						}
					}
				} elsif (/COMPANY:\s+([0-9]{3}[0-9A-Z])\s+HAS A PRESENCE ON THIS SWITCH/) {
					$data{ocn} = $1;
				} elsif (/\b___\/___\b/) {
					# skip these lines
					if ($changing) {
						unless ($changenxx) {
							# changing NXXs, must clear old data
							$data{npas} = [];
							$data{nxxs} = [];
							$data{blks} = [];
							$data{rngs} = [];
							$data{lins} = [];
							$data{total} = 0;
							$changenxx = 1;
						}
					}
				} elsif (/"\s+\/\s+"/) {
					# skip these lines
					if ($changing) {
						$changenxx = 1;
					} else {
						if ($ugly) {
							printf STDERR "E: $fname($flineno) UGLY(A) %2d: %s\n", $lineno,$_;
							printf STDERR "E: $fname($flineno) UGLY(A) %2d: %s\n", $lineno,"changing: $changing, willchange: $willchange, couldchange: $couldchange, changenxx: $changenxx";
						}
					}
				} elsif (/--- Continued/) {
					# skip these lines
				} else {
					if ($ugly) {
						printf STDERR "E: $fname($flineno) UGLY(B) %2d: %s\n", $lineno,$_;
						printf STDERR "E: $fname($flineno) UGLY(B) %2d: %s\n", $lineno,"changing: $changing, willchange: $willchange, couldchange: $couldchange, changenxx: $changenxx";
					}
				}
			}
		}
		close(INFILE);

	}
	$dbh->commit;
}

sub dodata {
	my $dbh = shift;
	doocndata($dbh);
	docontactdata($dbh);
	donecadata($dbh);
}

makedb::makedb('necadata',\&dodata);

exit;

1;

__END__
