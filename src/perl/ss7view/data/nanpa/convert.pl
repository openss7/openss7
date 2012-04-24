#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";
my $geondir = "$progdir/../geonames";
my $npandir = "$progdir/../npanxxsource";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Geo::Coordinates::VandH;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %fcount = ();

my @keys = (
	'NPA',
	'NXX',
	'X',
	'OCN',
	'Company',
	'Status',
	'RateCenter',
	'Remarks',
	'State',
	'EffectiveDate',
	'Use',
	'AssignDate',
	'Initial/Growth',
);

my @ocn_keys = (
	'OCN',
	'Company',
);

my %ocns = ();

my @rc_keys = (
	'RCSHORT',
	'REGION',
	'RCGEOID',
	'RCVH',
	'RCLL',
	'NPA',
	'NXX',
	'RCCC',
	'RCST',
	'RCNAME',
	'RCGN',
);

my @rn_keys = (
	'RCCC',
	'RCST',
	'RCNAME',
	'RCGN',
	'RCGEOID',
	'RCVH',
	'RCLL',
	'NPA',
	'NXX',
	'REGION',
	'RCSHORT',
);

my %nsxst = (
	'202'=>'NS',
	'203'=>'NS',
	'204'=>'NS',
	'205'=>'NS',
	'206'=>'PE',
	'207'=>'NS',
	'208'=>'PE',
	'209'=>'NS',
	'210'=>'NS',
	'211'=>'NS',
	'212'=>'NS',
	'213'=>'PE',
	'214'=>'PE',
	'215'=>'PE',
	'216'=>'NS',
	'217'=>'NS',
	'218'=>'PE',
	'219'=>'NS',
	'220'=>'NS',
	'221'=>'NS',
	'222'=>'NS',
	'223'=>'NS',
	'224'=>'NS',
	'225'=>'NS',
	'226'=>'NS',
	'227'=>'NS',
	'228'=>'NS',
	'229'=>'NS',
	'230'=>'NS',
	'231'=>'PE',
	'232'=>'NS',
	'233'=>'NS',
	'234'=>'NS',
	'235'=>'NS',
	'236'=>'NS',
	'237'=>'NS',
	'238'=>'NS',
	'239'=>'NS',
	'240'=>'NS',
	'241'=>'NS',
	'242'=>'NS',
	'243'=>'NS',
	'244'=>'NS',
	'245'=>'NS',
	'246'=>'NS',
	'247'=>'NS',
	'248'=>'NS',
	'249'=>'NS',
	'250'=>'NS',
	'251'=>'NS',
	'252'=>'NS',
	'253'=>'NS',
	'254'=>'NS',
	'255'=>'NS',
	'256'=>'NS',
	'257'=>'NS',
	'258'=>'NS',
	'259'=>'NS',
	'260'=>'NS',
	'261'=>'NS',
	'262'=>'NS',
	'264'=>'NS',
	'265'=>'NS',
	'266'=>'NS',
	'268'=>'NS',
	'269'=>'NS',
	'270'=>'NS',
	'271'=>'NS',
	'272'=>'NS',
	'273'=>'NS',
	'274'=>'NS',
	'275'=>'NS',
	'276'=>'NS',
	'277'=>'NS',
	'278'=>'NS',
	'279'=>'NS',
	'280'=>'NS',
	'281'=>'NS',
	'282'=>'NS',
	'283'=>'PE',
	'284'=>'NS',
	'285'=>'NS',
	'286'=>'NS',
	'287'=>'NS',
	'288'=>'PE',
	'289'=>'NS',
	'290'=>'PE',
	'291'=>'PE',
	'292'=>'NS',
	'293'=>'NS',
	'294'=>'NS',
	'295'=>'NS',
	'296'=>'NS',
	'297'=>'NS',
	'298'=>'NS',
	'299'=>'NS',
	'300'=>'NS',
	'301'=>'NS',
	'302'=>'NS',
	'303'=>'PE',
	'304'=>'NS',
	'305'=>'NS',
	'306'=>'NS',
	'307'=>'NS',
	'308'=>'NS',
	'309'=>'NS',
	'310'=>'NS',
	'311'=>'NS',
	'312'=>'NS',
	'313'=>'PE',
	'314'=>'PE',
	'315'=>'PE',
	'316'=>'PE',
	'317'=>'NS',
	'318'=>'NS',
	'319'=>'NS',
	'320'=>'NS',
	'321'=>'NS',
	'322'=>'NS',
	'323'=>'NS',
	'324'=>'NS',
	'325'=>'NS',
	'326'=>'PE',
	'327'=>'PE',
	'328'=>'NS',
	'329'=>'NS',
	'330'=>'PE',
	'331'=>'NS',
	'332'=>'NS',
	'333'=>'NS',
	'334'=>'NS',
	'335'=>'NS',
	'336'=>'NS',
	'337'=>'NS',
	'338'=>'NS',
	'340'=>'NS',
	'341'=>'NS',
	'342'=>'NS',
	'343'=>'NS',
	'344'=>'NS',
	'345'=>'NS',
	'346'=>'NS',
	'347'=>'NS',
	'348'=>'NS',
	'349'=>'NS',
	'350'=>'NS',
	'351'=>'NS',
	'352'=>'NS',
	'354'=>'NS',
	'355'=>'PE',
	'356'=>'NS',
	'357'=>'PE',
	'358'=>'NS',
	'361'=>'PE',
	'362'=>'NS',
	'363'=>'NS',
	'364'=>'NS',
	'365'=>'NS',
	'366'=>'NS',
	'367'=>'PE',
	'368'=>'PE',
	'369'=>'NS',
	'370'=>'PE',
	'371'=>'NS',
	'372'=>'PE',
	'373'=>'NS',
	'374'=>'PE',
	'375'=>'NS',
	'376'=>'NS',
	'377'=>'NS',
	'378'=>'NS',
	'379'=>'NS',
	'380'=>'PE',
	'381'=>'PE',
	'382'=>'NS',
	'383'=>'NS',
	'384'=>'NS',
	'385'=>'NS',
	'386'=>'NS',
	'387'=>'NS',
	'388'=>'PE',
	'389'=>'NS',
	'390'=>'NS',
	'392'=>'NS',
	'393'=>'PE',
	'394'=>'PE',
	'396'=>'NS',
	'401'=>'NS',
	'402'=>'NS',
	'403'=>'NS',
	'404'=>'NS',
	'405'=>'NS',
	'406'=>'NS',
	'407'=>'NS',
	'411'=>'NS',
	'412'=>'NS',
	'414'=>'NS',
	'420'=>'NS',
	'421'=>'NS',
	'422'=>'NS',
	'423'=>'NS',
	'424'=>'NS',
	'425'=>'NS',
	'426'=>'NS',
	'427'=>'NS',
	'428'=>'NS',
	'429'=>'NS',
	'430'=>'NS',
	'431'=>'NS',
	'432'=>'PE',
	'433'=>'NS',
	'434'=>'NS',
	'435'=>'NS',
	'436'=>'PE',
	'437'=>'PE',
	'438'=>'PE',
	'439'=>'PE',
	'440'=>'NS',
	'441'=>'NS',
	'442'=>'NS',
	'443'=>'NS',
	'444'=>'NS',
	'445'=>'NS',
	'446'=>'NS',
	'447'=>'NS',
	'448'=>'NS',
	'449'=>'NS',
	'450'=>'NS',
	'451'=>'NS',
	'452'=>'NS',
	'453'=>'NS',
	'454'=>'NS',
	'455'=>'NS',
	'456'=>'NS',
	'457'=>'NS',
	'458'=>'NS',
	'459'=>'NS',
	'460'=>'NS',
	'461'=>'NS',
	'462'=>'NS',
	'463'=>'NS',
	'464'=>'NS',
	'465'=>'NS',
	'466'=>'NS',
	'467'=>'NS',
	'468'=>'NS',
	'469'=>'NS',
	'470'=>'NS',
	'471'=>'NS',
	'472'=>'NS',
	'473'=>'NS',
	'474'=>'NS',
	'475'=>'NS',
	'476'=>'NS',
	'477'=>'NS',
	'478'=>'NS',
	'479'=>'NS',
	'480'=>'NS',
	'481'=>'NS',
	'482'=>'NS',
	'483'=>'NS',
	'484'=>'NS',
	'485'=>'NS',
	'486'=>'NS',
	'487'=>'NS',
	'488'=>'NS',
	'489'=>'NS',
	'490'=>'NS',
	'491'=>'NS',
	'492'=>'NS',
	'493'=>'NS',
	'494'=>'NS',
	'495'=>'NS',
	'496'=>'NS',
	'497'=>'NS',
	'498'=>'NS',
	'499'=>'NS',
	'511'=>'NS',
	'512'=>'NS',
	'514'=>'NS',
	'515'=>'NS',
	'521'=>'NS',
	'522'=>'NS',
	'523'=>'NS',
	'524'=>'NS',
	'525'=>'NS',
	'526'=>'NS',
	'527'=>'NS',
	'528'=>'NS',
	'529'=>'NS',
	'530'=>'NS',
	'531'=>'NS',
	'532'=>'NS',
	'533'=>'NS',
	'534'=>'NS',
	'535'=>'NS',
	'536'=>'NS',
	'537'=>'NS',
	'538'=>'NS',
	'539'=>'NS',
	'541'=>'NS',
	'542'=>'NS',
	'543'=>'NS',
	'544'=>'NS',
	'545'=>'NS',
	'546'=>'NS',
	'547'=>'NS',
	'548'=>'NS',
	'549'=>'NS',
	'551'=>'NS',
	'552'=>'NS',
	'553'=>'NS',
	'555'=>'NS',
	'556'=>'PE',
	'557'=>'PE',
	'558'=>'NS',
	'559'=>'NS',
	'560'=>'NS',
	'561'=>'NS',
	'562'=>'NS',
	'563'=>'NS',
	'564'=>'NS',
	'565'=>'NS',
	'566'=>'PE',
	'567'=>'NS',
	'568'=>'NS',
	'569'=>'PE',
	'572'=>'NS',
	'574'=>'NS',
	'575'=>'NS',
	'576'=>'NS',
	'577'=>'NS',
	'578'=>'NS',
	'579'=>'NS',
	'580'=>'NS',
	'581'=>'NS',
	'582'=>'NS',
	'583'=>'PE',
	'584'=>'NS',
	'585'=>'NS',
	'586'=>'NS',
	'587'=>'NS',
	'588'=>'NS',
	'592'=>'NS',
	'594'=>'NS',
	'595'=>'NS',
	'597'=>'NS',
	'598'=>'PE',
	'611'=>'NS',
	'614'=>'NS',
	'616'=>'NS',
	'618'=>'NS',
	'619'=>'NS',
	'620'=>'PE',
	'621'=>'PE',
	'622'=>'NS',
	'623'=>'NS',
	'624'=>'NS',
	'625'=>'NS',
	'626'=>'PE',
	'627'=>'NS',
	'628'=>'PE',
	'629'=>'PE',
	'630'=>'NS',
	'631'=>'NS',
	'632'=>'NS',
	'633'=>'NS',
	'634'=>'NS',
	'635'=>'NS',
	'636'=>'NS',
	'637'=>'NS',
	'638'=>'NS',
	'639'=>'NS',
	'640'=>'NS',
	'641'=>'NS',
	'642'=>'NS',
	'643'=>'NS',
	'644'=>'NS',
	'645'=>'NS',
	'646'=>'NS',
	'647'=>'NS',
	'648'=>'NS',
	'649'=>'NS',
	'650'=>'NS',
	'651'=>'PE',
	'652'=>'PE',
	'653'=>'NS',
	'654'=>'NS',
	'655'=>'NS',
	'656'=>'NS',
	'657'=>'NS',
	'658'=>'PE',
	'659'=>'PE',
	'660'=>'NS',
	'661'=>'NS',
	'662'=>'NS',
	'663'=>'NS',
	'664'=>'NS',
	'665'=>'NS',
	'667'=>'NS',
	'668'=>'NS',
	'669'=>'NS',
	'670'=>'NS',
	'671'=>'NS',
	'672'=>'PE',
	'673'=>'NS',
	'674'=>'NS',
	'675'=>'PE',
	'676'=>'PE',
	'677'=>'NS',
	'678'=>'NS',
	'679'=>'NS',
	'680'=>'NS',
	'681'=>'NS',
	'682'=>'NS',
	'683'=>'NS',
	'684'=>'NS',
	'685'=>'NS',
	'686'=>'NS',
	'687'=>'PE',
	'688'=>'NS',
	'689'=>'NS',
	'690'=>'NS',
	'691'=>'NS',
	'692'=>'NS',
	'693'=>'NS',
	'694'=>'NS',
	'695'=>'NS',
	'697'=>'NS',
	'698'=>'NS',
	'699'=>'NS',
	'700'=>'NS',
	'701'=>'NS',
	'711'=>'NS',
	'714'=>'NS',
	'718'=>'NS',
	'719'=>'NS',
	'720'=>'NS',
	'721'=>'NS',
	'722'=>'NS',
	'723'=>'NS',
	'724'=>'PE',
	'725'=>'NS',
	'726'=>'PE',
	'727'=>'NS',
	'728'=>'NS',
	'729'=>'PE',
	'730'=>'PE',
	'731'=>'PE',
	'732'=>'NS',
	'733'=>'NS',
	'734'=>'PE',
	'735'=>'NS',
	'736'=>'NS',
	'737'=>'NS',
	'738'=>'NS',
	'739'=>'PE',
	'740'=>'NS',
	'741'=>'PE',
	'742'=>'NS',
	'743'=>'PE',
	'744'=>'NS',
	'745'=>'NS',
	'746'=>'NS',
	'747'=>'NS',
	'748'=>'NS',
	'749'=>'NS',
	'750'=>'NS',
	'751'=>'NS',
	'752'=>'NS',
	'753'=>'NS',
	'754'=>'NS',
	'755'=>'NS',
	'756'=>'NS',
	'757'=>'NS',
	'758'=>'NS',
	'759'=>'NS',
	'760'=>'NS',
	'761'=>'NS',
	'762'=>'NS',
	'763'=>'NS',
	'764'=>'NS',
	'765'=>'NS',
	'766'=>'NS',
	'767'=>'NS',
	'768'=>'NS',
	'769'=>'NS',
	'770'=>'NS',
	'771'=>'NS',
	'772'=>'NS',
	'773'=>'NS',
	'775'=>'PE',
	'776'=>'NS',
	'777'=>'NS',
	'778'=>'NS',
	'779'=>'NS',
	'783'=>'NS',
	'784'=>'PE',
	'786'=>'PE',
	'787'=>'NS',
	'788'=>'NS',
	'789'=>'NS',
	'790'=>'NS',
	'791'=>'NS',
	'792'=>'NS',
	'794'=>'NS',
	'795'=>'PE',
	'796'=>'NS',
	'798'=>'NS',
	'799'=>'NS',
	'800'=>'NS',
	'801'=>'NS',
	'802'=>'NS',
	'803'=>'NS',
	'804'=>'NS',
	'805'=>'NS',
	'806'=>'PE',
	'807'=>'PE',
	'808'=>'PE',
	'811'=>'NS',
	'812'=>'PE',
	'813'=>'PE',
	'814'=>'NS',
	'815'=>'NS',
	'816'=>'PE',
	'817'=>'NS',
	'818'=>'NS',
	'819'=>'NS',
	'820'=>'NS',
	'821'=>'NS',
	'822'=>'NS',
	'823'=>'NS',
	'824'=>'NS',
	'825'=>'NS',
	'826'=>'NS',
	'827'=>'NS',
	'828'=>'NS',
	'829'=>'NS',
	'830'=>'NS',
	'831'=>'PE',
	'832'=>'NS',
	'833'=>'NS',
	'834'=>'NS',
	'835'=>'NS',
	'836'=>'PE',
	'837'=>'NS',
	'838'=>'PE',
	'839'=>'NS',
	'840'=>'NS',
	'841'=>'NS',
	'842'=>'NS',
	'843'=>'NS',
	'844'=>'NS',
	'845'=>'NS',
	'846'=>'PE',
	'847'=>'NS',
	'848'=>'NS',
	'849'=>'NS',
	'850'=>'NS',
	'852'=>'NS',
	'853'=>'PE',
	'854'=>'PE',
	'855'=>'PE',
	'856'=>'PE',
	'857'=>'NS',
	'858'=>'NS',
	'859'=>'PE',
	'860'=>'NS',
	'861'=>'NS',
	'862'=>'NS',
	'863'=>'NS',
	'864'=>'NS',
	'865'=>'NS',
	'866'=>'NS',
	'867'=>'NS',
	'868'=>'NS',
	'869'=>'NS',
	'870'=>'NS',
	'871'=>'NS',
	'872'=>'NS',
	'873'=>'NS',
	'874'=>'NS',
	'875'=>'NS',
	'876'=>'NS',
	'877'=>'NS',
	'878'=>'NS',
	'879'=>'NS',
	'880'=>'NS',
	'881'=>'NS',
	'882'=>'PE',
	'883'=>'NS',
	'884'=>'NS',
	'885'=>'NS',
	'886'=>'PE',
	'887'=>'PE',
	'888'=>'PE',
	'889'=>'NS',
	'890'=>'NS',
	'891'=>'NS',
	'892'=>'PE',
	'893'=>'NS',
	'894'=>'PE',
	'895'=>'NS',
	'896'=>'NS',
	'897'=>'NS',
	'898'=>'NS',
	'899'=>'NS',
	'911'=>'NS',
	'921'=>'NS',
	'922'=>'NS',
	'923'=>'NS',
	'924'=>'NS',
	'925'=>'NS',
	'926'=>'NS',
	'928'=>'NS',
	'929'=>'NS',
	'931'=>'NS',
	'935'=>'NS',
	'940'=>'PE',
	'945'=>'NS',
	'947'=>'NS',
	'951'=>'NS',
	'952'=>'NS',
	'953'=>'NS',
	'954'=>'PE',
	'956'=>'NS',
	'957'=>'NS',
	'958'=>'NB',
	'960'=>'PE',
	'961'=>'PE',
	'962'=>'PE',
	'963'=>'PE',
	'964'=>'PE',
	'965'=>'NS',
	'968'=>'NS',
	'969'=>'PE',
	'971'=>'NS',
	'977'=>'NS',
	'978'=>'PE',
	'979'=>'NS',
	'980'=>'NS',
	'981'=>'NS',
	'982'=>'NS',
	'986'=>'NS',
	'989'=>'NS',
	'997'=>'NS',
	'998'=>'NS',
	'999'=>'NS',
);

my %nxxst = (
	'206'=>'NT',
	'211'=>'YT',
	'222'=>'NU',
	'252'=>'NU',
	'266'=>'NU',
	'311'=>'YT',
	'322'=>'YT',
	'332'=>'YT',
	'333'=>'YT',
	'334'=>'YT',
	'335'=>'YT',
	'336'=>'YT',
	'360'=>'NU',
	'370'=>'NT',
	'371'=>'NT',
	'390'=>'YT',
	'392'=>'NT',
	'393'=>'YT',
	'394'=>'NT',
	'396'=>'NT',
	'399'=>'YT',
	'411'=>'YT',
	'436'=>'NU',
	'439'=>'NU',
	'444'=>'NT',
	'445'=>'NT',
	'446'=>'NT',
	'455'=>'YT',
	'456'=>'YT',
	'462'=>'NU',
	'473'=>'NU',
	'511'=>'YT',
	'536'=>'YT',
	'537'=>'YT',
	'555'=>'YT',
	'561'=>'NU',
	'573'=>'NT',
	'580'=>'NT',
	'581'=>'NT',
	'587'=>'NT',
	'588'=>'NT',
	'589'=>'NT',
	'598'=>'NT',
	'602'=>'NT',
	'611'=>'YT',
	'620'=>'NT',
	'621'=>'NT',
	'633'=>'YT',
	'634'=>'YT',
	'645'=>'NU',
	'660'=>'YT',
	'667'=>'YT',
	'668'=>'YT',
	'669'=>'NT',
	'678'=>'NT',
	'690'=>'NT',
	'695'=>'NT',
	'699'=>'NT',
	'709'=>'NT',
	'711'=>'YT',
	'713'=>'NT',
	'765'=>'NT',
	'766'=>'NT',
	'767'=>'NT',
	'769'=>'NU',
	'770'=>'NT',
	'777'=>'NT',
	'793'=>'NU',
	'809'=>'NT',
	'811'=>'YT',
	'821'=>'YT',
	'825'=>'NT',
	'841'=>'YT',
	'851'=>'YT',
	'857'=>'NU',
	'862'=>'YT',
	'863'=>'YT',
	'870'=>'NT',
	'872'=>'NT',
	'873'=>'NT',
	'874'=>'NT',
	'875'=>'NT',
	'876'=>'NT',
	'880'=>'NT',
	'896'=>'NU',
	'897'=>'NU',
	'898'=>'NU',
	'899'=>'NU',
	'911'=>'YT',
	'920'=>'NT',
	'924'=>'NU',
	'925'=>'NU',
	'927'=>'NU',
	'928'=>'NU',
	'934'=>'NU',
	'939'=>'NU',
	'952'=>'NT',
	'953'=>'NT',
	'958'=>'NT',
	'959'=>'NT',
	'966'=>'YT',
	'969'=>'YT',
	'974'=>'NU',
	'975'=>'NU',
	'977'=>'NT',
	'978'=>'NT',
	'979'=>'NU',
	'980'=>'NU',
	'982'=>'NU',
	'983'=>'NU',
	'984'=>'NT',
	'993'=>'YT',
	'994'=>'YT',
	'995'=>'YT',
	'996'=>'YT',
	'997'=>'NT',
	'999'=>'NT',
);

my %nanpst = ();
my %nanpcc = ();

$fn = "nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't read $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my @tokens = split(/\t/,$_);
	$nanpst{$tokens[0]} = $tokens[2];
	$nanpcc{$tokens[0]} = $tokens[1];
}
close($fh);

my %rcs = ();
my %rns = ();

my %dbrc = ();

sub closerecord {
	my $data = shift;
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
	if (my $ocn = $data->{OCN}) {
		$ocns{$ocn} = {} unless exists $ocns{$ocn};
		my $rec = $ocns{$ocn};
		$rec->{OCN} = $ocn;
		for (my $i=1;$i<@ocn_keys;$i++) {
			my $k = $ocn_keys[$i];
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: $k changing from $rec->{$k} to $data->{$k}\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	my $cc = $data->{RCCC};
	my $st = $data->{RCST};
	my ($rc,$rn);
	if ($cc eq 'CA' or $nanpcc{$data->{NPA}} eq 'CA') {
		$cc = 'CA';
		$rc = undef;
		$rn = $data->{RateCenter};
	} else {
		$rc = $data->{RateCenter};
		$rn = undef;
	}
	next if $rc eq 'XXXXXXXXXX' or
		$rc eq 'DIR ASST' or
		$rc eq '7 DIGIT SE' or
#		($nanpcc{$data->{NPA}} eq 'CA' and
#			($data->{NXX} eq '310' or $data->{NXX} eq '610' or $data->{NXX} eq '810')) or
#		(($nanpcc{$data->{NPA}} eq 'US' or $nanpcc{$data->{NPA}} eq 'CA') and
#			($data->{NXX} eq '950' or $data->{NXX} eq '958' or $data->{NXX} eq '959')) or
		$data->{NXX} eq '555' or
		$data->{NXX} =~ /[2-9]11/ or
		$rn eq '7 Digit Service';
	my %cor = ( 'NL'=>'NF', 'NU'=>'VU', 'QC'=>'PQ' );
	my $rg = $data->{State}; $rg = $cor{$rg} if exists $cor{$rg};
	$rg = 'PQ' if $rg eq 'ON' and $rn eq 'St-Regis';
	if ($rc) {
		$rcs{$rg}{$rc} = {} unless exists $rcs{$rg}{$rc};
		my $rec = $rcs{$rg}{$rc};
		$rec->{REGION} = $rg;
		$rec->{RCSHORT} = $rc;
		$rec->{RCGEOID} = $data->{RCGEOID} if $data->{RCGEOID};
		$rec->{RCGN} = $data->{RCGN} if $data->{RCGN};
		$rec->{RCVH} = $data->{RCVH} if $data->{RCVH};
		$rec->{RCLL} = $data->{RCLL} if $data->{RCLL};
		$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		$rec->{NXX}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		$rec->{RCCC} = $data->{RCCC} if $data->{RCCC};
		$rec->{RCST} = $data->{RCST} if $data->{RCST};
		$rec->{RCNAME} = $rn if $rn;
	}
	if ($rn) {
		$rns{$cc}{$st}{$rn} = {} unless exists $rns{$cc}{$st}{$rc};
		my $rec = $rns{$cc}{$st}{$rn};
		$rec->{RCCC} = $cc;
		$rec->{RCST} = $st;
		$rec->{RCNAME} = $rn;
		$rec->{RCGEOID} = $data->{RCGEOID} if $data->{RCGEOID};
		$rec->{RCGN} = $data->{RCGN} if $data->{RCGN};
		$rec->{RCVH} = $data->{RCVH} if $data->{RCVH};
		$rec->{RCLL} = $data->{RCLL} if $data->{RCLL};
		$rec->{NPA}{$data->{NPA}}++ if $data->{NPA};
		$rec->{NXX}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
		$rec->{REGION} = $rg;
		$rec->{RCSHORT} = $rc if $rc;
	}
	if ($data->{NPA} and $data->{NXX}) {
		$dbrc{$data->{NPA}}{$data->{NXX}} = {} unless exists $dbrc{$data->{NPA}}{$data->{NXX}};
		my $rec = $dbrc{$data->{NPA}}{$data->{NXX}};
		$rec->{NPA} = $data->{NPA};
		$rec->{NXX} = $data->{NXX};
		$rec->{REGION} = $rg;
		$rec->{RCSHORT} = $rc if $rc;
		$rec->{RCCC} = $data->{RCCC} if $data->{RCCC};
		$rec->{RCST} = $data->{RCST} if $data->{RCST};
		$rec->{RCNAME} = $rn if $rn;
		$rec->{RCGEOID} = $data->{RCGEOID} if $data->{RCGEOID};
		$rec->{RCGN} = $data->{RCGN} if $data->{RCGN};
	}
}

my %mapping = (
	'NPA'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
		my $st = $nanpst{$val};
		$dat->{State} = $st;
		$dat->{RCST} = $st;
		$dat->{RCCC} = 'CA';
	},
	'NXX'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		if ($dat->{State} eq 'NT') {
			my $st = 'NT';
			$st = $nxxst{$val} if exists $nxxst{$val};
			$dat->{State} = $st;
			$dat->{RCST} = $st;
		} elsif ($dat->{State} eq 'NS') {
			my $st = 'NS';
			$st = $nsxst{$val} if exists $nsxst{$val};
			$dat->{State} = $st;
			$dat->{RCST} = $st;
		}
		$dat->{$fld} = $val;
	},
	'OCN'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Company'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Status'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$dat->{Use} = $val;
		$dat->{Use} =~ s/In Service/AS/;
		$dat->{Use} =~ s/Not Available/UA/;
		$dat->{Use} =~ s/Available/UA/;
		$dat->{Use} =~ s/Recovered\/Aging/UA/;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'RateCenter'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		#$val =~ s/['.]//g;
		#$val =~ s/-/ /g;
		#$val =~ s/\s{2,}/ /g;
		#$val =~ s/^STE /ST /i;
		#$val =~ s/^SAINT /ST /i;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
	'Remarks'=>sub{
		my ($dat,$val,$fld) = @_;
		return unless $val;
		$fcount{$fld}{count}++;
		$dat->{$fld} = $val;
	},
);

my @gn_fields = (
	'geonameid',
	'name',
	'asciiname',
	'alternatenames',
	'latitude',
	'longitude',
	'feature class',
	'feature code',
	'country code',
	'cc2',
	'admin1 code',
	'admin2 code',
	'admin3 code',
	'admin4 code',
	'population',
	'elevation',
	'dem',
	'timezone',
	'modification date',
);

sub normalize {
	my $nm = shift;
	$nm =~ s/-/ /g;
	$nm =~ s/\.//g;
	$nm =~ s/'//g;
	$nm =~ s/^\s+$//;
	$nm =~ s/\bSaint\b/St/gi;
	$nm =~ s/\bFort\b/Ft/gi;
	$nm =~ s/\bSainte\b/Ste/gi;
	$nm =~ s/\bRoad\b/Rd/gi;
	$nm =~ s/\bCenter\b/Ctr/gi;
	$nm =~ s/\bMount\b/Mt/gi;
	$nm =~ s/\bJunction\b/Jct/gi;
	$nm =~ s/\bCourt\b/Ct/gi;
	$nm =~ s/\bCompany\b/Co/gi;
	$nm =~ s/\bFalls\b/Fls/gi;
	$nm =~ s/\bSprints\b/Spg/gi;
	$nm =~ s/\bBuilding\b/Bldg/gi;
	$nm =~ s/\bCreek\b/Cr/gi;
	$nm =~ s/\bLake\b/Lk/gi;
	$nm =~ s/\bValey\b/Vly/gi;
	$nm =~ s/\bNorth.?East\b/NE/gi;
	$nm =~ s/\bNorth.?West\b/NW/gi;
	$nm =~ s/\bSouth.?East\b/SE/gi;
	$nm =~ s/\bSouth.?West\b/SW/gi;
	$nm =~ s/\bEast\b/E/gi;
	$nm =~ s/\bWest\b/W/gi;
	$nm =~ s/\bNorth\b/N/gi;
	$nm =~ s/\bSouth\b/S/gi;
	$nm =~ s/\bAir Force Base\fb/AFB/gi;
	$nm =~ s/\bAir Force Station\fb/AS/gi;
	$nm = "\U$nm\E";
	return $nm;
}

my %geonames = ();

my %a1codes = (
	'01'=>'AB',
	'02'=>'BC',
	'03'=>'MB',
	'04'=>'NB',
	'13'=>'NT',
	'07'=>'NS',
	'14'=>'NU',
	'08'=>'ON',
	'09'=>'PE',
	'10'=>'QC',
	'11'=>'SK',
	'12'=>'YT',
	'05'=>'NL',
);

foreach my $code (qw/AS CA GU MP PR US VI/) {
	my ($good,$bad) = (0,0);
	$fn = "$geondir/$code.zip";
	print STDERR "I: processing $fn\n";
	open($fh,"unzip -p $fn $code.txt |") or die "can't process $fn";
	while (<$fh>) { chomp; s/\r//g;
		my @tokens = split(/\t/,$_);
		my $data = {};
		for (my $i=0;$i<@gn_fields;$i++) {
			$data->{$gn_fields[$i]} = $tokens[$i] if $tokens[$i] or length($tokens[$i]);
		}
		my $fs = $data->{'feature class'};
		next unless $fs =~ /^(P|L|S|T|A)$/;
		my $fc = $data->{'feature code'};
		next unless $fc =~ /^(PPL|AREA|RESV|ISL|MILB|INSM|AIR|ADM)/;
		my $on = $data->{asciiname};
		my $nm = normalize($on);
		my $cc = $data->{'country code'};
		my $st = $cc; $st = $data->{'admin1 code'} if $cc eq 'US' or $cc eq 'CA';
		$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
		my $dt = $data->{'modification date'};
		if (exists $geonames{$cc}{$st}{$nm}) {
			my $rec = $geonames{$cc}{$st}{$nm};
			if (ref $rec eq 'ARRAY') {
				if ($rec->[0]{'feature class'} ne 'P' and $fs eq 'P') {
					$geonames{$cc}{$st}{$nm} = $data; $bad--; $good++;
				} else {
					push @{$rec}, $data;
					#printf STDERR "W: %d conflicting records for $cc-$st-$on\n",
					scalar(@{$rec});
				}
			} else {
				if ($rec->{'feature class'} ne $fs) {
					if ($rec->{'feature class'} ne 'P' and $fs eq 'P') {
						$geonames{$cc}{$st}{$nm} = $data;
					}
				} elsif ($rec->{'modification date'} eq $dt) {
					unless ($rec->{latitude} == $data->{latitude} and $rec->{longitude} == $data->{longitude}) {
						$geonames{$cc}{$st}{$nm} = [ $rec, $data ];
						$good--; $bad++;
						#print STDERR "W: 2 conflicting records for $cc-$st-$on\n";
					}
				} elsif ($rec->{'modification date'} lt $dt) {
					$geonames{$cc}{$st}{$nm} = $data;
				}
			}
		} else {
			$geonames{$cc}{$st}{$nm} = $data; $good++;
		}
	}
	close($fh);
	print STDERR "I: $code: $good good records; $bad bad records\n";
}

my %rcmap = ();
my %rnmap = ();
{
	$fn = "$npandir/rcmap.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		next unless /^"/;
		last if /^$/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@fields = @tokens;
			$heading = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i] if $tokens[$i];
		}
		$rcmap{$data->{REGION}}{$data->{RCSHORT}} = $data;
		$rnmap{$data->{RCCC}}{$data->{RCST}}{$data->{RCNAME}} = $data;
	}
	close($fh);
}


$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","', @keys), '"', "\n";
{
my ($found,$unusable,$failed);
$fn = "$datadir/NPANXX.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn cygdrive/d/cna/COCode-ESRDData/NPANXX.csv |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp; s/\r//g;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		#print STDERR "I: fields are '", join('|',@fields), "'\n";
		$header = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$tokens[$i],$fields[$i]);
		} else {
			print STDERR "W: no mapping for $fields[$i]\n";
		}
	}
	$data->{State} = $nanpst{$data->{NPA}};
	if ($data->{State} eq 'NT') {
		$data->{State} = $nxxst{$data->{NXX}} if exists $nxxst{$data->{NXX}};
	}
	if ($data->{State} eq 'NS') {
		$data->{State} = $nsxst{$data->{NXX}} if exists $nsxst{$data->{NXX}};
	}
	$data->{State} = 'QC' if $data->{State} eq 'ON' and $data->{RateCenter} eq 'St-Regis';
	$data->{RCST} = $data->{State};
	$data->{RCCC} = 'CA';
	if (my $nm = $data->{RateCenter}) {
		$data->{RCNAME} = $nm;
		if (my $st = $data->{State}) {
			$nm = normalize($nm);
			if (exists $geonames{CA}{$st}{$nm}) {
				if (ref $geonames{CA}{$st}{$nm} ne 'ARRAY') {
					my $geo = $geonames{CA}{$st}{$nm};
					#print STDERR "I: found geoname for $st-$nm\n";
					if ($geo->{latitude} and $geo->{longitude}) {
						$data->{RCLL} = "$geo->{latitude},$geo->{longitude}";
					}
					$data->{RCGEOID} = $geo->{geonameid};
					$data->{RCGN} = $geo->{name};
					$found++;
				} else {
					#print STDERR "W: cannot use geoname for $st-$nm\n";
					$unusable++;
				}
			} else {
				#print STDERR "W: cannot find geoname for $st-$nm\n";
				$failed++;
			}
		} else {
			print STDERR "E: no state for $nm\n";
		}
	}
	closerecord($data);
}
close($fh);
printf STDERR "I: CA %-8.8d matches found\n", $found;
printf STDERR "I: CA %-8.8d matches unusable\n", $unusable;
printf STDERR "I: CA %-8.8d matches failed\n", $failed;
}
{
my ($found,$unusable,$failed);
$fn = "$datadir/allutlzd.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn allutlzd.txt | expand |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($header) {
		$header = undef;
		next;
	}
	my @tokens = ();
	$tokens[0] = substr($_,0,8); # State
	$tokens[1] = substr($_,8,8); # NPA-NXX
	$tokens[2] = substr($_,16,8); # Ocn
	$tokens[3] = substr($_,24,64); # Company
	$tokens[4] = substr($_,88,16); # RateCenter
	$tokens[5] = substr($_,104,16); # EffectiveDate
	$tokens[6] = substr($_,120,8); # Use
	$tokens[7] = substr($_,128,16); # AssignDate
	$tokens[8] = substr($_,144,16); # Intial/Growth
	for (my $i=0;$i<@tokens;$i++) {
		$tokens[$i] =~ s/^\s*"?//;
		$tokens[$i] =~ s/"?\s*$//;
	}
	my $data = {};
	$data->{State} = $tokens[0];
	($data->{NPA},$data->{NXX}) = split(/-/,$tokens[1]);
	$data->{OCN} = $tokens[2];
	$data->{Company} = $tokens[3];
	$data->{RateCenter} = $tokens[4];
	$data->{EffectiveDate} = $tokens[5];
	$data->{Use} = $tokens[6];
	$data->{AssignDate} = $tokens[7];
	$data->{'Initial/Growth'} = $tokens[8];
	my $st = $data->{State} if $data->{State};
	my $cc = 'US';
	my %cor = ( 'AS'=>'AS', 'GU'=>'GU', 'PR'=>'PR', 'VI'=>'VI', 'NN'=>'MP' );
	$cc = $cor{$st} if exists $cor{$st};
	$st = $cc unless $cc eq 'CA' or $cc eq 'US';
	$data->{RCCC} = $cc;
	$data->{RCST} = $st;
	if (my $nm = $data->{RateCenter}) {
		if (my $st = $data->{State}) {
			$nm = normalize($nm);
			if (exists $geonames{$cc}{$st}{$nm}) {
				if (ref $geonames{$cc}{$st}{$nm} ne 'ARRAY') {
					my $rec = $geonames{$cc}{$st}{$nm};
					#print STDERR "I: found geoname for $st-$nm\n";
					if ($rec->{latitude} and $rec->{longitude}) {
						$data->{RCLL} = "$rec->{latitude},$rec->{longitude}";
					}
					$data->{RCGEOID} = $rec->{geonameid};
					$data->{RCGN} = $rec->{name};
					$found++;
				} else {
					#print STDERR "W: cannot use geoname for $st-$nm\n";
					$unusable++;
				}
			} else {
				#print STDERR "W: cannot find geoname for $st-$nm\n";
				$failed++;
			}
		} else {
			print STDERR "E: no state for $nm\n";
		}
	}
	closerecord($data);
}
close($fh);
printf STDERR "I: US %-8.8d matches found\n", $found;
printf STDERR "I: US %-8.8d matches unusable\n", $unusable;
printf STDERR "I: US %-8.8d matches failed\n", $failed;
}
close($of);

$fn = "$datadir/ocn.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@ocn_keys), '"', "\n";
foreach my $k (sort keys %ocns) {
	my $ocn = $ocns{$k};
	my @values = ();
	foreach (@ocn_keys) { push @values, $ocn->{$_} }
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach my $s (sort keys %rcs) {
	foreach my $k (sort keys %{$rcs{$s}}) {
		my $rc = $rcs{$s}{$k};
		my @values = ();
		foreach (@rc_keys) {
			if (ref $rc->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rc->{$_}});
			} else {
				push @values, $rc->{$_};
			}
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);

$fn = "$datadir/rn.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rn_keys), '"', "\n";
foreach my $cc (sort keys %rns) {
	foreach my $st (sort keys %{$rns{$cc}}) {
		foreach my $rn (sort keys %{$rns{$cc}{$st}}) {
			my $rec = $rns{$cc}{$st}{$rn};
			my @values = ();
			foreach (@rn_keys) {
				if (ref $rec->{$_} eq 'HASH') {
					push @values, join(',',sort keys %{$rec->{$_}});
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"', join('","',@values), '"', "\n";
		}
	}
}
close($of);

my @dbrc_keys = qw/NPA NXX REGION RCSHORT RCCC RCST RCNAME RCGN RCGEOID/;

$fn = "$datadir/db.rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@dbrc_keys), '"', "\n";
foreach my $npa (sort keys %dbrc) {
	foreach my $nxx (sort keys %{$dbrc{$npa}}) {
		my $rec = $dbrc{$npa}{$nxx};
		my @values = ();
		foreach (@dbrc_keys) {
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				push @values, join(',',sort keys %{$rec->{$_}});
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

exit;

1;

__END__

