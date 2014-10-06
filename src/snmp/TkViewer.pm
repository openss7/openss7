require threads;
require threads::shared;
require Thread;
#use Thread qw/:DEFAULT async yield/;
require Thread::Queue;
use strict;
use warnings;
require Carp;
require Tk;
require Tk::Xrm;
require Tk::Event;
require Tk::Trace;
require Tk::Toplevel;
require Tk::Adjuster;
require Tk::Dialog;
require Tk::HList;
require Tk::ItemStyle;
require Tk::ROText;
require Tk::NoteBook;
require Tk::Pane;
require Tk::Balloon;
require Tk::DragDrop;
require Tk::DropSite;
require Tk::TableMatrix;
require Tk::TableMatrix::Spreadsheet;
require Tk::Frame;
require Tk::TFrame;
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
package Gui::Tk; our @ISA = qw(Gui);
# ------------------------------------------
#package Gui::Tk;
sub mainloop {
	my $self = shift;
	$self->start_timeout;
	Tk::MainLoop();
}
#package Gui::Tk;
sub start_idle {
	my $self = shift;
	my $tag = delete $self->{tag};
	$tag->cancel if defined $tag;
	my @windows = values %{$self->{gobjs}{Window}};
	my $mw = $windows[0];
	if ($mw) {
		my $wind = $mw->{wind};
		$tag = $wind->afterIdle([$self->can('do_idle'),$self]);
		$self->{tag} = $tag;
	} else {
		Carp::confess "No main window";
	}
}
#package Gui::Tk;
sub start_quick {
	my $self = shift;
	my $tag = delete $self->{tag};
	$tag->cancel if defined $tag;
	my @windows = values %{$self->{gobjs}{Window}};
	my $mw = $windows[0];
	if ($mw) {
		my $wind = $mw->{wind};
		$tag = $wind->after(100,[$self->can('do_quick'),$self]);
		$self->{tag} = $tag;
	} else {
		Carp::confess "No main window";
	}
}
#package Gui::Tk;
sub start_timeout {
	my $self = shift;
	my $tag = delete $self->{tag};
	$tag->cancel if defined $tag;
	my @windows = values %{$self->{gobjs}{Window}};
	my $mw = $windows[0];
	if ($mw) {
		my $wind = $mw->{wind};
		$tag = $wind->after(1000,[$self->can('do_timeout'),$self]);
		$self->{tag} = $tag;
	} else {
		Carp::confess "No main window";
	}
}

# ------------------------------------------
package Viewer::Tk; our @ISA = qw(Viewer);
package Viewer::Internet::Tk; our @ISA = qw(Viewer::Internet);
package Viewer::Driv::Tk; our @ISA = qw(Viewer::Driv);
# ------------------------------------------

# ------------------------------------------
package Canvas::Tk; our @ISA = qw(Canvas);
# ------------------------------------------
#package Canvas::Tk;
sub init {
	my $self = shift;
	my ($gui,$id,$window,$viewer) = @_;
	$window = $self->{window} unless $window;
	my $note = $self->{note} = $window->{note};
	my $name = 'Canvas-'.$self->{no};
	my $page = $self->{page} = $note->add($name,-label=>'canvas');
	print STDERR "Creating canvas...\n";
	my $canv;
	if (1) {
		my $scro = $self->{scro} = $page->Scrolled('Canvas',
			-scrollbars=>'se',
			-height=>800,
			-width=>1024,
			-confine=>1,
			-scrollregion=>[-512,-400,512,400],
			-xscrollincrement=>25,
			-yscrollincrement=>25,
			-background=>'white',
		)->pack(
			-expand=>1,
			-fill=>'both',
			-side=>'left',
			-anchor=>'nw',
		);
		print STDERR "Manager for page is ",$page->manager,"\n";
		$canv = $self->{canv} = $scro->Subwidget('scrolled');
	} else {
		$canv = $self->{canv} = $page->Canvas(
			-height=>800,
			-width=>1024,
			-confine=>1,
			-scrollregion=>[-512,-400,512,400],
			-xscrollincrement=>25,
			-yscrollincrement=>25,
			-background=>'white',
		)->pack(
			-expand=>1,
			-fill=>'both',
			-side=>'left',
			-anchor=>'w',
		);
	}
	$note->raise($name);
	$canv->update;
	$viewer = $self->{viewer} unless $viewer;
	$self->{balloon} = $canv->toplevel->Balloon(-statusbar=>$window->{StatusBar});
	$self->{bmsgs} = {};
	$self->{items} = {};
	$canv->CanvasBind('<Configure>',[$self->can('canv_configure'),$self,Tk::Ev('w'),Tk::Ev('h')]);
	$canv->CanvasBind('<ButtonPress-1>',[$self->can('canv_buttonpress1'),$self,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$canv->CanvasBind('<ButtonPress-2>',[$self->can('canv_buttonpress2'),$self,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$canv->CanvasBind('<ButtonPress-3>',[$self->can('canv_buttonpress3'),$self,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$self->{layers} = { map {$_=>0} $viewer->kinds };
}
#package Canvas::Tk;
sub fini {
	my $self = shift;
	delete $self->{note};
	delete $self->{page};
	delete $self->{scro};
	delete $self->{canv};
	delete $self->{window};
	delete $self->{viewer};
}
#package Canvas::Tk;
sub refresh {
	my $self = shift;
	$self->{canv}->update;
}
#package Canvas::Tk;
sub getlayer {
	my ($self,$layer) = @_;
	$self->{layers}{$layer} = 0 unless exists $self->{layers}{$layer};
	return $layer;
}
#package Canvas::Tk;
sub restack {
	my $self = shift;
	my $canv = $self->{canv};
	foreach my $layer (map {$self->{layers}{$_}>0?$_:()} $self->{viewer}->kinds) {
		$canv->raise($layer);
	}
	$canv->update;
	$self->{balloon}->attach($canv,
		-balloonposition=>'mouse',
		-msg=>$self->{bmsgs},
		);
}
#package Canvas::Tk;
sub setitem {
	my ($self,$item,$draw,$bmsg) = @_;
	my $ward = delete $self->{items}{$item};
	$self->{items}{$item} = $draw;
	$self->{bmsgs}{$item} = $bmsg if $bmsg;
	my $kind = $draw->kind;
	$self->{layers}{$kind}++ unless $ward;
	$self->restack;
}
#package Canvas::Tk;
sub putitem {
	my ($self,$item,$group) = @_;
	my $canv = $self->{canv};
	if (my $draw = delete $self->{items}{$item}) {
		delete $self->{bmsgs}{$item};
		my $kind = $draw->kind;
		$self->{layers}{$kind}--;
		$canv->delete($item);
	}
}
#package Canvas::Tk;
sub setpart {
	my ($self,$part,$pnts) = (shift,shift,shift);
	my ($item,$draw,$bmsg) = @_;
	$draw->{$part} = {pnts=>$pnts,item=>$item};
	$self->setitem(@_);
}
#package Canvas::Tk;
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
#package Canvas::Tk;
sub canv_configure {
	my ($canv,$self,$w,$h) = @_;
	$self->{w} = $canv->width;
	$self->{h} = $canv->height;
}
#package Canvas::Tk;
sub canv_buttonpress1 {
	my ($canv,$self,$X,$Y,$x,$y) = @_;
	Carp::cluck "call trace";
	my $cx = $canv->canvasx($x,1);
	my $cy = $canv->canvasy($y,1);
	my @items = $canv->find('overlapping',$cx-2,$cy-2,$cx+2,$cy+2);
	if (@items) {
		my $item = $items[-1];
		if (my $draw = $self->{items}{$item}) {
			$draw->{view}{item}->show;
		}
	}
}
#package Canvas::Tk;
sub canv_buttonpress2 {
	my ($canv,$self,$X,$Y,$x,$y) = @_;
	Carp::cluck "call trace";
	my $cx = $canv->canvasx($x,1);
	my $cy = $canv->canvasy($y,1);
	my @items = $canv->find('overlapping',$cx-2,$cy-2,$cx+2,$cy+2);
	if (@items) {
		my $item = $items[-1];
		if (my $draw = $self->{items}{$item}) {
			$draw->{view}{item}->show;
		}
	}
}
#package Canvas::Tk;
sub canv_buttonpress3 {
	my ($canv,$self,$X,$Y,$x,$y) = @_;
	Carp::cluck "call trace";
	my $cx = $canv->canvasx($x,1);
	my $cy = $canv->canvasy($y,1);
	my @items = $canv->find('overlapping',$cx-2,$cy-2,$cx+2,$cy+2);
	if (@items) {
		my $item = $items[-1];
		my $draw = $self->{items}{$item};
		Carp::carp "found item $draw" if $draw;
	} else {
		Carp::carp "Need to create popup menu...";
	}
}
#package Canvas::Tk;
sub getdims {
	my $self = shift;
	my $canv = $self->{canv};
	my $hbox = $canv->screenmmheight;
	my $wbox = $canv->screenmmwidth;
	return ($hbox,$wbox);
}

# ------------------------------------------
package Draw::Tk; our @ISA = qw(Draw);
# ------------------------------------------
sub n2m { map {sprintf('%gm',$_)} @_ }

# ------------------------------------------
package Group::Draw::Tk; our @ISA = qw(Group::Draw);
# ------------------------------------------
#package Group::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $layer = $self->{layer} = $canvas->getlayer($self->kind);
	my $grp = $self->{grp} = {pnts=>[0,0]};
	my $item = $grp->{item} = $self->{view}->tag;
}
#package Group::Draw::Tk;
sub fini {
	my $self = shift;
	my $layer = delete $self->{layer};
	foreach (qw/grp/) { delete $self->{$_} }
}
#package Group::Draw::Tk;
sub placing {
	my $self = shift;
	my $vis = $self->isvisible ? 'normal' : 'hidden';
	my $canv = $self->{canvas}{canv};
	$canv->itemconfigure($self->{grp}{item},-state=>$vis);
}

# ------------------------------------------
package Node::Draw::Tk; our @ISA = qw(Node::Draw);
# ------------------------------------------
#package Node::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $canv = $canvas->{canv};
	my $layer = $self->{layer};
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my $text = $self->gettxt;
	my ($item,$pnts,@coords);
	$pnts = [-8.0,-4.5,16,9];
	@coords = (-8.0,-4.5,8.0,4.5);
	my $nod = $item = $canv->createRectangle(Draw::Tk::n2m(@coords),
		-fill=>'white',
		-activefill=>'white',
		-disabledfill=>'white',
		-outline=>$color,
		-activeoutline=>'cyan',
		-disabledoutline=>'lightgrey',
		-width=>'0.4m',
		-activewidth=>'0.6m',
		-disabledwidth=>'0.3m',
		-state=>'hidden',
		-tags=>[$group,'nod',$layer],
		 #-updatecommand=>sub{ shift->itemconfigure(shift,-color=>$self->mycolor); },
	);
	$canvas->setpart('nod',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,20];
	@coords = (0,0);
	$item = $canv->createText(Draw::Tk::n2m(@coords),
		-anchor=>'center',
		-fill=>$color,
		-activefill=>'cyan',
		-disabledfill=>'lightgrey',
		#-font=>???, # use default for now
		-justify=>'center',
		-state=>'hidden',
		-tags=>[$group,'txt',$layer],
		-text=>$text,
		 #-updatecommand=>sub{ shift->itemconfigure(shift,-text=>$self->gettxt); },
		-width=>Draw::Tk::n2m($pnts->[2]),
	);
	$canv->raise($item,$nod);
	$canvas->setpart('txt',$pnts,$item,$self,\$self->{text});
	$self->{pad} = [1.0,1.0];
}
#package Node::Draw::Tk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/nod txt/);
}
#package Node::Draw::Tk;
sub setcolor {
	my ($self,$color) = @_;
	my $canv = $self->{canvas}{canv};
	$canv->itemconfigure($self->{nod}{item},-outline=>$color);
	$canv->itemconfigure($self->{txt}{item},-fill=>$color);
}
#package Node::Draw::Tk;
sub placing {
	my $self = shift;
	my $canv = $self->{canvas}{canv};
	my $color = $self->mycolor;
	my $txt = $self->{txt};
	my $pnts = $txt->{pnts};
	my @coords = @$pnts[0..1];
	$canv->coords($txt->{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($txt->{item},-fill=>$color);
	my $nod = $self->{nod};
	$pnts = $nod->{pnts};
	@coords = (
		$pnts->[0],
		$pnts->[1],
		$pnts->[0]+$pnts->[2],
		$pnts->[1]+$pnts->[3],
		);
	$canv->coords($nod->{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($nod->{item},-outline=>$color);
}

# ------------------------------------------
package Bus::Draw::Tk; our @ISA = qw(Bus::Draw);
# ------------------------------------------
#package Bus::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $canv = $canvas->{canv};
	my $layer = $self->{layer};
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my ($item,$pnts,@coords);
	$pnts = [0,-10,0,10];
	@coords = (0,-10,0,10);
	$item = $canv->createLine(Draw::Tk::n2m(@coords),
		-capstyle=>'round',
		-fill=>$color,
		-activefill=>'cyan',
		-disabledfill=>'lightgrey',
		-joinstyle=>'round',
#-smooth=>1,
#-splinesteps=>32,
		-width=>'0.6m',
		-activewidth=>'0.8m',
		-disabledwidth=>'0.4m',
		-state=>'hidden',
		-tags=>[$group,'bus',$layer],
	);
	$canvas->setpart('bus',$pnts,$item,$self,\$self->{text});
	# NOTE: we can't rotate text like we do on Gtk, but we can do balloon
	# text instead, so we do.  Zero the length of text so that the margin
	# caculations are correct
	$self->{txt} = {pnts=>[0,0,0]};
	$self->{txb} = {pnts=>[0,0,0]};
	$self->{pad} = [1.0,1.0];
}
#package Bus::Draw::Tk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/bus txt txb/);
}
#package Bus::Draw::Tk;
sub setcolor {
	my ($self,$color) = @_;
	my $canv = $self->{canvas}{canv};
	$canv->itemconfigure($self->{bus}{item},-fill=>$color);
}
#package Bus::Draw::Tk;
sub placing {
	my $self = shift;
	my $canv = $self->{canvas}{canv};
	my $color = $self->mycolor;
	my $text = $self->gettxt;
	my ($x,$y) = $self->pos;
	my ($h,$w) = @{$self->{siz}{t}};
	my $bus = $self->{bus};
	my @coords = @{$bus->{pnts}}[0..3];
	$canv->coords($bus->{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($bus->{item},-fill=>$color);
}

# ------------------------------------------
package BoxAndLink::Draw::Tk; our @ISA = qw(BoxAndLink::Draw);
# ------------------------------------------
#package BoxAndLink::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $canv = $canvas->{canv};
	my $layer = $self->{layer};
	my $color = $view->mycolor;
	my $group = $self->{grp}{item};
	my $text = $self->gettxt;
	my ($item,$pnts,@coords);
	$pnts = [-1,-1,2,2];
	@coords = (-1,-1,1,1);
	my $box = $item = $canv->createRectangle(Draw::Tk::n2m(@coords),
		-fill=>'white',
		-activefill=>'white',
		-disabledfill=>'white',
		-outline=>$color,
		-activeoutline=>'cyan',
		-disabledoutline=>'lightgrey',
		-width=>'0.4m',
		-activewidth=>'0.5m',
		-disabledwidth=>'0.3m',
		-state=>'hidden',
		-tags=>[$group,'box',$layer],
		 #-updatecommand=>sub{ shift->itemconfigure(shift,-color=>$self->mycolor); },
	);
	$canvas->setpart('box',$pnts,$item,$self,\$self->{text});
	$pnts = [0,0,5,0,10,0];
	@coords = @$pnts;
	$item = $canv->createLine(Draw::Tk::n2m(@coords),
		-capstyle=>'round',
		-fill=>$color,
		-activefill=>'cyan',
		-disabledfill=>'lightgrey',
		-joinstyle=>'round',
#-smooth=>1,
#-splinesteps=>32,
		-width=>'0.4m',
		-activewidth=>'0.5m',
		-disabledwidth=>'0.3m',
		-state=>'hidden',
		-tags=>[$group,'lnk',$layer],
	);
	$canv->lower($item,$box);
	$canvas->setpart('lnk',$pnts,$item,$self,\$self->{text});
	$pnts = [5,0,20,'sw',-justify=>'left'];
	@coords = @$pnts[0..1];
	$item = $canv->createText(Draw::Tk::n2m(@coords),
		-anchor=>'sw',
		-fill=>$color,
		-activefill=>'cyan',
		-disabledfill=>'lightgrey',
		#-font=>???, # use default for now
		-justify=>'left',
		-state=>'hidden',
		-tags=>[$group,'txt',$layer],
		-text=>$text,
		 #-updatecommand=>sub{ shift->itemconfigure(shift,-text=>$self->gettxt); },
		-width=>Draw::Tk::n2m($pnts->[2]),
	);
	$canv->raise($item,$box);
	$canvas->setpart('txt',$pnts,$item,$self,\$self->{text});
	$self->{pad} = [1.0,1.0];
}
#package BoxAndLink::Draw::Tk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/box lnk txt/);
}
#package BoxAndLink::Draw::Tk;
sub setcolor {
	my ($self,$color) = @_;
	my $canv = $self->{canvas}{canv};
	$canv->itemconfigure($self->{box}{item},-outline=>$color);
	$canv->itemconfigure($self->{lnk}{item},-fill=>$color);
	$canv->itemconfigure($self->{txt}{item},-fill=>$color);
}
#package BoxAndLink::Draw::Tk;
sub placing {
	my $self = shift;
	my $canv = $self->{canvas}{canv};
	my $color = $self->mycolor;
	my @coords = @{$self->{lnk}{pnts}};
	$canv->coords($self->{lnk}{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($self->{lnk}{item},-fill=>$color);
	@coords = @{$self->{txt}{pnts}}[0..1];
	$canv->coords($self->{txt}{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($self->{txt}{item},
		-fill=>$color,
		-anchor=>$self->{txt}{pnts}[3],
		-justify=>$self->{txt}{pnts}[5],
		-text=>$self->gettxt,
		);
	@coords = @{$self->{box}{pnts}};
	$coords[2] = $coords[0]+$coords[2];
	$coords[3] = $coords[1]+$coords[3];
	$canv->coords($self->{box}{item},Draw::Tk::n2m(@coords));
	$canv->itemconfigure($self->{box}{item},-outline=>$color);
}

# ------------------------------------------
package Datum::Draw::Tk; our @ISA = qw(Datum::Draw);
# ------------------------------------------
#package Datum::Draw::Tk;
sub fini {
	my $self = shift;
	foreach my $dia (values %{$self->{mibentry}}) { $dia->destroy }
	delete $self->{mibentry};
}
#package Datum::Draw::Tk;
sub fillmenu {
	my ($self,$menu) = @_;
	return 0;
}

# ------------------------------------------
package ThreeColumn::Draw::Tk; our @ISA = qw(ThreeColumn::Draw);
# ------------------------------------------
#package ThreeColumn::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $canv = $canvas->{canv};
	Carp::confess "no canvas" unless $canv;
	my $layer = $self->{layer};
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	foreach my $part (qw/tbox lbox cbox rbox/) {
		my $pnts = [0,0,0,0,0,0,0,0];
		my $item = $canv->createPolygon(Draw::Tk::n2m(@$pnts),
			-fill=>undef,
			-activefill=>undef,
			-disabledfill=>undef,
			-joinstyle=>'round',
			-outline=>$color,
			-activeoutline=>'cyan',
			-disabledoutline=>'lightgrey',
			-width=>'0.1m',
			-activewidth=>'0.1m',
			-disabledwidth=>'0.1m',
			-state=>'hidden',
			-tags=>[$group,$part,$layer],
		);
		$canvas->setpart($part,$pnts,$item,$self);
	}
}
#package ThreeColumn::Draw::Tk;
sub fini {
	my $self = shift;
	$self->{canvas}->putparts($self,$self->{grp}{item},qw/tbox lbox cbox rbox/);
}
#package ThreeColumn::Draw::Tk;
sub setcolor {
	my ($self,$color) = @_;
	my $canv = $self->{canvas}{canv};
	foreach (qw/tbox lbox cbox rbox/) {
		$canv->itemconfigure($self->{$_}{item},-outline=>$color);
	}
}
#package ThreeColumn::Draw::Tk;
sub placing {
	my $self = shift;
	my $canv = $self->{canvas}{canv};
	my $color = $self->mycolor;
	foreach my $obj (qw/tbox cbox lbox rbox/) {
		my ($side) = split(//,$obj,2);
		my $box = $self->{$obj};
		my ($pnts,$item) = ($box->{pnts},$box->{item});
		$canv->coords($item,Draw::Tk::n2m(@$pnts));
		$canv->itemconfigure($item,-outline=>$color,-state=>'hidden');
	}
}

# ------------------------------------------
package Subnetwork::Internet::Draw::Tk; our @ISA = qw(Subnetwork::Internet::Draw);
package Internet::Internet::Draw::Tk; our @ISA = qw(Internet::Internet::Draw);
package Private::Internet::Draw::Tk; our @ISA = qw(Private::Internet::Draw);
package Private::Here::Internet::Draw::Tk; our @ISA = qw(Private::Here::Internet::Draw);
package Local::Internet::Draw::Tk; our @ISA = qw(Local::Internet::Draw);
package Local::Here::Internet::Draw::Tk; our @ISA = qw(Local::Here::Internet::Draw);
package Host::Internet::Draw::Tk; our @ISA = qw(Host::Internet::Draw);
package Host::Ip::Internet::Draw::Tk; our @ISA = qw(Host::Ip::Internet::Draw);
package Host::Ip::Here::Internet::Draw::Tk; our @ISA = qw(Host::Ip::Here::Internet::Draw);
package Subnet::Internet::Draw::Tk; our @ISA = qw(Subnet::Internet::Draw);
package Lan::Internet::Draw::Tk; our @ISA = qw(Lan::Internet::Draw);
package Vlan::Internet::Draw::Tk; our @ISA = qw(Vlan::Internet::Draw);
package Address::Internet::Draw::Tk; our @ISA = qw(Address::Internet::Draw);
package Port::Internet::Draw::Tk; our @ISA = qw(Port::Internet::Draw);
package Vprt::Internet::Draw::Tk; our @ISA = qw(Vprt::Internet::Draw);
package Route::Internet::Draw::Tk; our @ISA = qw(Route::Internet::Draw);
# ------------------------------------------

# ------------------------------------------
package Driv::Driv::Draw::Tk; our @ISA = qw(Driv::Driv::Draw);
# ------------------------------------------

# ------------------------------------------
package Card::Driv::Draw::Tk; our @ISA = qw(Card::Driv::Draw);
# ------------------------------------------
#package Card::Driv::Draw::Tk;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	my $canv = $canvas->{canv};
	my $layer = $self->{layer};
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
	$item = $canv->createPolygon(Draw::Tk::n2m(@$pnts),
		-fill=>'SeaGreen',
		-activefill=>'SeaGreen',
		-disabledfill=>'lightgrey',
		-joinstyle=>'round',
		-outline=>$color,
		-activeoutline=>'cyan',
		-disabledoutline=>'lightgrey',
		-width=>'0.4m',
		-activewidth=>'0.6m',
		-disabledwidth=>'0.2m',
		-state=>'hidden',
		-tags=>[$group,'otl',$layer],
	);
	$canvas->setpart('otl',$pnts,$item,$self);
}

# ------------------------------------------
package Span::Driv::Draw::Tk; our @ISA = qw(Span::Driv::Draw);
package Chan::Driv::Draw::Tk; our @ISA = qw(Chan::Driv::Draw);
package Xcon::Driv::Draw::Tk; our @ISA = qw(Xcon::Driv::Draw);
# ------------------------------------------

# ------------------------------------------
package Window::Tk; our @ISA = qw(Window);
# ------------------------------------------
#package Window::Tk;
sub destroy {
	my $self = shift;
	$self->{wind}->destroy;
}
#package Window::Tk;
sub init {
	my $self = shift;
	my ($db,$id) = @_;
	my $wind = $self->{wind} = new Tk::MainWindow(-class=>'NETdisc');
	$wind->minsize(600,400);
	$wind->geometry('1024x768');
	if (my $sub = $self->can('wm_delete_window')) {
		$wind->protocol('WM_DELETE_WINDOW',[$sub,$self]);
	}
	if (my $sub = $self->can('wm_save_yourself')) {
		$wind->protocol('WM_SAVE_YOURSELF',[$sub,$self]);
	}
	if (my $sub = $self->can('wm_take_focus')) {
		$wind->protocol('WM_TAKE_FOCUS',[$sub,$self]);
	}
	$wind->client(`hostname -f`);
	$wind->command('netdisc');
	#$wind->overrideredirect(0);
	MyOptions::assign($wind);
	MyPixmaps::assign($wind);
	$wind->iconimage('icon');
	$wind->iconname('Network Discovery Tool');
	my ($mb,$mi,$mc,$m2,$sb,$ba);
	$mb = $wind->Menu(-type=>'menubar');
	$ba = $self->{balloon} = $wind->Balloon();
	$wind->configure(-menu=>$mb);
	my $dialog = $self->{Dialog} = $wind->toplevel->Dialog(
		-buttons=>[qw/Ok Cancel Help/],
		-default_button=>'Cancel',
		-text=>'Please input capture file or interface from which to load.',
		-title=>'Source Selection',
	);
	$mi = $self->{FileMenu} = $mb->Menu(
		-tearoff=>1,
		-title=>'File Menu',
	);
	$mi->add(command=>
			-label=>'New',
			-underline=>0,
			-command=>[$self->can('menuFileNew'),$self],
		);
	$mi->add(command=>
			-label=>'Read...',
			-underline=>0,
			-command=>[$self->can('menuFileRead'),$self],
		);
	$mi->add(command=>
			-label=>'Open...',
			-underline=>0,
			-command=>[$self->can('menuFileOpen'),$self],
		);
	$mi->add(command=>
			-label=>'Play...',
			-underline=>0,
			-command=>[$self->can('menuFilePlay'),$self],
		);
	$mi->add(command=>
			-label=>'Save',
			-underline=>0,
			-command=>[$self->can('menuFileSave'),$self],
		);
	$mi->add(command=>
			-label=>'Save As...',
			-underline=>0,
			-command=>[$self->can('menuFileSaveAs'),$self],
		);
	$mi->add(command=>
			-label=>'Close',
			-underline=>0,
			-command=>[$self->can('menuFileClose'),$self],
		);
	$mi->add('separator');
	$mi->add(command=>
			-label=>'Properties...',
			-underline=>0,
			-command=>[$self->can('menuFileProperties'),$self],
		);
	$mc = $self->{RecentMenu} = $mi->Menu(
		-tearoff=>1,
		-title=>'Recent Files',
	);
	$mi->add(cascade=>
			-menu=>$mc,
			-label=>'Recent Files...',
			-underline=>0,
			-state=>'disabled',
		);
	$mi->add('separator');
	$mi->add(command=>
			-label=>'Exit All',
			-underline=>1,
			-command=>[$self->can('menuFileExitAll'),$self],
		);
	$mi->add(command=>
			-label=>'Debug Dump',
			-underline=>1,
			-command=>[sub {
				my $parent = shift->{FileMenu};
				my $w;
				while ($w = $parent) {
					$parent = $w->parent;
					print $w;
					foreach my $o (@{$w->configure}) {
						print "Option: ".join(', ',@$o);
					}
				}
			}, $self],
		);
	$ba->attach($mi,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"New canvas.",
			"Read an existing capture file.",
			"Open an existing capture file.",
			"Play back a capture file in simulated real-time.",
			"Save configuration.",
			"Save configuration in another file.",
			"Close capture.",
			undef, # separator
			"Show file properties.",
			"Select recent files.",
			undef, # separator
			"Exit all windows.",
			"Dump debuging information to stdout."
		]);
	$mb->add(cascade=>
			-menu=>$mi,
			-label=>'File',
			-underline=>0,
		);
	$mi = $self->{ViewMenu} = $mb->Menu(
		-tearoff=>1,
		-title=>'View Menu',
	);
	$m2 = $mi->Menu(
		-tearoff=>1,
		-title=>'New View Menu',
	);
	$m2->add(command=>
			-label=>'Full view...',
			-underline=>0,
			-command=>[sub{ },$self],
		);
	$m2->add(command=>
			-label=>'Signalling view..',
			-underline=>0,
			-command=>[sub{ },$self],
		);
	$m2->add(command=>
			-label=>'Circuit view..',
			-underline=>0,
			-command=>[sub{ },$self],
		);
	$m2->add(command=>
			-label=>'Transaction view..',
			-underline=>0,
			-command=>[sub{ },$self],
		);
	$ba->attach($m2,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"Full view with all nodes and arcs.",
			"Signalling view with linksets, adjacent SPs\nand signalling paths.",
			"Circuit view with SSPs and circuits.",
			"Transaction view with SSPs, GTTs and SCPs.",
		]);
	$mi->add(cascade=>
			-menu=>$m2,
			-label=>'New view...',
			-underline=>0,
		);
	$mi->add('separator');
	$mi->add(command=>
			-label=>'Zoom In',
			-underline=>5,
			-command=>[sub{ },$self],
		);
	$mi->add(command=>
			-label=>'Zoom Out',
			-underline=>5,
			-command=>[sub{ },$self],
		);
	$ba->attach($mi,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"New...",
			undef, # separator
			"Zoom in on canvas.",
			"Zoom out from canvas.",
		]);
	$mb->add(cascade=>
			-menu=>$mi,
			-label=>'View',
			-underline=>0,
		);
	$mi = $mb->Menu(
		-tearoff=>0,
		-title=>'About Menu',
		-type=>'normal',
	);
	$mi->add(command=>
			-label=>'Version',
			-underline=>0,
			-command=>[$self->can('menuAboutVersion'),$self],
		);
	$mi->add(command=>
			-label=>'Usage',
			-underline=>0,
			-command=>[$self->can('menuAboutUsage'),$self],
		);
	$mi->add(command=>
			-label=>'Copying',
			-underline=>0,
			-command=>[$self->can('menuAboutCopying'),$self],
		);
	$mi->add(command=>
			-label=>'Help',
			-underline=>0,
			-command=>[$self->can('menuAboutHelp'),$self],
		);
	$mb->add('separator');
	$mb->add(cascade=>
			-menu=>$mi,
			-label=>'About',
			-underline=>0,
		);
	my $nb = $self->{note} = $wind->toplevel->NoteBook(
		-ipadx=>0,
		-ipady=>0,
		-tabpadx=>0,
		-tabpady=>0,
		-dynamicgeometry=>0,
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'top',
		-anchor=>'nw',
	);
	$sb = $self->{StatusBar} = $self->{Message} = $wind->toplevel->Message(
		-text=>'Status bar.',
		-justify=>'left',
		-width=>'800',
		-anchor=>'w',
	)->pack(
		-expand=>0,
		-fill=>'x',
		-side=>'bottom',
		-anchor=>'sw',
	);
	$ba->configure(-statusbar=>$sb);
	$wind->bind('<Destroy>'=>[$self->can('delete_event'),$self]);
}
#package Window::Tk;
sub main_quit { }
#package Window::Tk;
sub Canvas {
	my $self = shift;
	# TODO: complete this
}
#package Window::Tk;
sub saveit {
	my $self = shift;
	my $row = $self->{row};
	my $wind = $self->{wind};
	$wind->update;
	($row->{xpos},$row->{ypos}) = ($wind->rootx,$wind->rooty);
	($row->{width},$row->{height}) = ($wind->width,$wind->height);
	return $self->SUPER::saveit;
}
#package Window::Tk;
sub saveme {
	my $self = shift;
	my $row = $self->{row};
	my $wind = $self->{wind};
	$wind->update;
	($row->{xpos},$row->{ypos}) = ($wind->rootx,$wind->rooty);
	($row->{width},$row->{height}) = ($wind->width,$wind->height);
	return $self->SUPER::saveme;
}
#package Window::Tk;
sub menuFileNew {
	my $self = shift;
}
#package Window::Tk;
sub menuFileRead {
	my $self = shift;
}
#package Window::Tk;
sub menuFileOpen {
	my $self = shift;
	my $wind = $self->{wind};
	my $file = $wind->getOpenFile(
		-defaultextension=>'.pcap',
		-initialdir=>'./testdata',
		-initialfile=>'ss7capall.pcap',
		-title=>'Open Dialog',
	);
	return unless $file;
	# TODO: read pcap file
}
#package Window::Tk;
sub menuFilePlay {
	my $self = shift;
	my $wind = $self->{wind};
	my $file = $wind->getOpenFile(
		-defaultextension=>'.pcap',
		-initialdir=>'./testdata',
		-initialfile=>'ss7capall.pcap',
		-title=>'Open Dialog',
	);
	return unless $file;
	# TODO: play pcap file
}
#package Window::Tk;
sub menuFileSave {
	my $self = shift;
}
#package Window::Tk;
sub menuFileSaveAs {
	my $self = shift;
}
#package Window::Tk;
sub menuFileClose {
	my $self = shift;
	my $wind = $self->{wind};
	$wind->destroy;
}

# ------------------------------------------
package MyOptions;
# ------------------------------------------
#package MyOptions;
sub assign {
	my $wind = shift;
	#$wind->optionAdd('*font'=>'-*-helvetica-medium-r-*--*-120-*-*-*-*-*-*');
	#$wind->optionAdd('*.Button.*.font'=>'-*-helvetica-bold-r-*--*-120-*-*-*-*-*-*');
	#$wind->optionAdd('*.Label.*.font'=>'-*-helvetica-bold-r-*--*-120-*-*-*-*-*-*');
	#$wind->optionAdd('*font'=>'Arial 9');
	#$wind->optionAdd('*font'=>'Helvetica 10');
	#$wind->optionAdd('*Button*font'=>'Arial-Bold 9');
	#$wind->optionAdd('*Label*font'=>'Arial-Bold 9');
	#$wind->optionAdd('*disabledBackground'=>'dark grey');
	#$wind->optionAdd('*highlightBackground'=>'light grey');
	#$wind->optionAdd('*highlightThickness'=>1);
	#$wind->optionAdd('*borderThickness'=>1);
	#$wind->optionAdd('*borderWidth'=>1);
	#$wind->optionAdd('*border'=>0);
	#$wind->optionAdd('*Menu*Width'=>1);
	#$wind->optionAdd('*Menu*Height'=>1);
	$wind->optionAdd('*Balloon*font'=>'Arial 8');
	#$wind->optionAdd('*padX'=>0);
	#$wind->optionAdd('*padY'=>0);
	#$wind->optionAdd('*relief'=>'groove');
	$wind->optionAdd('*Scrollbar*Width'=>12);
}

# ------------------------------------------
package MyPixmaps;
# ------------------------------------------
#package MyPixmaps;
our $plusImage = <<'EOF';
/* XPM */
static char * mini_plus_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"                ",
"                ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a    a    ab  ",
"  a    a    ab  ",
"  a  aaaaa  aaaa",
"  a    a    abbb",
"  a    a    ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF
#package MyPixmaps;
our $minusImage = <<'EOF';
/* XPM */
static char * mini_minus_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"                ",
"                ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a  aaaaa  aaaa",
"  a         abbb",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF
#package MyPixmaps;
our $boxImage = <<'EOF';
/* XPM */
static char * mini_box_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"       ab       ",
"       ab       ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         aaaa",
"  a         abbb",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF
#package MyPixmaps;
our $logo8Image = <<'EOF';
/* XPM */
static char * logo8_xpm[] = {
"64 48 424 2",
"  	c None",
". 	c #FFFFFF",
"+ 	c #E4E5F6",
"@ 	c #868BD9",
"# 	c #9EA2E0",
"$ 	c #F2F2F2",
"% 	c #717171",
"& 	c #4C4C4C",
"* 	c #A9A9A9",
"= 	c #E7E8F7",
"- 	c #242EBB",
"; 	c #565EC9",
"> 	c #4B54C6",
", 	c #2832BB",
"' 	c #F7F7FC",
") 	c #EAEAEA",
"! 	c #060606",
"~ 	c #3A3A3A",
"{ 	c #898989",
"] 	c #090909",
"^ 	c #A8A8A8",
"/ 	c #2E38BE",
"( 	c #AEB1E5",
"_ 	c #CCCEEE",
": 	c #3840AE",
"< 	c #121213",
"[ 	c #ACACAC",
"} 	c #171717",
"| 	c #F1F1F1",
"1 	c #DDDEF3",
"2 	c #3841C0",
"3 	c #FAFAFA",
"4 	c #080808",
"5 	c #2B3292",
"6 	c #FCFCFE",
"7 	c #7A7A7A",
"8 	c #9C9C9C",
"9 	c #6269CE",
"0 	c #B5B8E7",
"a 	c #6A6A6A",
"b 	c #4E4E4E",
"c 	c #9499DD",
"d 	c #7E84D6",
"e 	c #353535",
"f 	c #252FBB",
"g 	c #F0F0FA",
"h 	c #DCDCDC",
"i 	c #131313",
"j 	c #4C54C7",
"k 	c #DDDEF4",
"l 	c #2E2E2E",
"m 	c #F9F9F9",
"n 	c #F3F4FB",
"o 	c #373FC0",
"p 	c #F6F6FC",
"q 	c #4D4D4D",
"r 	c #838383",
"s 	c #C3C6EC",
"t 	c #636ACE",
"u 	c #373737",
"v 	c #DBDBDB",
"w 	c #EAEBF8",
"x 	c #343DBF",
"y 	c #FAFAFD",
"z 	c #E4E4E4",
"A 	c #1C1C1C",
"B 	c #5C63CC",
"C 	c #313131",
"D 	c #C7C7C7",
"E 	c #E5E6F7",
"F 	c #2933BC",
"G 	c #FDFDFE",
"H 	c #535353",
"I 	c #5F5F5F",
"J 	c #B0B4E6",
"K 	c #5E65CC",
"L 	c #363636",
"M 	c #BCBCBC",
"N 	c #E3E4F6",
"O 	c #252FBA",
"P 	c #242424",
"Q 	c #F0F0F0",
"R 	c #4D55C7",
"S 	c #3C3C3C",
"T 	c #B7B7B7",
"U 	c #E3E5F6",
"V 	c #2630BB",
"W 	c #383838",
"X 	c #9196DC",
"Y 	c #ACB0E5",
"Z 	c #3B3B3B",
"` 	c #B9B9B9",
" .	c #E6E7F7",
"..	c #2C35BD",
"+.	c #2C2C2C",
"@.	c #929292",
"#.	c #D6D8F2",
"$.	c #4951C6",
"%.	c #343434",
"&.	c #C0C0C0",
"*.	c #333CBF",
"=.	c #1D1D1D",
"-.	c #ECECEC",
";.	c #4049C3",
">.	c #FEFEFE",
",.	c #333333",
"'.	c #CDCDCD",
").	c #FDFDFD",
"!.	c #F0F0F9",
"~.	c #3740C0",
"{.	c #D5D5D9",
"].	c #FBFBFB",
"^.	c #F8F8F8",
"/.	c #7F85D4",
"(.	c #D5D6ED",
"_.	c #D7D7D7",
":.	c #EDEDED",
"<.	c #FCFCFC",
"[.	c #EEEEEE",
"}.	c #E9E9EC",
"|.	c #2C35BB",
"1.	c #535356",
"2.	c #6D6D6D",
"3.	c #DADADA",
"4.	c #D6D6D6",
"5.	c #CFCFCF",
"6.	c #C9C9C9",
"7.	c #C3C3C3",
"8.	c #BEBEBE",
"9.	c #BDBDBD",
"0.	c #A5A7BB",
"a.	c #5A60BA",
"b.	c #292929",
"c.	c #B4B4B4",
"d.	c #989898",
"e.	c #030303",
"f.	c #000000",
"g.	c #767676",
"h.	c #CACACA",
"i.	c #AEAEAE",
"j.	c #0A0A0A",
"k.	c #5D5D5D",
"l.	c #D9D9D9",
"m.	c #F5F5F5",
"n.	c #F3F3F3",
"o.	c #2730BA",
"p.	c #0A0A0B",
"q.	c #B8B8B8",
"r.	c #E6E6E6",
"s.	c #E7E7E7",
"t.	c #E1E1E1",
"u.	c #D5D6DE",
"v.	c #3A43BE",
"w.	c #EBEBEB",
"x.	c #232323",
"y.	c #DEDEDE",
"z.	c #E9E9E9",
"A.	c #E8E8E8",
"B.	c #949494",
"C.	c #9E9E9E",
"D.	c #F7F7F7",
"E.	c #5F66C8",
"F.	c #131317",
"G.	c #DDDDDD",
"H.	c #EFEFEF",
"I.	c #E7E7E8",
"J.	c #4049C1",
"K.	c #414244",
"L.	c #B3B3B3",
"M.	c #6E6E6E",
"N.	c #8E8E8E",
"O.	c #5E5E5E",
"P.	c #A3A3A3",
"Q.	c #F4F4F4",
"R.	c #9CA0D7",
"S.	c #09090F",
"T.	c #3F4596",
"U.	c #73737D",
"V.	c #191919",
"W.	c #151515",
"X.	c #D4D4D4",
"Y.	c #B0B0B0",
"Z.	c #AAAAB3",
"`.	c #111438",
" +	c #E3E3E3",
".+	c #35374D",
"++	c #8183A8",
"@+	c #B1B1B1",
"#+	c #C5C5C5",
"$+	c #7F7F7F",
"%+	c #9A9A9A",
"&+	c #868686",
"*+	c #969696",
"=+	c #4B4B4B",
"-+	c #919191",
";+	c #0B0B0B",
">+	c #414141",
",+	c #282828",
"'+	c #7C7C7D",
")+	c #191E67",
"!+	c #E6E6ED",
"~+	c #1A1A1C",
"{+	c #6F73BA",
"]+	c #141414",
"^+	c #9F9F9F",
"/+	c #575757",
"(+	c #C8C8C8",
"_+	c #B5B5B5",
":+	c #676767",
"<+	c #C6C6C6",
"[+	c #9D9D9D",
"}+	c #999999",
"|+	c #ADADAD",
"1+	c #4F4F4F",
"2+	c #828282",
"3+	c #E2E2E2",
"4+	c #7B7B7B",
"5+	c #8C8C8C",
"6+	c #878787",
"7+	c #404040",
"8+	c #979797",
"9+	c #424242",
"0+	c #242B93",
"a+	c #CCCDE8",
"b+	c #2B2B2B",
"c+	c #5860C4",
"d+	c #505050",
"e+	c #BABABA",
"f+	c #A1A1A1",
"g+	c #C1C1C1",
"h+	c #CECECE",
"i+	c #747474",
"j+	c #494949",
"k+	c #0F0F0F",
"l+	c #5C62B8",
"m+	c #8A8FD2",
"n+	c #E5E5E5",
"o+	c #C4C4C4",
"p+	c #454EC1",
"q+	c #858585",
"r+	c #B6B6B6",
"s+	c #565656",
"t+	c #AFAFAF",
"u+	c #AAAAAA",
"v+	c #2A2A2A",
"w+	c #555555",
"x+	c #757575",
"y+	c #F6F6F6",
"z+	c #070707",
"A+	c #B7B8CD",
"B+	c #4850C1",
"C+	c #E0E0E0",
"D+	c #656565",
"E+	c #474FC0",
"F+	c #464646",
"G+	c #D2D2D2",
"H+	c #D5D5D5",
"I+	c #D0D0D0",
"J+	c #323232",
"K+	c #515151",
"L+	c #5A5A5A",
"M+	c #525252",
"N+	c #5B5B5B",
"O+	c #3942BE",
"P+	c #D7D8E4",
"Q+	c #262626",
"R+	c #5058C3",
"S+	c #4D4E55",
"T+	c #181818",
"U+	c #252525",
"V+	c #737373",
"W+	c #DFDFDF",
"X+	c #4E55C7",
"Y+	c #989CDE",
"Z+	c #5A62CB",
"`+	c #3E3F4A",
" @	c #1E1E1E",
".@	c #C9CCEE",
"+@	c #5057C8",
"@@	c #5B62CB",
"#@	c #9999A6",
"$@	c #454DC5",
"%@	c #E0E1F5",
"&@	c #5D64CC",
"*@	c #EDEEF9",
"=@	c #8C91DB",
"-@	c #545CC9",
";@	c #8B8B8B",
">@	c #5A61CB",
",@	c #707070",
"'@	c #606060",
")@	c #3F3F3F",
"!@	c #7D7D7D",
"~@	c #727272",
"{@	c #111111",
"]@	c #3E47C2",
"^@	c #444444",
"/@	c #5159C8",
"(@	c #909090",
"_@	c #787878",
":@	c #B2B2B2",
"<@	c #D8D8D8",
"[@	c #9B9B9B",
"}@	c #595959",
"|@	c #818181",
"1@	c #5860CB",
"2@	c #6067CD",
"3@	c #C2C2C2",
"4@	c #797979",
"5@	c #CBCBCB",
"6@	c #3A43C1",
"7@	c #E9EAF8",
"8@	c #3E3E3E",
"9@	c #DCDDF4",
"0@	c #989DDF",
"a@	c #D1D1D1",
"b@	c #D9DBF2",
"c@	c #39393A",
"d@	c #656CCE",
"e@	c #DFE0F4",
"f@	c #272727",
"g@	c #2C2D3F",
"h@	c #292E76",
"i@	c #F8F8FC",
"j@	c #414AC3",
"k@	c #FBFBFD",
"l@	c #202020",
"m@	c #A2A2A2",
"n@	c #656CCF",
"o@	c #4F57C8",
"p@	c #B8BBE8",
"q@	c #BFC2EB",
"r@	c #5058C8",
"s@	c #CFD1F0",
"t@	c #8A8A8A",
"u@	c #D9DAF3",
"v@	c #686FD0",
"w@	c #D9DAF2",
"x@	c #F8F9FD",
"y@	c #F5F5FB",
"z@	c #F1F1FA",
"A@	c #F9FAFD",
"B@	c #DADBF3",
"C@	c #7F84D7",
"D@	c #D4D6F1",
"E@	c #7A80D5",
"F@	c #A4A8E2",
"G@	c #9DA2E0",
"H@	c #858AD9",
"I@	c #BDC0EA",
"J@	c #848AD8",
"K@	c #D7D9F2",
"L@	c #C0C3EB",
"M@	c #A0A4E1",
"N@	c #ABAFE5",
"O@	c #D2D4F1",
"P@	c #8F94DC",
"Q@	c #A5A9E3",
"R@	c #F9F9FD",
"S@	c #9A9EDF",
"T@	c #D1D3F0",
"U@	c #D9DBF3",
"V@	c #9095DC",
"W@	c #F2F3FB",
"X@	c #DBDCF3",
"Y@	c #E1E2F5",
"Z@	c #8A8FDA",
"`@	c #CBCDEF",
" #	c #EFF0FA",
".#	c #EEEFF9",
"+#	c #B9BCE9",
"@#	c #D3D5F1",
"##	c #EAEAF8",
"$#	c #F1F2FA",
"%#	c #CBCDEE",
"&#	c #CDCFEF",
"*#	c #989CDF",
"=#	c #BCBFEA",
"-#	c #6D74D1",
";#	c #CCCEEF",
">#	c #D0D2F0",
",#	c #C8CBED",
"'#	c #A1A6E2",
")#	c #C4C7EC",
"!#	c #A7ABE3",
"~#	c #C1C4EB",
"{#	c #797FD5",
"]#	c #5159C9",
"^#	c #C7CAED",
"/#	c #555CCA",
"(#	c #E2E3F6",
"_#	c #5860CA",
":#	c #ABAEE4",
"<#	c #E8E9F7",
"[#	c #9DA1E0",
"}#	c #D8DAF3",
"|#	c #B7BAE8",
"1#	c #6E75D1",
"2#	c #C5C7EC",
"3#	c #8F94DB",
"4#	c #B5B8E8",
"5#	c #A1A5E1",
"6#	c #DADCF3",
"7#	c #6168CD",
"8#	c #7177D2",
"9#	c #CFD2F0",
"0#	c #C6C8ED",
"a#	c #ECEDF9",
"b#	c #BBBEE9",
"c#	c #7F85D7",
"d#	c #8186D7",
"e#	c #D5D6F1",
"f#	c #9CA1E0",
"g#	c #BABDE9",
"h#	c #969ADE",
"i#	c #C9CBEE",
"j#	c #BEC1EA",
"k#	c #878CD9",
"l#	c #999EDF",
"m#	c #7B81D6",
"n#	c #969BDE",
"o#	c #EFEFFA",
"p#	c #B9BDE9",
"q#	c #898FDA",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . + @ # . . . $ % & * . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . = - ; > , ' ) ! ~ { ] ^ . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . / ( . . _ : < [ . . $ } | . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . 1 2 . . . 3 4 5 6 . . . 7 8 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . 9 0 . . . a b c d . . . 3 e . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . f g . . h i . . j k . . . l m . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . n o p . . q r . . s t . . . u v . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . w x y . z A . . . . B = . . C D . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . E F G . H I . . . . J K . . L M . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . N O . . P Q . . . . . R . . S T . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . U V . * W . . . . . . X Y . Z ` . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . .  ...G +.@.. . . . . . #.$.. %.&.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . w *.y =.-.. . . . . . . ;.>.,.'.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . >.>.).).!.~.{.C ].].].3 m m ^.^./.(.W _.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.:.3 3 m m :.].].].].<.).>.. . . . . . . . . ",
". . ).^.$ [.:.}.|.1.2.3.3.4.5.6.7.8.9.0.a.b.c.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.d.e.f.g.h.i.j.f.k.l.L f.f.f.i >.. . . . . . . . ",
". . ].3 m m.n.Q o.p.q.r.r.s.s.s.z t.t.u.v.P t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.b.h h f.w.x.y.| e.^.z.A.) B.C.. . . . . . . . . ",
". >.].].].^.D.$ E.F.G.H.H.H.H.[.) z.z.I.J.K.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.L.M.[.:.N.) O.m.m.P.m ^.^.D.x.).. . . . . . . . . ",
". >.m m m m m Q.R.S.$ $ $ $ $ [.-.-.-.-.T.U.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-._.V._.$ $ | W.X.m m m m ^.7 Y.. . . . . . . . . . ",
". . <.m.$ | | Q Z.`. + + + + +y.y.y.y.y..+++y.@+` 7 @+y.y.#+$+%+y._.c.&+*+y.y.y.$+4 =+s.z.-+;+>+Q.m m T ,+z.. . . . . . . . . . ",
". . . . ].D.D.D.'+)+!+[.[.[.w.A.A.A.A.A.~+{+A.]+^+D /+(+_+:+<+[+}+|+1+D 2+8.A.A.A.3+4+5+H.H.) 6+O.^.m 7+8+:.. . . . . . . . . . ",
". . . . ).m ^.^.9+0+a+$ n.$ :.[.[.[.[.3+b+c+[.d+[.[.[.*+7+6.e+` >+% <+[.'.f+:.g+[.[.h+i+9.n.$ h.7+^.$ j+. . . . . . . . . . . . ",
">.3 3 3 D.m.H.[.k+l+m+z.z.n+z z z z z o+j+p+` q+z z 5.r+s+t+u+t+8.+.3+z @+_.z v+z.z.w+v j.Q.n.x+7.^.C _.. . . . . . . . . . . . ",
">.m D.y+Q H.w.A.z+A+B+C+y.3.3.3.3.3.3.D+_+E+&+j+3.D C _.F+G+H+M.I++.3.3.4+3.3.J+K+K+L+A.7+b O.M+^.n+N+. . . . . . . . . . . . . ",
". >.m Q.$ $ $ ) ]+v O+P+n+ + + + + + +Q+3+R+S+}+T+>+4. +6.U++.7. +V+ + +i+ + +C+X.W+[.[.w.G.s.m 3 D.3 . . . . . . . . . . . . . ",
". . . . . . . D.=.n.X+Y+. . . . . . h+W . Z+`+. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . y+ @Q..@+@. . . . . . x+&.. @@#@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . ^.=.n.. $@%@. . . . ^.S . . &@*@C+}+u+2.. <.. . . . ].. . . m.. . <.. 3 . 8.:.o+. $ . . y+. . . . . . . . . . . . ",
". . . . . . . >.]+Q . =@-@. . . . ;@D+. . >@p *+. . . 3+,@'@s.w+)@!@s+. '@g.w.& o+k.~@s+C r+* !@$+(+~ r 5+. . . . . . . . . . . ",
". . . . . . . . {@-.. 6 ]@>.. . . ^@>.. . /@. (@. . . _@. :@<@. :+. |+[@. . ^ &.}+. D.C.}@3.|@W+. b ;@. }+. . . . . . . . . . . ",
". . . . . . . . M+5.. . c 1@. . 2+_@. . . 2@. r D.) f+|@3 f+3@. % D.2+M 3  +| I+{ <.(+s.4@B.^+ +3+&.5@. d.. . . . . . . . . . . ",
". . . . . . . . c.g.. . 6 6@7@. 8@).. . 9@0@. ) q.h.. _.q.r.h D.6.q.A.y+r+h.3 | [.c.h.. C+a@. T (+. D.. G.. . . . . . . . . . . ",
". . . . . . . . >.T+. . . b@x c@D . . . d@e@. . . . . . . . . ^.^.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . f@|+. . . g@h@. . . i@j@k@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . D.l@5+m@K+:+n@o@p@q@r@s@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . H.t@{ 3+. . u@v@&@w@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . y . . x@. y@>.. . . k@#.' . . . . . . . . k . . . . . . . z@. . . . . . . . . . . . . . . . . . . 6 . A@. . . . . . . ",
". . . . B@C@. D@E@F@G@H@. . . I@*@G . . . . . . . y J@. . . . . . K@p@L@. . . . . . . . . . . . . . . . U I@ .. ( . . . . . . . ",
". . . . M@N@O@P@Q@. . 0 . . R@S@. T@. U@O@. . 7@V@W@X@' Y@W@. . Z@6 . . `@G . p *@7@7@#.6 U  #x@.#. . >.+#@#X = Q@. p ##$##.. . ",
". . . . . %#X@. + n &#U@. . . *@9@*#=#-#>.. . ;#>#n ,#+ p@'#. . x . . U c k@)#!#~#{#]#@@^#/#!#( (#. . . . X@_#I@:#<#[#}#|#1#. . ",
". . . 2#3#X@4#5#6#. 7#. . . + *@8#9#0#a#. . . Q@s .@b#T@@ . . . s c#>#d#g e#%#f#K@g#h#N@d@i#b#j#. . . k#2#. l#. c#m#n#.#o#p#. . ",
". . . . . . . . . . .#. . . q#p . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .#. . . . . . . ",
". . . . . . . . . . . . . . F@G . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . "};
EOF
#package MyPixmaps;
our $logoImage = <<'EOF';
/* XPM */
static char *openss7-logo-small[] = {
/* columns rows colors chars-per-pixel */
"64 64 191 2",
"   c #020202",
".  c #020206",
"X  c #060606",
"o  c gray4",
"O  c #0e0e0e",
"+  c gray7",
"@  c #161616",
"#  c #161a1a",
"$  c gray10",
"%  c #1a1a1e",
"&  c #1e1e1e",
"*  c #060a22",
"=  c #0e1232",
"-  c #1a1a22",
";  c #222222",
":  c gray15",
">  c #2a2a2a",
",  c gray18",
"<  c #262632",
"1  c #323232",
"2  c gray21",
"3  c #3a3a3a",
"4  c #3e3e3e",
"5  c #1a1e46",
"6  c #3e3e52",
"7  c gray26",
"8  c #464646",
"9  c gray29",
"0  c #4e4e4e",
"q  c #4e4e52",
"w  c gray32",
"e  c #565656",
"r  c #5a5a5a",
"t  c gray37",
"y  c #626262",
"u  c gray40",
"i  c #6a6a6a",
"p  c gray43",
"a  c #6e6e72",
"s  c #727272",
"d  c #767676",
"f  c gray48",
"g  c #7e7e7e",
"h  c #1e2696",
"j  c #1e2ab2",
"k  c #2a2e86",
"l  c #222aba",
"z  c #222eba",
"x  c #262eba",
"c  c #2632ba",
"v  c #2632be",
"b  c #2a32ba",
"n  c #2a32be",
"m  c #2a36be",
"M  c #2e36be",
"N  c #2e3abe",
"B  c #323abe",
"V  c #2e3ac2",
"C  c #363ec2",
"Z  c #3642c2",
"A  c #3a42c2",
"S  c #3e46c2",
"D  c #3e46c6",
"F  c #7e7e8a",
"G  c #7276a2",
"H  c #424ac6",
"J  c #464ec6",
"K  c #4a52c6",
"L  c #4e56ca",
"P  c #525aca",
"I  c #565aca",
"U  c #565eca",
"Y  c #5a62ca",
"T  c #5a62ce",
"R  c #5e62ce",
"E  c #5e66ce",
"W  c #626ace",
"Q  c #666ece",
"!  c #6a72d2",
"~  c #6e76d2",
"^  c #7276d2",
"/  c #7276d6",
"(  c #727ad2",
")  c #727ad6",
"_  c #767ad6",
"`  c #767ed6",
"'  c #7a7ed6",
"]  c #7a82d6",
"[  c #7e86da",
"{  c gray51",
"}  c #868686",
"|  c gray54",
" . c #8e8e8e",
".. c #8a8a9e",
"X. c #929292",
"o. c gray59",
"O. c #9a9a9a",
"+. c gray62",
"@. c #a2a2a2",
"#. c gray65",
"$. c #aaaaaa",
"%. c #aeaeae",
"&. c #b2b2b2",
"*. c #b6b6b6",
"=. c gray73",
"-. c gray",
";. c #8286da",
":. c #828ada",
">. c #868ada",
",. c #868eda",
"<. c #8a8eda",
"1. c #8a92da",
"2. c #8e92da",
"3. c #8e92de",
"4. c #8e96de",
"5. c #9296de",
"6. c #929ade",
"7. c #969ade",
"8. c #969ede",
"9. c #9a9ee2",
"0. c #9ea2e2",
"q. c #9ea6e2",
"w. c #bebece",
"e. c #b6b6d6",
"r. c #a2a2e2",
"t. c #a2a6e2",
"y. c #a6aae2",
"u. c #aaaae6",
"i. c #aaaee6",
"p. c #aeb2e6",
"a. c #b2b6e6",
"s. c #b6b6ea",
"d. c #b6baea",
"f. c #babeea",
"g. c #bebeea",
"h. c #bec2ea",
"j. c #bec2ee",
"k. c gray76",
"l. c #c6c6c6",
"z. c #c6c6ca",
"x. c #cacaca",
"c. c #cecece",
"v. c #cacad2",
"b. c #cacade",
"n. c #cecede",
"m. c #d2d2d2",
"M. c gray84",
"N. c #d6d6de",
"B. c #dadada",
"V. c #dadade",
"C. c gray87",
"Z. c #c2c2ee",
"A. c #c2c6ee",
"S. c #c6c6ee",
"D. c #c6caee",
"F. c #cacee6",
"G. c #cacaee",
"H. c #caceee",
"J. c #ceceee",
"K. c #ced2f2",
"L. c #d6d6e2",
"P. c #dadae2",
"I. c #dedee6",
"U. c #d2d2f2",
"Y. c #d2d6f2",
"T. c #d6d6f2",
"R. c #d6daf2",
"E. c #dadaf2",
"W. c #dadef6",
"Q. c #dedef6",
"!. c #dee2f6",
"~. c #e2e2e2",
"^. c #e6e6e6",
"/. c #eaeaea",
"(. c #eeeeee",
"). c #e2e2f6",
"_. c #e6e6f6",
"`. c #e6eafa",
"'. c #eaeafa",
"]. c #eaeefa",
"[. c #eeeefa",
"{. c gray95",
"}. c #f6f6f6",
"|. c #f2f2fa",
" X c #f6f6fa",
".X c #f6f6fe",
"XX c #f6fafe",
"oX c gray98",
"OX c #fafafe",
"+X c #fafefe",
"@X c #fefefe",
/* pixels */
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XOXa.:.` ;.y.'.@X@X@X@X@X@XoX%.r , & , t c.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XJ.S j x J H z c ` '.@X@X@Xg >   X 7 u 1 X X } @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XH.m l Q _.@X@XQ.' c C h.@Xd   X i ^.@X@X@X&.@    .@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.A l ` @X@X@X@X@X@XJ.S z 6   $ g @X@X@X@X@X@Xc.O o m.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X,.l J .X@X@X@X@X@X@X@X_.5   - C.@X@X@X@X@X@X@X@XX.  0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.N l Z.@X@X@X@X@X@X@X@Xs   * h i.@X@X@X@X@X@X@X@X}.; X B.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xu.z J @X@X@X@X@X@X@X@X@.  O w.S n K.@X@X@X@X@X@X@X@X}   g @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW j 4.@X@X@X@X@X@X@XM.O   %.@XT.m A [.@X@X@X@X@X@X@XC.  1 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XXXM l Y.@X@X@X@X@X@XoX2   p @X@X@Xy.z ^ @X@X@X@X@X@X@X@X> X (.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XT.l B .X@X@X@X@X@X@X|   : {.@X@X@X@XW j d.@X@X@X@X@X@X@Xu   =.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xp.l U @X@X@X@X@X@X~.o   =.@X@X@X@X@X_.M A .X@X@X@X@X@X@XX.   .@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X3.l _ @X@X@X@X@X@Xe   r @X@X@X@X@X@X@X9.x >.@X@X@X@X@X@X*.  p @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X) j 5.@X@X@X@X@Xl.  o ~.@X@X@X@X@X@X@XXXJ N ).@X@X@X@X@Xm.  w @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XE z y.@X@X@X@X@X9 X d @X@X@X@X@X@X@X@X@Xa.x ` @X@X@X@X@X/.  3 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XL l s.@X@X@X@Xk.  O /.@X@X@X@X@X@X@X@X@X@XP V ).@X@X@X@X}.O 1 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK l f.@X@X@X@X9 X f @X@X@X@X@X@X@X@X@X@X@Xf.M ] @X@X@X@X}.O : @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK z h.@X@X@Xm.X o ^.@X@X@X@X@X@X@X@X@X@X@XoXP H ].@X@X@XoX$ , @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK c A.@X@X@Xp O y @X@X@X@X@X@X@X@X@X@X@X@X@Xp.A 7.@X@X@X}.& , @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XR A h.@X@X(.$ @ x.@X@X@X@X@X@X@X@X@X@X@X@X@X|.T W @X@X@XoX2 0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X[ Y A.@X@X$., 0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xt.! J.@X@X(.0 p @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xy.,.H.@X@Xg e *.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X`.1.p.@X@X(.p X.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XG.u.U.@X(.{ g (.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xa.i..X@X^.X.=.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW.h.E.@XM.+.*.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW.d.!.@X^.@.m.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X_.G.R.@Xx.*.C.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.h.J.@XC.*.C.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@Xx.$.x.@X@X@X@X@X@X@X@X|.U.E.}.x.l.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xz.$.l.Y.G.OXB.-.(.^.$.$.@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X$.& 2 e $ & x.@X@X@X@X@X@XXXR.E.^.c.B.@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.3 & e 3 % e.'.m.k.p   e e   $.@X@X .e e e e e e @X@X",
"@X@X@Xx.  $.@X@X@X .  ^.@X@X@X@X@X@XR.R.V.m./.@X@X@X@X@X@X@X@X@X@X@X@X@X@Xs 3 @X@X@Xw @ W.c.O.  ~.@X@X$.  x.@Xl.$.$.$.$.e e @X@X",
"@X@X@X2 s @X@X@X@X@X3 2 @X@X@X@Xx.$.!.E.m.m.}.^.$.x.@X@X@X@X@X@X$.x.@X@X@X  $.@X@X@X@X  ..l.8 w @X@X@X@Xe w @X@X@X@X@X~.& ~.@X@X",
"@X@X^.  x.@X@X@X@X@X$.  ^.$.& w 3 3 % #.m.M.w & w & e @X@Xe e 3 w   e @X@X  s @X@X@X@XQ.b.l.9 & @X@X@X@X@X@X@X@X@X@X@X2  .@X@X@X",
"@X@X$.  @X@X@X@X@X@X@X  $.$.  s @X@Xq # m.%.& ~.@X~.&  .@Xe   ~.@Xl.  $.@X .  & s $.@X].F.l.*.&   e $.~.@X@X@X@X@X@Xx.3 @X@X@X@X",
"@X@X$.  @X@X@X@X@X@X@X  $.$.  ~.@X@XP.  #.0 s @X@X@X .e @Xw e @X@X@X  $.@X@Xl. .3     p v.l.}.~.$.e     3 ^.@X@X@X@X3  .@X@X@X@X",
"@X@X$.  ^.@X@X@X@X@X~.  $.$.  @X@X@X@X  | w           e @Xe e @X@X@X  $.@X@X@X@X@X@X .  a l.oX@X@X@X@Xl.& 2 @X@X@Xl.& @X@X@X@X@X",
"@X@X@X   .@X@X@X@X@X .  @X$.  @X@X@X X   .q  .@X@X@X@X@X@Xe e @X@X@X  $.$.  @X@X@X@X@Xw 7 v.  $.@X@X@X@X$.  @X@X@Xw s @X@X@X@X@X",
"@X@X@Xs & ^.@X@X@X^.$ s @X$.   .@X@X}   m.F 3 @X@X@X3 s @Xw e @X@X@X  $.^.  $.@X@X@X@X$ r b.2 e @X@X@X@Xs & @X@X@X  $.@X@X@X@X@X",
"@X@X@X@Xw &  .$. .& e @X@X$.  &  . .$ t N.k.& 3 $.s & ^.@Xe e @X@X@X  $.@X$.  s $. .& @ %.F.n.& 3 $.$.3   $.@X@Xl.  @X@X@X@X@X@X",
"@X@X@X@X@Xl.e e s l.@X@X@X$.  ~.e e g m.Y.R.P.s e s ~.@X@Xl.l.@X@X@X$.~.@X@Xl.s e e  .l.x.J.!.~. .e e s ~.@X@X@X~.$.@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X$.  @X@X@XV.m.L.R.'.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X(.-.N.G.Y.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X$.  @X@X@Xm.x.`.K.R.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xc.&.(.A.A.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X-.=.}.S.j.|.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XoX#.#.oXg.p.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X{.+.@.OXA.r.D.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XM.g =.@Xd.6.|.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.p g @XW./ [ +X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X} w ~.@Xi.` _.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xc.7 t @XOXQ J S.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X(., w oX@Xt.I Y.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X-.; w @X@X0.Z ~ @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X| + +.@X@Xq.D D.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X=.+ 9 @X@X'.b V _.@X@X@X@X@X@X@X@X@X@X@X@X@XoX: $ (.@X@X0.M f.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X&.X 8 @X@X@X^ j :.@X@X@X@X@X@X@X@X@X@X@X@X@X#.  f @X@X@Xq.b f.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X*.X 7 @X@X@XT.x n [.@X@X@X@X@X@X@X@X@X@X@X@X2 X C.@X@X@X0.n d.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xk.  2 @X@X@X@XW z 5.@X@X@X@X@X@X@X@X@X@X@X&.X u @X@X@X@X7.c A.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xm.  > @X@X@X@XY.l M [.@X@X@X@X@X@X@X@X@X@X2 X C.@X@X@X@X<.x J.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.  @ @X@X@X@X@X! z >.@X@X@X@X@X@X@X@X@X#.. s @X@X@X@X@X' c !.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XoXo   (.@X@X@X@XQ.v n !.@X@X@X@X@X@X@X}.; @ /.@X@X@X@X@XE n |.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X1   x.@X@X@X@X@X,.l W @X@X@X@X@X@X@Xf   O.@X@X@X@X@X@XH S @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xt   +.@X@X@X@X@X XS z a.@X@X@X@X@Xm.o 4 oX@X@X@X@X@X|.M W @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xo.  y @X@X@X@X@X@XS.x C '.@X@X@X}.1 O B.@X@X@X@X@X@XD.x 5.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XB.  & oX@X@X@X@X@X@X:.l T @X@X@Xp   O.@X@X@X@X@X@X@X>.x G.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X1   k.@X@X@X@X@X@XXXU j <.@X+.  y @X@X@X@X@X@X@X@XJ Z OX@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xo.  w @X@X@X@X@X@X@X'.S z G X 7 oX@X@X@X@X@X@X@Xh.l :.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X{.$ X l.@X@X@X@X@X@X@XW.k . < (.@X@X@X@X@X@X@X@XP x _.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X$.  ; /.@X@X@X@X@XOX| X = j 2.@X@X@X@X@X@X@X5.j <.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xd   , c.@X@X@Xx.7 X s I.P j T T.@X@X@X@X8.x U .X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X .O X 3 r ,   4 k.@X@XOX7.M z T 5.3.J l W [.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X/.#.} } O.B.@X@X@X@X@X@X|.y.( Y E ) g.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X"
};
EOF
#package MyPixmaps;
our $iconImage = <<'EOF';
/* XPM */
static char * ss7view_xpm[] = {
"48 48 510 2",
"  	c None",
". 	c #FFFFFF",
"+ 	c #FEFEFE",
"@ 	c #FBFBFB",
"# 	c #F8F8F8",
"$ 	c #F8F9FD",
"% 	c #D6D8F2",
"& 	c #ACAFE4",
"* 	c #8A8FDA",
"= 	c #8187D7",
"- 	c #9499DD",
"; 	c #B3B6E7",
"> 	c #D4D6F1",
", 	c #F5F5FB",
"' 	c #F0F0F0",
") 	c #C7C7C7",
"! 	c #8F8F8F",
"~ 	c #5A5A5A",
"{ 	c #404040",
"] 	c #3F3F3F",
"^ 	c #595959",
"/ 	c #989898",
"( 	c #DCDCDC",
"_ 	c #D5D7F2",
": 	c #686FCF",
"< 	c #323BBF",
"[ 	c #2A34BC",
"} 	c #313ABF",
"| 	c #3841C1",
"1 	c #363FC0",
"2 	c #373FBF",
"3 	c #424BC4",
"4 	c #5D65CC",
"5 	c #A7ABE2",
"6 	c #F5F5FC",
"7 	c #F4F4F4",
"8 	c #999999",
"9 	c #373737",
"0 	c #151515",
"a 	c #101010",
"b 	c #1D1D1D",
"c 	c #303030",
"d 	c #2F2F2F",
"e 	c #1A1A1A",
"f 	c #0E0E0E",
"g 	c #1E1E1E",
"h 	c #878787",
"i 	c #F7F7F7",
"j 	c #BEC1EA",
"k 	c #333CBF",
"l 	c #2731BB",
"m 	c #5960CB",
"n 	c #C0C2EA",
"o 	c #F3F4FB",
"p 	c #FCFCFE",
"q 	c #DCDDF3",
"r 	c #9EA2E0",
"s 	c #5E66CC",
"t 	c #3D46C2",
"u 	c #4E56C7",
"v 	c #898CA9",
"w 	c #282828",
"x 	c #7C7C7C",
"y 	c #D3D3D3",
"z 	c #B3B3B3",
"A 	c #2D2D2D",
"B 	c #121212",
"C 	c #555555",
"D 	c #FAFAFA",
"E 	c #D1D3F0",
"F 	c #3039BD",
"G 	c #252EBA",
"H 	c #888EDA",
"I 	c #F7F7FC",
"J 	c #C1C4EB",
"K 	c #25295E",
"L 	c #030308",
"M 	c #07070C",
"N 	c #ECECEC",
"O 	c #3B3B3B",
"P 	c #161616",
"Q 	c #686868",
"R 	c #636ACE",
"S 	c #2C35BC",
"T 	c #6168CD",
"U 	c #F1F1F1",
"V 	c #4F4F4F",
"W 	c #0A0A0A",
"X 	c #13131F",
"Y 	c #2932A3",
"Z 	c #4C54C6",
"` 	c #EEEFF9",
" .	c #191919",
"..	c #BABABA",
"+.	c #BFC2EA",
"@.	c #2B35BD",
"#.	c #2932BC",
"$.	c #E5E6F7",
"%.	c #EAEAEA",
"&.	c #484848",
"*.	c #0D0D0D",
"=.	c #3C3C3C",
"-.	c #F6F6F6",
";.	c #B0B3E6",
">.	c #343DBF",
",.	c #4D54C7",
"'.	c #E0E1F5",
").	c #979797",
"!.	c #FAFAFD",
"~.	c #7F85D7",
"{.	c #323CBE",
"].	c #6B72D0",
"^.	c #FBFBFD",
"/.	c #4B4B4B",
"(.	c #0F0F0F",
"_.	c #434343",
":.	c #F3F3F3",
"<.	c #B8BBE8",
"[.	c #4048C3",
"}.	c #545BC9",
"|.	c #DDDFF4",
"1.	c #F2F2F2",
"2.	c #242424",
"3.	c #D1D1D1",
"4.	c #E7E8F7",
"5.	c #444DC4",
"6.	c #2F39BE",
"7.	c #BBBEE9",
"8.	c #535353",
"9.	c #EBEBEB",
"0.	c #ADB1E5",
"a.	c #434CC4",
"b.	c #5961CA",
"c.	c #EAEAF8",
"d.	c #626262",
"e.	c #2E2E2E",
"f.	c #A3A3A3",
"g.	c #D8DAF3",
"h.	c #313ABE",
"i.	c #333DBF",
"j.	c #DCDDF4",
"k.	c #636363",
"l.	c #0B0B0B",
"m.	c #252525",
"n.	c #E3E3E3",
"o.	c #9CA0DF",
"p.	c #9C9C9C",
"q.	c #292929",
"r.	c #3039BE",
"s.	c #424BC3",
"t.	c #838383",
"u.	c #060606",
"v.	c #D9D9D9",
"w.	c #7E83D6",
"x.	c #3E46C2",
"y.	c #6F76D1",
"z.	c #C2C2C2",
"A.	c #272727",
"B.	c #5F5F5F",
"C.	c #A9ACE3",
"D.	c #565EC9",
"E.	c #EFEFFA",
"F.	c #BCBCBC",
"G.	c #020202",
"H.	c #B7B7B7",
"I.	c #4F57C8",
"J.	c #3A43C1",
"K.	c #A0A4E1",
"L.	c #525252",
"M.	c #9A9DDD",
"N.	c #3C45C0",
"O.	c #6970CF",
"P.	c #353535",
"Q.	c #6E6E6E",
"R.	c #EAEBF8",
"S.	c #2F38BE",
"T.	c #3B43C1",
"U.	c #D9DAF3",
"V.	c #D5D5D5",
"W.	c #333333",
"X.	c #4A4A4A",
"Y.	c #E6E6E6",
"Z.	c #9FA4E0",
"`.	c #5259C8",
" +	c #797DD3",
".+	c #B2B5E6",
"++	c #424AC1",
"@+	c #686FCE",
"#+	c #F7F8FC",
"$+	c #D7D7D7",
"%+	c #363636",
"&+	c #E5E5E5",
"*+	c #B6BAE7",
"=+	c #777DD4",
"-+	c #8F94DA",
";+	c #F4F4FB",
">+	c #1F1F1F",
",+	c #131313",
"'+	c #6269CD",
")+	c #B7BBE7",
"!+	c #3E3E3E",
"~+	c #4E4E4E",
"{+	c #FDFDFE",
"]+	c #D4D5F0",
"^+	c #A0A5E0",
"/+	c #ADB0E4",
"(+	c #F5F5FA",
"_+	c #FDFDFD",
":+	c #3D3D3D",
"<+	c #6A6A6A",
"[+	c #DEDFF3",
"}+	c #878DD9",
"|+	c #969ADB",
"1+	c #F1F1F9",
"2+	c #DDDDDD",
"3+	c #5E5E5E",
"4+	c #696969",
"5+	c #E9EAF6",
"6+	c #C4C7EB",
"7+	c #C6C8EC",
"8+	c #E9E9E9",
"9+	c #898989",
"0+	c #767676",
"a+	c #D4D4D4",
"b+	c #C6C9EC",
"c+	c #AEB2E5",
"d+	c #D9DBF2",
"e+	c #939393",
"f+	c #9D9D9D",
"g+	c #F4F5FB",
"h+	c #D8D9F2",
"i+	c #D7D9F1",
"j+	c #F9F9FC",
"k+	c #EEEEF9",
"l+	c #CACDED",
"m+	c #CED1EE",
"n+	c #FBFBFC",
"o+	c #C3C3C3",
"p+	c #CDCDCD",
"q+	c #FCFCFC",
"r+	c #E1E2F4",
"s+	c #E0E1F4",
"t+	c #F8F8FB",
"u+	c #DADBF2",
"v+	c #DBDBDB",
"w+	c #E8E8E8",
"x+	c #E5E6F6",
"y+	c #E3E4F5",
"z+	c #F7F7FB",
"A+	c #CACACA",
"B+	c #D6D6D6",
"C+	c #F6F6FB",
"D+	c #E2E3F5",
"E+	c #E2E2E2",
"F+	c #DADADA",
"G+	c #6C6C6C",
"H+	c #444444",
"I+	c #A1A1A1",
"J+	c #E8E9F6",
"K+	c #E3E5F5",
"L+	c #CFD0D5",
"M+	c #656565",
"N+	c #4D4D4D",
"O+	c #959595",
"P+	c #EDEDED",
"Q+	c #B5B5B5",
"R+	c #B4B4B4",
"S+	c #7B7B7B",
"T+	c #A2A2A2",
"U+	c #F9F9F9",
"V+	c #E6E8F7",
"W+	c #F3F3F9",
"X+	c #E4E4E4",
"Y+	c #4C4C4C",
"Z+	c #010101",
"`+	c #474747",
" @	c #090909",
".@	c #B9BAC1",
"+@	c #DCDDEE",
"@@	c #5B5B5D",
"#@	c #000000",
"$@	c #414141",
"%@	c #050505",
"&@	c #A8A8A8",
"*@	c #CECECE",
"=@	c #080808",
"-@	c #181818",
";@	c #2A2A2A",
">@	c #707070",
",@	c #F3F3FA",
"'@	c #E4E6F6",
")@	c #E5E6F3",
"!@	c #E1E1E1",
"~@	c #494949",
"{@	c #969696",
"]@	c #5C5D5F",
"^@	c #D2D3E2",
"/@	c #383838",
"(@	c #D2D2D2",
"_@	c #BFBFBF",
":@	c #3A3A3A",
"<@	c #BBBBBB",
"[@	c #B2B2B2",
"}@	c #D8D8D8",
"|@	c #AAAAAA",
"1@	c #C0C0C0",
"2@	c #A5A5A5",
"3@	c #8A8A8A",
"4@	c #C0C1D0",
"5@	c #E1E2EC",
"6@	c #B5B5B6",
"7@	c #A9A9A9",
"8@	c #DEDEDE",
"9@	c #BDBDBD",
"0@	c #F3F3F6",
"a@	c #C9CAD3",
"b@	c #1C1C1C",
"c@	c #141414",
"d@	c #7D7D7D",
"e@	c #C4C4C4",
"f@	c #171717",
"g@	c #B6B6B6",
"h@	c #646464",
"i@	c #1F1F21",
"j@	c #BDBDC1",
"k@	c #6A6A6B",
"l@	c #030303",
"m@	c #CBCBCB",
"n@	c #070707",
"o@	c #B9B9B9",
"p@	c #D9DADE",
"q@	c #5A5A5B",
"r@	c #343434",
"s@	c #A7A7A7",
"t@	c #737374",
"u@	c #909091",
"v@	c #B0B0B0",
"w@	c #797979",
"x@	c #2C2C2C",
"y@	c #828282",
"z@	c #E2E3E4",
"A@	c #D5D5D6",
"B@	c #77777C",
"C@	c #5D5D5D",
"D@	c #747474",
"E@	c #9B9B9B",
"F@	c #474748",
"G@	c #B8B8BC",
"H@	c #040404",
"I@	c #202020",
"J@	c #666666",
"K@	c #9A9A9A",
"L@	c #E0E0E1",
"M@	c #AFAFB0",
"N@	c #CBCCDA",
"O@	c #F3F4FA",
"P@	c #EFEFEF",
"Q@	c #F5F5F5",
"R@	c #929292",
"S@	c #727272",
"T@	c #C5C5C5",
"U@	c #323232",
"V@	c #69696A",
"W@	c #E4E4EE",
"X@	c #161617",
"Y@	c #E7E7E7",
"Z@	c #3A3B3F",
"`@	c #D3D4E2",
" #	c #676767",
".#	c #B1B1B1",
"+#	c #868686",
"@#	c #9F9FA0",
"##	c #E5E6F5",
"$#	c #4D4E51",
"%#	c #616162",
"&#	c #353539",
"*#	c #404041",
"=#	c #111111",
"-#	c #575757",
";#	c #393939",
">#	c #505050",
",#	c #5C5C5C",
"'#	c #EEEEEE",
")#	c #8B8B8C",
"!#	c #86878E",
"~#	c #E0E0E0",
"{#	c #616161",
"]#	c #636369",
"^#	c #121214",
"/#	c #111112",
"(#	c #8D8D8D",
"_#	c #E8E8E9",
":#	c #E6E7F7",
"<#	c #C5C6D2",
"[#	c #3D3D3E",
"}#	c #7A7A7A",
"|#	c #F4F5F8",
"1#	c #D6D7EB",
"2#	c #D3D4EA",
"3#	c #EFF0F6",
"4#	c #F0F0F6",
"5#	c #DFDFDF",
"6#	c #DEE0F4",
"7#	c #CED0EF",
"8#	c #DFE1F4",
"9#	c #CCCCCC",
"0#	c #F0F0F9",
"a#	c #9297DC",
"b#	c #878CD8",
"c#	c #CDCFEE",
"d#	c #B9BCE8",
"e#	c #A4A4A4",
"f#	c #A3A6E1",
"g#	c #535AC8",
"h#	c #757CD3",
"i#	c #F1F2FA",
"j#	c #CCCEEE",
"k#	c #808080",
"l#	c #525AC9",
"m#	c #3C45C1",
"n#	c #ABADE2",
"o#	c #5B5B5B",
"p#	c #565656",
"q#	c #A2A7E0",
"r#	c #6C72D0",
"s#	c #BBBEE8",
"t#	c #B9BCE9",
"u#	c #323BBE",
"v#	c #333CBE",
"w#	c #818181",
"x#	c #959ADC",
"y#	c #5B63CC",
"z#	c #B5B8E6",
"A#	c #7B81D5",
"B#	c #535AC9",
"C#	c #B8B8B8",
"D#	c #1B1B1B",
"E#	c #868CD8",
"F#	c #575FCA",
"G#	c #4851C5",
"H#	c #2D37BD",
"I#	c #8086D7",
"J#	c #232323",
"K#	c #7076D2",
"L#	c #5C63CC",
"M#	c #C8CAED",
"N#	c #787878",
"O#	c #757575",
"P#	c #DADBF3",
"Q#	c #353EC0",
"R#	c #2D36BD",
"S#	c #9CA1E0",
"T#	c #A6A6A6",
"U#	c #CDCFEF",
"V#	c #313ABD",
"W#	c #A9ADE4",
"X#	c #E3E4F6",
"Y#	c #3F48C3",
"Z#	c #676ECF",
"`#	c #ABABAB",
" $	c #999DDE",
".$	c #9297DD",
"+$	c #CFD2F0",
"@$	c #ACACAC",
"#$	c #3942C1",
"$$	c #4851C6",
"%$	c #3B43C2",
"&$	c #2A33BB",
"*$	c #797DB5",
"=$	c #262626",
"-$	c #8C92DB",
";$	c #4A52C6",
">$	c #7F84D7",
",$	c #CDCEDA",
"'$	c #2B2F65",
")$	c #06071A",
"!$	c #13152F",
"~$	c #B4B5D2",
"{$	c #F9FAFD",
"]$	c #B6B9E8",
"^$	c #8E8E8E",
"/$	c #7F7F7F",
"($	c #AFAFAF",
"_$	c #5D5F82",
":$	c #4049C3",
"<$	c #676DCF",
"[$	c #EDEEF9",
"}$	c #E1E2F5",
"|$	c #848AD8",
"1$	c #555DCA",
"2$	c #313131",
"3$	c #909090",
"4$	c #E8E9F7",
"5$	c #969BDE",
"6$	c #565ECA",
"7$	c #414AC3",
"8$	c #3C44C1",
"9$	c #4149C4",
"0$	c #3D45C2",
"a$	c #4B53C6",
"b$	c #878BD8",
"c$	c #EBECF8",
"d$	c #F0F0FA",
"e$	c #CDD0EE",
"f$	c #AFB3E5",
"g$	c #959BDD",
"h$	c #8C91DA",
"i$	c #979CDE",
"j$	c #B4B7E6",
"k$	c #DFE0F4",
". . . . . . . . . . . . . . . + + . . . . . . . . . . . . @ # # @ . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . $ % & * = - ; > , . . . . . ' ) ! ~ { ] ^ / ( . . . . . . . . . . . . . ",
". . . . . . . . . . . _ : < [ } | 1 2 3 4 5 6 . 7 8 9 0 a b c d e f g h i . . . . . . . . . . . ",
". . . . . . . . . . j k l m n o . p q r s t u v d a w x y + . . i z A B C D . . . . . . . . . . ",
". . . . . . . . . E F G H $ . . . . . . I J K L M h N . . . . . . . ' O P Q . . . . . . . . . . ",
". . . . . . . . $ R S T . . . . . . . . U V W X Y Z ` . . . . . . . . ' 0  .... . . . . . . . . ",
". . . . . . . . +.@.#.$.. . . . . . . %.&.*.=.-.;.>.,.'.. . . . . . . . ).b C . . . . . . . . . ",
". . . . . . . !.~.{.].^.. . . . . . N /.(._.:.. . <.[.}.|.. . . . . . . 1.2.c 3.. . . . . . . . ",
". . . . . . . 4.5.6.7.. . . . . . -.8.(.O 9.. . . . 0.a.b.c.. . . . . . D d.e.f.. . . . . . . . ",
". . . . . . . g.h.i.j.. . . . . . k.l.m.n.. . . . . . o.[.4 p . . . . . + p.q.x + . . . . . . . ",
". . . . . . . J r.s.4.. . . . . t.u.*.v.. . . . . . . . w.x.y.. . . . . . z.A.B.7 . . . . . . . ",
". . . . . . + C.k D.E.. . . . F.W G.H.. . . . . . . . . . I.J.K.. . . . . 3.e.L.9.. . . . . . . ",
". . . . . . p M.N.O.o . . . %.P.*.Q.. . . . . . . . . . . R.S.T.U.. . . . V.W.X.Y.. . . . . . . ",
". + . . + + p Z.`. +, . + + h (.w # . . . . . . . . . + . + .+++@+#++ + . $+%+&.&+. . . . . . . ",
"+ . . + + + p *+=+-+;+. + $+>+,+H.+ . . . . + . + . + . + + + w.'+)+. . . v.!+~+Y.. . . + . + + ",
". + . . . + {+]+^+/+(++ _+h :+<+-.. + + . + + + + + + . + . . [+}+|+1++ . 2+3+4+%.. . . . . . . ",
". . + + . + + 5+6+7+I + 8+9+0+a+. + . + + + + + + + + . + . + ^.b+c+d+{+. Y.e+f+7 + . . . + + . ",
". + + + + . + g+h+i+j+. 3...) + + . . . + + . + . + . . + . + . k+l+m+n++ 9.o+p+q++ . + . . + . ",
"+ + . . + + . j+r+s+t+i V.a+U + . . + . + . . . + . + . + . + + . '.u+` _+9.v+w+. . + . + + . . ",
"+ + . . _+8+1.!.x+y+z+Y.A+B+. + . . + . + . + . . + . . . _++ + + C+D+y+q+%.E+U . . + . + . . . ",
"+ . + F+G+H+8.I+J+K+L+M+=.N+O++ P+..Q+R+R+F.-.+ . + + . w+S+T+U++ + V+K+W+X+X+-.. . + . . + . . ",
". + # Y+Z+`+,+ @.@+@@@#@$@e %@&@*@ .u.=@-@;@X++ + . . . ( P.>@7 + . ,@'@)@!@&+D + . . . . + + . ",
"+ . 1.#@~@1.{@#@]@^@e /@(@_@#@:@+ <@H.z Z+P.%.[@X+. F+<@}@|@1@@ + 2@3@4@5@6@7@D 8@9@U ( ... + + ",
". . :.#@w *@. i 0@a@b@c@o+. i -.+ . . d@#@z.e@#@x + N+f@g@w h@+ 3+#@#@i@j@k@l@m@0+n@e@4+=@+ . + ",
"+ . i Y+#@#@=.o@q+p@q@#@#@r@s@+ . . 7 c@%++ F+q.`+D f q.X+g k.a+Z+!+o@Z+t@u@#@f+C #@v@:+q.+ + . ",
"+ . . 7 w@x@ @#@y@z@A@B@d f #@B.. + O+#@&@. i C@P o+#@D@n.g 4+E@Z+C ! -@F@G@H@G+{ *.d@I@J@+ + . ",
"+ + @ _@w+. K@=@B L@M@N@O@Q+H@-@P@+ /.#@Q@. + R@#@S@#@T@( >+4+R@l@e U@W.V@W@X@:+r@~+O =@f.. + + ",
"+ . 7 (.Y+# Y@*.,+v.e.Z@`@. W -@1.w+;@e.+ . . z.b@,+*.. }@g  #.##@J@%.+#@###$#l.c 9+f #@8@+ . + ",
"+ . U+C #@!+%+#@Q !@%##@&#*##@N++ T@=#-#+ + + -.;##@>#. V.-@,#'#f@,+N+ @)#K+!##@U@..=@f Q@+ + + ",
"+ + + ~#{#=#0 S@8@~#y ]#^#/#d.8+. A+~+/ . + + . (#] g@+ !@,#3@@ ) e.(.B._#:#<#[#}#n.^ 0+D + + + ",
". . . . D -.-.# Y.~#U |#1#2#3#. + _+U+@ + . . . @ # q++ _+D @ + . F+3.N q+4.y+4#D + U+D + . + + ",
"+ + . . + + + D 5#( :.. 6#7+7#{++ . + + + . + + + . . . + . + . P+A+p+@ _+8#h+` . . . + . + + . ",
". . . . + . + U+y 9#' . 0#a#b#h+. + . . . + + + + . . + + + . D F.s@( + ^.c#d#s++ + . . + + + + ",
". . + . + . . :.Q+e#Y.. + f#g#h#i#. . . . . + . . + . + + . + (@w@).Q@. p *+-+j#+ . + . . . . . ",
". . + + . . . P+k#M+v.+ . 4.l#m#n#. + + . . + . . . + . + + U+o#p#A++ . p q#r#s#+ + . . . . + . ",
". . + + . + + Y.V W.p+. . + t#u#v#:#. . . + + + . + + . . . }#9 w#+ . . ^.x#y#z#. . . . . + + . ",
". + + . + + . Y@O b@o+. . . . A#r.B#+ . . . . + . . . . . C#D#%+_+. + . !.E#F#<.+ . . . . . + . ",
". . . . . . + U /.b@7@+ . . . i#G#H#I#. . . . . . . . . V.g J#8@. . . . $ K#L#M#. . + . . . . . ",
". . . . . . . _+N#>+O#@ . . . . P#Q#R#S#. . . . . . . ( /@q.9@. . . . . I l#T 6#. . . . . . . . ",
". . . . . . . . T#>+O D . . . . . U#>.V#W#. . . . . X+X.e.v@. . . . . . X#Y#Z#p . . . . . . . . ",
". . . . . . . . $+2.=#( . . . . . . 7#Q#< .+. . . '#Y+e.`#. . . . . . .  $B#.$. . . . . . . . . ",
". . . . . . . . . k.D#M+. . . . . . . +$1 S.; . ' !+;@@$. . . . . . . . #$$$'.. . . . . . . . . ",
". . . . . . . . . a+c@n@9#. . . . . . . _ %$&$*$=$w ... . . . . . . . -$;$>$. . . . . . . . . . ",
". . . . . . . . . . e+=# .V.. . . . . . . ,$'$)$!$~${$. . . . . . . ]$J.;${+. . . . . . . . . . ",
". . . . . . . . . . . ^$D#>+/$F+. . 9.($Q.:+:@_$:$J.<$;.[$. . . }$|$:$1$$.. . . . . . . . . . . ",
". . . . . . . . . . . . z.L.q.J#2.q.2$!+C 3$Y.. 4$5$6$7$8$s.;$9$0$a$b$c$. . . . . . . . . . . . ",
". . . . . . . . . . . . . # a+.#2@T#z 3.:.. . . . . d$e$f$g$h$i$j$k${+. . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . + _+. . . . . . . . . . . . . . . . . "};
EOF
#package MyPixmaps;
sub assign {
	my $wind = shift;
	$wind->Pixmap('icon', -data=>$iconImage);
	$wind->Pixmap('logo8', -data=>$logo8Image);
	$wind->Pixmap('logo', -data=>$logoImage);
	$wind->Pixmap('plus', -data=>$plusImage);
	$wind->Pixmap('minus', -data=>$minusImage);
	$wind->Pixmap('box', -data=>$boxImage);
}

1;

__END__



