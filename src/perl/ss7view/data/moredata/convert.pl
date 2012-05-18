#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Geo::Coordinates::VandH;
use POSIX;
use File::stat;
use Time::gmtime;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

my %db = (
	sw=>{},
	wc=>{},
	pl=>{},
);

my %keys = (
	sw=>[qw/SWCLLI NPA NXX X LATA OOCN OCN SWNAME SWTYPE SWABBV SWDESC SWFUNC WCVH WCLL RCVH RCLL TGCLLI CLS45SW SPC APC FEAT STP1 STP2 ACTUAL AGENT HOST MATE TDM TDMILT TDMFGC TDMFGD TDMLCL TDMFGB TDMOPS TDM911 REMOTES SECT FDATE/],
	wc=>[qw/WCCLLI NPA NXX X LATA OOCN OCN WCNAME SWCLLI WCVH WCLL RCVH RCLL WCADDR WCZIP WCCITY WCCOUNTY WCST WCCC SECT FDATE/],
	pl=>[qw/PLCLLI NPA NXX X LATA OOCN OCN WCCLLI WCCITY WCCOUNTY WCST WCCC SECT FDATE/],
	pc=>[qw/SPC APC SWCLLI HOST CLS45SW STP1 STP2 MATE SECT FDATE/],
);

sub getclli {
	my ($type,$key) = @_;
	$db{$type}{$key}{"\U$type\ECLLI"} = $key unless exists $db{$type}{$key}{"\U$type\ECLLI"};
	return $db{$type}{$key};
}

sub getpc {
	my $key = shift;
	$db{pc}{$key}{SPC} = $key unless exists $db{pc}{$key}{SPC};
	return $db{pc}{$key};
}

sub getsw {
	return getclli('sw',shift);
}

sub getwc {
	return getclli('wc',shift);
}

sub getpl {
	return getclli('pl',shift);
}

my $lineno = 0;

sub addrec {
	my ($type,$dat,$key) = @_;
	my $rec = getclli($type,$key);
	foreach my $fld (@{$keys{$type}}) {
		if (length($dat->{$fld})) {
			if ($fld =~ /^(NPA|NXX|X|LATA|OCN|SECT|SWFUNC|TDM911)$/ or
			    ($fld eq 'SWCLLI' and $type ne 'sw') or
			    ($fld eq 'WCCLLI' and $type ne 'wc') or
			    ($fld eq 'PLCLLI' and $type ne 'pl')
			) {
				$dat->{$fld} =~ s/\//,/g;
				foreach my $m (split(/,/,$dat->{$fld})) { $rec->{$fld}{$m}++ }
			} elsif ($fld =~ /^(WCVH|WCLL)$/) {
				foreach my $m (split(/;/,$dat->{$fld})) { $rec->{$fld}{$m}++ }
			} elsif ($fld =~ /^(FEAT)$/) {
				foreach my $m (split(/\s/,$dat->{$fld})) { $rec->{$fld}{$m}++ }
			} else {
				if (length($rec->{$fld}) and $rec->{$fld} ne $dat->{$fld}) {
					if ($fld =~ /^(SWNAME|WCADDR|WCCITY|WCCOUNTY)$/) {
						$rec->{$fld} =~ s/[\.,]//g; $dat->{$fld} =~ s/[\.,]//g;
						$rec->{$fld} =~ s/\s{2,}/ /g; $dat->{$fld} =~ s/\s{2,}/ /g;
						if ("\U$rec->{$fld}\E" eq "\U$dat->{$fld}\E") {
							if ($dat->{$fld} =~ /[a-z]/) {
								$rec->{$fld} = $dat->{$fld};
							}
						} else {
							print STDERR "E: \U$type\E: '$key' '$key' $fld changing from $rec->{$fld} (line $rec->{LINENO}) to $dat->{$fld} (line $lineno)\n";
							$rec->{$fld} = $dat->{$fld};
						}
					} else {
						print STDERR "E: \U$type\E: '$key' '$key' $fld changing from $rec->{$fld} (line $rec->{LINENO}) to $dat->{$fld} (line $lineno)\n";
						$rec->{$fld} = $dat->{$fld};
					}
				} else {
					$rec->{$fld} = $dat->{$fld};
				}
			}
			if ($type eq 'sw') {
				if ($fld eq 'HOST') {
					my ($sw,$wc,$pl) = ($dat->{$fld},substr($dat->{$fld},0,8),substr($dat->{$fld},0,6));
					my $r;
					$r = getsw($sw); $r->{REMOTES}{$key}++;
					$r = getwc($wc); $r->{SWCLLI}{$sw}++;
					$r = getpl($pl); $r->{WCCLLI}{$wc}++;
					$rec->{SWFUNC}{REMOTE}++;
				}
				if ($fld =~ /^(CLS45SW|STP1|STP2|ACTUAL|AGENT|HOST|MATE|TDM|TDMILT|TDMFGC|TDMFGD|TDMLCL|TDMFGB|TDMOPS|TDM911)$/) {
					my ($sw,$wc,$pl) = ($dat->{$fld},substr($dat->{$fld},0,8),substr($dat->{$fld},0,6));
					my $r;
					$r = getsw($sw); $r->{SWFUNC}{$fld}++;
					$r = getwc($wc); $r->{SWCLLI}{$sw}++;
					$r = getpl($pl); $r->{WCCLLI}{$wc}++;
				}
				if ($fld eq 'APC') {
					my ($pc) = ($dat->{$fld});
					my $r = getpc($pc); $r->{PCS}{$key}++;
				}
			}
		}
	}
	$rec->{LINENO} = $lineno;
}

sub addsw {
	addrec('sw',@_);
}

sub addwc {
	addrec('wc',@_);
}

sub addpl {
	addrec('pl',@_);
}

sub addpc {
	addrec('pc',@_);
}

$fn = "data.csv";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
$lineno = 0;
while (<$fh>) { chomp;
	$lineno++;
	if (/^Sheet[ =]+(.*?)$/) {
		print STDERR "I: processing sheet $1\n";
		next;
	}
	if (/^---/) {
		$heading = 1;
		@fields = ();
		next;
	}
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	if (scalar @tokens != scalar @fields) {
		print STDERR "E: bad line $lineno: ",scalar(@tokens)," tokens instead of ",scalar(@fields),"\n";
		next;
	}
	my %data = ();
	$data{FDATE} = stat($fn)->mtime;
	for (my $i=0;$i<@fields;$i++) {
		$tokens[$i] =~ s/^\s+//;
		$tokens[$i] =~ s/\s+$//;
		$tokens[$i] = '' if $tokens[$i] eq '-';
		$data{$fields[$i]} = $tokens[$i] if length($tokens[$i]);
	}
	delete $data{TDMLCL} if $data{TDMLCL} eq 'No Local Tandem';
	delete $data{TDM911} if $data{TDM911} eq 'NO E-911';
	my $f;
#	$f = 'SWCLLI';
#	unless (length($data{$f}) == 11) {
#		print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
#		next;
#	}
	for $f (qw/SWCLLI TGCLLI CLS45SW STP1 STP2 ACTUAL AGENT HOST MATE TDM TDMILT TDMFGC TDMFGD TDMLCL TDMFGB TDMOPS TDM911/) {
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
	$f = 'NPA';
	if (length($data{$f})) {
		foreach my $npa (split(/,/,$data{$f})) {
			if (length($npa) and $npa !~ /^[0-9]{3}$/) {
				print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
				next;
			}
		}
	}
	for $f (qw/SPC APC/) {
		if (length($data{$f}) and $data{$f} !~ /^[0-9]{3}-[0-9]{3}-[0-9]{3}$/) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
	}
	$f = 'LATA';
	if (length($data{$f}) and $data{$f} !~ /^[0-9]{3}$/ and $data{$f} !~ /^[0-9]{5}$/) {
		print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
		next;
	}
	$f = 'WCZIP';
	if (length($data{$f}) and $data{$f} !~ /^[0-9]{5}$/) {
		print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
		next;
	}
	$f = 'WCST';
	if (length($data{$f}) and $data{$f} !~ /^[A-Z]{2}$/) {
		print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
		next;
	}
	$f = 'WCVH';
	if (length($data{$f}) and $data{$f} !~ /^0[0-9]{4},0[0-9]{4}$/) {
		print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
		next;
	}
	my $sw = $data{SWCLLI};
	my $wc = $data{WCCLLI}; $wc = $data{WCCLLI} = substr($data{SWCLLI},0,8) unless $wc;
	my $pl = $data{PLCLLI}; $pl = $data{PLCLLI} = substr($data{SWCLLI},0,6) unless $pl;
	my $pc = $data{SPC};
	$data{SECT} = "XLS-$lineno";
	addsw(\%data,$sw) if $sw;
	addwc(\%data,$wc) if $wc;
	addpl(\%data,$pl) if $pl;
	addpc(\%data,$pc) if $pc and not $data{HOST};
}
close($fh);

foreach my $t (qw/sw wc pl pc/) {
	$fn = "$t.nogeo.csv";
	print STDERR "I: writing $fn\n";
	open($of,">:utf8",$fn) or die "can't write $fn";
	print $of '"',join('","',@{$keys{$t}}),'"',"\n";
	foreach my $s (sort keys %{$db{$t}}) {
		my $rec = $db{$t}{$s};
		$rec->{SWFUNC}{TDM}++    if delete $rec->{SWFUNC}{EAT}  or delete $rec->{SWFUNC}{TANDEM};
		$rec->{SWFUNC}{STP}++    if delete $rec->{SWFUNC}{STP1};
		$rec->{SWFUNC}{STP}++    if delete $rec->{SWFUNC}{STP2};
		$rec->{SWFUNC}{STP}++    if delete $rec->{SWFUNC}{MATE};
		$rec->{SWFUNC}{HOST}++   if delete $rec->{SWFUNC}{'HOST-NO LINE'};
		$rec->{SWFUNC}{TDMOPS}++ if delete $rec->{SWFUNC}{TOPS} or delete $rec->{SWFUNC}{OPS};
		$rec->{SWFUNC}{TDMILT}++ if delete $rec->{SWFUNC}{IAT}  or delete $rec->{SWFUNC}{ILT};
		$rec->{SWFUNC}{TDMLCL}++ if delete $rec->{SWFUNC}{LCL}  or delete $rec->{SWFUNC}{LCL};
		$rec->{SWFUNC}{TDMCHK}++ if delete $rec->{SWFUNC}{CHK}  or delete $rec->{SWFUNC}{CHOKE};
		$rec->{SWFUNC}{TDM911}++ if delete $rec->{SWFUNC}{911}  or delete $rec->{SWFUNC}{E911};
		delete $rec->{SWFUNC}{STANDALONE};
		delete $rec->{SWFUNC}{'SMART-REMOTE'};
		my @values = ();
		foreach (@{$keys{$t}}) {
			if (exists $rec->{$_}) {
				if (ref $rec->{$_} eq 'HASH') {
					if ($_ =~ /^(WCVH|WCLL)$/) {
						push @values, join(';',sort keys %{$rec->{$_}});
					} else {
						push @values, join(',',sort keys %{$rec->{$_}});
					}
				} elsif (ref $rec->{$_} eq 'ARRAY') {
					if ($_ =~ /^(WCVH|WCLL)$/) {
						push @values, join(';',@{$rec->{$_}});
					} else {
						push @values, join(',',@{$rec->{$_}});
					}
				} else {
					push @values, $rec->{$_};
				}
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"',join('","',@values),'"',"\n";
	}
	close($of);
}

exit;

1;

__END__
