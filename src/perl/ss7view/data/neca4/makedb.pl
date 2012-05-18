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

my $dn = 'necadata';
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
#	pcdata=>    [ qw/spc apc switch host cls45sw e911sw stp1 stp2 mate sect fdate/ ],
#	rcdata=>    [ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect fdate/ ],
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
#	pcdata=>    [ qw/spc/ ],
#	rcdata=>    [ qw/region rcshort/ ],
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

sub doocndata {
	$dbh->begin_work;
	$fn = "$datadir/NECA-4-1.pdf";
	print STDERR "I: processing $fn\n";
	open($fh,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	binmode($fh,':utf8');
	my $start = 0;
	my $frag = '';
	my $fdate = stat($fn)->mtime;
	while (<$fh>) { chomp;
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
			updatedata(\%data,$fdate);
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
			updatedata(\%data,$fdate);
			%data = (fdate=>$fdate);
		} elsif (/^([0-9][0-9][0-9][A-Z0-9] +)?([0-9][0-9][0-9][A-Z0-9])(\s+([YN])\s+([A-Z]+))?$/) {
			$frag = $_;
		} else {
			warn "cannot parse line '$_'";
		}
	}
	close($fh);
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
	$dbh->begin_work;
	$fn = "$datadir/NECA-4-1.pdf";
	print STDERR "I: processing $fn\n";
	open($fh,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
	binmode($fh,':utf8');
	my $fdate = stat($fn)->mtime;
	my @lines1 = (); my $wasblank1 = 1;
	my @lines2 = (); my $wasblank2 = 1;
	my @holdln = ();
	my $start = 0;
	my $holding = 0;
	while (<$fh>) { chomp;
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
	close($fh);
	my %data = (fdate=>$fdate);
	my $lno = 0;
	my $continue = 0;
	foreach (@lines1,'',@lines2,'') {
		#printf STDERR "line %d: %s\n", $lno, $_;
		if (/^$/) {
			if ($lno) {
				updatedata(\%data,$fdate);
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
				updatedata($ocn,$fdate);
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
	updatedata($dat,$fdate,$sect);
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
			updatedata(\%data,$fdate,$sect);
		}
	}
}

sub donecadata {
	$dbh->begin_work;
	foreach my $fname (qw/NECA-4-12.pdf NECA-4-44.pdf NECA-4-400.pdf/) {
		$fn = "$datadir/$fname";
		print STDERR "I: processing $fn\n";
		open($fh,"pdftotext -layout -nopgbrk $fn - |") or die "can't process $fn";
		binmode($fh,':utf8');
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
		while (<$fh>) { chomp; $flineno++;
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
		close($fh);

	}
	$dbh->commit;
}

sub dodata {
	doocndata;
	docontactdata;
	donecadata;
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
