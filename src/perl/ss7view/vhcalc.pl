#!/usr/bin/perl -w
#
use Gtk2 '-init';
use Glib qw(TRUE FALSE);

our $geo = new Geo::Coordinates::VandH;

sub vh2ll {
	my ($v,$h) = @_;
	my ($la,$lo) = Geo::Coordinates::VandH->vh2ll($v,$h);
	$lo = -$lo;
	$lo += 360 if $lo < -180;
	$lo -= 360 if $lo >  180;
	return ($la,$lo);
}

my %vhcache = ();

sub vh2llfast {
	my ($v,$h,$inc) = @_;
	my ($la,$lo);
	if (exists $vhcache{$v}{$h}) {
		($la,$lo) = @{$vhcache{$v}{$h}};
	} else {
		($la,$lo) = vh2ll($v,$h);
		$vhcache{$v}{$h} = [$la,$lo] if $inc and $inc >= 100;
	}
	return ($la,$lo);
}

sub dist {
	my ($la1,$lo1,$la2,$lo2) = @_;
	my $d = sqrt((($la1-$la2)**2)+(($lo1-$lo2)**2));
	return $d;
}

sub adjv {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	while ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h,$inc)))) { $d = $dt; $v += $inc; }
	while ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h,$inc)))) { $d = $dt; $v -= $inc; }
	return ($v,$d);
}

sub adjh {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	while ($d > ($dt = dist($la,$lo,vh2llfast($v,$h+$inc,$inc)))) { $d = $dt; $h += $inc; }
	while ($d > ($dt = dist($la,$lo,vh2llfast($v,$h-$inc,$inc)))) { $d = $dt; $h -= $inc; }
	return ($h,$d);
}

sub adjvh {
	my ($la,$lo,$v,$h,$inc,$d,$dt) = @_;
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h+$inc,$inc)))) { $v+=$inc; $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h,     $inc)))) { $v+=$inc;           return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v+$inc,$h-$inc,$inc)))) { $v+=$inc; $h-=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v,     $h+$inc,$inc)))) {           $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h+$inc,$inc)))) { $v-=$inc; $h+=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h,     $inc)))) { $v-=$inc;           return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v,     $h-$inc,$inc)))) {           $h-=$inc; return ($v,$h,$dt); }
	if ($d > ($dt = dist($la,$lo,vh2llfast($v-$inc,$h-$inc,$inc)))) { $v-=$inc; $h-=$inc; return ($v,$h,$dt); }
	return ($v,$h,$d);
}

sub adj {
	my ($la,$lo,$v,$h,$inc,$d) = @_;
	my $changed = 1;
	while ($changed) {
		my ($vsav,$hsav) = ($v,$h);
		($v,$h,$d) = adjvh($la,$lo,$v,$h,$inc,$d);
		$changed = ($v != $vsav or $h != $hsav);
	}
	return ($v,$h,$d);
}

sub ll2vhold {
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
	for (my $v=$gv-8000;$v<=$gv+8000;$v+=2000) {
		for (my $h=$gh-8000;$h<=$gh+8000;$h+=2000) {
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
	for (my $v=$gv-4000;$v<=$gv+4000;$v+=1000) {
		for (my $h=$gh-4000;$h<=$gh+4000;$h+=1000) {
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
	for (my $v=$gv-2000;$v<=$gv+2000;$v+=500) {
		for (my $h=$gh-2000;$h<=$gh+2000;$h+=500) {
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
	for (my $v=$gv-800;$v<=$gv+800;$v+=200) {
		for (my $h=$gh-800;$h<=$gh+800;$h+=200) {
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
	for (my $v=$gv-400;$v<=$gv+400;$v+=100) {
		for (my $h=$gh-400;$h<=$gh+400;$h+=100) {
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
	for (my $v=$gv-200;$v<=$gv+200;$v+=50) {
		for (my $h=$gh-200;$h<=$gh+200;$h+=50) {
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
	for (my $v=$gv-80;$v<=$gv+80;$v+=20) {
		for (my $h=$gh-80;$h<=$gh+80;$h+=20) {
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
	for (my $v=$gv-40;$v<=$gv+40;$v+=10) {
		for (my $h=$gh-40;$h<=$gh+40;$h+=10) {
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
	for (my $v=$gv-20;$v<=$gv+20;$v+=5) {
		for (my $h=$gh-20;$h<=$gh+20;$h+=5) {
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
	for (my $v=$gv-8;$v<=$gv+8;$v+=2) {
		for (my $h=$gh-8;$h<=$gh+8;$h+=2) {
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
	for (my $v=$gv-4;$v<=$gv+4;$v+=1) {
		for (my $h=$gh-4;$h<=$gh+4;$h+=1) {
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

sub ll2vh {
	my ($la,$lo) = @_;
	my ($v,$h) = (-5000,-5000);
	my $d = dist($la,$lo,vh2llfast($v,$h,5000));
	($v,$h,$d) = adj($la,$lo,$v,$h,5000,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,1250,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,400,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,100,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,25,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,5,$d);
	($v,$h,$d) = adj($la,$lo,$v,$h,1,$d);
	if (0) {
	my ($vo,$ho) = ll2vhold($la,$lo);
	if ($v != $vo or $h != $ho) {
		my ($la1,$lo1) = vh2ll($v,$h);
		my $d1 = dist($la,$lo,$la1,$lo1);
		my ($la2,$lo2) = vh2ll($vo,$ho);
		my $d2 = dist($la,$lo,$la2,$lo2);
		print STDERR "E: VH mismatch $la1,$lo1($v,$h)[$d1] instead of $la2,$lo2($vo,$ho)[$d2] targetting $la,$lo";
		if ($d1 < $d2) {
			print STDERR " new is closer!\n";
		} elsif ($d2 < $d1) {
			print STDERR " old is closer!\n";
		} else {
			print STDERR " equidistant!\n";
		}
	}
	}
	return ($v,$h);
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
	#print STDERR "activated with label $label and text '$text'\n";
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
		my ($ver,$hor) = main::ll2vh($lat,$lon);
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
