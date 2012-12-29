#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

our $program = $0; $program =~ s/^.*\///;
our $progdir = $0; $progdir =~ s/\/[^\/]*$//;
our $datadir = "$progdir/data";

use Carp;
use Data::Dumper;
use Date::Parse;
use XML::Simple;
use XML::SAX;
use XML::SAX::Base;
#use Time::HiRes qw(gettimeofday);

use Gtk2 '-init';
use Glib qw(TRUE FALSE);
use Goo::Canvas;

use Convert::Color;

use DBI;

our    $blue_rgba = hex(Convert::Color->new("x11:blue"   )->as_rgb8->hex.'70');
our   $blue4_rgba = hex(Convert::Color->new("x11:blue4"  )->as_rgb8->hex.'70');
our  $orange_rgba = hex(Convert::Color->new("x11:orange" )->as_rgb8->hex.'70');
our $orange4_rgba = hex(Convert::Color->new("x11:orange4")->as_rgb8->hex.'70');
our  $green4_rgba = hex(Convert::Color->new("x11:green4" )->as_rgb8->hex.'70');
our  $yellow_rgba = hex(Convert::Color->new("x11:yellow" )->as_rgb8->hex.'70');
our $yellow4_rgba = hex(Convert::Color->new("x11:yellow4")->as_rgb8->hex.'70');
our   $black_rgba = hex(Convert::Color->new("x11:black"  )->as_rgb8->hex.'70');
our     $red_rgba = hex(Convert::Color->new("x11:red"    )->as_rgb8->hex.'70');
our    $cyan_rgba = hex(Convert::Color->new("x11:cyan"   )->as_rgb8->hex.'70');
our $magenta_rgba = hex(Convert::Color->new("x11:magenta")->as_rgb8->hex.'70');
our   $green_rgba = hex(Convert::Color->new("x11:green"  )->as_rgb8->hex.'70');
our    $grey_rgba = hex(Convert::Color->new("x11:grey"   )->as_rgb8->hex.'70');

sub vh2ll {
	my ($v,$h) = @_;
	my ($la,$lo);
	if ($v and $h) {
		($la,$lo) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
	}
	return ($la,$lo);
}
sub ll2xy {
	my ($y,$x) = @_;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	$x += 180; $x -= 360 if $x > 360;
	$x = 360 - $x;
	$x *= $scalex;
	$y = 90 - $y;
	$y *= $scaley;
	return ($x,$y);
}
sub vh2xy {
	my ($v,$h) = @_;
	my ($y,$x) = vh2ll($v,$h);
	($x,$y) = ll2xy($y,$x) if $y and $x;
	return ($x,$y);

}
#------------------------------
package MyTop; use strict;
use Gtk2;
use Glib qw(TRUE FALSE);
use Goo::Canvas;
use Geo::Coordinates::VandH;
#------------------------------
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	my ($w,$vbox,$mb,$me,$mi,$mc,$c,$sw);
	$w = $self->{TopLevel} = Gtk2::Window->new('toplevel');
	$w->signal_connect('delete_event'=>sub { Gtk2->main_quit; });
	$w->set_default_size(1024,768);
	$vbox = Gtk2::VBox->new();
	$vbox->set_spacing(0);
	$mb = $self->{MenuBar} = Gtk2::MenuBar->new;
	$me = $self->{FileMenu} = Gtk2::Menu->new;
	$mi = Gtk2::ImageMenuItem->new_from_stock('gtk-new',undef);
	$mi->signal_connect('activate'=>\&MyTop::menuFileNew,$self);
	$me->append($mi);
	$mi = Gtk2::ImageMenuItem->new_from_stock('gtk-open',undef);
	$mi->signal_connect('activate'=>\&MyTop::menuFileOpen,$self);
	$me->append($mi);
	$mc = Gtk2::MenuItem->new('_File');
	$mc->set_submenu($me);
	$mb->append($mc);
	$me = $self->{ViewMenu} = Gtk2::Menu->new;
	$mi = Gtk2::ImageMenuItem->new_from_stock('gtk-open',undef);
	$mi->signal_connect('activate'=>\&MyTop::viewStates,$self);
	$me->append($mi);
	$mc = Gtk2::MenuItem->new('_View');
	$mc->set_submenu($me);
	$mb->append($mc);
	$vbox->pack_start($mb,FALSE,FALSE,0);
	$sw = $self->{ScrolledWindow} = Gtk2::ScrolledWindow->new();
	$sw->set_policy('always','always');
	$c = $self->{Canvas} = Goo::Canvas->new;
	$c->set(
		'background-color'=>'white',
		'automatic-bounds'=>TRUE,
		'units'=>'pixel',
		'anchor'=>'center',
	);
	$c->set_size_request(1024,768);
	$c->set_bounds(0,0,1024,768);
	$w->signal_connect('scroll-event'=>\&MyTop::eventScroll,$self);
	$sw->signal_connect('scroll-event'=>\&MyTop::eventScroll,$self);
	$c->signal_connect('scroll-event'=>\&MyTop::eventScroll,$self);
	$self->{cscale} = 1.0;
	$c->set_scale($self->{cscale});
	$sw->add($c);
	$vbox->pack_start($sw,TRUE,TRUE,0);
	$w->add($vbox);
	$w->show_all;
	return $self;
}
#package MyTop;
sub relax {
	my $self = shift;
	#Gtk2->main_iteration;
	Gtk2->main_iteration_do(FALSE);
}
sub eventScroll {
	my ($canvas,$event,$self) = @_;
	my $state = $event->get_state;
	my $dir = $event->direction;
	my $states = join(',',@{$state});
	#warn "$canvas $states $dir";
	if ($dir eq 'up' or $dir eq 'down') {
		if ($states=~/control-mask/) {
			if ($dir eq 'up') {
				$self->{cscale} *= 1.5;
				$self->{Canvas}->set_scale($self->{cscale});
				$event->set_state([]);
				return TRUE;
			} elsif ($dir eq 'down') {
				$self->{cscale} /= 1.5;
				$self->{Canvas}->set_scale($self->{cscale});
				$event->set_state([]);
				return TRUE;
			}
		} elsif ($states=~/shift-mask/) {
			if ($dir eq 'up') {
				$event->direction('left');
				$event->set_state([]);
				$self->{ScrolledWindow}->signal_emit('scroll-event',$event)
					unless $self->{ScrolledWindow} eq $canvas;
				return TRUE;
			} elsif ($dir eq 'down') {
				$event->direction('right');
				$event->set_state([]);
				$self->{ScrolledWindow}->signal_emit('scroll-event',$event)
					unless $self->{ScrolledWindow} eq $canvas;
				return TRUE;
			}
		}
	}
	#warn "args: ".join(', ',@_);
	#warn "event is: ".$event->type;
	#warn "state is: ".$event->get_state;
	#warn "direction is: ".$event->direction;
	#warn "x is: ".$event->x;
	#warn "y is: ".$event->y;
	$self->{ScrolledWindow}->signal_emit('scroll-event',$event)
		unless $self->{ScrolledWindow} eq $canvas;
	return FALSE;
}
sub closest {
	my ($self,$lat,$lon) = @_;
	my $geo = new Geo::Coordinates::VandH;
	$lon -= 360 if $lon > 0;
	my ($dd,$td,$cv,$ch);
	my ($gv,$gh);
	for (my $v=0;$v<=10000;$v+=1000) {
		for (my $h=-5000;$h<=27000;$h+=1000) {
			my ($y,$x) = $geo->vh2ll($v,$h);
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
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=100) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=100) {
			my ($y,$x) = $geo->vh2ll($v,$h);
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
	for (my $v=$gv-200;$v<=$gv+200;$v+=10) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=10) {
			my ($y,$x) = $geo->vh2ll($v,$h);
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
	for (my $v=$gv-20;$v<=$gv+20;$v+=1) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=1) {
			my ($y,$x) = $geo->vh2ll($v,$h);
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
sub readlerg7 {
	my ($self) = @_;
	my $fh = \*INFILE;
	open($fh,"<","$datadir/LERG7.DAT") or die "can't open file";
	while (<$fh>) { chomp;
		next if /^#/;
		my $clli = substr($_,32,11); $clli =~ s/^\s+//; $clli =~ s/\s+$//;
		my $lata = substr($_, 0, 5); $lata =~ s/^\s+//; $lata =~ s/\s+$//;
		my $socn = substr($_,53, 4); $socn =~ s/^\s+//; $socn =~ s/\s+$//;
		my $v = substr($_,57,5); $v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
		my $h = substr($_,62,5); $h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
		my $st = substr($_,158,2); $st =~ s/^\s+//; $st =~ s/\s+$//;
		$self->{db}{lerg7}{$clli}{clli} = $clli;
		$self->{db}{lerg7}{$clli}{lata} = $lata;
		$self->{db}{lerg7}{$clli}{wcv}  = $v;
		$self->{db}{lerg7}{$clli}{wch}  = $h;
		$self->{db}{lerg7}{$clli}{cc}   = $socn;
		$self->{db}{lerg7}{$clli}{st}   = $st;
	}
	close($fh);
}
sub readlerg8 {
	my ($self) = @_;
	my $fh = \*INFILE;
	open($fh,"<","$datadir/LERG8.DAT") or die "can't open file";
	while (<$fh>) { chomp;
		next if /^#/;
		my $lata = substr($_, 0, 5); $lata =~ s/^\s+//; $lata =~ s/\s+$//;
		my $stat = substr($_,32, 2); $stat =~ s/^\s+//; $stat =~ s/\s+$//;
		my $abbv = substr($_,34,10); $abbv =~ s/^\s+//; $abbv =~ s/\s+$//;
		my $name = substr($_,45,50); $name =~ s/^\s+//; $name =~ s/\s+$//;
		my $v = substr($_, 95,5); $v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
		my $h = substr($_,100,5); $h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
		$self->{db}{lerg8}{$stat}{$abbv}{lata} = $lata;
		$self->{db}{lerg8}{$stat}{$abbv}{stat} = $stat;
		$self->{db}{lerg8}{$stat}{$abbv}{abbv} = $abbv;
		$self->{db}{lerg8}{$stat}{$abbv}{name} = $name;
		$self->{db}{lerg8}{$stat}{$abbv}{rcv} = $v;
		$self->{db}{lerg8}{$stat}{$abbv}{rch} = $h;
	}
	close($fh);
}
sub readneca4 {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/neca4/sw.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $clli = $tokens[0];
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{neca4}{$clli}{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}
sub readnpanxxsource {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/npanxxsource/sw.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i];
		}
		my $clli = $rec->{CLLI};
		$self->{db}{npanxxsource}{clli}{$clli} = $rec;
	}
	close($fh);
	$header = 1;
	@fields = ();
	open($fh,"<","$datadir/npanxxsource/rc.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i];
		}
		my $rcshort = $rec->{RCSHORT};
		my $region  = $rec->{REGION};
		unless ($rec->{'Rate Center Country'} =~ /^(US|CA)$/) {
			$region = $rec->{REGION} = $rec->{'Rate Center Country'};
		}
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{npanxxsource}{rate}{$region}{$rcshort} = $rec;
		}
	}
	close($fh);
	close($fh);
}
sub readareacodes {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/areacodes/sw.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $clli = $tokens[0];
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{areacodes}{$clli}{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}
sub readlocal {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/localcallingguide/rc.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $rcshort = $tokens[0];
		my $region = $tokens[1];
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{localcallingguide}{$region}{$rcshort}{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}
sub readwire {
	my ($self) = @_;
	if (my $db = do "$datadir/cllis.pm") {
		$self->{db}{wire} = $db->{clli};
	}
}
sub readarea {
	my ($self) = @_;
	my $fh = \*INFILE;
	open($fh,"<","$datadir/AREACODEWORLD-GOLD-SAMPLE.csv") or die "can't open file";
	my $header = 1;
	my @fields = ();
	my $db = {};
	while (<$fh>) { chomp;
		if ($header) {
			@fields = split(/\t/,$_);
			for (my $i=0;$i<@fields;$i++) {
				$fields[$i] =~ s/^"(.*?)"$/\1/;
			}
			$header = undef;
			next;
		}
		my @values = split(/\t/,$_);
		for (my $i=0;$i<@values;$i++) {
			$values[$i] =~ s/^"(.*?)"$/\1/;
		}
		my $data = {};
		if (exists $db->{$values[0]}{$values[1]}) {
			$data = $db->{$values[0]}{$values[1]};
		} else {
			$db->{$values[0]}{$values[1]} = $data;
		}
		for (my $i=0;$i<@fields;$i++) {
			if (exists $data->{$fields[$i]}) {
				if (ref $data->{$fields[$i]} eq 'HASH') {
					$data->{$fields[$i]}{$values[$i]} = 1;
				} else {
					$data->{$fields[$i]} = {$data->{$fields[$i]}=>1,$values[$i]=>1 }
					unless $data->{$fields[$i]} eq $values[$i];
				}
			} else {
				$data->{$fields[$i]} = $values[$i] if $values[$i];
			}
		}
		$db->{switch}{$data->{SWITCH_CLLI}}{nxx}{"$data->{NPA}-$data->{NXX}"} = 1;
		$db->{rate}{"$data->{RC_VERTICAL},$data->{RC_HORIZONTAL}"}++;
		$db->{city}{"$data->{LATITUDE},$data->{LONGITUDE}"}++;
	}
	$self->{db} = $db;
	close($fh);

}
sub readuk {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/england/numplan.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i];
		}
		my $rc = $rec->{'Rate Center'};
		$self->{db}{uk}{rate}{$rc} = $rec;
	}
	close($fh);
}
sub readcc {
	my ($self) = @_;
	my $fh = \*INFILE;
	my $header = 1;
	my @fields = ();
	open($fh,"<","$datadir/geonames/countries.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $rec = {};
		for (my $i=0;$i<@fields;$i++) {
			$rec->{$fields[$i]} = $tokens[$i];
		}
		my $cc = $rec->{'country code'};
		$self->{db}{cc}{country}{$cc} = $rec;
	}
	close($fh);
}
sub plotcity {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	foreach (sort keys %{$self->{db}{city}}) {
		#my $r = $self->{db}{rate}{$_};
		my $r = 0.8;
		my ($y,$x) = split(/,/,$_);
		$x += 180; $x -= 360 if $x > 360;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$blue_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
		);
		push @{$self->{items}{city}}, $item;
	}
}
sub plotlerg7 {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach (values %{$self->{db}{lerg7}}) {
		my $r = 0.6;
		my ($v,$h) = ($_->{wcv},$_->{wch});
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$orange_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{wcv},$rec->{wch});
				my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
				$x = -$x;
				print STDERR "entering clli: $rec->{clli} $y,$x ($v,$h)\n";
			},$_);
		my $st = $_->{st};
		push @{$self->{items}{$st}}, $item;
	}
}
sub plotlerg8 {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach my $st (values %{$self->{db}{lerg8}}) {
		foreach (values %{$st}) {
			my $r = 0.5;
			my ($v,$h) = ($_->{rcv},$_->{rch});
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
				'antialias'=>'default',
				'fill-color-rgba'=>$green4_rgba,
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'visible',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					my ($v,$h) = ($rec->{rcv},$rec->{rch});
					my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
					$x = -$x;
					print STDERR "entering rc: $rec->{abbv} $y,$x ($v,$h)\n";
				},$_);
			my $st = $_->{stat};
			push @{$self->{items}{$st}}, $item;
		}
	}
}
sub plotneca4 {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach (values %{$self->{db}{neca4}}) {
		my $r = 0.3;
		#my $r = 0.8;
		my ($v,$h) = split(/,/,$_->{wcvh});
		$v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
		$h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
		next unless $v and $h;
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			#'fill-color-rgba'=>$yellow_rgba,
			'fill-color-rgba'=>$red_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = split(/,/,$rec->{wcvh});
				my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
				$x = -$x;
				print STDERR "entering clli: $rec->{CLLI} $y,$x ($v,$h)\n";
			},$_);
		my $st = substr($_->{CLLI},4,2);
		push @{$self->{items}{$st}}, $item;
	}
}
sub plotnpanxxsource {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach my $region (values %{$self->{db}{npanxxsource}{rate}}) {
		foreach (values %{$region}) {
			my $r = 0.25 * 6;
			my ($v,$h) = split(/,/,$_->{RCVH});
			$v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
			$h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
			unless ($v and $h) {
				my ($y,$x) = split(/,/,$_->{'Rate Center Latitude/Logitude'});
				next unless $y and $x;
				($v,$h) = $self->closest($y,$x);
				$_->{RCVH} = sprintf('%05d,%05d',$v,$h);
			}
			next unless $v and $h;
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
				'antialias'=>'default',
				'fill-color-rgba'=>$cyan_rgba,
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'visible',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					my ($v,$h) = split(/,/,$rec->{RCVH});
					my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
					$x = -$x;
					print STDERR "entering rc: $rec->{RCSHORT} $y,$x ($v,$h)\n";
				},$_);
			my $st = $_->{REGION};
			push @{$self->{items}{$st}}, $item;
		}
	}
	foreach (values %{$self->{db}{npanxxsource}{clli}}) {
		my $r = 0.2 * 6;
		my ($v,$h) = split(/,/,$_->{'Wire Center V&H'});
		$v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
		$h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
		next unless $v and $h;
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$magenta_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = split(/,/,$rec->{'Wire Center V&H'});
				my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
				$x = -$x;
				print STDERR "entering clli: $rec->{CLLI} $y,$x ($v,$h)\n";
			},$_);
		my $st = $_->{'Wire Center State'};
		$st = $_->{'Wire Center Country'} unless $_->{'Wire Center Country'} =~ /^(US|CA)$/;
		push @{$self->{items}{$st}}, $item;
	}
}
sub plotareacodes {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach (values %{$self->{db}{areacodes}}) {
		my $r = 0.8;
		my ($v,$h) = ($_->{RC_VERTICAL},$_->{RC_HORIZONTAL});
		$v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
		$h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
		next unless $v and $h;
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$blue_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{RC_VERTICAL},$rec->{RC_HORIZONTAL});
				my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
				$x = -$x;
				print STDERR "entering clli: $rec->{CLLI} $y,$x ($v,$h)\n";
			},$_);
		my $st = substr($_->{CLLI},4,2);
		push @{$self->{items}{$st}}, $item;
	}
}
sub plotlocal {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach my $region (values %{$self->{db}{localcallingguide}}) {
		foreach (values %{$region}) {
			my $r = 0.5;
			my ($v,$h) = ($_->{'RC-V'},$_->{'RC-H'});
			$v =~ s/^\s+0*//; $v =~ s/\s+$//; $v = int($v);
			$h =~ s/^\s+0*//; $h =~ s/\s+$//; $h = int($h);
			next unless $v and $h;
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
				'antialias'=>'default',
				'fill-color-rgba'=>$green4_rgba,
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'visible',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					my ($v,$h) = ($rec->{'RC-V'},$rec->{'RC-H'});
					my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
					$x = -$x;
					print STDERR "entering rc: $rec->{RCSHORT} $y,$x ($v,$h)\n";
				},$_);
			my $st = $_->{'REGION'};
			push @{$self->{items}{$st}}, $item;
		}
	}
}
sub plotdbi {
	my $self = shift;
	my $root = $self->{Canvas}->get_root_item;
	my $dbh = DBI->connect("DBI:SQLite:$datadir/joindata.sqlite",undef,undef,{ReadOnly=>1}) or die "can't open $datadir/joindata.sqlite";
	my ($sql,$sth);
	$sql = "SELECT exch,exshort,region,exlata,rcv,rch,rclat,rclon FROM exdata;\n";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		my $rec = { %$row };
		my $r = 1.5 * 4;
		my ($x,$y) = ::vh2xy($rec->{rcv},$rec->{rch});
		next unless $x and $y;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$cyan_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{rcv},$rec->{rch});
				my ($la,$lo) = ($rec->{rclat},$rec->{rclon});
				unless ($v==1) { ($la,$lo) = ::vh2ll($v,$h); $lo = -$lo; }
				print STDERR "entering ex: $rec->{exch} $rec->{region}-$rec->{exshort} $la,$lo ($v,$h)\n";
			},$rec);
		my $st = $rec->{region};
		my $lt = $rec->{exlata};
		push @{$self->{items}{$st}}, $item if $st;
		push @{$self->{latas}{$lt}}, $item if $lt;
	}
	$sth->finish;
	$sql = "SELECT region,rcshort,rclata,rcv,rch,rclat,rclon FROM rcdata;\n";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		my $rec = { %$row };
		my $r = 1.0 * 4;
		my ($x,$y) = ::vh2xy($rec->{rcv},$rec->{rch});
		next unless $x and $y;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$green4_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{rcv},$rec->{rch});
				my ($la,$lo) = ($rec->{rclat},$rec->{rclon});
				unless ($v==1) { my ($la,$lo) = ::vh2ll($v,$h); $lo = -$lo; }
				print STDERR "entering rc: $rec->{region}-$rec->{rcshort} $la,$lo ($v,$h)\n";
			},$rec);
		my $st = $rec->{region};
		my $lt = $rec->{rclata};
		push @{$self->{items}{$st}}, $item if $st;
		push @{$self->{latas}{$lt}}, $item if $lt;
	}
	$sth->finish;
	$sql = "SELECT wc,wclata,wcv,wch,wclat,wclon FROM wcdata;\n";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		my $rec = { %$row };
		my $r = 0.5 * 4;
		my ($x,$y) = ::vh2xy($rec->{wcv},$rec->{wch});
		next unless $x and $y;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$red_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{wcv},$rec->{wch});
				my ($la,$lo) = ($rec->{wclat},$rec->{wclon});
				unless ($v==1) { ($la,$lo) = ::vh2ll($v,$h); $lo = -$lo; }
				print STDERR "entering wc: [$rec->{wclata}] $rec->{wc} $la,$lo ($v,$h)\n";
			},$rec);
		my $st = substr($rec->{wc},4,2);
		my $lt = $rec->{wclata};
		push @{$self->{items}{$st}}, $item if $st;
		push @{$self->{latas}{$lt}}, $item if $lt;
	}
	$sth->finish;
	$dbh->disconnect;
}
sub plotwire {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach (values %{$self->{db}{wire}}) {
		my $r = 0.6;
		my ($v,$h) = ($_->{wcv},$_->{wch});
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$green_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($v,$h) = ($rec->{wcv},$rec->{wch});
				my ($y,$x) = Geo::Coordinates::VandH->new()->vh2ll($v,$h);
				$x = -$x;
				print STDERR "entering clli: $rec->{clli} $y,$x ($v,$h)\n";
			},$_);
		my $st = substr($_->{clli},4,2);
		push @{$self->{items}{$st}}, $item;
	}
}
sub plotrate {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	foreach (sort keys %{$self->{db}{rate}}) {
		#my $r = $self->{db}{rate}{$_};
		#$r = log($r);
		my $r = 0.4;
		my ($v,$h) = split(/,/,$_);
		my ($y,$x) = $blah->vh2ll($v,$h);
		$x += 180; $x -= 360 if $x > 360;
		$x = 360 - $x;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$red_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
		);
		push @{$self->{items}{rate}}, $item;
	}
}
sub plotgrid {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	my $blah = new Geo::Coordinates::VandH;
	my @coords = ();
	my ($v,$h);
	for ($v=0;$v<=10000;$v+=1000) {
		for ($h=-5000;$h<=27000;$h+=1000) {
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			if (($coords[-2] > 180 and $x < 180) or ($coords[-2] < 180 and $x > 180)) {
				my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
					'antialias'=>'default',
					'line-width'=>0.2,
					'line-cap'=>'round',
					'line-join'=>'round',
					'stroke-color-rgba'=>$grey_rgba,
				);
				@coords = ();
			}
			push @coords, $x, $y;
		}
		my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
			'antialias'=>'default',
			'line-width'=>0.2,
			'line-cap'=>'round',
			'line-join'=>'round',
			'stroke-color-rgba'=>$grey_rgba,
		);
		@coords = ();
	}
	for ($h=-5000;$h<=27000;$h+=1000) {
		for ($v=0;$v<=10000;$v+=1000) {
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			if (($coords[-2] > 180 and $x < 180) or ($coords[-2] < 180 and $x > 180)) {
				my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
					'antialias'=>'default',
					'line-width'=>0.2,
					'line-cap'=>'round',
					'line-join'=>'round',
					'stroke-color-rgba'=>$grey_rgba,
				);
				@coords = ();
			}
			push @coords, $x, $y;
		}
		my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
			'antialias'=>'default',
			'line-width'=>0.2,
			'line-cap'=>'round',
			'line-join'=>'round',
			'stroke-color-rgba'=>$grey_rgba,
		);
		@coords = ();
	}
}
sub plotdata {
	my ($self,$color,$width,@lines) = @_;
	my @coords = ();
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	$self->{items} = {};
	my $line = 0;
	while ($_ = shift @lines) {
		$line++;
		#printf STDERR "processing line %d: '%s'\n", $line, $_;
		next if /^#/;
		if (/^>/) {
			if (@coords) {
				#print STDERR "creating polyline...\n";
				my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
					'antialias'=>'default',
					'line-width'=>$width,
					'line-cap'=>'round',
					'line-join'=>'round',
					'stroke-color-rgba'=>$color,
				);
				push @{$self->{items}{gmt}}, $item;
				@coords = ();
			}
		} else {
			my ($x,$y) = split(/\t/,$_);
			$x += 180; $x -= 360 if $x > 360;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			push @coords, $x, $y;
			#print STDERR "adding coordinates $x, $y\n";
		}
	}
	if (@coords) {
		#print STDERR "creating polyline...\n";
		my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
			'antialias'=>'default',
			'line-width'=>$width,
			'line-cap'=>'round',
			'line-join'=>'round',
			'stroke-color-rgba'=>$color,
		);
		push @{$self->{items}{gmt}}, $item;
	}
}
sub plotuk {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	foreach (values %{$self->{db}{uk}{rate}}) {
		my $r = 0.40;
		my ($y,$x) = ($_->{latitude},$_->{longitude});
		next unless defined $x and defined $y and $x and $y and $x != 0 and $y != 0;
		$x += 180; $x -= 360 if $x > 360;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$blue_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($y,$x) = ($rec->{latitude},$rec->{longitude});
				print STDERR "entering rc: $rec->{'Rate Center'} $y,$x\n";
			},$_);
		push @{$self->{items}{GB}}, $item;
	}
}
sub plotcc {
	my $self = shift;
	my $scalex = 1920/360;
	my $scaley = 1200/180;
	my $root = $self->{Canvas}->get_root_item;
	foreach (values %{$self->{db}{cc}{country}}) {
		my $r = 2.0;
		my ($y,$x) = ($_->{latitude},$_->{longitude});
		next unless defined $x and defined $y and $x and $y and $x != 0 and $y != 0;
		$x += 180; $x -= 360 if $x > 360;
		$x *= $scalex;
		$y = 90 - $y;
		$y *= $scaley;
		my $item = Goo::Canvas::Ellipse->new($root,$x,$y,$r,$r,
			'antialias'=>'default',
			'fill-color-rgba'=>$yellow_rgba,
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'visible',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				my ($y,$x) = ($rec->{latitude},$rec->{longitude});
				print STDERR "entering cc: $rec->{'country code'} $y,$x\n";
			},$_);
		push @{$self->{items}{cc}}, $item;
	}
}
sub menuFileNew {
	my ($menuitem,$self) = @_;
	#warn join(', ',@_).": menu item invoked";
	#print STDERR "getting coastline...\n";
	my @lines;
	@lines = `pscoast -Rd -N1 -Dl -m`;
	$self->plotdata($orange4_rgba,0.3,@lines);
	@lines = `pscoast -Rd -N2 -Di -m`;
	$self->plotdata($yellow4_rgba,0.2,@lines);
	@lines = `pscoast -Rd -W1 -Dl -m`;
	$self->plotdata($black_rgba,0.15,@lines);
	@lines = `pscoast -Rd -I2 -Di -m`;
	$self->plotdata($blue4_rgba,0.05,@lines);
#	$self->readareacodes;
##	$self->readarea;
##	$self->readwire;
##	$self->readlerg7;
##	$self->readlerg8;
#	$self->readlocal;
#	$self->readneca4;
#	$self->readnpanxxsource;
	$self->readuk;
	$self->readcc;
##	$self->plotcity;
##	$self->plotrate;
##	$self->plotwire;
#	$self->plotareacodes;
##	$self->plotlerg7;
##	$self->plotlerg8;
#	$self->plotlocal;
#	$self->plotneca4;
#	$self->plotnpanxxsource;
	$self->plotdbi;
	$self->plotuk;
	$self->plotcc;
	$self->plotgrid;
}
sub menuFileOpen {
	my ($menuitem,$self) = @_;
	my $f = Gtk2::FileFilter->new;
	$f->set_name('Kicad .brd files');
	$f->add_pattern('*.brd');
	my $d = Gtk2::FileChooserDialog->new(
		'Open File',
		$self->{TopLevel},
		'open',
		'gtk-cancel'=>'cancel',
		'gtk-open'=>'ok',
	);
	$d->set_default_response('ok');
	$d->add_filter($f);
	$d->set_current_folder('/home/brian/oc48card/4portoc48-2');
	$d->set_filename('/home/brian/oc48card/4portoc48-2/4portoc48-2.brd');
	my $response = $d->run;
	if ($response ne 'ok') {
		$d->destroy;
		return;
	}
	my $file = $d->get_filename;
	$d->destroy;
	Gtk2->main_iteration;
	return unless $self->{BOARD} = BOARD->new($file);
	$self->draw;
}
#package MyTop;
sub togglestate {
	my ($b,$d) = @_;
	my ($self,$st) = @$d;
	my $vis;
	#print STDERR "$st toggled\n";
	if ($b->get_active) {
		#print STDERR "$st is now active\n";
		$vis = 'visible';
	} else {
		#print STDERR "$st is now inactive\n";
		$vis = 'invisible';
	}
	if (exists $self->{items}{$st}) {
		foreach (@{$self->{items}{$st}}) {
			$_->set('visibility'=>$vis);
		}
	}
}
#package MyTop;
sub viewStates {
	my ($menuitem,$self) = @_;
	my $w = $self->{ViewStates};
	if ($w) {
		$w->show_all if !$w->visible;
		return;
	}
	$w = $self->{ViewStates} = Gtk2::Window->new;
	$w->set_title('State Chooser');
	$w->signal_connect(destroy=>sub{
			my ($window,$self) = @_;
			$self->{ViewStates} = undef;
			1;
		},$self);
	$w->set_border_width(4);
	my $f = Gtk2::Frame->new('States Chooser');
	$w->add($f);
	my $v = Gtk2::VBox->new(FALSE,8);
	#$v->set_border_width(8);
	$f->add($v);
	my $h = Gtk2::HBox->new(FALSE,8);
	$v->pack_start($h,FALSE,FALSE,0);
	my $t = Gtk2::Table->new(24,4,TRUE);
	#$t->set_row_spacings(4);
	#$t->set_col_spacings(4);
	$h->pack_start($t,TRUE,TRUE,0);
	my ($r,$c) = (0,0);
	if (0) {
	foreach (
		'AB', 'AG', 'AI', 'AK', 'AL', 'AR', 'AS', 'AZ', 'BB', 'BC',
		'BM', 'BS', 'CA', 'CO', 'CT', 'DC', 'DE', 'DM', 'DO', 'FL',
		'GA', 'GD', 'GU', 'HI', 'IA', 'ID', 'IL', 'IN', 'JM', 'KN',
		'KS', 'KY', 'KY', 'LA', 'LC', 'MA', 'MB', 'MD', 'ME', 'MI',
		'MN', 'MO', 'MP', 'MS', 'MS', 'MT', 'NB', 'NC', 'ND', 'NE',
		'NH', 'NJ', 'NL', 'NM', 'NS', 'NT', 'NU', 'NV', 'NY', 'OH',
		'OK', 'ON', 'OR', 'PA', 'PE', 'PR', 'QC', 'RI', 'SC', 'SD',
		'SK', 'SX', 'TC', 'TN', 'TT', 'TX', 'UT', 'VA', 'VC', 'VG',
		'VI', 'VT', 'WA', 'WI', 'WV', 'WY', 'YT',
	) {
		$self->{items}{$_} = [] unless exists $self->{items}{$_};
	}
	}
	foreach (sort keys %{$self->{items}}) {
		my $b = Gtk2::CheckButton->new_with_label($_);
		$self->{vs}{$_} = 1 unless exists $self->{vs}{$_};
		$b->set_active(TRUE) if $self->{vs}{$_};
		$b->signal_connect('toggled'=>\&MyTop::togglestate,[$self,$_]);
		$t->attach_defaults($b,$c,$c+1,$r,$r+1);
		$r++;
		if ($r >= 24) {
			$r = 0;
			$c++;
		}
	}
	$w->show_all;
}
#package MyTop;
sub draw {
	my $self = shift;
	$self->{BOARD}->draw($self->{Canvas});
}
#package MyTop;
sub relayer {
	my $self = shift;
	$self->{BOARD}->relayer($self->{Canvas});
}

#------------------------------
package main; use strict;
use Gtk2;
use Glib qw(TRUE FALSE);
#------------------------------

our $top = MyTop->new;

Gtk2->main;

exit;

1;
