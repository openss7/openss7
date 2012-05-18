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

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

my ($dbh,%sth,$sql);

my $dn = 'nnxsdata';
$fn = "$dn.sqlite";
print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:$fn") or die "can't open $fn";

$sql = q{PRAGMA foreign_keys = OFF;
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

my %cols = (
	lergst=>    [ qw/cc st rg cs pc ps loc/ ],
	ocndata=>   [ qw/ocn ocntype companyname companytype overall neca trgt commonname carrier abbv dba fka oocn address name title org careof street city state zip country suite pobox pocity pocountry sect fdate/ ],
	pcdata=>    [ qw/spc apc switch host cls45sw e911sw stp1 stp2 mate sect fdate/ ],
	rcdata=>    [ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect fdate/ ],
#	exdata=>    [ qw/exch rc region rcshort seeexch seerc seeregion ilecocn lata rcv rch rclat rclon udate sect fdate/ ],
#	lcadata=>   [ qw/exchob exchib plantype calltype monthlylimit note effdate sect fdate/ ],
	wcdata=>    [ qw/wc wcname wclata wcv wch wclat wclon wcaddr wczip wccity wccounty wcst wccc sect fdate/ ],
	switchdata=>[ qw/switch switchname switchtype switchdesc npa nxx swlata swoocn swocn swabbv swdesc swfunc wcv wch wclat wclon feat tgclli cls45sw e911sw spc apc stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv udate sect fdate/ ],
	prefixdata=>[ qw/npa nxx nxxtype use wirelessblock portableblock tbpooling nbpooling contaminated retained overlays switch ocn carrier exch region rcshort rc lata loc st newnpa overlay assigndate effdate actdate discdate udate sect fdate/ ],
	pooldata=>  [ qw/npa nxx x switch ocn udate sect fdate/ ],
	linedata=>  [ qw/npa nxx xxxx yyyy switch ocn sect fdate/ ],
);

my %keys = (
	lergst=>    [ qw/rg/ ],
	ocndata=>   [ qw/ocn/ ],
	pcdata=>    [ qw/spc/ ],
	rcdata=>    [ qw/region rcshort/ ],
#	exdata=>    [ qw/exch/ ],
#	lcadata=>   [ qw/exchob exchib/ ],
	wcdata=>    [ qw/wc/ ],
	switchdata=>[ qw/switch/ ],
	prefixdata=>[ qw/npa nxx/ ],
	pooldata=>  [ qw/npa nxx x/ ],
	linedata=>  [ qw/npa nxx xxxx yyyy/ ],
);

if (exists $keys{lergst}) {
$sql = q{
	CREATE TABLE IF NOT EXISTS lergst (
		cc CHARACTER(2), -- iso alpha2 country code
		st CHARACTER(2), -- iso alpha2 state code
		rg CHARACTER(2) PRIMARY KEY, -- lerg state/prov/territory
		cs CHARACTER(2) UNIQUE, -- CLLI state/prov/territory
		pc CHARACTER(2), -- USPS country code
		ps CHARACTER(2), -- USPS state code
		loc TEXT
	);
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

$sql = q{CREATE UNIQUE INDEX IF NOT EXISTS ccst_idx ON lergst (cc ASC, st ASC);
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
$sql = q{CREATE UNIQUE INDEX IF NOT EXISTS pcps_idx ON lergst (pc ASC, ps ASC);
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}


if (exists $keys{nanpst}) {
$sql = q{
	CREATE TABLE IF NOT EXISTS nanpst (
		npa CHARACTER(3) PRIMARY KEY,
		cc CHARACTER(2),
		st CHARACTER(2),
		loc TEXT,
		FOREIGN KEY(cc,st) REFERENCES lergst(cc,st)
	);
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{ocndata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{pcdata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{rcdata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{exdata}) {
$sql = q{
	CREATE TABLE IF NOT EXISTS exdata (
		exch CHARACTER(6) PRIMARY KEY,
		rc TEXT,
		region CHARACTER(2),
		rcshort VCHAR(10),
		seeexch CHARACTER(6),
		seerc TEXT,
		seeregion CHARACTER(2),
		ilecocn CHARACTER(4),
		lata INTEGER,
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{lcadata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{wcdata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{switchdata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{prefixdata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{pooldata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

if (exists $keys{linedata}) {
$sql = q{
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
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;
}

sub getinsert {
	my $tab = shift;
	my @bind = (); foreach (@{$cols{$tab}}) { push @bind, '?' }
	my $sql = qq{
	INSERT OR REPLACE INTO $tab (
		}.join(',',@{$cols{$tab}}).q{
	) VALUES (
		}.join(',',@bind).q{
	);
};
	print STDERR "S: $sql";
	my $sth = $dbh->prepare($sql) or die $dbh->errstr;
	return $sth;
}

sub getupdate {
	my $tab = shift;
	my @bind = (); foreach (@{$cols{$tab}}) { push @bind, "$_=?" }
	my @indx = (); foreach (@{$keys{$tab}}) { push @indx, "$_=?" }
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
	my $tab = shift;
	my @indx = (); foreach (@{$keys{$tab}}) { push @indx, "$_=?" }
	my $sql = q{
	SELECT
		}.join(',',@{$cols{$tab}}).qq{
	FROM $tab WHERE
		}.join(' AND ',@indx).q{;
};
	print STDERR "S: $sql";
	my $sth = $dbh->prepare($sql) or die $dbh->errstr;
	return $sth;
}

foreach my $tab (keys %cols) {
	$sth{$tab}{insert} = getinsert($tab);
	$sth{$tab}{update} = getupdate($tab);
	$sth{$tab}{select} = getselect($tab);
}

sub insertit {
	my ($dat,$tab) = @_;
	my @values = ();
	foreach my $k (@{$cols{$tab}}) {
		push @values, $dat->{$k};
	}
	unless ($sth{$tab}{insert}->execute(@values)) {
		print STDERR "E: $tab: '",join("','",@values),"'\n";
	}
}

sub modifyit {
	my ($dat,$tab,$binds) = @_;
	my @values = ();
	foreach my $k (@{$cols{$tab}}) {
		push @values, $dat->{$k};
	}
	unless ($sth{$tab}{update}->execute(@values,@{$binds})) {
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

sub updateit {
	my ($data,$tab,$binds) = @_;
	unless ($sth{$tab}{select}->execute(@{$binds})) {
		warn $sth{$tab}{select}->errstr;
		return;
	}
	if (my $row = $sth{$tab}{select}->fetchrow_hashref) {
		my %dat = %$data;
		my %rec = %$row;
		my @new = ();
		my @old = ();
		my $modified = undef;
		my $overwrite = ($rec{fdate} and $rec{fdate} > $dat{fdate});
		my $conflict = ($rec{fdate} and $dat{fdate} and $rec{fdate} == $dat{fdate});
		foreach my $k (@{$cols{$tab}}) {
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
			foreach my $k (@{$keys{$tab}}) { push @bounds,"$k=='$dat{$k}'"; }
			my $sql = "UPDATE $tab SET ".join(',',@old)." WHERE ".join(' AND ',@bounds)."; -- FIXME: (A) pick one! ($dat{sect})\n";
			print STDERR "S: $sql";
			print $of $sql;
			my $sql = "UPDATE $tab SET ".join(',',@new)." WHERE ".join(' AND ',@bounds)."; -- FIXME: (B) pick one! ($dat{sect})\n";
			print STDERR "S: $sql";
			print $of $sql;
		}
		modifyit(\%dat,$tab,$binds) if $modified;
	} else {
		insertit($data,$tab);
	}
}

my %ccst2rg = ();
my %ccst2pc = ();
my %ccst2ps = ();
my %pcps2rg = ();

sub dolergst {
	$dbh->begin_work;
	$fn = "$progdir/../lergst.txt";
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
		unless ($sth{lergst}{insert}->execute(split(/\t/,$_))) {
			print STDERR "E: lergst: '",join("','",@tokens),"'\n";
		}
	}
	close($fh);
	$dbh->commit;
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
	if (exists $keys{ocndata}) {
		if ($dat->{ocn}) {
			updateit($dat,'ocndata',[$dat->{ocn}]);
		}
		if ($dat->{ilecocn}) {
			my $ilec = {
				ocn=>$dat->{ilecocn},
				companyname=>$dat->{ilecname},
				companytype=>'I',
				sect=>$sect,
				fdate=>$fdate,
			};
			updateit($ilec,'ocndata',[$ilec->{ocn}]);
		}
	}
	if (exists $keys{lcadata} and $dat->{exchib} and $dat->{exchob}) {
		updateit($dat,'lcadata',[$dat->{exchib},$dat->{exchob}]);
	}
	if (exists $keys{exdata} and $dat->{exch}) {
		updateit($dat,'exdata',[$dat->{exch}]);
	}
	if (exists $keys{rcdata}) {
		my $havem = 1;
		my $binds = [];
		foreach my $k (@{$keys{rcdata}}) {
			$havem = undef unless $dat->{$k};
			push @$binds, $dat->{$k};
		}
		updateit($dat,'rcdata',$binds) if $havem;
	}
	if (exists $keys{wcdata} and $dat->{wc}) {
		updateit($dat,'wcdata',[$dat->{wc}]);
	}
	if (exists $keys{switchdata} and $dat->{switch}) {
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
			updateit(\%rec,'switchdata',[$rec{switch}]) if $rec{switch};
		}
		$data{swfunc} = join(',',sort keys %{$data{swfunc}}) if $data{swfunc} and ref $data{swfunc} eq 'HASH';
		updateit(\%data,'switchdata',[$data{switch}]);
	}
	if (exists $keys{prefixdata} and $dat->{npa} and $dat->{nxx}) {
		my %data = %$dat;
		if (exists $keys{linedata} and length($data{xxxx}) and length($data{yyyy})) {
			updateit(\%data,'linedata',[$data{npa},$data{nxx},$data{xxxx},$data{yyyy}]);
			delete $data{switch};
			delete $data{ocn};
			delete $data{udate};
			delete $data{fdate};
			updateit(\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
			updateit(\%data,'prefixdata',[$data{npa},$data{nxx}]);
		} elsif (exists $keys{pooldata} and length($data{x})) {
			updateit(\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
			delete $data{switch};
			delete $data{ocn};
			delete $data{udate};
			delete $data{fdate};
			updateit(\%data,'prefixdata',[$data{npa},$data{nxx}]);
		} else {
			updateit(\%data,'prefixdata',[$data{npa},$data{nxx}]);
		}
	}
	if (exists $keys{pcdata} and $dat->{spc}) {
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
		updateit(\%data,'pcdata',[$data{spc}]) if $data{spc};
	}
}

sub dodata {
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
			'NPA NXX Record'=>\&simplefield,
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
			'Wireless Block'=>\&booleanfield,
			'NXX Type'=>\&simplefield,
			'Portable Block'=>\&booleanfield,
			'1,000 Block Pooling'=>sub{
				my ($dat,$fld,$val) = @_;
				booleanfield($dat,'tbpooling',$val);
			},
			'Block Contaminated'=>sub{
				my ($dat,$fld,$val) = @_;
				booleanfield($dat,'contaminated',$val);
			},
			'Block Retained'=>sub{
				my ($dat,$fld,$val) = @_;
				booleanfield($dat,'retained',$val);
			},
			'Date Assigned'=>sub{
				my ($dat,$fld,$val) = @_;
				datefield($dat,'assigndate',$val);
			},
			'Effective Date'=>sub{
				my ($dat,$fld,$val) = @_;
				datefield($dat,'effdate',$val);
			},
			'Date Activated'=>sub{
				my ($dat,$fld,$val) = @_;
				datefield($dat,'actdate',$val);
			},
			'Last Modified'=>sub{
				my ($dat,$fld,$val) = @_;
				datefield($dat,'udate',$val);
			},
		},
		'Carrier'=>{
			'Common Name'=>\&simplefield,
			'OCN'=>\&simplefield,
			'OCN Type'=>\&simplefield,
			'Name'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'carrier',$val);
			},
			'Abbreviation'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'abbv',$val);
			},
			'DBA'=>\&simplefield,
			'FKA'=>\&simplefield,
			'Address'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'addr',$val);
			},
			'City'=>\&simplefield,
			'State'=>\&simplefield,
			'Zip'=>\&simplefield,
			'Country'=>\&simplefield,
		},
		'Wire Center'=>{
			'LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wclata',$val);
			},
			'Other switches in WC'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'other',$val);
			},
			'Address'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wcaddr',$val);
			},
			'County'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wccounty',$val);
			},
			'City'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wccity',$val);
			},
			'Operator Services'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'tdmops',$val);
			},
			'Class 4/5 Switch'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'cls45sw',$val);
			},
			'State'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wcst',$val);
			},
			'Trunk Gateway'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'tgclli',$val);
			},
			'Point Code'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'spc',$val);
			},
			'Switch type'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'switchtype',$val);
			},
			'Tandems'=>{
				'IntraLATA'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmilt',$val);
				},
				'Feature Group C'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmfgc',$val);
				},
				'Feature Group D'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmfgd',$val);
				},
				'Local'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmlcl',$val);
				},
				'Feature Group B'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmfgb',$val);
				},
				'Operator Services'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'tdmops',$val);
				},
			},
			'Zip'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wczip',$val);
			},
			'SS7 STP 1'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'stp1',$val);
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
				simplefield($dat,'wc',$val);
			},
			'Switch'=>\&simplefield,
			'SS7 STP 2'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'stp2',$val);
			},
			'Actual Switch'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'actual',$val);
			},
			'Country'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wccc',$val);
			},
			'Call Agent'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'agent',$val);
			},
			'Host'=>\&simplefield,
			'Wire Center Name'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'wcname',$val);
			},
		},
		'LATA'=>{
			'LATA'=>\&simplefield,
			'Name'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'ltname',$val);
			},
			'Historical Region'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'lthist',$val);
			},
			'Area Codes in LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'ltnpa',$val);
			},
			'Remark'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'ltremark',$val);
			},
		},
		'Rate Center - Locality'=>{
			'LATA'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rclata',$val);
			},
			'Rate Center Name'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rc',$val);
			},
			'State'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rcst',$val);
			},
			'Country'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rccc',$val);
				$dat->{region} = $pcps2rg{$dat->{rccc}}{$dat->{rcst}} if exists $pcps2rg{$dat->{rccc}}{$dat->{rcst}};
			},
			'LERG Abbreviation'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rcshort',$val);
			},
			'Zone'=>\&simplefield,
			'Zone Type'=>\&simplefield,
			'Number Pooling'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'nbpooling',$val);
			},
			'Point Identifier'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'pointid',$val);
			}, # Canada only
			'Rate Step'=>\&simplefield,
			'Area Codes in Rate Center'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rcnpa',$val);
			},
			'Embedded Overlays'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'overlays',$val);
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
				simplefield($dat,'tzone',$val);
			},
			'DST Recognized'=>sub{
				my ($dat,$fld,$val) = @_;
				booleanfield($dat,'dst',$val);
			},
			'Location within Rate Center'=>{
				'Type'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'loctype',$val);
				},
				'Name'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'locname',$val);
				},
			},
			'County or Equivalent'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rccounty',$val);
			},
			'Federal Feature ID'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'fedfeatid',$val);
			},
			'FIPS County Code'=>\&simplefield,
			'FIPS Place Code'=>\&simplefield,
			'Land Area'=>\&simplefield,
			'Water Area'=>\&simplefield,
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
				simplefield($dat,'pop2000',$val);
			},
			'Population 2009'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'pop2009',$val);
			},
			'Rural-Urban Continuum Code'=>sub{
				my ($dat,$fld,$val) = @_;
				simplefield($dat,'rucc',$val);
			},
			'Core Based Statistical Area'=>{
				'CBSA'=>\&simplefield,
				'CBSA Level'=>\&simplefield,
				'County Relation to CBSA'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'cbsarel',$val);
				},
				'Metro Division'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'metrodiv',$val);
				},
				'Part of CSA'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'csa',$val);
				},
			},
			'Market Area'=>{
				'Cellular Market'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'cellmarket',$val);
				},
				'Major Trading Area'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'mta',$val);
				},
				'Basic Trading Area'=>sub{
					my ($dat,$fld,$val) = @_;
					simplefield($dat,'bta',$val);
				},
			},
		},
	);
	print STDERR "I: locating files...\n";
	my @files = `find $datadir -name '*.html.xz' | sort`;
	foreach $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open($fh,"xzcat $fn |") or die "can't process $fn";
		my $fdate = stat($fn)->mtime;
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.html\.xz//;
		my $entry='';
		while (<$fh>) { chomp; s/\r//g; chomp;
			$entry .= $_;
		}
		close($fh);
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
		updatedata(\%data,$fdate,$sect);
	}
	$dbh->commit;
}

$fn = "db.bad.sql";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";

dolergst;
dodata;

close($of);

$sql = q{VACUUM;
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

undef %sth;
print STDERR "I: disconnecting database\n";
$dbh->disconnect;
print STDERR "I: database disconnected\n";

exit;

1;

__END__
