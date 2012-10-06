require threads;
require threads::shared;
require Thread;
#use Thread qw/:DEFAULT async yield/;
require Thread::Queue;
use strict;
use warnings;
require Carp;
require Gtk2;
require Gtk2::SimpleList;
require Glib;
require Goo::Canvas;
require Gtk2::Helper;
require SNMP;
require Net::IP;
require Net::SNMP;
require Data::Dumper;
require Net::Libdnet::Arp;
require Net::Libdnet::Intf;
require Net::Libdnet::Route;
require Net::hostent;
require Socket;
require SNMP::Info;
require SNMP::Info::LLDP;
require Data::Dumper;
require FileHandle;
require DBI;
require Time::HiRes;
#use Time::HiRes qw(gettimeofday tv_interval);
require Date::Parse;

# ------------------------------------------
package Gui::Gtk; our @ISA = qw(Gui);
# ------------------------------------------
#package Gui::Gtk;
sub mainloop {
	my $self = shift;
	$self->start_timeout;
	Gtk2->main();
}
#package Gui::Gtk;
sub start_idle {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Idle->add($self->can('do_idle'),$self);
	$self->{tag} = $tag;
}
#package Gui::Gtk;
sub start_quick {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Timeout->add(100,$self->can('do_quick'),$self);
	$self->{tag} = $tag;
}
#package Gui::Gtk;
sub start_timeout {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Timeout->add(1000,$self->can('do_timeout'),$self);
	$self->{tag} = $tag;
}

# ------------------------------------------
package Canvas::Gtk; our @ISA = qw(Canvas);
# ------------------------------------------
#package Canvas::Gtk;
sub init {
	my $self = shift;
	my ($gui,$id,$window,$viewer) = @_;
	my $scro = $self->{scro} = new Gtk2::ScrolledWindow;
	$scro->set_policy('automatic','automatic');
	my $canv = $self->{canv} = new Goo::Canvas;
	$canv->set(
		'background-color'=>'white',
		'automatic-bounds'=>1,
		'bounds-from-origin'=>0,
		'bounds-padding'=>3,
		'units'=>'mm',
	);
	$canv->show_all;
	$scro->add($canv);
	$scro->show_all;
	$window = $self->{window} unless $window;
	$window->{note}->append_page($scro,'canvas');
	my $root = $self->{root} = $canv->get_root_item;
	$root->set('pointer-events'=>'all');
	if (my $sub = $self->can('scrollWindow')) {
		$scro->signal_connect_swapped('scroll-event'=>$sub,$self);
	}
	if (my $sub = $self->can('scrollCanvas')) {
		$canv->signal_connect_swapped('scroll-event'=>$sub,$self);
	}
	if (my $sub = $self->can('button_press_event')) {
		$root->signal_connect_swapped('button-press-event'=>$sub,$self);
	}
	if (my $sub = $self->can('enter_notify_event')) {
		$root->signal_connect_swapped('enter-notify-event'=>$sub,$self);
	}
	if (my $sub = $self->can('leave_notify_event')) {
		$root->signal_connect_swapped('leave-notify-event'=>$sub,$self);
	}
	$viewer = $self->{viewer} unless $viewer;
	$self->{layers} = {};
	foreach ($viewer->stack) { $self->getlayer($_) }
}
#package Canvas::Gtk;
sub fini {
	my $self = shift;
	my $root = $self->{root};
	foreach my $layer (values %{delete $self->{layers}}) {
		my $numb = $root->find_child($layer);
		if (defined $numb) {
			$root->remove_child($numb);
			# FIXME: do we need to destroy it?
		} else {
			Carp::carp "Could not find child $layer in group $self";
		}
	}
	delete $self->{scro};
	delete $self->{canv};
	delete $self->{root};
	delete $self->{window};
	delete $self->{viewer};
}
#package Canvas::Gtk;
sub refresh {
	my $self = shift;
	$self->{canv}->update;
}
#package Canvas::Gtk;
sub getlayer {
	my ($self,$layer) = @_;
	my $group = $self->{layers}{$layer};
	$group = $self->{layers}{$layer} = new Goo::Canvas::Group($self->{root},'pointer-events'=>'all') unless $group;
	return $group;
}
#package Canvas::Gtk;
sub setitem {
	my ($self,$item,$draw,$bmsg) = @_;
	$item->{draw} = $draw;
	$item->{bmsg} = $bmsg if $bmsg;
}
#package Canvas::Gtk;
sub putitem {
	my ($self,$item,$group) = @_;
	if (my $numb = $group->find_child($item)) {
		$group->remove_child($numb);
	}
	delete $item->{draw};
	delete $item->{bmsg};
}
#package Canvas::Gtk;
sub setpart {
	my ($self,$part,$pnts) = (shift,shift,shift);
	my ($item,$draw,$bmsg) = @_;
	$draw->{$part} = {pnts=>$pnts,item=>$item};
	$self->setitem(@_);
}
#package Canvas::Gtk;
sub putparts {
	my ($self,$draw,$group,@items) = @_;
	foreach (@items) {
		if (my $itm = delete $draw->{$_}) {
			if (my $item = $itm->{item}) {
				$self->putitem($item,$group);
			}
		}
	}
}
#package Canvas::Gtk;
sub scrollWindow {
	my ($self,$ev,$scro) = @_;
	my $canv = $self->{canv};
	my $state = $ev->get_state;
	my $dir = $ev->direction;
	my $states = join(',',@{$state});
	if ($dir eq 'up' or $dir eq 'down') {
		if ($states=~/control-mask/) {
			if ($dir eq 'up') {
				$canv->set_scale($canv->get_scale * 1.1);
			} else {
				$canv->set_scale($canv->get_scale / 1.1);
			}
			$ev->set_state([]);
			return Gtk2::EVENT_STOP;
		} elsif ($state=~/shift-mask/) {
			if ($dir eq 'up') {
				$ev->direction('left');
			} else {
				$ev->direction('right');
			}
			$ev->set_state([]);
			return Gtk2::EVENT_STOP;
		}
	}
	return Gtk2::EVENT_PROPAGATE;
}
#package Canvas::Gtk;
sub scrollCanvas {
	my ($self,$ev,$canv) = @_;
	my $scro = $self->{scro};
	my $state = $ev->get_state;
	my $dir = $ev->direction;
	my $states = join(',',@{$state});
	if ($dir eq 'up' or $dir eq 'down') {
		if ($states=~/control-mask/) {
			if ($dir eq 'up') {
				$canv->set_scale($canv->get_scale * 1.1);
			} else {
				$canv->set_scale($canv->get_scale / 1.1);
			}
			$ev->set_state([]);
			return Gtk2::EVENT_STOP;
		} elsif ($states=~/shift-mask/) {
			if ($dir eq 'up') {
				$ev->direction('left');
			} else {
				$ev->direction('right');
			}
			$ev->set_state([]);
			$scro->signal_emit('scroll-event',$ev);
			return Gtk2::EVENT_STOP;
		}
	}
	$scro->signal_emit('scroll-event',$ev);
	return Gtk2::EVENT_PROPAGATE;
}
#package Canvas::Gtk;
sub enter_notify_event {
	my ($self,$targ,$ev,$root) = @_;
	if ($targ) {
		if (my $draw = $targ->{draw}) {
			$draw->setcolor('cyan');
			return Gtk2::EVENT_STOP;
		}
	}
	return Gtk2::EVENT_PROPAGATE;
}
#package Canvas::Gtk;
sub leave_notify_event {
	my ($self,$targ,$ev,$root) = @_;
	if ($targ) {
		if (my $draw = $targ->{draw}) {
			$draw->setcolor($draw->mycolor);
			return Gtk2::EVENT_STOP;
		}
	}
	return Gtk2::EVENT_PROPAGATE;
}
#package Canvas::Gtk;
sub button1_press_event {
	my ($self,$draw,$ev) = @_;
	$draw->{view}{item}->show;
	return Gtk2::EVENT_STOP;
}
#package Canvas::Gtk;
sub button2_press_event {
	my ($self,$draw,$ev) = @_;
	$draw->{view}{item}->show;
	return Gtk2::EVENT_STOP;
}
#package Canvas::Gtk;
sub button3_press_event {
	my ($self,$draw,$ev) = @_;
	if (my $sub = $draw->can('fillmenu')) {
		my $menu = new Gtk2::Menu;
		if (&$sub($draw,$menu)) {
			$menu->show_all;
			$menu->popup(undef,undef,undef,undef,$ev->button,$ev->time);
			return Gtk2::EVENT_STOP;
		} else {
			Carp::carp "could not fill menu";
		}
		$menu->destroy;
	}
	return Gtk2::EVENT_PROPAGATE;
}
#package Canvas::Gtk;
sub button_press_event {
	my ($self,$targ,$ev,$root) = @_;
	if ($targ) {
		if (my $draw = $targ->{draw}) {
			if
			($ev->button == 1) { return $self->button1_press_event($draw,$ev); } elsif
			($ev->button == 2) { return $self->button2_press_event($draw,$ev); } elsif
			($ev->button == 3) { return $self->button3_press_event($draw,$ev); }
		}
		return Gtk2::EVENT_PROPAGATE;
	}
	return Gtk2::EVENT_PROPAGATE unless $ev->button == 3;
	my $menu = Gtk2::Menu->new();
	my $mi = Gtk2::MenuItem->new_with_label("View");
	$mi->show_all;
	$menu->append($mi);
	my $mv = Gtk2::Menu->new;
	$mi->set_submenu($mv);
	my ($to,$se);
	my $viewer = $self->{viewer};
	my @selections = $viewer->vis_sel;
	if (@selections) {
		unless ($to) {
			$to = Gtk2::TearoffMenuItem->new();
			$to->show_all;
			$mv->append($to);
		}
		foreach my $kind (@selections) {
			my $mr = Gtk2::CheckMenuItem->new($kind);
			$mr->set_draw_as_radio(1);
			my @pair = map {$viewer->isvisible($_)} split(/\//,$kind);
			if ($pair[0] and $pair[1]) {
				$mr->set_active(1);
				$mr->set_inconsistent(0);
			}
			elsif (not $pair[0] and not $pair[1]) {
				$mr->set_active(0);
				$mr->set_inconsistent(0);
			}
			else {
				$mr->set_active(0);
				$mr->set_inconsistent(1);
			}
			$mr->show_all;
			$mr->signal_connect('toggled'=>sub{
					my ($radio,$args) = @_;
					my ($viewer,$kind) = @$args;
					$viewer->selected($kind);
					#$radio->set_inconsistent(0);
					#$radio->set_active(1);
				},[$viewer,$kind]);
			$mv->append($mr);
		}
	}
	my @toggles = $viewer->vis_tog;
	if (@toggles) {
		unless ($to) {
			$to = Gtk2::TearoffMenuItem->new();
			$to->show_all;
			$mv->append($to);
		} else {
			$se = Gtk2::SeparatorMenuItem->new();
			$se->show_all;
			$mv->append($se);
		}
		foreach my $kind (@toggles) {
			my $mb = Gtk2::CheckMenuItem->new($kind);
			$mb->set_active($viewer->isvisible($kind));
			$mb->show_all;
			$mb->signal_connect('toggled'=>sub{
					my ($check,$args) = @_;
					my ($viewer,$kind) = @$args;
					$viewer->toggled($kind);
					#$check->set_active(not $check->get_active);
				},[$viewer,$kind]);
			$mv->append($mb);
		}
	}
	$mv->show_all;
	$menu->show_all;
	$menu->popup(undef,undef,undef,undef,$ev->button,$ev->time);
	return Gtk2::EVENT_STOP;
}
#package Canvas::Gtk;
sub getdims {
	my $self = shift;
	my $canv = $self->{canv};
	my $hbox = $canv->get_screen->get_height_mm;
	my $wbox = $canv->get_screen->get_width_mm;
	return ($hbox,$wbox);
}

# ------------------------------------------
package Draw::Gtk; our @ISA = qw(Draw);
# ------------------------------------------

# ------------------------------------------
package Group::Draw::Gtk; our @ISA = qw(Group::Draw);
# ------------------------------------------
#package Group::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $layer = $self->{layer} = $canvas->getlayer($self->kind);
	my ($item,$pnts);
	$pnts = [0,0];
	$item = new Goo::Canvas::Group($layer,
			pointer_events=>'all',
			visibility=>'invisible',
			);
	$canvas->setpart('grp',$pnts,$item,$self);
}
#package Group::Draw::Gtk;
sub fini {
	my $self = shift;
	my $canvas = $self->{canvas};
	my $layer = delete $self->{layer};
	$self->{canvas}->putparts($self,$layer,qw/grp/);
}
#package Group::Draw::Gtk;
sub placing {
	my $self = shift;
	my $vis = $self->isvisible ? 'visible' : 'invisible';
	$self->{grp}{item}->set(visibility=>$vis);
}

# ------------------------------------------
package Node::Draw::Gtk; our @ISA = qw(Node::Draw);
# ------------------------------------------
#package Node::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my $text = $self->gettxt;
	my ($item,$pnts);
	$pnts = [-8.0,-4.5,16,9];
	my $nod = $item = new Goo::Canvas::Rect(
		$group,@$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	$canvas->setpart('nod',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,20];
	$item = new Goo::Canvas::Text(
		$group,$text,@$pnts,'center',
#'antialias'=>'subpixel',
		alignment=>'center',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$item->raise($nod);
	$canvas->setpart('txt',$pnts,$item,$self,\$self->{text});
	$self->{pad} = [1.0,1.0];
}
#package Node::Draw::Gtk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/nod txt/);
}
#package Node::Draw::Gtk;
sub setcolor {
	my ($self,$color) = @_;
	$self->{nod}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
}
#package Node::Draw::Gtk;
sub placing {
	my $self = shift;
	my $color = $self->mycolor;
	my $text = $self->gettxt;
	my $vis = $self->isvisible ? 'visible' : 'invisible';
	my ($pnts,$item);
	($pnts,$item) = ($self->{txt}{pnts},$self->{txt}{item});
	$item->set(x=>$pnts->[0],y=>$pnts->[1],text=>$text,'fill-color'=>$color,visibility=>$vis);
	($pnts,$item) = ($self->{nod}{pnts},$self->{nod}{item});
	$item->set(x=>$pnts->[0],y=>$pnts->[1],'stroke-color'=>$color,visibility=>$vis);
}

# ------------------------------------------
package Bus::Draw::Gtk; our @ISA = qw(Bus::Draw);
# ------------------------------------------
#package Bus::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my $text = $self->gettxt;
	my ($item,$pnts);
	$pnts = [0,-10,0,10];
	my $bus = $item = new Goo::Canvas::Polyline(
		$group,0,$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.6,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	$canvas->setpart('bus',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,12];
	$item = new Goo::Canvas::Text(
		$group,$text,@$pnts,'west',
#'antialias'=>'subpixel',
		alignment=>'left',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$item->raise($bus);
	$canvas->setpart('txt',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,12];
	$item = new Goo::Canvas::Text(
		$group,$text,@$pnts,'west',
#'antialias'=>'subpixel',
		alignment=>'left',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$item->raise($bus);
	$canvas->setpart('txb',$pnts,$item,$self,\$self->{text});
	$self->{pad} = [1.0,1.0];
}
#package Bus::Draw::Gtk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/bus txt txb/);
}
#package Bus::Draw::Gtk;
sub setcolor {
	my ($self,$color) = @_;
	$self->{bus}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
	$self->{txb}{item}->set('fill-color'=>$color);
}
#package Bus::Draw::Gtk;
sub placing {
	my $self = shift;
	my $color = $self->mycolor;
	my $text = $self->gettxt;
	my $vis = $self->isvisible ? 'visible' : 'invisible';
	my ($pnts,$item);
	($pnts,$item) = ($self->{bus}{pnts},$self->{bus}{item});
	$item->set(points=>Goo::Canvas::Points->new($pnts),'stroke-color'=>$color,visibility=>$vis);
	($pnts,$item) = ($self->{txt}{pnts},$self->{txt}{item});
	$item->set(text=>$self->gettxt(),'fill-color'=>$color,visibility=>$vis);
	$item->set_simple_transform(@$pnts[0..1],1.0,90.0);
	($pnts,$item) = ($self->{txb}{pnts},$self->{txb}{item});
	$item->set(text=>$self->gettxt(),'fill-color'=>$color,visibility=>$vis);
	$item->set_simple_transform(@$pnts[0..1],1.0,-90.0);
}

# ------------------------------------------
package BoxAndLink::Draw::Gtk; our @ISA = qw(BoxAndLink::Draw);
# ------------------------------------------
#package BoxAndLink::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my $text = $self->gettxt;
	my ($item,$pnts);
	$pnts = [-1,-1,2,2];
	my $box = $item = new Goo::Canvas::Rect(
		$group,@$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	$canvas->setpart('box',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,5,0,10,0];
	$item = new Goo::Canvas::Polyline(
		$group,0,$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.2,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	$canvas->setpart('lnk',$pnts,$item,$self,\$self->{text});
	$box->raise($item);
	$pnts = [5,0,20,'sw',alignment=>'left'];
	Carp::confess "undefined group" unless defined $group;
	Carp::confess "undefined text" unless defined $text;
	Carp::confess "undefined color" unless defined $color;
	$item = new Goo::Canvas::Text(
		$group,$text,@$pnts,
#'antialias'=>'subpixel',
		font=>'Monospace 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$item->raise($box);
	$canvas->setpart('txt',$pnts,$item,$self,\$self->{text});
	$self->{pad} = [0,0];
}
#package BoxAndLink::Draw::Gtk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/box lnk txt/);
}
#package BoxAndLink::Draw::Gtk;
sub setcolor {
	my ($self,$color) = @_;
	$self->{box}{item}->set('stroke-color'=>$color);
	$self->{lnk}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
}
#package BoxAndLink::Draw::Gtk;
sub placing {
	my $self = shift;
	my $color = $self->mycolor;
	my $text = $self->gettxt;
	my $vis = $self->isvisible ? 'visible' : 'invisible';
	my ($pnts,$item);
	($pnts,$item) = ($self->{lnk}{pnts},$self->{lnk}{item});
	$item->set(points=>Goo::Canvas::Points->new($pnts),'stroke-color'=>$color,visibility=>$vis);
	($pnts,$item) = ($self->{txt}{pnts},$self->{txt}{item});
	$item->set(x=>$pnts->[0],y=>$pnts->[1],anchor=>$pnts->[3],alignment=>$pnts->[5],text=>$text,'fill-color'=>$color,visibility=>$vis);
	($pnts,$item) = ($self->{box}{pnts},$self->{box}{item});
	$item->set(x=>$pnts->[0],y=>$pnts->[1],'stroke-color'=>$color,visibility=>$vis);
}

# ------------------------------------------
package MibInfo::Gtk; our @ISA = qw(Base);
# ------------------------------------------
#package MibInfo::Gtk;
sub widthof {
	my $self = shift;
	my $text = shift;
	return length($text)*8;
}

# ------------------------------------------
package MibLabel::Gtk; our @ISA = qw(MibLabel MibInfo::Gtk);
# ------------------------------------------
#package MibLabel::Gtk;
sub init {
	my $self = shift;
	#my ($dialog,$view,$table,$label) = @_;
	my $f = $self->{f} = new Gtk2::Frame;
	my $l = $self->{l} = new Gtk2::Label($self->{short});
	$l->set_alignment(1.0,0.5);
	$f->set_shadow_type('etched-out');
	$f->add($l);
}
#package MibLabel::Gtk;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}
#package MibLabel::Gtk;
sub add_tooltip {
	my ($self,$tips) = @_;
	my $tip = $self->description(450);
	$tips->set_tip($self->{l},$tip) if $tip;
}

# ------------------------------------------
package MibUnits::Gtk; our @ISA = qw(MibUnits MibInfo::Gtk);
# ------------------------------------------
#package MibUnits::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label) = @_;
	my $f = $self->{f} = new Gtk2::Frame;
	my $u = $self->{u} = new Gtk2::Label($self->{units});
	$u->set_alignment(0.0,0.5);
	$f->set_shadow_type('etched-out');
	$f->add($u);
}
#package MibUnits::Gtk;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}
#package MibUnits::Gtk;
sub add_tooltip {
	my ($self,$tips) = @_;
	my $tip = $self->TCDescription(450);
	$tips->set_tip($self->{u},$tip) if $tip;
}

# ------------------------------------------
package MibNode::Gtk; our @ISA = qw(MibNode MibInfo::Gtk);
# ------------------------------------------
#package MibNode::Gtk;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}

# ------------------------------------------
package MibNode::Truth::Gtk; our @ISA = qw(MibNode::Truth);
# ------------------------------------------
#package MibNode::Truth::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	my ($f,$e);
	$e = $self->{e} = new Gtk2::ToggleButton();
	$e->set_active(($$val eq 'true(1)' or $$val eq '1') ? 1 : 0);
	$f = $self->{f} = $e;
}

# ------------------------------------------
package MibNode::Boolean::Gtk; our @ISA = qw(MibNode::Boolean);
# ------------------------------------------
#package MibNode::Boolean::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	my ($f,$e);
	$e = $self->{e} = new Gtk2::ToggleButton();
	$e->set_active(($$val eq 'true(1)' or $$val eq '1') ? 1 : 0);
	$f = $self->{f} = $e;
}

# ------------------------------------------
package MibNode::List::Gtk; our @ISA = qw(MibNode::List);
# ------------------------------------------
#package MibNode::List::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	my ($f,$e);
	$f = $self->{f} = new Gtk2::Frame;
	$e = $self->{e} = new Gtk2::SimpleList(tag=>'text',value=>'int');
	$e->set_headers_visible(0);
	$e->set_grid_lines('horizontal');
	@{$e->{data}} = @{$self->{rows}};
	$e->get_selection->set_mode('multiple');
	$e->select(@{$self->{sels}});
	$f->set_shadow_type('in');
	$f->add($e);
}

# ------------------------------------------
package MibNode::Combo::Gtk; our @ISA = qw(MibNode::Combo);
# ------------------------------------------
#package MibNode::Combo::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	my ($f,$e);
	$e = $self->{e} = Gtk2::ComboBox->new_text;
	$e->set_title($label);
	foreach (@{$self->{tags}}) { $e->append_text($_); }
	foreach (@{$self->{sels}}) { $e->set_active($_); }
	my $sens = $self->{editable} ? 'on' : 'off';
	$e->set('button-sensitivity'=>$sens);
	$f = $self->{f} = $e;
}

# ------------------------------------------
package MibNode::Entry::Gtk; our @ISA = qw(MibNode::Entry);
# ------------------------------------------
#package MibNode::Entry::Gtk;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	my ($f,$e,$b);
	$b = $self->{b} = Gtk2::EntryBuffer->new($$val);
	$e = $self->{e} = Gtk2::Entry->new_with_buffer($b);
	$e->set_editable($self->{editable});
	$e->set(visibility=>0) if $self->{password};
	$f = $self->{f} = $e;
}

# ------------------------------------------
package MibEntry::Gtk; our @ISA = qw(MibEntry Gtk2::Window);
# ------------------------------------------
#package MibEntry::Gtk;
sub make {
	my $type = shift;
	my ($view,$table) = @_;
	my $self = new Gtk2::Window('toplevel');
	bless $self,$type;
	return $self;
}
#package MibEntry::Gtk;
sub init {
	my $self = shift;
	my ($view,$table) = @_;
	my $data = $view->{item}{data}{$table};
	my @rows = $self->getrows($table);
	my @cols = $self->getcols($table,$data);
	$self->{view} = $view;
	$self->{table} = $table;
	$self->{data} = $data;
	$self->{rows} = \@rows;
	my $fr = new Gtk2::Frame($table);
	my $ta = new Gtk2::Table(scalar(@rows),scalar(@cols)+2,0);
	$ta->set_col_spacings(0);
	$ta->set_row_spacings(0);
	my $sw = new Gtk2::ScrolledWindow;
	$sw->set_policy((scalar(@cols)>2 ? 'automatic' : 'never'),'automatic');
	$sw->add_with_viewport($ta);
	$fr->add($sw);
	my $tt = $self->{tooltips} = new Gtk2::Tooltips;
	my $i = 0;
	foreach my $row (@rows) {
		my $j = 0;
		my $lab = $self->{labels}{$row} = MibLabel::Gtk->new($self,$view,$table,$row);
		$lab->add_to_table($ta,$i,$j); $j++;
		$lab->add_tooltip($tt);
		foreach my $col (@cols) {
			$col->{$row} = undef unless exists $col->{$row};
			my $val = \$col->{$row};
			my $ent = $self->{entries}{$row}{$j} = MibNode::Gtk->new($self,$view,$table,$row,$val);
			$ent->add_to_table($ta,$i,$j); $j++;
		}
		my $uni = $self->{units}{$row} = MibUnits::Gtk->new($self,$view,$table,$row);
		$uni->add_to_table($ta,$i,$j); $j++;
		$uni->add_tooltip($tt);
		$i++;
	}
	$tt->enable;
	my $bb = new Gtk2::HButtonBox;
	$bb->set_layout('end');
	$bb->set_spacing(10);
	my $b;
	$b = Gtk2::Button->new_from_stock('gtk-refresh');
	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->refresh_view; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-revert-to-saved');
	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->revert; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-apply');
	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->store; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-close');
	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->hide; },$self);
	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-connect');
#	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->connectButton; },$self);
#	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-cancel');
#	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->cancelButton; },$self);
#	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-ok');
#	$b->signal_connect('clicked'=>sub{ my ($b,$self) = @_; return $self->okButton; },$self);
#	$bb->add($b);
	my $vb = new Gtk2::VBox;
	$vb->set_spacing(0);
	$fr->set_border_width(5);
	$vb->pack_start($fr,1,1,0);
	$bb->set_border_width(5);
	$vb->pack_start($bb,0,0,0);
	$self->set_type_hint('normal');
	$self->set_default_size(-1,600);
	$self->set_opacity(0.5);
	$self->set_position('mouse');
	$self->{deleted} = 0;
	$self->signal_connect('delete-event'=>sub{
		my ($self,$ev) = @_;
		$self->hide_all;
		return Gtk2::EVENT_STOP;
	});
	$self->add($vb);
}
#package MibEntry::Gtk;
sub closeButton {
	my ($self,$ev) = @_;
	$self->hide_all;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub applyButton {
	my ($self,$ev) = @_;
	$self->store;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub connectButton {
	my ($self,$ev) = @_;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub refreshButton {
	my ($self,$ev) = @_;
	$self->revert;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub revertButton {
	my ($self,$ev) = @_;
	$self->revert;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub cancelButton {
	my ($self,$ev) = @_;
	$self->hid_all;
	return Gtk2::EVENT_STOP;
}
#package MibEntry::Gtk;
sub okButton {
	my ($self,$ev) = @_;
	$self->store;
	$self->hide_all;
	return Gtk2::EVENT_STOP;
}

# ------------------------------------------
package Datum::Draw::Gtk; our @ISA = qw(Datum::Draw);
# ------------------------------------------
#package Datum::Draw::Gtk;
sub fini {
	my $self = shift;
	foreach my $dia (values %{$self->{mibentry}}) { $dia->destroy }
	delete $self->{mibentry};
}
#package Datum::Draw::Gtk;
sub fillmenu {
	my ($self,$menu) = @_;
	my $gotone = 0;
	if (my $data = $self->{view}{item}{data}) {
		foreach my $table (Model::SNMP::lexical_sort(keys %$data)) {
			next if $table eq 'extra';
			if ($data->{$table} and scalar values %{$data->{$table}}) {
				my $mi = Gtk2::MenuItem->new_with_label("$table...");
				$mi->signal_connect('activate'=>sub{
						my ($item,$args) = @_;
						my ($self,$table) = @$args;
						my $ent = $self->{mibentry}{$table};
						unless ($ent) {
							$ent = $self->{mibentry}{$table} =
							MibEntry::Gtk->new($self,$table);
						}
						$ent->show_all;
						return Gtk2::EVENT_STOP;
					},[$self,$table]);
				$mi->show_all;
				$menu->append($mi);
				$gotone++;
			}
		}
	}
	return $gotone;
}

# ------------------------------------------
package ThreeColumn::Draw::Gtk; our @ISA = qw(ThreeColumn::Draw);
# ------------------------------------------
#package ThreeColumn::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	foreach my $part (qw/tbox lbox cbox rbox/) {
		my $pnts = [0,0,0,0,0,0,0,0];
		my $item = new Goo::Canvas::Polyline(
			$group,1,$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
			'line-width'=>0.1,
			'stroke-color'=>$color,
			'pointer-events'=>'visible',
			'fill-pattern'=>undef,
			 visibility=>'invisible',
		);
		$canvas->setpart($part,$pnts,$item,$self);
	}
}
#package ThreeColumn::Draw::Gtk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/tbox lbox cbox rbox/);
}
#package ThreeColumn::Draw::Gtk;
sub setcolor {
	my ($self,$color) = @_;
	foreach (qw/tbox lbox cbox rbox/) {
		$self->{$_}{item}->set('stroke-color'=>$color);
	}
}
#package ThreeColumn::Draw::Gtk;
sub placing {
	my $self = shift;
	my $color = $self->mycolor;
	my $vis = $self->isvisible ? 'visible' : 'invisible';
	$vis = 'invisible';
	foreach my $obj (qw/tbox cbox lbox rbox/) {
		my ($side) = split(//,$obj,2);
		my $box = $self->{$obj};
		my ($pnts,$item) = ($box->{pnts},$box->{item});
		$item->set(points=>Goo::Canvas::Points->new($pnts),'stroke-color'=>$color,visibility=>$vis);
	}
}

# ------------------------------------------
package Subnetwork::Network::Draw::Gtk; our @ISA = qw(Subnetwork::Network::Draw);
package Network::Network::Draw::Gtk; our @ISA = qw(Network::Network::Draw);
package Private::Network::Draw::Gtk; our @ISA = qw(Private::Network::Draw);
package Private::Here::Network::Draw::Gtk; our @ISA = qw(Private::Here::Network::Draw);
package Local::Network::Draw::Gtk; our @ISA = qw(Local::Network::Draw);
package Local::Here::Network::Draw::Gtk; our @ISA = qw(Local::Here::Network::Draw);
package Host::Network::Draw::Gtk; our @ISA = qw(Host::Network::Draw);
package Host::Ip::Network::Draw::Gtk; our @ISA = qw(Host::Ip::Network::Draw);
package Host::Ip::Here::Network::Draw::Gtk; our @ISA = qw(Host::Ip::Here::Network::Draw);
package Subnet::Network::Draw::Gtk; our @ISA = qw(Subnet::Network::Draw);
package Lan::Network::Draw::Gtk; our @ISA = qw(Lan::Network::Draw);
package Vlan::Network::Draw::Gtk; our @ISA = qw(Vlan::Network::Draw);
package Address::Network::Draw::Gtk; our @ISA = qw(Address::Network::Draw);
package Port::Network::Draw::Gtk; our @ISA = qw(Port::Network::Draw);
package Vprt::Network::Draw::Gtk; our @ISA = qw(Vprt::Network::Draw);
package Route::Network::Draw::Gtk; our @ISA = qw(Route::Network::Draw);
# ------------------------------------------

# ------------------------------------------
package Driv::Driv::Draw::Gtk; our @ISA = qw(Driv::Driv::Draw);
# ------------------------------------------

# ------------------------------------------
package Card::Driv::Draw::Gtk; our @ISA = qw(Card::Driv::Draw);
# ------------------------------------------
#package Card::Driv::Draw::Gtk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my ($x,$y) = (-107.0/2.0,-95.0/2.0);
	my ($item,$pnts);
	$pnts = [
		$x+  0,$y+ 0,
		$x+130,$y+ 0,
		$x+130,$y+87,
		$x+122,$y+87,
		$x+122,$y+95,
		$x+107,$y+95,
		$x+107,$y+86,
		$x+105,$y+86,
		$x+105,$y+95,
		$x+ 58,$y+95,
		$x+ 58,$y+86,
		$x+ 56,$y+86,
		$x+ 56,$y+95,
		$x+ 41,$y+95,
		$x+ 41,$y+87,
		$x+ 16,$y+87,
		$x+ 16,$y+95,
		$x+  0,$y+95,
	];
	$item = new Goo::Canvas::Polyline(
			$group,0,$pnts,
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
			'line-width'=>0.4,
#'line-cap'=>'round',
			'stroke-color'=>$color,
			'fill-color'=>'SeaGreen',
			'pointer-events'=>'visible',
	);
	$canvas->setpart('otl',$pnts,$item,$self);
}

# ------------------------------------------
package Span::Driv::Draw::Gtk; our @ISA = qw(Span::Driv::Draw);
package Chan::Driv::Draw::Gtk; our @ISA = qw(Chan::Driv::Draw);
package Xcon::Driv::Draw::Gtk; our @ISA = qw(Xcon::Driv::Draw);
# ------------------------------------------

# ------------------------------------------
package Window::Gtk; our @ISA = qw(Window);
# ------------------------------------------
#package Window::Gtk;
sub init {
	my $self = shift;
	my ($gui,$id) = @_;
	my $wind = $self->{wind} = new Gtk2::Window('toplevel');
	my $mb = new Gtk2::MenuBar();
	my $me = new Gtk2::Menu();
	my $mi;
	$mi = new_from_stock Gtk2::ImageMenuItem('gtk-new',undef);
	$mi->signal_connect_swapped('activate'=>$self->can('menuSnmpNew'),$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-open',undef);
	$mi->signal_connect_swapped('activate'=>$self->can('menuSnmpOpen'),$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-close',undef);
	$mi->signal_connect_swapped('activate'=>$self->can('menuSnmpClose'),$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-quit',undef);
	$mi->signal_connect_swapped('activate'=>$self->can('menuSnmpQuit'),$self);
	$me->append($mi);
	my $mc = new Gtk2::MenuItem('_SNMP');
	$mc->set_submenu($me);
	$mb->append($mc);
	my $nb = new Gtk2::Notebook();
	$nb->popup_disable;
	my $vb = new Gtk2::VBox();
	$vb->set_spacing(0);
	$vb->pack_start($mb,0,0,0);
	$vb->pack_start($nb,1,1,0);
	$wind->set_type_hint('normal');
	$wind->set_default_size(1000,800);
	my $row = $self->{row};
	if ($self->{readme} and $row->{xpos} and $row->{ypos}) {
		$wind->move($row->{xpos},$row->{ypos});
	}
	if ($self->{readme} and $row->{width} and $row->{height}) {
		$wind->resize($row->{width},$row->{height});
	}
	$wind->set_opacity(0.5);
	$wind->set_position('mouse');
	$wind->signal_connect_swapped('delete-event'=>$self->can('delete_event'),$self);
	$wind->add($vb);
	$wind->show_all;
	$self->{note} = $nb;
}
#package Window::Gtk;
sub main_quit { Gtk2->main_quit }
#package Window::Gtk;
sub Canvas {
	my $self = shift;
	# TODO: complete this
}
#package Window::Gtk;
sub saveit {
	my $self = shift;
	my $row = $self->{row};
	my $wind = $self->{wind};
	($row->{xpos},$row->{ypos}) = $wind->get_position;
	($row->{width},$row->{height}) = $wind->get_size;
	return $self->SUPER::saveit;
}
#package Window::Gtk;
sub saveme {
	my $self = shift;
	my $row = $self->{row};
	my $wind = $self->{wind};
	($row->{xpos},$row->{ypos}) = $wind->get_position;
	($row->{width},$row->{height}) = $wind->get_size;
	return $self->SUPER::saveme;
}
