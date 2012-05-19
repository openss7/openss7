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

my $dn = 'veridata';
$fn = "$dn.sqlite";
print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:$fn") or die "can't open $fn";

$sql = q{PRAGMA foreign_keys = OFF;
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

my %cols = (
	lergst=>    [ qw/cc st rg cs pc ps loc/ ],
#	ocndata=>   [ qw/ocn ocntype companyname companytype overall neca trgt commonname carrier abbv dba fka oocn address name title org careof street city state zip country suite pobox pocity pocountry sect fdate/ ],
	pcdata=>    [ qw/spc apc switch host cls45sw e911sw stp1 stp2 mate sect fdate/ ],
#	rcdata=>    [ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect fdate/ ],
#	exdata=>    [ qw/exch rc region rcshort seeexch seerc seeregion ilecocn lata rcv rch rclat rclon udate sect fdate/ ],
#	lcadata=>   [ qw/exchob exchib plantype calltype monthlylimit note effdate sect fdate/ ],
	wcdata=>    [ qw/wc wcname wclata wcv wch wclat wclon wcaddr wczip wccity wccounty wcst wccc sect fdate/ ],
	switchdata=>[ qw/switch switchname switchtype switchdesc npa nxx swlata swoocn swocn swabbv swdesc swfunc wcv wch wclat wclon feat tgclli cls45sw e911sw spc apc stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv udate sect fdate/ ],
#	prefixdata=>[ qw/npa nxx nxxtype use wirelessblock portableblock tbpooling nbpooling contaminated retained overlays switch ocn carrier exch region rcshort rc lata loc st newnpa overlay assigndate effdate actdate discdate udate sect fdate/ ],
#	pooldata=>  [ qw/npa nxx x switch ocn udate sect fdate/ ],
#	linedata=>  [ qw/npa nxx xxxx yyyy switch ocn sect fdate/ ],
);

my %keys = (
	lergst=>    [ qw/rg/ ],
#	ocndata=>   [ qw/ocn/ ],
	pcdata=>    [ qw/spc/ ],
#	rcdata=>    [ qw/region rcshort/ ],
#	exdata=>    [ qw/exch/ ],
#	lcadata=>   [ qw/exchob exchib/ ],
	wcdata=>    [ qw/wc/ ],
	switchdata=>[ qw/switch/ ],
#	prefixdata=>[ qw/npa nxx/ ],
#	pooldata=>  [ qw/npa nxx x/ ],
#	linedata=>  [ qw/npa nxx xxxx yyyy/ ],
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
	} elsif ($k =~ /^(wccity|wccounty|switchname)$/) {
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
	'SMART-REMOTE'=>'REMOTE', STANDALONE=>'', TANDEM=>'EAT', 'LIDB'=>'LIDB', 'SCP'=>'SCP',
	'TGW'=>'TGW',
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

sub dodatacsv {
	$dbh->begin_work;
	$fn = "$progdir/data.csv";
	print STDERR "I: processing $fn\n";
	open($fh,"<:utf8",$fn) or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $header = 1;
	my @fields = ();
	my $lineno = 0;
	while (<$fh>) { chomp; $lineno++;
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
		my $sect = "XLS-$lineno";
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
		$data{swocn}	    = delete $data{ocn} if $data{ocn};
		$data{swoocn}	    = delete $data{oocn} if $data{oocn};
		$data{swlata}	    = delete $data{lata} if $data{lata};
		my $f = 'switch';
		unless (not $data{$f} or length($data{$f}) == 11) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		my $f = 'wc';
		unless (length($data{$f}) == 8) {
			print STDERR "E: bad value at $lineno in field $f '$data{$f}'\n";
			next;
		}
		for $f (@allcllis) {
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
		updatedata(\%data,$fdate,$sect);
	}
	close($fh);
	$dbh->commit;
}

sub doe911data {
	$dbh->begin_work;
	$fn = "$progdir/E911_ActGuide_March2007_X.pdf";
	print STDERR "I: processing $fn\n";
	open($fh,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $sect = 'E911';
	while (<$fh>) { chomp;
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
			updatedata(\%data,$fdate,$sect);
		}
	}
	close($fh);
	$dbh->commit;
}

sub dopchangedata {
	$dbh->begin_work;
	$fn = "$progdir/ND10-0003.pdf";
	print STDERR "I: processing $fn\n";
	open($fh,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $sect = 'ND10';
	while (<$fh>) { chomp;
		if (/^([A-Z]{2})\s+(.*?)\s+([A-Z0-9]{11})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})\s+([0-9]{3}-[0-9]{3}-[0-9]{3})/) {
			my %data = (sect=>$sect,fdate=>$fdate);
			$data{st} = $1;
			$data{switchname} = $2;
			$data{switch} = $3;
			$data{oldspc} = $4;
			$data{spc} = $5;
			updatedata(\%data,$fdate,$sect);
		}
	}
	close($fh);
	$dbh->commit;
}

sub dodata {
	dodatacsv;
	dopchangedata;
	doe911data;
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
