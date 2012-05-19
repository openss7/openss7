package makedb;

use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;
use File::stat;
use Time::gmtime;
use Date::Parse;
use DBI;
use strict;

my %schema = (
	tables=>[ qw/lergst nanpst ocndata pcdata rcdata exdata lcadata wcdata switchdata prefixdata pooldata linedata/ ],
	lergst=>{
		keys=>[ qw/rg/ ],
		cols=>[ qw/cc st rg cs pc ps loc/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS lergst (
		cc CHARACTER(2), -- iso alpha2 country code
		st CHARACTER(2), -- iso alpha2 state code
		rg CHARACTER(2) PRIMARY KEY, -- lerg state/prov/territory
		cs CHARACTER(2) UNIQUE, -- CLLI state/prov/territory
		pc CHARACTER(2), -- USPS country code
		ps CHARACTER(2), -- USPS state code
		loc TEXT
	);
},
	},
	nanpst=>{
		keys=>[ qw/npa/ ],
		cols=>[ qw/npa cc st loc/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS nanpst (
		npa CHARACTER(3) PRIMARY KEY,
		cc CHARACTER(2),
		st CHARACTER(2),
		loc TEXT,
		FOREIGN KEY(cc,st) REFERENCES lergst(cc,st)
	);
},
	},
	ocndata=>{
		keys=>[ qw/ocn/ ],
		cols=>[ qw/ocn ocntype companyname companytype overall neca trgt commonname carrier abbv dba fka oocn address name title org careof street city state zip country suite pobox pocity pocountry sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS ocndata (
		ocn CHARACTER(4) PRIMARY KEY,
		ocntype TEXT,
		companyname TEXT,
		companytype VCHAR(4),
		overall BOOLEAN,
		neca BOOLEAN,
		trgt CHARACTER(4),
		commonname TEXT,
		carrier TEXT,
		abbv TEXT,
		dba TEXT,
		fka TEXT,
		oocn CHARACTER(4),
		address TEXT,
		name TEXT,
		title TEXT,
		org TEXT,
		careof TEXT,
		street TEXT,
		city TEXT,
		state TEXT,
		zip TEXT,
		country TEXT,
		suite TEXT,
		pobox TEXT,
		pocity TEXT,
		pocountry TEXT,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	pcdata=>{
		keys=>[ qw/spc/ ],
		cols=>[ qw/spc apc switch host cls45sw e911sw stp1 stp2 mate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS pcdata (
		spc CHARACTER(11) PRIMARY KEY,
		apc CHARACTER(11),
		switch CHARACTER(11),
		host CHARACTER(11),
		cls45sw CHARACTER(11),
		e911sw CHARACTER(11),
		stp1 CHARACTER(11),
		stp2 CHARACTER(11),
		mate CHARACTER(11),
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	rcdata=>{
		keys=>[ qw/region rcshort/ ],
		cols=>[ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS rcdata (
		region CHARACTER(2),
		rcshort VCHAR(10),
		rclata CHARACTER(4),
		rc TEXT,
		rcst CHARACTER(2),
		rccc CHARACTER(2),
		rccity TEXT,
		rcv INTEGER,
		rch INTEGER,
		rclat DOUBLE,
		rclon DOUBLE,
		tzone TEXT,
		dst BOOLEAN,
		zone TEXT,
		zonetype TEXT,
		pointid TEXT,
		ratestep TEXT,
		loctype TEXT,
		locname TEXT,
		rccounty TEXT,
		cbsa TEXT,
		cbsalevel TEXT,
		csa TEXT,
		cellmarket TEXT,
		mta TEXT,
		bta TEXT,
		udate DOUBLE,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(region,rcshort),
		FOREIGN KEY(region) REFERENCES lergst(ps)
	);
},
	},
	exdata=>{
		keys=>[ qw/exch/ ],
		cols=>[ qw/exch ex region exshort seeexch seeex seeregion ilecocn exlata rcv rch rclat rclon udate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS exdata (
		exch CHARACTER(6) PRIMARY KEY,
		ex TEXT,
		region CHARACTER(2),
		exshort VCHAR(10),
		seeexch CHARACTER(6),
		seeex TEXT,
		seeregion CHARACTER(2),
		ilecocn CHARACTER(4),
		exlata INTEGER,
		rcv INTEGER,
		rch INTEGER,
		rclat DOUBLE,
		rclon DOUBLE,
		udate DOUBLE,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		FOREIGN KEY(ilecocn) REFERENCES ocndata(ocn),
		FOREIGN KEY(region) REFERENCES lergst(ps),
		FOREIGN KEY(seeregion) REFERENCES lergst(ps)
	);
},
	},
	lcadata=>{
		keys=>[ qw/exchob exchib/ ],
		cols=>[ qw/exchob exchib plantype calltype monthlylimit note effdate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS lcadata (
		exchob CHARACTER(6),
		exchib CHARACTER(6),
		plantype VCHAR(3),
		calltype CHARACTER(1),
		monthlylimit TEXT,
		note TEXT,
		effdate INTEGER,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(exchob,exchib),
		FOREIGN KEY(exchob) REFERENCES exdata(exch),
		FOREIGN KEY(exchib) REFERENCES exdata(exch)
	);
},
	},
	wcdata=>{
		keys=>[ qw/wc/ ],
		cols=>[ qw/wc wcname wclata wcv wch wclat wclon wcaddr wczip wccity wccounty wcst wccc sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS wcdata (
		wc CHARACTER(8) PRIMARY KEY,
		wcname TEXT,
		wclata CHARACTER(5),
		wcv INTEGER,
		wch INTEGER,
		wclat DOUBLE,
		wclon DOUBLE,
		wcaddr TEXT,
		wczip VCHAR(10),
		wccity TEXT,
		wccounty TEXT,
		wcst CHARACTER(2),
		wccc CHARACTER(2),
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	switchdata=>{
		keys=>[ qw/switch/ ],
		cols=>[ qw/switch switchname switchtype switchdesc npa nxx swlata swoocn swocn swabbv swdesc swfunc wcv wch wclat wclon feat tgclli cls45sw e911sw spc apc stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv udate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS switchdata (
		switch CHARACTER(11) PRIMARY KEY,
		switchname TEXT,
		switchtype VCHAR(3),
		switchdesc TEXT,
		npa TEXT, -- served NPAs comma separated
		nxx TEXT, -- served NPA-NXXs comma separated
		swlata VCHAR(5), -- only the switch lata
		swoocn CHARACTER(4), -- only switch oocn
		swocn CHARACTER(4), -- only switch ocn
		swabbv TEXT,
		swdesc TEXT,
		swfunc TEXT,
		wcv INTEGER,
		wch INTEGER,
		wclat DOUBLE,
		wclon DOUBLE,
		feat TEXT,
		tgclli CHARACTER(11),
		cls45sw CHARACTER(11),
		e911sw CHARACTER(11),
		spc CHARACTER(11),
		apc CHARACTER(11),
		stp1 CHARACTER(11),
		stp2 CHARACTER(11),
		actual CHARACTER(11),
		agent CHARACTER(11),
		host CHARACTER(11),
		mate CHARACTER(11),
		tdm CHARACTER(11),
		tdmilt CHARACTER(11),
		tdmfgc CHARACTER(11),
		tdmfgd CHARACTER(11),
		tdmlcl CHARACTER(11),
		tdmfgb CHARACTER(11),
		tdmops CHARACTER(11),
		tdm911 CHARACTER(11),
		tdmchk CHARACTER(11),
		tdmblv CHARACTER(11),
		udate DOUBLE,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	prefixdata=>{
		keys=>[ qw/npa nxx/ ],
		cols=>[ qw/npa nxx nxxtype use wirelessblock portableblock tbpooling nbpooling contaminated retained overlays switch ocn carrier exch region rcshort rc lata loc st newnpa overlay assigndate effdate actdate discdate udate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS prefixdata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		nxxtype TEXT,
		use CHARACTER(1),
		wirelessblock BOOLEAN,
		portableblock BOOLEAN,
		tbpooling BOOLEAN,
		nbpooling TEXT,
		contaminated BOOLEAN,
		retained BOOLEAN,
		overlays TEXT,
		switch CHARACTER(11),
		ocn CHARACTER(4),
		carrier TEXT,
		exch CHARACTER(6),
		region CHARACTER(2),
		rcshort VCHAR(10),
		rc TEXT,
		lata VCHAR(5),
		loc TEXT,
		st CHARACTER(2),
		newnpa BOOLEAN,
		overlay BOOLEAN,
		assigndate INTEGER,
		effdate INTEGER,
		actdate INTEGER,
		discdate INTEGER,
		udate DOUBLE,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(npa,nxx)
		-- FOREIGN KEY(exch) REFERENCES exdata(exch),
		-- FOREIGN KEY(region) REFERENCES lergst(ps),
		-- FOREIGN KEY(switch) REFERENCES switchdata(switch),
		-- FOREIGN KEY(ocn) REFERENCES ocndata(ocn),
		-- FOREIGN KEY(region,rcshort) REFERENCES rcdata(region,rcshort)
	);
},
	},
	pooldata=>{
		keys=>[ qw/npa nxx x/ ],
		cols=>[ qw/npa nxx x switch ocn udate sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS pooldata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		x CHARACTER(1),
		switch CHARACTER(11),
		ocn CHARACTER(4),
		udate DOUBLE,
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(npa,nxx,x),
		FOREIGN KEY(npa,nxx) REFERENCES prefixdata(npa,nxx),
		FOREIGN KEY(switch) REFERENCES switchdata(switch),
		FOREIGN KEY(ocn) REFERENCES ocndata(ocn)
	);
},
	},
	linedata=>{
		keys=>[ qw/npa nxx xxxx yyyy/ ],
		cols=>[ qw/npa nxx xxxx yyyy switch ocn sect fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS linedata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		xxxx CHARACTER(4),
		yyyy CHARACTER(4),
		switch CHARACTER(11),
		ocn CHARACTER(4),
		sect TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(npa,nxx,xxxx,yyyy),
		FOREIGN KEY(npa,nxx) REFERENCES prefixdata(npa,nxx),
		FOREIGN KEY(switch) REFERENCES switchdata(switch),
		FOREIGN KEY(ocn) REFERENCES ocndata(ocn)
	);
},
	},
);

my %sth = ();

sub getinsert {
	my ($schema,$tab,$dbh) = @_;
	my @bind = (); foreach (@{$schema->{$tab}{cols}}) { push @bind, '?' }
	my $sql = qq{
	INSERT OR REPLACE INTO $tab (
		}.join(',',@{$schema->{$tab}{cols}}).q{
	) VALUES (
		}.join(',',@bind).q{
	);
};
	print STDERR "S: $sql";
	my $sth = $dbh->prepare($sql) or die $dbh->errstr;
	return $sth;
}

sub getupdate {
	my ($schema,$tab,$dbh) = @_;
	my @bind = (); foreach (@{$schema->{$tab}{cols}}) { push @bind, "$_=?" }
	my @indx = (); foreach (@{$schema->{$tab}{keys}}) { push @indx, "$_=?" }
	my $sql = qq{
	UPDATE OR REPLACE $tab SET
		}.join(',',@bind).q{
	WHERE }.join(' AND ',@indx).q{;
};
	print STDERR "S: $sql";
	my $sth = $dbh->prepare($sql) or die $dbh->errstr;
	return $sth;
} 

sub getselect {
	my ($schema,$tab,$dbh) = @_;
	my @indx = (); foreach (@{$schema->{$tab}{keys}}) { push @indx, "$_=?" }
	my $sql = q{
	SELECT
		}.join(',',@{$schema->{$tab}{cols}}).qq{
	FROM $tab WHERE
		}.join(' AND ',@indx).q{;
};
	print STDERR "S: $sql";
	my $sth = $dbh->prepare($sql) or die $dbh->errstr;
	return $sth;
}

sub insertit {
	my ($schema,$dat,$tab) = @_;
	my @values = ();
	foreach my $k (@{$schema->{$tab}{cols}}) {
		push @values, $dat->{$k};
	}
	unless ($schema->{$tab}{insert}->execute(@values)) {
		print STDERR "E: $tab: '",join("','",@values),"'\n";
	}
}

sub modifyit {
	my ($schema,$dat,$tab,$binds) = @_;
	my @values = ();
	foreach my $k (@{$schema->{$tab}{cols}}) {
		push @values, $dat->{$k};
	}
	unless ($schema->{$tab}{update}->execute(@values,@{$binds})) {
		print STDERR "E: $tab: '",join("','",@values),"'\n";
	}
}

sub addrnorm {
	my $val = shift;
	$val =~ s/[\.,;]//g;
	$val =~ s/-/ /g;
	$val =~ s/\s{2,}/ /g;
	$val =~ s/street/st/ig;
	$val =~ s/avenue/ave/ig;
	$val = "\U$val\E";
	return $val;
}

sub cmpfields {
	my ($k,$dat,$rec) = @_;
	if ($k eq 'wcaddr') {
		return (addrnorm($dat) ne addrnorm($rec));
	} elsif ($k =~ /^(wccity|wccounty)$/) {
		return ("\U$dat\E" ne "\U$rec\E");
	} elsif ($k =~ /^(wcv|wch|wclat|wclon|rcv|rch|rclat|rclon)$/) {
		return $dat != $rec;
	} else {
		return ($dat ne $rec);
	}
}

my $of = \*OUTFILE;

sub updateit {
	my ($schema,$data,$tab,$binds) = @_;
	unless ($schema->{$tab}{select}->execute(@{$binds})) {
		warn $schema->{$tab}{select}->errstr;
		return;
	}
	if (my $row = $schema->{$tab}{select}->fetchrow_hashref) {
		my %dat = %$data;
		my %rec = %$row;
		my @new = ();
		my @old = ();
		my $modified = undef;
		my $overwrite = ($rec{fdate} and $rec{fdate} > $dat{fdate});
		my $conflict = ($rec{fdate} and $dat{fdate} and $rec{fdate} == $dat{fdate});
		foreach my $k (@{$schema->{$tab}{cols}}) {
			if (length($rec{$k})) {
				if ($k =~ /wcv|wch|rcv|rch/) {
					$rec{$k} = sprintf('%05d',$rec{$k}) if $rec{$k};
				} elsif ($k =~ /^(wclat|wclon|rclat|rclon)$/) {
					$rec{$k} = sprintf('%.8f',$rec{$k}) if $rec{$k};
				} elsif ($k eq 'feat') {
					my %feats = ();
					foreach my $f (split(/[,;\/ ]\s*/,$dat{$k}),split(/[,;\/ ]\s*/,$rec{$k})) {
						$feats{$f}++ if $f;
					}
					if ($dat{$k}) {
						foreach my $f (keys %feats) {
							$modified = 1 unless $feats{$f} == 2;
						}
					}
					$dat{$k} = $rec{$k} = join(' ',sort keys %feats);
				} elsif ($k =~ /^(npa|nxx|wcst|sect|swfunc)$/) {
					my %sects = ();
					foreach my $s (split(/[,;\/ ]\s*/,$dat{$k}),split(/[,;\/ ]\s*/,$rec{$k})) {
						$sects{$s}++ if $s;
					}
					if ($dat{$k}) {
						foreach my $s (keys %sects) {
							$modified = 1 unless $sects{$s} == 2;
						}
					}
					$dat{$k} = $rec{$k} = join(',',sort keys %sects);
				}
				if ($overwrite or not length($dat{$k})) {
					$dat{$k} = $rec{$k};
				} elsif ($conflict) {
					if (cmpfields($k,$dat{$k},$rec{$k})) {
						if ($k eq 'udate') {
							$dat{$k} = $rec{$k} if $rec{$k} > $dat{$k};
						} else {
							#pick one, generate sql to change it back
							push @old, "$k='$rec{$k}'";
							push @new, "$k='$dat{$k}'";
						}
					} else {
						$dat{$k} = $rec{$k};
					}
				}
			} else {
				delete $dat{$k} if $overwrite;
			}
			$modified = 1 if ($dat{$k} ne $rec{$k});
		}
		if (@new or @old) {
			my @bounds = ();
			foreach my $k (@{$schema->{$tab}{keys}}) { push @bounds,"$k=='$dat{$k}'"; }
			my $sql = "UPDATE $tab SET ".join(',',@old)." WHERE ".join(' AND ',@bounds)."; -- FIXME: (A) pick one! ($dat{sect})\n";
			print STDERR "S: $sql";
			print $of $sql;
			my $sql = "UPDATE $tab SET ".join(',',@new)." WHERE ".join(' AND ',@bounds)."; -- FIXME: (B) pick one! ($dat{sect})\n";
			print STDERR "S: $sql";
			print $of $sql;
		}
		modifyit($schema,\%dat,$tab,$binds) if $modified;
	} else {
		insertit($schema,$data,$tab);
	}
}

sub maketables {
	my ($schema,$dbh) = @_;
	foreach my $tab (@{$schema->{tables}}) {
		my $sql = $schema->{$tab}{tsql};
		next unless defined $sql;
		print STDERR "S: $sql";
		$dbh->do($sql) or die $dbh->errstr;
		$schema->{$tab}{insert} = getinsert($schema,$tab,$dbh);
		$schema->{$tab}{update} = getupdate($schema,$tab,$dbh);
		$schema->{$tab}{select} = getselect($schema,$tab,$dbh);
	}
}

my %ccst2rg = ();
my %ccst2pc = ();
my %ccst2ps = ();
my %pcps2rg = ();

sub dolergst {
	my $dbh = shift;
	my $fh = \*INFILE;
	$dbh->begin_work;
	my $fn = "../lergst.txt";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	while (<$fh>) { chomp;
		next unless /^[A-Z][A-Z]\t/;
		my @tokens = split(/\t/,$_);
		$ccst2rg{$tokens[0]}{$tokens[1]} = $tokens[2];
		$ccst2pc{$tokens[0]}{$tokens[1]} = $tokens[4] if $tokens[4] ne 'ZZ';
		$ccst2ps{$tokens[0]}{$tokens[1]} = $tokens[5] if $tokens[5] ne 'XX';
		$pcps2rg{$tokens[4]}{$tokens[5]} = $tokens[2] if $tokens[2] ne 'XX' and $tokens[2] ne 'ZZ';
		next if $tokens[3] eq 'XX';
		unless ($schema{lergst}{insert}->execute(split(/\t/,$_))) {
			print STDERR "E: lergst: '",join("','",@tokens),"'\n";
		}
	}
	close($fh);
	$dbh->commit;
}

sub makedb {
	my ($dn,$callback) = @_;
	my $fn = "$dn.sqlite";
	print STDERR "I: connecting to database $fn\n";
	my $dbh = DBI->connect("dbi:SQLite:$fn") or die "can't open $fn";
	my $sql = qq{PRAGMA foreign_keys = OFF;\n};
	print STDERR "S: $sql";
	$dbh->do($sql) or die $dbh->errstr;
	maketables(\%schema,$dbh);
	$fn = 'db.bad.sql';
	print STDERR "I: writing $fn\n";
	open($of,">:utf8",$fn) or die "can't write $fn";
	dolergst($dbh);
	&{$callback}($dbh);
	close($of);
	$sql = qq{VACUUM;\n};
	print STDERR "S: $sql";
	print STDERR "I: disconnecting database\n";
	$dbh->disconnect;
	print STDERR "I: database disconnected\n";
}

sub simplefield {
	my ($dat,$fld,$val) = @_;
	$fld =~ s/\?//g; $fld =~ s/\s+//g;
	$val =~ s/^\s+//; $val =~ s/\s+$//;
	$dat->{"\L$fld\E"} = $val if length($val);
}

sub datefield {
	my ($dat,$fld,$val) = @_;
	if (length($val)) {
		if (my $time = str2time($val)) {
			simplefield($dat,$fld,$time);
		} else {
			unless ($val eq '00/00/0000') {
				print STDERR "E: cannot grok time format '$val'\n";
			}
		}
	}
}

sub mappedfield {
	my ($dat,$fld,$val,$map) = @_;
	if (length($val)) {
		if (exists $map->{$val}) {
			simplefield($dat,$fld,$map->{$val});
		} else {
			print STDERR "E: no map for $fld:'$val'\n";
		}
	}
}

sub booleanfield {
	my ($dat,$fld,$val) = @_;
	mappedfield($dat,$fld,$val,{
			'No'=>0,
			'Yes'=>1,
			'NO'=>0,
			'YES'=>1,
			'N'=>0,
			'Y'=>1,
			'0'=>0,
			'1'=>1,
			'X'=>0,
		});
}

my %swfuncmap = (
	TDMILT=>'IAL', TDMFGB=>'FGB', TDMFGC=>'FGC', TDMFGD=>'FGD', TDMLCL=>'LCL', LOC=>'LCL',
	ILT=>'IAL', TDM=>'EAT', '800'=>'800', TDMLOC=>'LCL', TOPS=>'OPS', TDMOPS=>'OPS',
	TDM911=>'911', CHK=>'CHK', TDMCHK=>'CHK', CLS5=>'', ACS=>'EAT', BLVI=>'BLVI',
	'HOST-NO'=>'HOST', BLV=>'BLV', ITL=>'ILT', HUB=>'', ISP=>'', LINE=>'', ONLY=>'', SSP=>'',
	IAL=>'IAL', IAT=>'IAL', EAT=>'EAT', IXC=>'IXC', FGB=>'FGB', FGC=>'FGC', FGD=>'FGD',
	LCL=>'LCL', OPS=>'OPS', '911'=>'911', E911=>'E911', CLS45SW=>'CLS45SW', STP=>'STP',
	MATE=>'MATE', ACTUAL=>'ACTUAL', AGENT=>'AGENT', HOST=>'HOST', REMOTE=>'REMOTE',
	'SMART-REMOTE'=>'REMOTE', STANDALONE=>'', TANDEM=>'EAT',
);

my @swcllis = qw/cls45sw e911sw stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv/;
my @allcllis = (qw/switch tgclli/,@swcllis);

my %swfuncflg = (
	cls45sw=>'CLS45SW',
	e911sw=>'E911',
	stp1=>'STP',
	stp2=>'STP',
	actual=>'ACTUAL',
	agent=>'AGENT',
	host=>'HOST',
	mate=>'MATE,STP',
	tdm=>'TDM',
	tdmilt=>'IAL',
	tdmfgc=>'FGC',
	tdmfgd=>'FGD',
	tdmlcl=>'LCL',
	tdmfgb=>'FGB',
	tdmops=>'OPS',
	tdm911=>'911',
	tdmchk=>'CHK',
	tdmblv=>'BLVI',
);

my %swfuncoth = (
	cls45sw=>'CLS45SW',
	actual=>'POI',
	agent=>'TGW',
	host=>'REMOTE',
	mate=>'MATE,STP',
);

sub updatedata {
	my ($dat,$fdate,$sect) = @_;
	foreach my $p (qw/wc rc/) {
		if ($dat->{$p.'vh'}) {
			my ($nv,$nh) = split(/,/,$dat->{$p.'vh'});
			$dat->{$p.'v'} = $nv if $nv;
			$dat->{$p.'h'} = $nh if $nh;
		}
		if ($dat->{$p.'ll'}) {
			my ($na,$no) = split(/,/,$dat->{$p.'ll'});
			$dat->{$p.'lat'} = $na if $na;
			$dat->{$p.'lon'} = $no if $no;
		}
	}
	foreach my $f (qw/wcv wch rcv rch/) {
		$dat->{$f} = sprintf('%05d',$dat->{$f}) if $dat->{$f};
	}
	foreach my $f (qw/wclat wclon rclat rclon/) {
		$dat->{$f} = sprintf('%.8f',$dat->{$f}) if $dat->{$f};
	}
	$dat->{wc} = substr($dat->{switch},0,8) if $dat->{switch} and not $dat->{wc};
	if (exists $schema{ocndata}{keys}) {
		if ($dat->{ocn}) {
			updateit(\%schema,$dat,'ocndata',[$dat->{ocn}]);
		}
		if ($dat->{ilecocn}) {
			my $ilec = {
				ocn=>$dat->{ilecocn},
				companyname=>$dat->{ilecname},
				companytype=>'I',
				sect=>$sect,
				fdate=>$fdate,
			};
			updateit(\%schema,$ilec,'ocndata',[$ilec->{ocn}]);
		}
	}
	if (exists $schema{lcadata}{keys} and $dat->{exchib} and $dat->{exchob}) {
		updateit(\%schema,$dat,'lcadata',[$dat->{exchib},$dat->{exchob}]);
	}
	if (exists $schema{exdata}{keys} and $dat->{exch}) {
		updateit(\%schema,$dat,'exdata',[$dat->{exch}]);
	}
	if (exists $schema{rcdata}{keys}) {
		my $havem = 1;
		my $binds = [];
		foreach my $k (@{$schema{rcdata}{keys}}) {
			$havem = undef unless $dat->{$k};
			push @$binds, $dat->{$k};
		}
		updateit(\%schema,$dat,'rcdata',$binds) if $havem;
	}
	if (exists $schema{wcdata}{keys} and $dat->{wc}) {
		updateit(\%schema,$dat,'wcdata',[$dat->{wc}]);
	}
	if (exists $schema{switchdata}{keys} and $dat->{switch}) {
		my %data = %$dat;
		if (exists $data{feat} and ref $data{feat} eq 'HASH') {
			$data{feat} = join(' ',sort keys %{$data{feat}});
		}
		if (my $func = delete $data{swfunc}) {
			$data{swfunc} = {};
			foreach my $f (split(/[,;\/ ]\s*/,$func)) {
				my $g = $swfuncmap{$f};
				if (defined $g) {
					$f = $g;
				} else {
					print STDERR "E: no mapping for swfunc '$f'\n";
				}
				$data{swfunc}{$f}++ if $f;
			}
		}
		foreach my $f (@swcllis) {
			next unless $data{$f};
			my %rec = (sect=>$sect,fdate=>$fdate);
			$rec{switch} = $data{$f};
			$rec{swfunc} = $swfuncflg{$f};
			my $func = $swfuncoth{$f};
			$data{swfunc}{$func}++ if $func;
			updateit(\%schema,\%rec,'switchdata',[$rec{switch}]) if $rec{switch};
		}
		$data{swfunc} = join(',',sort keys %{$data{swfunc}}) if $data{swfunc} and ref $data{swfunc} eq 'HASH';
		updateit(\%schema,\%data,'switchdata',[$data{switch}]);
	}
	if (exists $schema{prefixdata}{keys} and $dat->{npa} and $dat->{nxx}) {
		my %data = %$dat;
		if (exists $schema{linedata}{keys} and length($data{xxxx}) and length($data{yyyy})) {
			updateit(\%schema,\%data,'linedata',[$data{npa},$data{nxx},$data{xxxx},$data{yyyy}]);
			delete $data{switch};
			delete $data{ocn};
			delete $data{udate};
			delete $data{fdate};
			updateit(\%schema,\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
			updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
		} elsif (exists $schema{pooldata}{keys} and length($data{x})) {
			updateit(\%schema,\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
			delete $data{switch};
			delete $data{ocn};
			delete $data{udate};
			delete $data{fdate};
			updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
		} else {
			updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
		}
	}
	if (exists $schema{pcdata}{keys} and $dat->{spc}) {
		my %data = %$dat;
		foreach my $s (qw/host cls45sw e911sw/) {
			$data{apc} = delete $data{spc} if $data{$s};
		}
		if ($data{switch}) {
			if ($data{switch} =~ /[0-9A-Z]ED$/) {
				$data{e911sw} = delete $data{switch};
			}
			if ($data{cls45sw}) {
				if ($data{switch} =~ /([0-9][0-9G]T|CT[0-9A-F])$/ and $data{cls45sw} !~ /([0-9][0-9G]T|CT[0-9A-F])$/) {
					($data{cls45sw},$data{switch}) = ($data{switch},$data{cls45sw});
				}
			} elsif ($data{switch} =~ /([0-9][0-9G]T|CT[0-9A-F])$/) {
				$data{cls45sw} = delete $data{switch};
			}
		}
		updateit(\%schema,\%data,'pcdata',[$data{spc}]) if $data{spc};
	}
}


