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
	warn "$canvas $states $dir";
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
	open($fh,"<","$datadir/neca4/clli.csv");
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
	open($fh,"<","$datadir/npanxxsource/clli.csv");
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $clli = $tokens[0];
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{npanxxsource}{clli}{$clli}{$fields[$i]} = $tokens[$i];
		}
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
		my $rcshort = $tokens[0];
		my $region  = $tokens[1];
		for (my $i=0;$i<@fields;$i++) {
			$self->{db}{npanxxsource}{rate}{$region}{$rcshort}{$fields[$i]} = $tokens[$i];
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
	open($fh,"<","$datadir/areacodes/clli.csv");
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
			'fill-color'=>'blue',
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
			'fill-color'=>'orange',
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'all',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				print STDERR "entering clli: $rec->{clli} ($rec->{wcv},$rec->{wch})\n";
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
				'fill-color'=>'green4',
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'all',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					print STDERR "entering rc: $rec->{abbv} ($rec->{rcv},$rec->{rch})\n";
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
		#my $r = 0.3;
		my $r = 0.8;
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
			#'fill-color'=>'yellow',
			'fill-color'=>'red',
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'all',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				print STDERR "entering clli: $rec->{CLLI} ($rec->{wcvh})\n";
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
			my $r = 0.25;
			my ($v,$h) = split(/,/,$_->{RCVH});
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
				'fill-color'=>'cyan',
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'all',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					print STDERR "entering rc: $rec->{RCSHORT} ($rec->{RCVH})\n";
				},$_);
			my $st = $_->{REGION};
			push @{$self->{items}{$st}}, $item;
		}
	}
	foreach (values %{$self->{db}{npanxxsource}{clli}}) {
		my $r = 0.2;
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
			'fill-color'=>'magenta',
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'all',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				print STDERR "entering clli: $rec->{CLLI} ($rec->{'Wire Center V&H'})\n";
			},$_);
		my $st = $_->{'Wire Center State'};
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
			'fill-color'=>'blue',
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'all',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				print STDERR "entering clli: $rec->{CLLI} ($rec->{RC_VERTICAL},$rec->{RC_HORIZONTAL})\n";
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
				'fill-color'=>'green4',
				'line-width'=>0,
				'stroke-color'=>'white',
				'pointer-events'=>'all',
			);
			$item->signal_connect('enter_notify_event'=>sub{
					my ($item,$targ,$ev,$rec) = @_;
					print STDERR "entering rc: $rec->{RCSHORT} ($rec->{'RC-V'},$rec->{'RC-H'})\n";
				},$_);
			my $st = $_->{'REGION'};
			push @{$self->{items}{$st}}, $item;
		}
	}
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
			'fill-color'=>'green',
			'line-width'=>0,
			'stroke-color'=>'white',
			'pointer-events'=>'all',
		);
		$item->signal_connect('enter_notify_event'=>sub{
				my ($item,$targ,$ev,$rec) = @_;
				print STDERR "entering clli: $rec->{clli} ($rec->{wcv},$rec->{wch})\n";
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
			'fill-color'=>'red',
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
		for ($h=0;$h<=10000;$h+=1000) {
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			push @coords, $x, $y;
		}
		my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
			'antialias'=>'default',
			'line-width'=>0.2,
			'line-cap'=>'round',
			'line-join'=>'round',
			'stroke-color'=>'grey',
		);
		@coords = ();
	}
	for ($h=0;$h<=10000;$h+=1000) {
		for ($v=0;$v<=10000;$v+=1000) {
			my ($y,$x) = $blah->vh2ll($v,$h);
			$x += 180; $x -= 360 if $x > 360;
			$x = 360 - $x;
			$x *= $scalex;
			$y = 90 - $y;
			$y *= $scaley;
			push @coords, $x, $y;
		}
		my $item = Goo::Canvas::Polyline->new($root,FALSE,\@coords,
			'antialias'=>'default',
			'line-width'=>0.2,
			'line-cap'=>'round',
			'line-join'=>'round',
			'stroke-color'=>'grey',
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
					'stroke-color'=>$color,
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
			'stroke-color'=>$color,
		);
		push @{$self->{items}{gmt}}, $item;
	}
}
sub menuFileNew {
	my ($menuitem,$self) = @_;
	#warn join(', ',@_).": menu item invoked";
	#print STDERR "getting coastline...\n";
	my @lines;
	@lines = `pscoast -Rd -N1 -Dl -m`;
	$self->plotdata('orange4',0.3,@lines);
	@lines = `pscoast -Rd -N2 -Di -m`;
	$self->plotdata('yellow4',0.2,@lines);
	@lines = `pscoast -Rd -W1 -Dl -m`;
	$self->plotdata('black',0.15,@lines);
	@lines = `pscoast -Rd -I2 -Di -m`;
	$self->plotdata('blue4',0.05,@lines);
	$self->readareacodes;
#	$self->readarea;
#	$self->readwire;
#	$self->readlerg7;
#	$self->readlerg8;
	$self->readlocal;
	$self->readneca4;
	$self->readnpanxxsource;
#	$self->plotcity;
#	$self->plotrate;
#	$self->plotwire;
	$self->plotareacodes;
#	$self->plotlerg7;
#	$self->plotlerg8;
	$self->plotlocal;
	$self->plotneca4;
	$self->plotnpanxxsource;
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
