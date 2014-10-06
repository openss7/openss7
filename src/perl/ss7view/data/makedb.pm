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
	tables=>[ qw/lergst nanpst ocndata pcdata rcdata rndata exdata lcadata wcdata switchdata npadata prefixdata pooldata linedata/ ],
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
		cols=>[ qw/ocn ocntype companyname companytype overall neca trgt commonname carrier abbv dba fka oocn address name title org careof street city state zip country suite pobox pocity pocountry sect cref fdate/ ],
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
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	pcdata=>{
		keys=>[ qw/spc/ ],
		cols=>[ qw/spc apc switch host cls45sw e911sw stp1 stp2 mate sect cref fdate/ ],
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
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	rcdata=>{
		keys=>[ qw/region rcshort/ ],
		cols=>[ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect cref fdate/ ],
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
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(region,rcshort),
		FOREIGN KEY(region) REFERENCES lergst(ps)
	);
},
	},
	rndata=>{
		keys=>[ qw/region rc/ ],
		cols=>[ qw/region rcshort rclata rc rcst rccc rccity rcv rch rclat rclon tzone dst zone zonetype pointid ratestep loctype locname rccounty cbsa cbsalevel csa cellmarket mta bta udate sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS rndata (
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
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(region,rc),
		FOREIGN KEY(region) REFERENCES lergst(ps)
	);
},
	},
	exdata=>{
		keys=>[ qw/exch/ ],
		cols=>[ qw/exch ex region exshort seeexch seeex seeregion ilecocn exlata rcv rch rclat rclon udate sect cref fdate/ ],
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
		cref TEXT,
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
		cols=>[ qw/exchob exchib plantype calltype monthlylimit note effdate sect cref fdate/ ],
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
		cref TEXT,
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
		cols=>[ qw/wc wcname wclata wcv wch wclat wclon wcaddr wczip wccity wccounty wcst wccc sect cref fdate/ ],
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
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
		gsql=>[
			q{
	CREATE TRIGGER IF NOT EXISTS wcdata_wcvh_ins AFTER INSERT ON wcdata FOR EACH ROW WHEN NEW.wcv NOT NULL and NEW.wch NOT NULL
	BEGIN
		UPDATE switchdata SET wcv=NEW.wcv,wch=NEW.wch WHERE switch LIKE (NEW.wc||'%') and wcv IS NULL and wch IS NULL;
	END
},
			q{
	CREATE TRIGGER IF NOT EXISTS wcdata_wcvh_upd AFTER UPDATE OF wcv,wch ON wcdata FOR EACH ROW WHEN NEW.wcv NOT NULL and NEW.wch NOT NULL
	BEGIN
		UPDATE switchdata SET wcv=NEW.wcv,wch=NEW.wch WHERE switch LIKE (OLD.wc||'%') and wcv IS NULL and wch IS NULL;
	END
},
		],
	},
	switchdata=>{
		keys=>[ qw/switch/ ],
		cols=>[ qw/switch switchname switchtype switchdesc swlata swoocn swocn swabbv swdesc swfunc wcv wch wclat wclon feat tgclli cls45sw e911sw spc apc stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv tdmpvt repl idate ddate udate sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS switchdata (
		switch CHARACTER(11) PRIMARY KEY,
		switchname TEXT,
		switchtype VCHAR(3),
		switchdesc TEXT,
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
		tdmpvt CHARACTER(11),
		repl CHARACTER(11),
		idate DOUBLE,
		ddate DOUBLE,
		udate DOUBLE,
		sect TEXT,
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
#		gsql=>[
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_insert AFTER INSERT ON switchdata FOR EACH ROW
#	BEGIN
#		INSERT OR IGNORE INTO wcdata (wc,wcv,wch) VALUES(substr(NEW.switch,1,8),NEW.wcv,NEW.wch);
#		UPDATE switchdata SET wcv=(SELECT wcv FROM wcdata WHERE wc==substr(NEW.switch,1,8)),
#		                      wch=(SELECT wch FROM wcdata WHERE wc==substr(NEW.switch,1,8))
#			WHERE switch==NEW.switch and wcv IS NULL and wch IS NULL;
#	END
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_func AFTER UPDATE ON switchdata FOR EACH ROW WHEN OLD.swfunc NOT NULL
#	BEGIN
#		UPDATE switchdata SET swfunc=(OLD.swfunc||','||NEW.swfunc) WHERE switch==OLD.switch
#			and OLD.swfunc NOT LIKE ('%'||NEW.swfunc||'%');
#		UPDATE switchdata SET swfunc= OLD.swfunc                   WHERE switch==OLD.switch
#			and OLD.swfunc     LIKE ('%'||NEW.swfunc||'%');
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tgw AFTER UPDATE OF switch,tgclli ON switchdata FOR EACH ROW WHEN NEW.tgclli NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tgclli,'TGW');
#		UPDATE switchdata SET swfunc='TGW' WHERE switch==NEW.tgclli or switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_cls45 AFTER UPDATE OF switch,cls45sw ON switchdata FOR EACH ROW WHEN NEW.cls45sw NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc,cls45sw) VALUES(NEW.cls45sw,'CLS45SW',OLD.switch);
#		UPDATE switchdata SET swfunc='CLS45SW',cls45sw=OLD.switch WHERE switch==NEW.cls45sw;
#		UPDATE switchdata SET swfunc='CLS45SW' WHERE switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_e911 AFTER UPDATE OF switch,e911sw ON switchdata FOR EACH ROW WHEN NEW.e911sw NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.e911sw,'E911');
#		UPDATE switchdata SET swfunc='E911' WHERE switch==NEW.e911sw or switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_stp AFTER UPDATE OF stp1,stp2 ON switchdata FOR EACH ROW WHEN NEW.stp1 NOT NULL or NEW.stp2 NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc,mate) VALUES(NEW.stp1,'STP,MATE',NEW.stp2);
#		INSERT OR IGNORE INTO switchdata (switch,swfunc,mate) VALUES(NEW.stp2,'STP,MATE',NEW.stp1);
#		UPDATE switchdata SET swfunc='STP' WHERE switch==NEW.stp1 or switch==NEW.stp2;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_actual AFTER UPDATE OF switch,actual ON switchdata FOR EACH ROW WHEN NEW.actual NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.actual,'ACTUAL');
#		UPDATE switchdata SET swfunc='ACTUAL' WHERE switch==NEW.actual;
#		UPDATE switchdata SET swfunc='POI'    WHERE switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_agent AFTER UPDATE OF switch,agent ON switchdata FOR EACH ROW WHEN NEW.agent NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.agent,'AGENT');
#		UPDATE switchdata SET swfunc='AGENT' WHERE switch==NEW.agent;
#		UPDATE switchdata SET swfunc='MGW'   WHERE switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_host AFTER UPDATE OF switch,host ON switchdata FOR EACH ROW WHEN NEW.host NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.host,'HOST');
#		UPDATE switchdata SET swfunc='HOST'   WHERE switch==NEW.host;
#		UPDATE switchdata SET swfunc='REMOTE' WHERE switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_mate AFTER UPDATE OF switch,mate ON switchdata FOR EACH ROW WHEN NEW.mate NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc,mate) VALUES(NEW.mate,'MATE',OLD.switch);
#		UPDATE switchdata SET swfunc='MATE',mate=OLD.switch WHERE switch==NEW.mate;
#		UPDATE switchdata SET swfunc='MATE' WHERE switch==OLD.switch;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdm AFTER UPDATE OF tdm ON switchdata FOR EACH ROW WHEN NEW.tdm NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdm,'EAT');
#		UPDATE switchdata SET swfunc='EAT' WHERE switch==NEW.tdm;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmilt AFTER UPDATE OF tdmilt ON switchdata FOR EACH ROW WHEN NEW.tdmilt NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmilt,'IAL');
#		UPDATE switchdata SET swfunc='IAL' WHERE switch==NEW.tdmilt;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmfgc AFTER UPDATE OF tdmfgc ON switchdata FOR EACH ROW WHEN NEW.tdmfgc NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmfgc,'FGC');
#		UPDATE switchdata SET swfunc='FGC' WHERE switch==NEW.tdmfgc;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmfgd AFTER UPDATE OF tdmfgd ON switchdata FOR EACH ROW WHEN NEW.tdmfgd NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmfgd,'FGD');
#		UPDATE switchdata SET swfunc='FGD' WHERE switch==NEW.tdmfgd;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmlcl AFTER UPDATE OF tdmlcl ON switchdata FOR EACH ROW WHEN NEW.tdmlcl NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmlcl,'LCL');
#		UPDATE switchdata SET swfunc='LCL' WHERE switch==NEW.tdmlcl;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmfgb AFTER UPDATE OF tdmfgb ON switchdata FOR EACH ROW WHEN NEW.tdmfgb NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmfgb,'FGB');
#		UPDATE switchdata SET swfunc='FGB' WHERE switch==NEW.tdmfgb;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmops AFTER UPDATE OF tdmops ON switchdata FOR EACH ROW WHEN NEW.tdmops NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmops,'OPS');
#		UPDATE switchdata SET swfunc='OPS' WHERE switch==NEW.tdmops;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdm911 AFTER UPDATE OF tdm911 ON switchdata FOR EACH ROW WHEN NEW.tdm911 NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdm911,'911');
#		UPDATE switchdata SET swfunc='911' WHERE switch==NEW.tdm911;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmchk AFTER UPDATE OF tdmchk ON switchdata FOR EACH ROW WHEN NEW.tdmchk NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmchk,'CHK');
#		UPDATE switchdata SET swfunc='CHK' WHERE switch==NEW.tdmchk;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmblv AFTER UPDATE OF tdmblv ON switchdata FOR EACH ROW WHEN NEW.tdmblv NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmblv,'BLVI');
#		UPDATE switchdata SET swfunc='BLVI' WHERE switch==NEW.tdmblv;
#	END;
#},
#			q{
#	CREATE TRIGGER IF NOT EXISTS switchdata_tdmpvt AFTER UPDATE OF tdmpvt ON switchdata FOR EACH ROW WHEN NEW.tdmpvt NOT NULL
#	BEGIN
#		INSERT OR IGNORE INTO switchdata (switch,swfunc) VALUES(NEW.tdmpvt,'PVT');
#		UPDATE switchdata SET swfunc='PVT' WHERE switch==NEW.tdmpvt;
#	END;
#},
#		],
	},
	npadata=>{
		keys=>[ qw/npa/ ],
		cols=>[ qw/npa type assignable explanation reserved assigned assigndate use service location country inservice effdate status pl overlay complex parent tzones map jeoparady relief hnpalocl hnpatoll fnpalocl fnpatoll permhnpalocl permhnpatoll permfnpalocl notes sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS npadata (
		npa CHARACTER(3) PRIMARY KEY,
		type CHARACTER(1), -- (G|E)
		assignable BOOLEAN,
		explanation VCHAR(3),
		reserved BOOLEAN,
		assigned BOOLEAN,
		assigndate INTEGER,
		use CHARACTER(1), -- (G|N)
		service CHARACTER(3),
		location CHARACTER(2), -- postal state
		country CHARACTER(2), -- postal country
		inservice BOOLEAN,
		effdate INTEGER,
		status CHARACTER(1), -- (A|S)
		pl TEXT, -- planning letter
		overlay BOOLEAN,
		complex TEXT,
		parent CHARACTER(3),
		tzones VCHAR(10),
		map TEXT, -- NANPA map
		jeoparady BOOLEAN,
		relief BOOLEAN,
		hnpalocl VCHAR(10),
		hnpatoll VCHAR(10),
		fnpalocl VCHAR(10),
		fnpatoll VCHAR(10),
		permhnpalocl VCHAR(10),
		permhnpatoll VCHAR(10),
		permfnpalocl VCHAR(10),
		notes TEXT,
		sect TEXT,
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN
	);
},
	},
	prefixdata=>{
		keys=>[ qw/npa nxx/ ],
		cols=>[ qw/npa nxx nxxtype status nxxuse initialgrowth use wirelessblock portableblock tbpooling nbpooling contaminated retained overlays switch ocn carrier exch region rcshort rc lata loc st newnpa overlay discon special remarks assigndate effdate actdate discdate udate sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS prefixdata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		nxxtype TEXT,
		status CHARACTER(2),
		nxxuse CHARACTER(2),
		initialgrowth CHARACTER(1),
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
		discon BOOLEAN,
		special BOOLEAN,
		remarks TEXT,
		assigndate INTEGER,
		effdate INTEGER,
		actdate INTEGER,
		discdate INTEGER,
		udate DOUBLE,
		sect TEXT,
		cref TEXT,
		fdate DOUBLE,
		updating BOOLEAN,
		PRIMARY KEY(npa,nxx)
		-- FOREIGN KEY(exch) REFERENCES exdata(exch),
		-- FOREIGN KEY(region) REFERENCES lergst(ps),
		-- FOREIGN KEY(switch) REFERENCES switchdata(switch),
		-- FOREIGN KEY(ocn) REFERENCES ocndata(ocn)
	);
},
	},
	pooldata=>{
		keys=>[ qw/npa nxx x/ ],
		cols=>[ qw/npa nxx x switch ocn udate sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS pooldata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		x CHARACTER(1),
		switch CHARACTER(11),
		ocn CHARACTER(4),
		udate DOUBLE,
		sect TEXT,
		cref TEXT,
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
		cols=>[ qw/npa nxx xxxx yyyy switch ocn sect cref fdate/ ],
		tsql=>q{
	CREATE TABLE IF NOT EXISTS linedata (
		npa CHARACTER(3),
		nxx CHARACTER(3),
		xxxx CHARACTER(4),
		yyyy CHARACTER(4),
		switch CHARACTER(11),
		ocn CHARACTER(4),
		sect TEXT,
		cref TEXT,
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

sub getschema {
	return \%schema;
}

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
	my @written = ();
	foreach my $k (@{$schema->{$tab}{cols}}) {
		push @values, $dat->{$k};
		push @written,$k if length($dat->{$k});
	}
	unless ($schema->{$tab}{insert}->execute(@values)) {
		print STDERR "E: $tab: '",join("','",@values),"'\n";
	}
	return \@written;
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
	my $modified = [];
	if (my $row = $schema->{$tab}{select}->fetchrow_hashref) {
		my %dat = %$data;
		my %rec = %$row;
		my @new = ();
		my @old = ();
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
							push @$modified,$k unless $feats{$f} == 2;
						}
					}
					$dat{$k} = $rec{$k} = join(' ',sort keys %feats);
				} elsif ($k =~ /^(npa|nxx|wcst|sect|cref|swfunc)$/) {
					my %sects = ();
					foreach my $s (split(/[,;\/ ]\s*/,$dat{$k}),split(/[,;\/ ]\s*/,$rec{$k})) {
						$sects{$s}++ if $s;
					}
					if ($dat{$k}) {
						foreach my $s (keys %sects) {
							push @$modified,$k unless $sects{$s} == 2;
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
			push @$modified,$k if ($dat{$k} ne $rec{$k});
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
		$schema->{$tab}{select}->finish;
		modifyit($schema,\%dat,$tab,$binds) if @$modified;
	} else {
		$schema->{$tab}{select}->finish;
		$modified = insertit($schema,$data,$tab);
	}
	return $modified;
}

sub maketables {
	my ($schema,$dbh) = @_;
	foreach my $tab (@{$schema->{tables}}) {
		my $sql = $schema->{$tab}{tsql};
		next unless defined $sql;
		print STDERR "S: $sql";
		$dbh->do($sql) or die $dbh->errstr;
	}
	foreach my $tab (@{$schema->{tables}}) {
		next unless exists $schema->{$tab}{tsql};
		if (exists $schema->{$tab}{gsql} and ref $schema->{$tab}{gsql} eq 'ARRAY') {
			foreach my $sql (@{$schema->{$tab}{gsql}}) {
				print STDERR "S: $sql";
				$dbh->do($sql) or die $dbh->errstr;
			}
		}
	}
	foreach my $tab (@{$schema->{tables}}) {
		next unless exists $schema->{$tab}{tsql};
		$schema->{$tab}{insert} = getinsert($schema,$tab,$dbh);
		$schema->{$tab}{update} = getupdate($schema,$tab,$dbh);
		$schema->{$tab}{select} = getselect($schema,$tab,$dbh);
	}
}

sub destroysth {
	my ($schema,$dbh) = @_;
	foreach my $tab (@{$schema->{tables}}) {
		delete $schema->{$tab}{insert};
		delete $schema->{$tab}{update};
		delete $schema->{$tab}{select};
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
	$fn = "lergst.txt" unless -f $fn;
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

my %npapc = ();
my %npaps = ();
my %nparg = ();

sub donanpst {
	my $dbh = shift;
	$dbh->begin_work;
	my $fh = \*INFILE;
	my $fn = "../nanpa/nanpst.txt";
	$fn = "nanpa/nanpst.txt" unless -f $fn;
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	while (<$fh>) { chomp;
		my @tokens = split(/\t/,$_);
		unless ($tokens[0] =~ /[2-9]11/) {
			if ($tokens[2]) {
				$nparg{$tokens[0]} = $ccst2rg{$tokens[1]}{$tokens[2]};
				$npapc{$tokens[0]} = $ccst2pc{$tokens[1]}{$tokens[2]};
				$npaps{$tokens[0]} = $ccst2ps{$tokens[1]}{$tokens[2]};
			} elsif ($tokens[1]) {
				$npapc{$tokens[0]} = $tokens[1];
			}
		}
		unless ($schema{nanpst}{insert}->execute(split(/\t/,$_))) {
			print STDERR "E: nanpst: '",join("','",@tokens),"'\n";
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
	donanpst($dbh);
	&{$callback}($dbh);
	close($of);
	$sql = qq{VACUUM;\n};
	print STDERR "S: $sql";
	destroysth(\%schema,$dbh);
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
	'SMART-REMOTE'=>'REMOTE', STANDALONE=>'', TANDEM=>'EAT', SCP=>'SCP', '8XX'=>'8XX',
	LNP=>'LNP', CNAM=>'CNAM', LIDB=>'LIDB',
);

my %swcllis = (
	'tgclli'=>1,
	'cls45sw'=>1,
	'e911sw'=>1,
	'stp1'=>1,
	'stp2'=>1,
	'actual'=>1,
	'agent'=>1,
	'host'=>1,
	'mate'=>1,
	'tdm'=>1,
	'tdmilt'=>1,
	'tdmfgc'=>1,
	'tdmfgd'=>1,
	'tdmlcl'=>1,
	'tdmfgb'=>1,
	'tdmops'=>1,
	'tdm911'=>1,
	'tdmchk'=>1,
	'tdmblv'=>1,
	'tdmpvt'=>1,
	'repl'=>1,
);
my @allcllis = ('switch',keys %swcllis);

my %swfuncflg = (
	tgclli=>'TGW',
	cls45sw=>'CLS45SW',
	e911sw=>'E911',
	stp1=>'STP',
	stp2=>'STP',
	actual=>'ACTUAL',
	agent=>'AGENT',
	host=>'HOST',
	mate=>'MATE,STP',
	tdm=>'EAT',
	tdmilt=>'IAL',
	tdmfgc=>'FGC',
	tdmfgd=>'FGD',
	tdmlcl=>'LCL',
	tdmfgb=>'FGB',
	tdmops=>'OPS',
	tdm911=>'911',
	tdmchk=>'CHK',
	tdmblv=>'BLVI',
	tdmpvt=>'PVT',
);

my %swfuncoth = (
	cls45sw=>'CLS45SW',
	e911sw=>'E911',
	actual=>'POI',
	agent=>'MGW',
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
	if (exists $schema{rndata}{keys}) {
		my $havem = 1;
		my $binds = [];
		foreach my $k (@{$schema{rndata}{keys}}) {
			$havem = undef unless $dat->{$k};
			push @$binds, $dat->{$k};
		}
		updateit(\%schema,$dat,'rndata',$binds) if $havem;
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
		foreach my $f (keys %swcllis) { $data{swfunc}{$swfuncoth{$f}}++ if $swfuncoth{$f} and $data{$f}; }
		$data{swfunc} = join(',',sort keys %{$data{swfunc}}) if $data{swfunc} and ref $data{swfunc} eq 'HASH';
		my $fields = updateit(\%schema,\%data,'switchdata',[$data{switch}]);
		if (@$fields) {
			if (exists $schema{wcdata}{keys}) {
				foreach my $c (@$fields) {
					if ($c eq 'switch' or $c eq 'wcv' or $c eq 'wch') {
						$data{wc} = substr($data{switch},0,8) unless $data{wc};
						updateit(\%schema,\%data,'wcdata',[$data{wc}]);
						last;
					}
				}
			}
			foreach my $f (@$fields) {
				if ($swcllis{$f}) {
					my %rec = (
						switch=>$data{$f},
						swfunc=>$swfuncflg{$f},
						sect=>$sect,
						fdate=>$fdate,
					);
					my $result = updateit(\%schema,\%rec,'switchdata',[$rec{switch}]) if $rec{switch};
					if (@$result) {
						if (exists $schema{wcdata}{keys}) {
							foreach my $c (@$result) {
								if ($c eq 'switch' or $c eq 'wcv' or $c eq 'wch') {
									$rec{wc} = substr($rec{switch},0,8) unless $rec{wc};
									updateit(\%schema,\%rec,'wcdata',[$rec{wc}]);
									last;
								}
							}
						}
					}
				}
			}
		}
	} elsif (exists $schema{wcdata}{keys} and $dat->{wc}) {
		updateit(\%schema,$dat,'wcdata',[$dat->{wc}]);
	}
	if (exists $schema{npadata}{keys} and $dat->{npa}) {
		my %data = %$dat;
		if (exists $schema{prefixdata}{keys} and $dat->{npa} and $dat->{nxx}) {
			if (exists $schema{linedata}{keys} and length($data{xxxx}) and length($data{yyyy})) {
				my $result = updateit(\%schema,\%data,'linedata',[$data{npa},$data{nxx},$data{xxxx},$data{yyyy}]);
				if (@$result) {
					delete $data{switch};
					delete $data{ocn};
					delete $data{udate};
					delete $data{fdate};
					$result = updateit(\%schema,\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
					if (@$result) {
						$result = updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
						if (@$result) {
							delete $data{use};
							delete $data{status};
							delete $data{assigndate};
							delete $data{effdate};
							delete $data{overlay};
							updateit(\%schema,\%data,'npadata',[$data{npa}]);
						}
					}
				}
			} elsif (exists $schema{pooldata}{keys} and length($data{x})) {
				my $result = updateit(\%schema,\%data,'pooldata',[$data{npa},$data{nxx},$data{x}]);
				if (@$result) {
					delete $data{switch};
					delete $data{ocn};
					delete $data{udate};
					delete $data{fdate};
					$result = updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
					if (@$result) {
						delete $data{use};
						delete $data{status};
						delete $data{assigndate};
						delete $data{effdate};
						delete $data{overlay};
						updateit(\%schema,\%data,'npadata',[$data{npa}]);
					}
				}
			} else {
				my $result = updateit(\%schema,\%data,'prefixdata',[$data{npa},$data{nxx}]);
				if (@$result) {
					delete $data{use};
					delete $data{status};
					delete $data{assigndate};
					delete $data{effdate};
					delete $data{overlay};
					updateit(\%schema,\%data,'npadata',[$data{npa}]);
				}
			}
		} else {
			updateit(\%schema,\%data,'npadata',[$data{npa}]);
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


