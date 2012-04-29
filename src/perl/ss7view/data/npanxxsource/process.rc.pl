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

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

sub vh2ll {
	my ($v,$h) = @_;
	my ($la,$lo) = Geo::Coordinates::VandH->vh2ll($v,$h);
	$lo = -$lo;
	$lo += 360 if $lo < -180;
	$lo -= 360 if $lo >  180;
	return ($la,$lo);
}

sub ll2vh {
	my ($lat,$lon) = @_;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=-5000;$v<=10000;$v+=5000) {
		for (my $h=-5000;$h<=30000;$h+=5000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-6000;$v<=$gv+6000;$v+=2000) {
		for (my $h=$gh-6000;$h<=$gh+6000;$h+=2000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-3000;$v<=$gv+3000;$v+=1000) {
		for (my $h=$gh-3000;$h<=$gh+3000;$h+=1000) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-1500;$v<=$gv+1500;$v+=500) {
		for (my $h=$gh-1500;$h<=$gh+1500;$h+=500) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-600;$v<=$gv+600;$v+=200) {
		for (my $h=$gh-600;$h<=$gh+600;$h+=200) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-300;$v<=$gv+300;$v+=100) {
		for (my $h=$gh-300;$h<=$gh+300;$h+=100) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-150;$v<=$gv+150;$v+=50) {
		for (my $h=$gh-150;$h<=$gh+150;$h+=50) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-60;$v<=$gv+60;$v+=20) {
		for (my $h=$gh-60;$h<=$gh+60;$h+=20) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-30;$v<=$gv+30;$v+=10) {
		for (my $h=$gh-30;$h<=$gh+30;$h+=10) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-15;$v<=$gv+15;$v+=5) {
		for (my $h=$gh-15;$h<=$gh+15;$h+=5) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-6;$v<=$gv+6;$v+=2) {
		for (my $h=$gh-6;$h<=$gh+6;$h+=2) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	($gv,$gh) = ($cv,$ch);
	for (my $v=$gv-3;$v<=$gv+3;$v+=1) {
		for (my $h=$gh-3;$h<=$gh+3;$h+=1) {
			my ($y,$x) = Geo::Coordinates::VandH->vh2ll($v,$h);
			$x = -$x;
			if (defined $dd) {
				$td = ($lat-$y)**2 + ($lon-$x)**2;
				if ($td < $dd) {
					$cv = $v;
					$ch = $h;
					$dd = $td;
				}
			} else {
				$cv = $v;
				$ch = $h;
				$dd = ($lat-$y)**2 + ($lon-$x)**2;
			}
		}
	}
	return ($cv,$ch);
}

my %nanpst = ();

$fn = "$codedir/nanpst.txt";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	my ($npa,$cc,$st,$name) = split(/\t/,$_);
	$nanpst{$npa} = $st;
}
close($fh);

my %lergcc = ();
my %lergst = ();
my %cllist = ();
my %cllicc = ();
my %cllirg = ();
my %statrg = ();
my %countries = ();

$fn = "$codedir/lergst.txt";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^ISO2/;
	next if /^\s*$/;
	my @tokens = split(/\t/,$_);
	$statrg{$tokens[1]} = $tokens[2];
	$lergcc{$tokens[2]} = $tokens[0];
	$lergst{$tokens[2]} = $tokens[1];
	$cllicc{$tokens[3]} = $tokens[0];
	$cllist{$tokens[3]} = $tokens[1];
	$cllirg{$tokens[3]} = $tokens[2];
	$countries{$tokens[0]}++;
}
close($fh);

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

my %gn_dontneed = (
	'alternatenames'=>1,
	'admin2 code'=>1,
	'admin3 code'=>1,
	'admin4 code'=>1,
	'population'=>1,
	'elevation'=>1,
	'dem'=>1,
	'timezone'=>1,
);

sub normalize {
	my $nm = shift;
	$nm =~ s/-/ /g;
	$nm =~ s/[[:punct:]]//g;
	$nm =~ s/^\s+$//;
	$nm =~ s/^\s+//;
	$nm =~ s/\s+$//;
	$nm =~ s/ \([^\)]*\)//g;
	$nm =~ s/ \(historical\)//i;
	$nm =~ s/\b(The )?Villages? of (the )?//i;
	$nm =~ s/\bIsle of //i;
	$nm =~ s/\bCity of //i;
	$nm =~ s/\bEstates of //i;
	$nm =~ s/\bUnorganized Territory of //i;
	$nm =~ s/\bTown(ship)? of //i;
	$nm =~ s/\bBorough of //i;
	$nm =~ s/\bPoint of //i;
	$nm =~ s/\BLakes? of the //i;
	$nm =~ s/\bHead of (the )?//i;
	#$nm =~ s/ Subdivision\b//i;
	#$nm =~ s/ County$//i;
	#$nm =~ s/ (Military|Indian)? Reservation$//i;
	#$nm =~ s/ Town(ship)?$//i;
	#$nm =~ s/ City$//i;
	#$nm =~ s/ Village$//i;
	#$nm =~ s/^New //i;
	#$nm =~ s/^Old //i unless $nm =~ /Old Rural Town/i;
	$nm =~ s/\bD\s+C\b/DC/;
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
	$nm =~ s/\bAir Force Base\b/AFB/gi;
	$nm =~ s/\bAir Force Station\b/AFS/gi;
	$nm =~ s/\bAir Station\b/AS/gi;
	$nm = "\U$nm\E";
	return $nm;
}

sub cmpnames {
	my ($nm1,$nm2) = @_;
	my $n1 = normalize($nm1);
	my $n2 = normalize($nm2);
	return 1 if $n1 eq $n2;
	($n1,$n2) = ($n2,$n1) if length($n2) < length($n1);
	my $dist = distance($n1,$n2);
	return 1 if $dist <= 3 and $dist < length($n1)/3;
	#return 1 if $dist < 3 or $dist < length($n2)/3 or $dist < length($n2) - length($n1) + 3;
	{
		my $a1 = $n1; $a1 =~ s/[AEIOU ]//gi; $a1 =~ s/([A-Z])\1/\1/gi;
		my $a2 = $n2; $a2 =~ s/[AEIOU ]//gi; $a2 =~ s/([A-Z])\1/\1/gi;
		($a1,$a2) = ($a2,$a1) if length($a2) < length($a1);
		if (length($a1) > 4) {
			return 1 if $a1 eq $a2;
			return 1 if index($a2,$a1) != -1 and length($a1) >= length($a2)/3;
		}
	}
	my @t1 = split(/\s+/,$n1);
	my @t2 = split(/\s+/,$n2);
	my (@c1,@c2);
	if (scalar @t1 < scalar @t2) {
		@c1 = @t1; @c2 = @t2;
	} else {
		@c1 = @t2; @c2 = @t1;
	}
	my $cnt = scalar @c1;
	my $mtc = 0;
	foreach my $k1 (@c1) {
		foreach my $k2 (@c2) {
			if ($k1 eq $k2) {
				$mtc++;
				last;
			} else {
				my ($i1,$i2,$good) = (0,0,0);
				my ($s1,$s2);
				if (length($k1)<length($k2)) {
					($s1,$s2) = ($k1,$k2);
				} else {
					($s1,$s2) = ($k2,$k1);
				}
				while ($i1<length($s1)) {
					if (substr($s1,$i1,1) eq substr($s2,$i2,1)) {
						$good++; $i1++; $i2++;
					} elsif (substr($s1,$i1,1) eq substr($s2,$i2+1,1)) {
						$good++; $i1++; $i2+=2;
					} elsif (substr($s1,$i1+1,1) eq substr($s2,$i2,1)) {
						$good++; $i1+=2; $i2++;
					} elsif (substr($s1,$i1,1) eq substr($s2,$i2+2,1)) {
						$good++; $i1++; $i2+=3;
					} else {
						$i1++; $i2++;
					}
				}
				if ($good/length($s1) > 0.8 and $good/length($s2) > 0.3) {
					$mtc += 0.5;
					last;
				}
			}
		}
	}
	return 1 if $mtc > $cnt/2;
	#print STDERR "I: no match: '$nm1' '$nm2'\n";
	return 0;
}

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

my $maxradius = 50; # maximum of 100 miles (50 mile radius)

my %geonmids = ();
my %geonames = ();
my %features = ();

foreach my $code (qw/cities1000 AG AI AS BB BM BS CA DM DO GD GU JM KN KY LC MP MS PR SX TC TT US VC VG VI/) {
	my ($good,$bad) = (0,0);
	$fn = "$geondir/$code.csv";
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't read $fn";
	my $totals = `wc -l $fn`; chomp $totals; $totals = int($totals); $totals--;
	my $heading = 1;
	my @fields = ();
	my $count = 0;
	while (<$fh>) { chomp; s/\r//g;
		s/^"//; s/"$//; my @tokens = split('","',$_);
		if ($heading) { @fields = @tokens; $heading = undef; next; }
		my %data = ();
		for (my $i=0;$i<@fields;$i++) {
			next if exists $gn_dontneed{$fields[$i]};
			$data{$fields[$i]} = $tokens[$i] if $tokens[$i] or length($tokens[$i]);
		}
		$count++;
		print STDERR "I: uniq: $good, dupl: $bad, totl: $count/$totals\r" if $count % 1000 == 0;
		my $cc = $data{'country code'};
		my $st = $cc; $st = $data{'admin1 code'} if $cc eq 'US' or $cc eq 'CA';
		$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
		my $fc = $data{'feature class'};
		$fc = 'C' if $code eq 'cities1000';
		my $id = $data{geonameid};
		$geonmids{$id} = \%data if $id;
		my $nm = normalize($data{asciiname});
		if (exists $geonames{$cc}{$st}{$nm}{$fc}) {
			my $rec = $geonames{$cc}{$st}{$nm}{$fc};
			if (ref $rec eq 'ARRAY') {
				if ($rec->[0]{'modification date'} le $data{'modification date'}) {
					unshift @{$rec}, \%data;
				} else {
					push @{$rec}, \%data;
				}
			} else {
				if ($rec->{'modification date'} le $data{'modification date'}) {
					$geonames{$cc}{$st}{$nm}{$fc} = [ \%data, $rec ];
				} else {
					$geonames{$cc}{$st}{$nm}{$fc} = [ $rec, \%data ];
				}
				$good--; $bad++;
			}
		} else {
			$geonames{$cc}{$st}{$nm}{$fc} = \%data; $good++;
		}
		my ($v,$h) = ($data{vertical},$data{horizontal});
		for (my $lev=-2;$lev<=0;$lev++) {
			my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
			my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
			$features{$fc}{$lev}{"$vf,$hf"} = [] unless exists $features{$fc}{$lev}{"$vf,$hf"};
			push @{$features{$fc}{$lev}{"$vf,$hf"}}, \%data;
		}
	}
	close($fh);
	print STDERR "I: uniq: $good, dupl: $bad, totl: $count/$totals\n";
}

sub closestname {
	my ($v,$h,$nm) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-2;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/C P A L S T/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				next unless cmpnames($nm,$_->{asciiname}) or cmpnames($nm,$_->{name});
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub closestcity {
	my ($v,$h) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-2;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/C/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub closestfeat {
	my ($v,$h) = @_;
	my ($geo,$delta);
	for (my $lev=0;$lev>=-2;$lev--) {
		my $vf = POSIX::floor($v*(10**$lev)+0.5)/(10**$lev);
		my $hf = POSIX::floor($h*(10**$lev)+0.5)/(10**$lev);
		foreach my $fc (qw/P A L S T/) {
			next unless exists $features{$fc}{$lev}{"$vf,$hf"};
			foreach (@{$features{$fc}{$lev}{"$vf,$hf"}}) {
				my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
				my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
				if (defined $delta) {
					if ($d < $delta) { $delta = $d; $geo = $_; }
				} else { $delta = $d; $geo = $_; }
			}
			last if defined $geo;
		}
		last if defined $geo;
	}
	return ($geo,$delta);
}

sub bestname {
	my ($cc,$st,$nm) = @_;
	my $geo;
	foreach my $fc (qw/C P A L S T/) {
		foreach my $ln (keys %{$geonames{$cc}{$st}}) {
			next unless exists $geonames{$cc}{$st}{$ln}{$fc};
			$_ = $geonames{$cc}{$st}{$ln}{$fc};
			if (ref $_ ne 'ARRAY' or $_ = $_->[0]) {
				unless (cmpnames($nm,$_->{asciiname}) or cmpnames($nm,$_->{name})) {
					#print STDERR "I: $cc-$st '$nm' does not match name $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n" unless defined $geo;
					next;
				} else {
					#printf STDERR "I: $cc-$st '$nm' matches %3.6f miles $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n", $d;
				}
				$geo = $_;
			}
			last if $geo;
		}
		last if $geo;
	}
	return $geo;
}

my ($found,$unusable,$failed,$recovered,$toofar);

sub lookupgeo {
	my ($cc,$st,$nm,$v,$h) = @_;
	my $ln = normalize($nm);
	my $geo;
	if (exists $geonames{$cc}{$st}{$ln}) {
		foreach my $fc (qw/C P A L S T/) {
			next unless exists $geonames{$cc}{$st}{$ln}{$fc};
			if (ref $geonames{$cc}{$st}{$ln}{$fc} ne 'ARRAY') {
				$geo = $geonames{$cc}{$st}{$ln}{$fc};
				if ($v and $h) {
					my ($vp,$hp) = ($geo->{vertical},$geo->{horizontal});
					my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
					if ($d > $maxradius) {
						#printf STDERR "I: $cc-$st '$nm' too far %3.6f miles $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}'\n", $d;
						$geo = undef;
						$toofar++;
					}
				}
			} else {
				if ($v and $h) {
					my $delta;
					foreach (@{$geonames{$cc}{$st}{$ln}{$fc}}) {
						my ($vp,$hp) = ($_->{vertical},$_->{horizontal});
						my $d = sqrt(((($vp-$v)**2)+(($hp-$h)**2))/10);
						if ($d > $maxradius) {
							#printf STDERR "I: $cc-$st '$nm' too far %3.6f miles $_->{'feature class'}.$_->{'feature code'} '$_->{name}'\n", $d;
							next;
						}
						if (defined $delta) {
							if ($d < $delta) { $delta = $d; $geo = $_; }
						} else { $delta = $d; $geo = $_; }
					}
				} else {
					# just have to take the first one on the list
					$geo = $geonames{$cc}{$st}{$ln}{$fc}[0];
				}
				unless (defined $geo) {
						print STDERR "W: cannot use geoname for $cc-$st-$nm\n";
					$unusable++;
				}
			}
			if (defined $geo) { $found++; last; }
		}
	} else {
		#print STDERR "W: cannot find geoname for $cc-$st-$nm\n";
		$failed++;
	}
	return $geo;
}

my ($nolook,$noname,$nocity,$nofeat);
my ($alook,$aname,$acity,$afeat);

sub geoassign {
	my ($data,$geo) = @_;
	$data->{RCGEOID} = $geo->{geonameid};
	$data->{RCGN} = $geo->{name};
	$data->{RCGEOVH} = sprintf('%05d,%05d', $geo->{vertical}, $geo->{horizontal});
	$data->{RCGEOLL} = "$geo->{latitude},$geo->{longitude}";
	$data->{RCCODE} = "$geo->{'feature class'}.$geo->{'feature code'}";
	$data->{RCNAME} = $geo->{name} unless $data->{RCNAME} and "\U$data->{RCNAME}\E" ne "\U$geo->{asciiname}\E";
	$data->{RCCITY} = $geo->{name} unless $data->{RCNAME} and "\U$data->{RCCITY}\E" ne "\U$geo->{asciiname}\E";
	$data->{RCCC} = $geo->{'country code'} if $geo->{'country code'};
	$data->{RCST} = $geo->{'admin1 code'} if length($geo->{'admin1 code'});
	$data->{RCST} = $a1codes{$data->{RCST}} if $data->{RCCC} eq 'CA' and exists $a1codes{$data->{RCST}};
}

sub geoshowfail {
	my ($cc,$st,$nm,$data,$geo,$kind,$dist) = @_;
	my $vh = sprintf('%05d,%05d',$geo->{vertical},$geo->{horizontal});
	my $ll = "$geo->{latitude},$geo->{longitude}";
	my $gn = $geo->{asciiname};
	my $fc = "$geo->{'feature class'}.$geo->{'feature code'}";
	$cc = $geo->{'country code'};
	$st = $geo->{'admin1 code'};
	$st = $a1codes{$st} if $cc eq 'CA' and exists $a1codes{$st};
	my $mi = sprintf(' %.2f mi', $dist) if defined $dist;
	printf STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' $kind: $fc$mi $ll ($vh $gn)\n", $dist;
}

sub geofound {
	my ($cc,$st,$nm,$data,$geo,$kind) = @_;
	geoassign($data,$geo);
	print STDERR "I: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' found as $kind $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}' ($geo->{asciiname})\n";
}

sub georestore {
	my ($cc,$st,$nm,$data,$id,$kind) = @_;
	if (my $geo = $geonmids{$id}) {
		geoassign($data,$geo);
		#print STDERR "I: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' restored as $kind $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}' ($geo->{asciiname})\n";
	}
}

sub georecover {
	my ($cc,$st,$nm,$data,$geo,$kind,$dist) = @_;
	geoassign($data,$geo);
	geoshowfail($cc,$st,$nm,$data,$geo,$kind,$dist);
	$found++; $failed--; $recovered++;
	print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' recovered as $kind $geo->{'feature class'}.$geo->{'feature code'} '$geo->{name}' ($geo->{asciiname})\n";
}

my %rcs = ();
my %rns = ();
my %dbrc = ();

$fn = "$datadir/rc.save.csv";
$fp = "$datadir/rc.csv";
system("mv -v $fp $fn") if -f $fp and not -f $fn;
if ( -f $fn ) {
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) { @fields = @tokens; $heading = undef; next }
		my %data = ();
		for (my $i=0;$i<@fields;$i++) {
			my $k = $fields[$i];
			my $v = $tokens[$i];
			if ($k =~ /^(RCVH|RCLL|WCVH|WCLL)$/) {
				if (length($v)) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$v)) {
						next if not $m or exists $data{$K}{$m};
						$data{$K}{$m}++;
						$data{$k} = $data{$k} ? join(';',$data{$k},$m) : $m;
					}
				}
			} elsif ($k =~ /^(NPA|NXX|X|LATA|OCN)$/) {
				foreach my $m (split(/,/,$v)) { $data{$k}{$m}++ if $m }
			} else {
				$data{$k} = $v if length($v);
			}
		}
		$rcs{$data{REGION}}{$data{RCSHORT}} = \%data;
	}
	close($fh);
}

$fn = "$datadir/rn.save.csv";
$fp = "$datadir/rn.csv";
system("mv -v $fp $fn") if -f $fp and not -f $fn;
if ( -f $fn ) {
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) { @fields = @tokens; $heading = undef; next }
		my %data = ();
		for (my $i=0;$i<@fields;$i++) {
			my $k = $fields[$i];
			my $v = $tokens[$i];
			if ($k =~ /^(RCVH|RCLL|WCVH|WCLL)$/) {
				if (length($v)) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$v)) {
						next if not $m or exists $data{$K}{$m};
						$data{$K}{$m}++;
						$data{$k} = $data{$k} ? join(';',$data{$k},$m) : $m;
					}
				}
			} elsif ($k =~ /^(NPA|NXX|X|LATA|OCN)$/) {
				foreach my $m (split(/,/,$v)) { $data{$k}{$m}++ if $m }
			} else {
				$data{$k} = $v if length($v);
			}
		}
		$rns{$data{RCCC}}{$data{RCST}}{"\U$data{RCNAME}\E"} = \%data;
	}
	close($fh);
}

$fn = "$datadir/db.rc.save.csv";
$fp = "$datadir/db.rc.csv";
system("mv -v $fp $fn") if -f $fp and not -f $fn;
if ( -f $fn ) {
	print STDERR "I: reading $fn\n";
	open($fh,"<:utf8",$fn) or die "can't open $fn";
	my $heading = 1;
	my @fields = ();
	while (<$fh>) { chomp;
		next unless /^"/;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) { @fields = @tokens; $heading = undef; next }
		my %data = ();
		for (my $i=0;$i<@fields;$i++) { $data{$fields[$i]} = $tokens[$i] if length($tokens[$i]) }
		$dbrc{$data{NPA}}{$data{NXX}}{$data{X}} = \%data;
	}
	close($fh);
}

my @rc_keys = qw/RCSHORT REGION RCVH RCLL WCVH WCLL RCGEOID RCCODE RCGN RCGEOVH RCGEOLL RCCITY RCCOUNTY NPA NXX X RCCC RCST RCNAME/;
my @rn_keys = qw/RCCC RCST RCNAME RCVH RCLL WCVH WCLL RCGEOID RCCODE RCGN RCGEOVH RCGEOLL RCCITY RCCOUNTY NPA NXX X REGION RCSHORT/;
my @dbrc_keys = qw/NPA NXX X REGION RCSHORT RCCC RCST RCNAME/;

my %cor = (
	'AG.AN'=>'AG.AG.AN',
	'BS.BA'=>'BS.BS.BA',
	'BB.BD'=>'BB.BB.BD',
	'VG.BV'=>'VG.VG.BV',
	'KY.CQ'=>'KY.KY.CQ',
	'DO.DR'=>'DO.DO.DR',
	'GD.GN'=>'GD.GD.GN',
	'US.GU'=>'GU.GU.GU',
	'KN.KA'=>'KN.KN.KA',
	'US.MP'=>'MP.MP.NN',
	'CA.NL'=>'CA.NL.NF',
	'CA.NU'=>'CA.NU.VU',
	'US.PR'=>'PR.PR.PR',
	'CA.QC'=>'CA.QC.PQ',
	'MS.RT'=>'MS.MS.RT',
	'LC.SA'=>'LC.LC.SA',
	'SX.SF'=>'SX.SX.SF',
	'US.SK'=>'CA.SK.SK',
	'TT.TR'=>'TT.TT.TR',
	'US.AS'=>'AS.AS.AS',
	'US.VI'=>'VI.VI.VI',
	'VC.ZF'=>'VC.VC.ZF',
);

my %mapping = (
	'NPA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'NXX'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'X'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'NPA NXX Record'=>sub{
	},
	'Line From'=>sub{
	},
	'Line To'=>sub{
	},
	'Wireless Block'=>sub{
	},
	'NXX Type'=>sub{
	},
	'Portable Block'=>sub{
	},
	'1,000 Block Pooling'=>sub{
	},
	'Block Contaminated'=>sub{
	},
	'Block Retained'=>sub{
	},
	'Date Assigned'=>sub{
	},
	'Effective Date'=>sub{
	},
	'Date Activated'=>sub{
	},
	'Last Modified'=>sub{
	},
	'Common Name'=>sub{
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'OCN Type'=>sub{
	},
	'Carrier Name'=>sub{
	},
	'Abbreviation'=>sub{
	},
	'DBA'=>sub{
	},
	'FKA'=>sub{
	},
	'Carrier Address'=>sub{
	},
	'Carrier City'=>sub{
	},
	'Carrier State'=>sub{
	},
	'Carrier Zip'=>sub{
	},
	'Carrier Country'=>sub{
	},
	'Wire Center LATA'=>sub{
	},
	'Other switches in WC'=>sub{
	},
	'Wire Center Address'=>sub{
	},
	'Wire Center County'=>sub{
	},
	'Wire Center City'=>sub{
	},
	'Class 4/5 Switch'=>sub{
	},
	'Wire Center State'=>sub{
	},
	'Trunk Gateway'=>sub{
	},
	'Point Code'=>sub{
	},
	'Switch type'=>sub{
	},
	'Tandem IntraLATA'=>sub{
	},
	'Tandem Feature Group C'=>sub{
	},
	'Tandem Feature Group D'=>sub{
	},
	'Tandem Local'=>sub{
	},
	'Tandem Feature Group B'=>sub{
	},
	'Tandem Operator Services'=>sub{
	},
	'Wire Center Zip'=>sub{
	},
	'SS7 STP 1'=>sub{
	},
	'Wire Center Google maps'=>sub{
	},
	'Wire Center Latitude/Longitude'=>sub{
	},
	'Wire Center V&H'=>sub{
	},
	'Wire Center CLLI'=>sub{
	},
	'Switch'=>sub{
	},
	'SS7 STP 2'=>sub{
	},
	'Actual Switch'=>sub{
	},
	'Wire Center Country'=>sub{
	},
	'Call Agent'=>sub{
	},
	'Host'=>sub{
	},
	'Wire Center Name'=>sub{
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'LATA Name'=>sub{
	},
	'Historical Region'=>sub{
	},
	'Area Codes in LATA'=>sub{
	},
	'Remark'=>sub{
	},
	'Rate Center LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{LATA} = $val if length($val);
	},
	'Rate Center Name'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCNAME} = $val if length($val);
	},
	'Rate Center State'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCST} = $val if length($val);
	},
	'Rate Center Country'=>sub{
		my ($dat,$fld,$val) = @_;
		my $cc = $val;
		my $st = $dat->{RCST};
		my $rg = $st;
		($cc,$st,$rg) = split(/\./,$cor{"$cc.$st"}) if exists $cor{"$cc.$st"};
		$cc = $lergcc{$rg} if exists $lergcc{$rg};
		$st = $lergst{$rg} if exists $lergst{$rg};
		if ($cc and $val and $cc ne $val) {
			print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}) Country: $val \-> $cc\n";
		}
		if ($st and $dat->{RCST} and $st ne $dat->{RCST}) {
			print STDERR "W: $dat->{NPA}-$dat->{NXX}($dat->{X}) State: $dat->{RCST} \-> $st\n";
		}
		$dat->{RCCC} = $cc;
		$dat->{RCST} = $st;
		$dat->{REGION} = $rg;
	},
	'LERG Abbreviation'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCSHORT} = $val if length($val);
	},
	'Zone'=>sub{
	},
	'Zone Type'=>sub{
	},
	'Number Pooling'=>sub{
	},
	'Point Identifier'=>sub{
	},
	'Rate Step'=>sub{
	},
	'Area Codes in Rate Center'=>sub{
	},
	'Embedded Overlays'=>sub{
	},
	'Major V&H'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = split(/,/,$val);
			if ($v and $h) {
				$dat->{VH} = $dat->{RCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{LL} = $dat->{RCLL} = join(',',vh2ll($v,$h));
			}
		}
	},
	'Minor V&H'=>sub{
	},
	'Rate Center Latitude/Longitude'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($la,$lo) = split(/,/,$val);
			if ($la and $lo) {
				$dat->{RCVH} = sprintf('%05d,%05d',ll2vh($la,$lo));
				$dat->{RCLL} = join(',',$la,$lo);
			}
			if ($dat->{VH}) {
				if ($dat->{VH} ne $dat->{RCVH}) {
					$dat->{VH} = $dat->{RCVH} = join(';',$dat->{VH},$dat->{RCVH});
				}
			} else {
				$dat->{VH} = $dat->{RCVH};
			}
			if ($dat->{LL}) {
				if ($dat->{LL} ne $dat->{RCLL}) {
					$dat->{LL} = $dat->{RCLL} = join(';',$dat->{LL},$dat->{RCLL});
				}
			} else {
				$dat->{LL} = $dat->{RCLL};
			}
		}
	},
	'Rate Center Google maps'=>sub{
	},
	'Time Zone'=>sub{
	},
	'DST Recognized'=>sub{
	},
	'Location within Rate Center Type'=>sub{
	},
	'Location within Rate Center Name'=>sub{
	},
	'County or Equivalent'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{RCCOUNTY} = $val if length($val);
	},
	'Federal Feature ID'=>sub{
	},
	'FIPS County Code'=>sub{
	},
	'FIPS Place Code'=>sub{
	},
	'Land Area'=>sub{
	},
	'Water Area'=>sub{
	},
	'Population 2000'=>sub{
	},
	'Population 2009'=>sub{
	},
	'Rural-Urban Continuum Code'=>sub{
	},
	'CBSA'=>sub{
	},
	'CBSA Level'=>sub{
	},
	'County Relation to CBSA'=>sub{
	},
	'Metro Division'=>sub{
	},
	'Part of CSA'=>sub{
	},
	'Cellular Market'=>sub{
	},
	'Major Trading Area'=>sub{
	},
	'Basic Trading Area'=>sub{
	},
);

my ($oldnpa,$oldrg);

$fn = "$progdir/db.csv";
print STDERR "I: processing $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
my $heading = 1;
my @fields = ();
while (<$fh>) { chomp;
	next unless /^"/;
	s/^"//; s/"$//; my @tokens = split(/","/,$_);
	if ($heading) {
		@fields = @tokens;
		$heading = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$fields[$i],$tokens[$i]);
		} else {
			print STDERR "E: no mapping for '$fields[$i]'\n";
		}
	}
	#print STDERR "I: processing $data->{NPA} $data->{NXX} $data->{X}\n";
	my $npa = $data->{NPA};
	my $rg = $data->{REGION};
	if ($npa ne $oldnpa and $rg ne $oldrg) {
		print STDERR "I: processing $npa $rg...\n";
	}
	($oldnpa,$oldrg) = ($npa,$rg);
	my $cc = $data->{RCCC};
	my $st = $data->{RCST};
	my $rc = $data->{RCSHORT};
	my $rn = $data->{RCNAME};
	my $rt = $data->{RCCITY};
	my $nm = $rn ? $rn : ( $rc ? $rc : $rt );
	$rn = "\U$rn\E";
	if ($cc and $st and $nm) {
		my $rec;
		unless ($rec or exists $data->{RCGEOID}) {
			if ($rg and $rc and exists $rcs{$rg}{$rc}) {
				$rec = $rcs{$rg}{$rc};
				#print STDERR "I: $data->{NPA}-$data->{NXX}($data->{X}) RC '$rc' found '$rg-$rc'\n";
				if ($rec->{RCGEOID}) {
					$data->{RCGEOID} = $rec->{RCGEOID};
					georestore($cc,$st,$nm,$data,$rec->{RCGEOID},'save');
				} else {
					foreach my $vh (split(/;/,$data->{VH})) {
						next unless $vh;
						next if exists $rec->{VH}{$vh};
						my $tested = join(';',sort keys %{$rec->{VH}});
						print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) RC '$rc' '$vh' not tested in '$tested'\n";
						$rec = undef; last;
					}
				}
			}
		}
		unless ($rec or exists $data->{RCGEOID}) {
			if ($cc and $st and $rn and exists $rns{$cc}{$st}{$rn}) {
				$rec = $rns{$cc}{$st}{$rn};
				#print STDERR "I: $data->{NPA}-$data->{NXX}($data->{X}) RN '$rn' found '$cc-$st-$rn'\n";
				if ($rec->{RCGEOID}) {
					$data->{RCGEOID} = $rec->{RCGEOID};
					georestore($cc,$st,$nm,$data,$rec->{RCGEOID},'save');
				} else {
					foreach my $vh (split(/;/,$data->{VH})) {
						next unless $vh;
						next if exists $rec->{VH}{$vh};
						my $tested = join(';',sort keys %{$rec->{VH}});
						print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) RN '$rn' '$vh' not tested in '$tested'\n";
						$rec = undef; last;
					}
				}
			}
		}
		unless ($rec or exists $data->{RCGEOID}) {
			foreach my $vh (split(/;/,$data->{VH})) {
				my ($v,$h) = split(/,/,$vh);
				if (my $geo = lookupgeo($cc,$st,$nm,$v,$h)) { $alook++;
					geofound($cc,$st,$nm,$data,$geo);
				} else {
					#print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' could not find closest look\n";
					$nolook++;
				}
			}
		}
		unless ($rec or exists $data->{RCGEOID}) {
			foreach my $vh (split(/;/,$data->{VH})) {
				my ($v,$h) = split(/,/,$vh);
				next unless $v and $h;
				unless ($rec or exists $data->{RCGEOID}) {
					print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' targ: $data->{LL} ($data->{VH} $nm)\n";
				}
				unless ($rec or exists $data->{RCGEOID}) {
					my ($geo,$dist) = closestname($v,$h,$nm);
					if ($geo) { $aname++;
						if ($dist < $maxradius) {
							georecover($cc,$st,$nm,$data,$geo,'name',$dist);
						} else {
							geoshowfail($cc,$st,$nm,$data,$geo,'name',$dist);
						}
					} else {
						#print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' could not find closest name\n";
						$noname++;
					}
				}
				unless ($rec or exists $data->{RCGEOID}) {
					my ($geo,$dist) = closestcity($v,$h);
					if ($geo) { $acity++;
						if ($dist < $maxradius and (cmpnames($nm,$geo->{asciiname}) or cmpnames($nm,$geo->{name}))) {
							georecover($cc,$st,$nm,$data,$geo,'city',$dist);
						} elsif ($dist < 1) {
							georecover($cc,$st,$nm,$data,$geo,'city',$dist);
						} else {
							geoshowfail($cc,$st,$nm,$data,$geo,'city',$dist);
						}
					} else {
						#print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' could not find closest city\n";
						$nocity++;
					}
				}
				unless ($rec or exists $data->{RCGEOID}) {
					my ($geo,$dist) = closestfeat($v,$h);
					if ($geo) { $afeat++;
						if ($dist < $maxradius and (cmpnames($nm,$geo->{asciiname}) or cmpnames($nm,$geo->{name}))) {
							georecover($cc,$st,$nm,$data,$geo,'feat',$dist);
						} elsif ($dist < 1) {
							georecover($cc,$st,$nm,$data,$geo,'feat',$dist);
						} else {
							geoshowfail($cc,$st,$nm,$data,$geo,'feat',$dist);
						}
					} else {
						#print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' could not find closest feat\n";
						$nofeat++;
					}
				}
			}
			unless ($data->{VH}) {
				unless ($rec or exists $data->{RCGEOID}) {
					if (my $geo = bestname($cc,$st,$nm)) { $aname++;
						georecover($cc,$st,$nm,$data,$geo,'best');
					} else {
						#print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' could not find closest name\n";
						$noname++;
					}
				}
			}
			unless ($rec or exists $data->{RCGEOID}) {
				print STDERR "W: $data->{NPA}-$data->{NXX}($data->{X}) $cc-$st '$nm' cannot find geoname\n";
			}
		}
	}
	{
		$dbrc{$data->{NPA}}{$data->{NXX}}{$data->{X}} = {} unless exists
		$dbrc{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		my $rec = $dbrc{$data->{NPA}}{$data->{NXX}}{$data->{X}};
		foreach my $k (@dbrc_keys) {
			if ($data->{$k}) {
				if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
					print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) DBRC $data->{NPA}-$data->{NXX}-$data->{X} $k changing from '$rec->{$k}' to '$data->{$k}'\n";
				}
				$rec->{$k} = $data->{$k};
			}
		}
	}
	$rn = "\U$data->{RCNAME}\E"; # in case it changed
	if ($rg and $rc) {
		$rcs{$rg}{$rc} = {} unless exists $rcs{$rg}{$rc};
		my $rec = $rcs{$rg}{$rc};
		foreach my $k (@rc_keys) {
			if ($k =~ /^(NPA|LATA|OCN)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k =~ /^(RCVH|RCLL|WCVH|WCLL)$/) {
				if ($data->{$k}) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$data->{$k})) {
						next if not $m or exists $rec->{$K}{$m};
						$rec->{$K}{$m}++;
						if ($rec->{$k}) {
							unless ($K eq 'LL') {
								my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
								print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RC $rc $k old value '$rec->{$k}' ($sects)\n" unless $rec->{$k} =~ /;/;
								print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RC $rc $k new value '$m' ($data->{SECT})\n";
							}
							$rec->{$k} = join(';',$rec->{$k},$m);
						} else {
							$rec->{$k} = $m;
						}
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RCSHORT $rg-$rc $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	} elsif ($cc and $st and $rn) {
		$rns{$cc}{$st}{$rn} = {} unless exists $rns{$cc}{$st}{$rn};
		my $rec = $rns{$cc}{$st}{$rn};
		foreach my $k (@rn_keys) {
			if ($k =~ /^(NPA|LATA|OCN)$/) {
				$rec->{$k}{$data->{$k}}++ if $data->{$k};
			} elsif ($k =~ /^(RCVH|RCLL|WCVH|WCLL)$/) {
				if ($data->{$k}) {
					my $K = substr($k,2,2);
					foreach my $m (split(/;/,$data->{$k})) {
						next if not $m or exists $rec->{$K}{$m};
						$rec->{$K}{$m}++;
						if ($rec->{$k}) {
							unless ($K eq 'LL') {
								my $sects = join(';',sort keys %{$rec->{SECT}}) if $rec->{SECT};
								print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RN $rn $k old value '$rec->{$k}' ($sects)\n" unless $rec->{$k} =~ /;/;
								print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RN $rn $k new value '$m' ($data->{SECT})\n";
							}
							$rec->{$k} = join(';',$rec->{$k},$m);
						} else {
							$rec->{$k} = $m;
						}
					}
				}
			} elsif ($k eq 'NXX') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}"}++ if $data->{NPA} and $data->{NXX};
			} elsif ($k eq 'X') {
				$rec->{$k}{"$data->{NPA}-$data->{NXX}-$data->{X}"}++ if $data->{NPA} and $data->{NXX} and $data->{X};
			} else {
				if ($data->{$k}) {
					if ($rec->{$k} and $rec->{$k} ne $data->{$k}) {
						print STDERR "E: $data->{NPA}-$data->{NXX}($data->{X}) RCNAME $cc-$st-$rn $k  changing from '$rec->{$k}' to '$data->{$k}'\n";
					}
					$rec->{$k} = $data->{$k};
				}
			}
		}
	}
}
close($fh);

printf STDERR "I: --------------------------\n";
printf STDERR "I: %8d matches found\n", $found;
printf STDERR "I: %8d matches unusable\n", $unusable;
printf STDERR "I: %8d matches failed\n", $failed;
printf STDERR "I: %8d matches recovered\n", $recovered;
printf STDERR "I: %8d matches too far\n", $toofar;
printf STDERR "I: --------------------------\n";
printf STDERR "I: %8d/%8d found/unfound look\n", $alook,$nolook;
printf STDERR "I: %8d/%8d found/unfound name\n", $aname,$noname;
printf STDERR "I: %8d/%8d found/unfound city\n", $acity,$nocity;
printf STDERR "I: %8d/%8d found/unfound feat\n", $afeat,$nofeat;
printf STDERR "I: --------------------------\n";

$fn = "$datadir/rc.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@rc_keys),'"',"\n";
foreach my $rg (sort keys %rcs) {
	foreach my $rc (sort keys %{$rcs{$rg}}) {
		my $rec = $rcs{$rg}{$rc};
		my @values = ();
		foreach (@rc_keys) {
			if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
				if (/^(RCVH|RCLL|RCGEOVH|RCGEOLL)$/) {
					push @values, join(';',sort keys %{$rec->{$_}});
				} else {
					push @values, join(',',sort keys %{$rec->{$_}});
				}
			} else {
				push @values, $rec->{$_};
			}
		}
		print $of '"',join('","',@values),'"',"\n";
	}
}
close($of);

$fn = "$datadir/db.rc.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@dbrc_keys),'"',"\n";
foreach my $npa (sort keys %dbrc) {
	foreach my $nxx (sort keys %{$dbrc{$npa}}) {
		foreach my $x (sort keys %{$dbrc{$npa}{$nxx}}) {
			my $rec = $dbrc{$npa}{$nxx}{$x};
			my @values = ();
			foreach (@dbrc_keys) {
				if (exists $rec->{$_} and ref $rec->{$_} eq 'HASH') {
					if (/^(RCVH|RCLL|RCGEOVH|RCGEOLL)$/) {
						push @values, join(';',sort keys %{$rec->{$_}});
					} else {
						push @values, join(',',sort keys %{$rec->{$_}});
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

$fn = "$datadir/rn.csv";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";
print $of '"',join('","',@rn_keys),'"',"\n";
foreach my $cc (sort keys %rns) {
	foreach my $st (sort keys %{$rns{$cc}}) {
		foreach my $rn (sort keys %{$rns{$cc}{$st}}) {
			my $rec = $rns{$cc}{$st}{$rn};
			next if $rec->{REGION} and $rec->{RCSHORT};
			my @values = ();
			foreach (@rn_keys) {
				if (exists $rec->{$_}) {
					if (ref $rec->{$_} eq 'HASH') {
						if (/^(RCVH|RCLL|RCGEOVH|RCGEOLL)$/) {
							push @values, join(';',sort keys %{$rec->{$_}});
						} else {
							push @values, join(',',sort keys %{$rec->{$_}});
						}
					} elsif (ref $rec->{$_} eq 'ARRAY') {
						push @values, join(';',@{$rec->{$_}});
					} else {
						push @values, $rec->{$_};
					}
				} else {
					push @values, $rec->{$_};
				}
			}
			print $of '"',join('","',@values),'"',"\n";
		}
	}
}
close($of);

exit;

1;

__END__

