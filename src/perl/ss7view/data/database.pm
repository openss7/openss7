
my $base = {};

sub restorem {
	my ($rec,$dat,$fields) = @_;
	foreach (@$fields) {
		$dat->{$_} = $rec->{$_} if $rec->{$_} and !$dat->{$_};
	}
}
sub assignem {
	my ($rec,$dat,$key,$fields) = @_;
	foreach (@$fields) {
		if ($dat->{$_}) {
			if ($rec->{$_} and $rec->{$_} ne $dat->{$_}) {
				print STDERR "W: \U$key\E $dat->{$key} $_ changing from $rec->{$_} to $dat->{$_}\n";
				$rec->{mismatch}{$_}{$rec->{$_}}++;
				$rec->{mismatch}{$_}{$dat->{$_}}++;
			}
			$rec->{$_} = $dat->{$_};
		}
	}
}
sub copyem {
	my ($rec,$dat,$fields) = @_;
	foreach (@$fields) {
		$rec->{$_} = $dat->{$_} if $dat->{$_};
	}
}
sub addem {
	my ($rec,$dat,$key,$fields,$check) = @_;
	foreach (@$fields) {
		if ($dat->{$_}) {
			if ($check and exists $rec->{$_} and !exists $rec->{$_}{$dat->{$_}}) {
				print STDERR "W: \U$key\E $rec->{$key} spans more than one \U$_\E\n";
				foreach my $key (keys %{$rec->{$_}}) {
					$rec->{spans}{$_}{$key}++;
				}
				$rec->{spans}{$_}{$dat->{$_}}++;
			}
			#print STDERR join(',', $rec, $_, $rec->{$_}, $dat, $dat->{$_}), "\n";
			$rec->{$_}{$dat->{$_}} += $dat->{lines};
		}
	}
}

sub closerecord {
	my ($data,$db) = @_;
	$db = $base unless $db;
	if ($data->{npa}) {
		$db->{npa}{$data->{npa}} = {} unless exists $db->{npa}{$data->{npa}};
		my $npa = $db->{npa}{$data->{npa}};
		restorem($npa,$data,[qw/state country/]);
		assignem($npa,$data,'npa',[qw/state tz ovly date map nongeo remark country parent complex assigned reserved used/]);
		copyem($npa,$data,[qw/dial tzs/]);
		#addem($npa,$data,'npa',[qw/state/],1);
		addem($npa,$data,'npa',[qw/clli stloc ocn lata tandem/]);
		$npa->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no npa record\n";
	}
	if ($data->{nxx}) {
		unless (exists $data->{blk}) {
			$data->{blk} = 'A';
			$data->{rng} = '0000-9999';
			$data->{lines} = 10000;
		}
		$db->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}} = {}
			unless exists $db->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}};
		my $nxx = $db->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}};
		#copyem($nxx,$data,[qw/npa nxx blk rng/]);
		copyem($nxx,$data,[qw/clli rate ocn lata loc state country county city ovly tandem/]);
		assignem($nxx,$data,'nxx',[qw/edate adate growth use atype note/]);
		#$nxx->{lines} = $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no nxx record\n";
	}
	if ($data->{state}) {
		$db->{state}{$data->{state}} = {} unless exists $db->{state}{$data->{state}};
		my $stat = $db->{state}{$data->{state}};
		#$stat->{state} = $data->{state};
		assignem($stat,$data,'state',[qw/country/]);
		addem($stat,$data,'state',[qw/npa clli loc ocn lata tandem/]);
		$stat->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no state record\n";
	}
	if ($data->{lata}) {
		$db->{lata}{$data->{lata}} = {} unless exists $db->{lata}{$data->{lata}};
		my $lata = $db->{lata}{$data->{lata}};
		#$lata->{lata} = $data->{lata};
		assignem($lata,$data,'lata',[qw/name/]);
		addem($lata,$data,'lata',[qw/state/],1);
		addem($lata,$data,'lata',[qw/npa clli stloc ocn tandem/]);
		$lata->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no lata record\n";
	}
	if ($data->{clli}) {
		$db->{clli}{$data->{clli}} = {} unless exists $db->{clli}{$data->{clli}};
		my $clli = $db->{clli}{$data->{clli}};
		#$clli->{clli} = $data->{clli};
		assignem($clli,$data,'clli',[qw/type wcvh/]);
		if ($data->{nxx}) {
			$clli->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}} += $data->{lines};
		}
		addem($clli,$data,'clli',[qw/state npa stloc lata ocn rate tandem/],0);
		$clli->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no clli record\n";
	}
	if ($data->{ocn}) {
		$db->{ocn}{$data->{ocn}} = {} unless exists $db->{ocn}{$data->{ocn}};
		my $ocn = $db->{ocn}{$data->{ocn}};
		#$ocn->{ocn} = $data->{ocn};
		assignem($ocn,$data,'ocn',[qw/company st overall trgt neca category/]);
		addem($ocn,$data,'ocn',[qw/state lata npa clli tandem/],0);
		$ocn->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no ocn record\n";
	}
	if ($data->{rate}) {
		$db->{rate}{$data->{state}}{$data->{rate}} = {}
			unless exists $db->{rate}{$data->{state}}{$data->{rate}};
		my $rate = $db->{rate}{$data->{state}}{$data->{rate}};
		#$rate->{st} = $data->{state};
		#$rate->{rate}  = $data->{rate};
		assignem($rate,$data,'rate',[qw/rcmjvh rcmnvh abbr/]);
		addem($rate,$data,'rate',[qw/state lata stloc npa clli ocn tandem/],0);
		$rate->{lines} += $data->{lines} if $data->{lines};
	} else {
		#print STDERR "W: no rate record\n";
	}
}

sub quoteit {
	my $datum = shift;
	#return $datum if int($datum);
	while ($datum =~ /^'(.+?)'$/) { $datum = $1; }
	while ($datum =~ /^"(.+?)"$/) { $datum = $1; }
	if ($datum =~ /'/) {
		$datum =~ s/"/\\"/g;
		$datum =~ s/\$/\\\$/g;
		$datum =~ s/\@/\\\@/g;
		$datum = "\"$datum\"";
	} else {
		$datum = "'$datum'";
	}
	return $datum;
}

sub dumpit {
	my ($file,$hash,$indent) = @_;
	if (ref $hash eq 'HASH') {
		foreach my $key (sort keys %{$hash}) {
			my $val = $hash->{$key};
			$key = quoteit($key);
			if (ref $val eq 'HASH') {
				print $file "$indent$key=>{\n";
				dumpit($file,$val,$indent."\t");
				print $file "$indent},\n";
			} elsif (ref $val eq 'ARRAY') {
				print $file "$indent$key=>[";
				dumpit($file,$val,$indent."\t");
				print $file "],\n";
			} else {
				$val = quoteit($val);
				print $file "$indent$key=>$val,\n";
			}
		}
	} elsif (ref $hash eq 'ARRAY') {
		foreach my $val (@{$hash}) {
			$val = quoteit($val);
			print $file "$val,";
		}
	} else {
		print STDERR "E: found scalar $hash\n";
	}
}

sub dumpem_old {
	my ($datadir,$db) = @_;
	$db = $base unless $db;

	my $of = \*OUTFILE;
	foreach my $part (qw/nxx npa state lata clli ocn rate/) {
		my $fn = "$datadir/$part.pm";
		print STDERR "writing $fn\n";
		open($of,">","$fn");
		print $of "\$VAR1 = {\n";
			dumpit($of,$db->{$part},"\t");
		print $of "};\n";
		close($of);
	}
}

sub dumpem {
	my ($datadir,$db) = @_;
	$db = $base unless $db;

	my $of = \*OUTFILE;
	my $fn = "$datadir/db.pm";
	print STDERR "writing $fn\n";
	open($of,">",$fn);
	print $of "\$VAR1 = {\n";
		dumpit($of,$db,"\t");
	print $of "};\n";
	close($of);
}

