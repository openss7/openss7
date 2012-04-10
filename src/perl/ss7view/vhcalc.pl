#!/usr/bin/perl -w
#
use Gtk2 '-init';
use Glib qw(TRUE FALSE);

our $geo = new Geo::Coordinates::VandH;

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
	my $t = Gtk2::Table->new(4,2,TRUE);
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
		$self->{Vertical}->set_text(sprintf("%05d",$ver));
		$self->{Horizontal}->set_text(sprintf("%05d",$hor));
		$self->{Latitude}->set_text($lat);
		$self->{Longitude}->set_text(-$lon);
	}
	if ($label eq 'Latitude' or $label eq 'Longitude') {
		my $lat = $self->{Latitude}->get_text;
		my $lon = $self->{Longitude}->get_text;
		my ($vmin,$ver,$vmax) = (0,0,10000);
		my ($hmin,$hor,$hmax) = (0,0,10000);
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
