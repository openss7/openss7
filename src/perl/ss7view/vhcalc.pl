#!/usr/bin/perl -w
#
use Gtk2 '-init';
use Glib qw(TRUE FALSE);

our $geo = new Geo::Coordinates::VandH;

sub closest {
	my ($lat,$lon) = @_;
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

#------------------------------
package MyTop; use strict;
use Gtk2;
use Glib qw(TRUE FALSE);
use Geo::Coordinates::VandH;
#------------------------------

sub new {
	my $type = shift;
	my $self = {};
	bless $self,$type;
	my $w = $self->{TopLevel} = Gtk2::Window->new('toplevel');
	$w->signal_connect(delete_event=>sub{ Gtk2->main_quit; });
	my $f = Gtk2::Frame->new('V&H Coordinate Converter');
	$w->add($f);
	my $v = Gtk2::VBox->new(FALSE,8);
	$f->add($v);
	my $h = Gtk2::HBox->new;
	$v->pack_start($h,FALSE,FALSE,0);
	my $t = Gtk2::Table->new(5,2,TRUE);
	$h->pack_start($t,TRUE,TRUE,0);
	my ($l,$b,$e);
	$l = Gtk2::Label->new('Vertical');
	$t->attach_defaults($l,0,1,0,1);
	$b = $self->{Vertical} = Gtk2::EntryBuffer->new('');
	$e = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable(TRUE);
	$e->signal_connect(activate=>\&MyTop::activate,[$self,'Vertical']);
	$t->attach_defaults($e,1,2,0,1);
	$l = Gtk2::Label->new('Horizontal');
	$t->attach_defaults($l,0,1,1,2);
	$b = $self->{Horizontal} = Gtk2::EntryBuffer->new('');
	$e = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable(TRUE);
	$e->signal_connect(activate=>\&MyTop::activate,[$self,'Horizontal']);
	$t->attach_defaults($e,1,2,1,2);
	$l = Gtk2::Label->new('Latitude');
	$t->attach_defaults($l,0,1,2,3);
	$b = $self->{Latitude} = Gtk2::EntryBuffer->new('');
	$e = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable(TRUE);
	$e->signal_connect(activate=>\&MyTop::activate,[$self,'Latitude']);
	$t->attach_defaults($e,1,2,2,3);
	$l = Gtk2::Label->new('Longitude');
	$t->attach_defaults($l,0,1,3,4);
	$b = $self->{Longitude} = Gtk2::EntryBuffer->new('');
	$e = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable(TRUE);
	$e->signal_connect(activate=>\&MyTop::activate,[$self,'Longitude']);
	$t->attach_defaults($e,1,2,3,4);
	$l = Gtk2::Label->new('Google');
	$t->attach_defaults($l,0,1,4,5);
	$b = $self->{Google} = Gtk2::EntryBuffer->new('');
	$e = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable(FALSE);
	$t->attach_defaults($e,1,2,4,5);
	$w->show_all;
}

sub activate {
	my ($widget,$data) = @_;
	my ($self,$label) = @{$data};
	my $text = $self->{$label}->get_text;
	print STDERR "activated with label $label and text '$text'\n";
	if ($label eq 'Vertical' or $label eq 'Horizontal') {
		$self->{$label}->set_text(sprintf("%05d",$text));	
		my $ver = $self->{Vertical}->get_text;
		my $hor = $self->{Horizontal}->get_text;
		my ($lat,$lon) = $geo->vh2ll($ver,$hor);
		$lon = -$lon;
		$lon += 360 if $lon < -180;
		$self->{Vertical}->set_text(sprintf("%05d",$ver));
		$self->{Horizontal}->set_text(sprintf("%05d",$hor));
		$self->{Latitude}->set_text($lat);
		$self->{Longitude}->set_text($lon);
		$self->{Google}->set_text("$lat,$lon ($ver,$hor)");
			
	}
	if ($label eq 'Latitude' or $label eq 'Longitude') {
		my $lat = $self->{Latitude}->get_text;
		my $lon = $self->{Longitude}->get_text;
		my ($ver,$hor) = main::closest($lat,$lon);
		$self->{Vertical}->set_text(sprintf("%05d",$ver));
		$self->{Horizontal}->set_text(sprintf("%05d",$hor));
		$self->{Google}->set_text("$lat,$lon ($ver,$hor)");
	}
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
