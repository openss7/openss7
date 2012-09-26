# ------------------------------------------
package Viewer; our @ISA = qw(Base Gtk2::ScrolledWindow);
#use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Viewer;
our $viewno = 0;
our %viewers = ();
#package Viewer;
sub find {
	my $type = shift;
	my ($windid,$pageid,$base,$viewid) = @_;
	my $viewer = $viewers{$viewid} if defined $viewid and exists $viewers{$viewid};
	return $viewer;
}
#package Viewer;
sub new {
	my $type = shift;
	my ($windid,$pageid,$base,$viewid) = @_;
	my $self = new Gtk2::ScrolledWindow();
	bless $self,$type;
	$self->{items} = {};
	$self->_init($type,@_);
	return $self;
}
#package Viewer;
sub init {
	my $self = shift;
	my ($windid,$pageid,$base,$viewid) = @_;
	if (defined $viewid) {
		if ($viewno <= $viewid) {
			$viewno = $viewid+1;
		}
	} else {
		$viewid = $viewno;
		$viewno++;
	}
	$self->{windid} = $windid;
	$self->{pageid} = $pageid;
	$self->{viewid} = $viewid;
	$viewers{$viewid} = $self;
	$self->set_policy('automatic','automatic');
	my $canv = $self->{canv} = new Goo::Canvas();
	$canv->set(
		'background-color'=>'white',
		'automatic-bounds'=>Glib::TRUE,
		'bounds-from-origin'=>Glib::FALSE,
		'bounds-padding'=>3,
		'units'=>'mm',
	);
	$self->add($canv);
	$self->{layers} = {};
	my $root = $self->{root} = new Goo::Canvas::Group();
	$root->set('pointer-events'=>'all');
	$root->{viewer} = $self;
	$canv->set_root_item($root);
	foreach my $layer (qw/Network Private Lan Port Vlan Vprt Subnet Address Host/) {
		$self->getlayer($layer);
	}
	$self->{base} = $base;
	$self->{model} = $base->{model};
	$self->signal_connect('scroll-event'=>\&Viewer::scrollWindow,$canv);
	$canv->signal_connect('scroll-event'=>\&Viewer::scrollCanvas,$self);
	if (my $sub = $self->can('click')) {
		$root->signal_connect('button-press-event'=>$sub,$self);
	}
	my $view = $self->{view} = $self->getbase($base);
	warn "******** POPULATING VIEWER";
	$view->populate();
	warn "******** LAYING OUT VIEWER";
	$self->layout();
	warn "******** DONE START VIEWER";
}
#package Viewer;
sub fini {
	my $self = shift;
	my $viewid = delete $self->{viewid};
	my $root = delete $self->{root};
	foreach my $layer (values %{$self->{layers}}) {
		my $numb = $root->find_child($layer);
		if (defined $numb) {
			$root->remove_child($numb);
		} else {
			warn "****** Could not find child $layer in group $root";
		}
	}
	delete $viewers{$viewid};
	delete $root->{viewer};
	delete $self->{base};
	delete $self->{model};
	$viewno-- if $viewno == $viewid+1;
	$self->destroy;
}
#package Viewer;
sub item {
	my $self = shift;
	return $self->findview($self->{model}->item(@_));
}
#package Viewer;
sub items {
	my $self = shift;
	return $self->findviews($self->{model}->items(@_));
}
#package Viewer;
sub canvas {
	my $self = shift;
	return $self->{canv};
}
#package Viewer;
sub setview {
	my ($self,$data,$view) = @_;
	$data->{views}[$self->{viewid}] = $view;
	my $type = ref $view;
	my $kind = $type->kind;
	$self->{items}{$type}{$view} = $view;
	$self->{items}{$kind}{$view} = $view;
}
#package Viewer;
sub putview {
	my ($self,$data) = @_;
	my $view = delete $data->{views}[$self->{viewid}];
	my $type = ref $view;
	my $kind = $type->kind;
	delete $self->{items}{$type}{$view};
	delete $self->{items}{$kind}{$view};
}
#package Viewer;
sub showstats {
	my $self = shift;
	my $model = $self->{model};
	$model->showstats;
	print STDERR "Viewer stats:\n-------------\n";
	foreach my $type (sort keys %{$self->{items}}) {
		my $count = scalar(values %{$self->{items}{$type}});
		printf STDERR "%6d items of type %s\n", $count, $type;
	}
	print STDERR "-------------\n";
}
#package Viewer;
sub getbase {
	my ($self,$data) = @_;
	return undef unless $data;
	my $type = ref($data).'::View';
	return $type->get($self,$data,$self->{viewid});
}
#package Viewer;
sub getview {
	my ($self,$data) = @_;
	return undef unless $data;
	my $type = ref($data).'::View';
	return undef if $type->isanchor();
	return $type->get($self,$data,$self->{viewid});
}
#package Viewer;
sub getviews {
	my ($self,@data) = @_;
	my $viewid = $self->{viewid};
	my @views = ();
	foreach my $data (@data) {
		my $view = $self->getview($data);
		push @views, $view if $view;
	}
	return @views;
}
#package Viewer;
sub findview {
	my ($self,$data) = @_;
	return undef unless $data;
	return $data->{views}[$self->{viewid}];
}
#package Viewer;
sub findviews {
	my ($self,@data) = @_;
	my $viewid = $self->{viewid};
	my @views = ();
	foreach my $data (@data) {
		my $view = $self->findview($data);
		push @views, $view if $view;
	}
	return @views;
}
#package Viewer;
sub getlayer {
	my ($self,$layer) = @_;
	$self->{layers}{$layer} = new Goo::Canvas::Group($self->{root}) unless exists $self->{layers}{$layer};
	return $self->{layers}{$layer};
}
#package Viewer;
sub scrollWindow {
	my ($self,$ev,$canv) = @_;
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
#package Viewer;
sub scrollCanvas {
	my ($canv,$ev,$self) = @_;
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
			$self->signal_emit('scroll-event',$ev);
			return Gtk2::EVENT_STOP;
		}
	}
	$self->signal_emit('scroll-event',$ev);
	return Gtk2::EVENT_PROPAGATE;
}
#package Viewer;
sub refresh {
	my $type = shift;
	foreach my $viewer (values %viewers) {
		if (delete $viewer->{changed}) {
			$viewer->layout();
		}
	}
}
#package Viewer;
$Viewer::added_child_count = 0;
#package Viewer;
sub added_child {
	my ($type,$pdata,$cdata) = @_;
	foreach my $viewer (values %viewers) {
		if (my $pview = $viewer->findview($pdata)) {
			if (my $cview = $viewer->getview($cdata)) {
				$cview->populate();
				if (my $sub = $pview->can('add_child')) {
					&$sub($pview,$cview);
				}
			}
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::added_path_count = 0;
#package Viewer;
sub added_path {
	my ($type,$ndata,$pdata) = @_;
	foreach my $viewer (values %viewers) {
		if (my $nview = $viewer->findview($ndata)) {
			if (my $pview = $viewer->getview($pdata)) {
				$pview->populate();
				if (my $sub = $nview->can('add_path')) {
					&$sub($nview,$pview);
				}
			}
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::removed_count = 0;
#package Viewer;
sub removed {
	my ($type,$data) = @_;
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			$view->put();
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::updated_count = 0;
#package Viewer;
sub updated {
	my ($type,$data) = @_;
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			if (my $sub = $view->can('update')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::merged_count = 0;
#package Viewer;
sub merged {
	my ($type,$data) = @_;
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->getview($data)) {
			$view->populate();
			if (my $sub = $view->can('reconfig')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::changed_count = 0;
#package Viewer;
sub changed {
	my ($type,@data) = @_;
	foreach my $viewer (values %viewers) {
		my @views = ($viewer->findviews(@data));
		foreach my $view (@views) {
			if (my $sub = $view->can('change')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
	}
}
#package Viewer;
$Viewer::xformed_count = 0;
#package Viewer;
sub xformed {
	my ($type,$data) = @_;
	my $vtyp = ref($data).'::View';
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			if (my $sub = $vtyp->can('xformed')) {
				&$sub($vtyp,$view);
			}
		}
		$viewer->{changed} = 1;
	}
}

# ------------------------------------------
package Viewer::Network; our @ISA = qw(Viewer);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Time::HiRes qw(gettimeofday tv_interval);
# ------------------------------------------
# Columns in the display are laid out as follows:
#   - HERENOD_COLUMN: local host
#   - HERENOD_COLUMN: private subnets of the local host
#   - HERENOD_COLUMN: hosts directly accessible via private subnets of the
#     local host
#   - HERENOD_COLUMN: public subnets
#   - HERENOD_COLUMN: hosts not directly accessible via private subnets of
#     the local host but which are directly accessible via public subnets.
#   - HERENOD_COLUMN: private subnets of foreign hosts
#   - HERENOD_COLUMN: hosts not directly accessible via private subnets of
#     the local host and not directly accessible via public subnets
#   - HERENOD_COLUMN: unknown subnets
#   - HERENOD_COLUMN: unknown hosts
#
# Note that local nets are displayed beneath the Host.
# ------------------------------------------
# This is the basic layout:
#                                                                                                 
#                                           |  |  |  |                                            
#                                           |  |  |  |                    | | |                   
#                                           |  |  |  |                    | | |       ______      
#                                           |  |  |  |                    &_|_|______|      |     
#                                           |  |  |  |                    | &_|______|      |     
#                                           |  |  |  |                    | | &______|      |     
#                                           |  |  |  |                    | | |      |______|     
#                                           |  |  |  |                    | | |                   
#                                           |  |  |  |       ______       | | |       ______      
#                                           |  |  |  |      |      |______&_|_|______|      |     
#                                           |  |  |__|______|      |______|_&_|______|      |     
#                                           |  |  |  |      |      |______|_|_&______|      |     
#                                           |  |  |  |      |______|      | | |      |______|     
#                                           |  |  |  |                    | | |                   
#                                           |  |  |  |                    | | |       ______      
#                                           |  |  |  |                    &_|_|______|      |     
#                                           |  |  |  |                    | &_|______|      |     
#                                           |  |  |  |                    | | &______|      |     
#                                           |  |  |  |                    | | |      |______|     
#                                           |  |  |  |                    | | |                   
#                                           |  |  |  |                                            
#                                           |  |  |  |                                            
#                                           |  |  |  |                    | | |                   
#                                           |  |  |  |                    | | |       ______      
#                                           |  |  |  |                    &_|_|______|      |     
#                                           |  |  |  |                    | &_|______|      |     
#                                           |  |  |  |                    | | &______|      |     
#                                           |  |  |  |                    | | |      |______|     
#                                           |  |  |  |                    | | |                   
#     ______       | | |                    |  |  |  |       ______       | | |       ______      
#    |      |______|_|_&                    |  |  |  |      |      |______&_|_|______|      |     
#    |      |______|_& |                    |  |  |__|______|      |______|_&_|______|      |     
#    |      |______& | |                    |  |  |  |      |      |______|_|_&______|      |     
#    |______|      | | |                    |  |  |  |      |______|      | | |      |______|     
#                  | | |                    |  |  |  |                    | | |                   
#     ______       | | |       ______       |  |  |  |                    | | |       ______      
#    |      |______|_|_&______|      |      |  |  |  |                    &_|_|______|      |     
#    |      |______|_&_|______|      |______|__|  |  |                    | &_|______|      |     
#    |      |______&_|_|______|      |      |  |  |  |                    | | &______|      |     
#    |______|      | | |      |______|      |  |  |  |                    | | |      |______|     
#                  | | |                    |  |  |  |                    | | |                   
#     ______       | | |                    |  |  |  |                                            
#    |      |______|_|_&                    |  |  |  |                                            
#    |      |______|_& |                    |  |  |  |       ______                               
#    |      |______& | |                    |  |  |  |      |      |                              
#    |______|      | | |                    |  |  |__|______|      |                              
#                                           |  |  |  |      |      |                              
#                                           |  |  |  |      |______|                              
#                                           |  |  |  |                                            
#                                           |  |  |  |       ______                               
#                                           |  |  |  |      |      |                              
#                                           |  |  |__|______|      |                              
#                                           |  |  |  |      |      |                              
#                                           |  |  |  |      |______|                              
#                                           |  |  |  |                                            
#                                           |  |  |  |       ______                               
#                                           |  |  |  |      |      |                              
#                                           |  |  |__|______|      |                              
#                                           |  |  |  |      |      |                              
#                                           |  |  |  |      |______|                              
#                                           |  |  |  |                                            
#                                           |  |  |  |       ______                               
#                                           |  |  |  |      |      |                              
#                                           |  |  |__|______|      |                              
#                                           |  |  |  |      |      |                              
#                                           |  |  |  |      |______|                              
#                                           |  |  |  |                                            
#                                                                                                 
# ------------------------------------------
#package Viewer::Network;
use constant {
	HERENOD_COLUMN=>0,
	LOCLNET_COLUMN=>1,
	LOCLNOD_COLUMN=>2,
	PUBLNET_COLUMN=>3,
	PUBLNOD_COLUMN=>4,
	PRIVNET_COLUMN=>5,
	PRIVNOD_COLUMN=>6,
	UNKNNET_COLUMN=>7,
	UNKNNOD_COLUMN=>8,
};
#package Viewer::Network;
sub init {
	my $self = shift;
	my ($windid,$pageid,$base,$viewid) = @_;
	$self->{visibility} = {};
	foreach (qw/Host Subnet Address Network Private/) {
		$self->{visibility}{$_} = Glib::TRUE;
	}
	foreach (qw/Vlan Lan Vprt Port/) {
		$self->{visibility}{$_} = Glib::FALSE;
	}
}
#package Viewer::Network;
sub fini {
	my $self = shift;
	delete $self->{visibility};
}
#package Viewer::Network;
sub selected {
	my ($radio,$args) = @_;
	my ($self,$kind,$from) = @$args;
	foreach my $view (qw{Subnet/Address Vlan/Vprt Lan/Port}) {
		my @pair = split(/\//,$view);
		my $vis = ($view eq $kind) ? Glib::TRUE : Glib::FALSE;
		$self->{visibility}{$pair[0]} = $vis;
		$self->{visibility}{$pair[1]} = $vis;
	}
	$radio->set_inconsistent(Glib::FALSE);
	$radio->set_active(Glib::TRUE);
	$self->layout();
}
#package Viewer::Network;
sub toggled {
	my ($check,$args) = @_;
	my ($self,$kind,$from) = @$args;
	if ($self->{visibility}{$kind}) {
		$self->{visibility}{$kind} = Glib::FALSE;
		$check->set_active(Glib::FALSE);
	} else {
		$self->{visibility}{$kind} = Glib::TRUE;
		$check->set_active(Glib::TRUE);
	}
	$self->layout();
}
#package Viewer::Network;
sub click {
	my ($root,$targ,$ev,$self) = @_;
	unless ($targ) {
		if ($ev->button == 3) {
			my $menu = Gtk2::Menu->new();
			my $mi = Gtk2::MenuItem->new_with_label("View");
			$mi->show_all;
			$menu->append($mi);
			my $mv = Gtk2::Menu->new;
			$mi->set_submenu($mv);
			$mi = Gtk2::TearoffMenuItem->new();
			$mi->show_all;
			$mv->append($mi);
			foreach my $view (qw{Subnet/Address Vlan/Vprt Lan/Port}) {
				my $mr = Gtk2::CheckMenuItem->new($view);
				$mr->set_draw_as_radio(Glib::TRUE);
				my @pair = map {$self->isvisible($_)} split(/\//,$view);
				if ($pair[0] and $pair[1]) {
					$mr->set_active(Glib::TRUE);
					$mr->set_inconsistent(Glib::FALSE);
				}
				elsif (not $pair[0] and not $pair[1]) {
					$mr->set_active(Glib::FALSE);
					$mr->set_inconsistent(Glib::FALSE);
				}
				else {
					$mr->set_active(Glib::FALSE);
					$mr->set_inconsistent(Glib::TRUE);
				}
				$mr->show_all;
				if (my $sub = $self->can('selected')) {
					$mr->signal_connect('toggled'=>$sub,[$self,$view,'toggled']);
				}
				$mv->append($mr);
			}
			$mi = Gtk2::SeparatorMenuItem->new();
			$mi->show_all;
			$mv->append($mi);
			foreach my $kind (qw/Host Vlan Lan Subnet Vprt Port Address Network Private/) {
				my $mb = Gtk2::CheckMenuItem->new($kind);
				$mb->set_active($self->isvisible($kind));
				$mb->show_all;
				if (my $sub = $self->can('toggled')) {
					$mb->signal_connect('toggled'=>$sub,[$self,$kind,'toggled']);
				}
				$mv->append($mb);
			}
			$mv->show_all;
			$menu->show_all;
			$menu->popup(undef,undef,undef,undef,$ev->button,$ev->time);
			return Gtk2::EVENT_STOP;
		}
	}
	return Gtk2::EVENT_PROPAGATE;
}
#package Viewer::Network;
sub isvisible {
	my $self = shift;
	my $kind = shift;
	return $self->{visibility}{$kind};
}
#package Viewer::Network;
$Viewer::Network::callcount = 0;
#package Viewer::Network;
sub layout {
	my $self = shift;
	my $view = $self->{view};
	my $canv = $self->{canv};
	my $hbox = $canv->get_screen->get_height_mm;
	my $wbox = $canv->get_screen->get_width_mm;
	my ($h,$w,$mh,$mw) = $view->resize($hbox,$wbox);
	if (0) {
		$w =
			$view->{pad}[1]+
			$view->{siz}{l}{total}[1] + 2.0*$view->{siz}{l}{total}[3]+
			$view->{siz}{c}{total}[1]/2.0 + $view->{siz}{c}{total}[3];
		$h = $h/2.0 + $mh;
	} else {
		$w = 0;
		$h = 0;
	}
	$view->move_view($w,$h,0,0);
	$view->layout($hbox,$wbox);
	$view->place_view($w,$h,0,0);
	$view->show_view($w,$h,0,0);
	$canv->update;
}
#package Viewer::Network;
sub start_idle {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Idle->add($self->can('do_idle'),$self);
	$self->{tag} = $tag;
}
#package Viewer::Network;
sub start_quick {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Timeout->add(100,$self->can('do_quick'),$self);
	$self->{tag} = $tag;
}
#package Viewer::Network;
sub start_timeout {
	my $self = shift;
	my $tag = delete $self->{tag};
	Glib::Source->remove($tag) if defined $tag;
	$tag = Glib::Timeout->add(1000,$self->can('do_timeout'),$self);
	$self->{tag} = $tag;
}
#package Viewer::Network;
sub do_quick {
	my $self = shift;
	my $model = $self->{model};
	if ($model->snmp_doone()) {
		$self->start_quick();
	} else {
		$self->start_timeout();
	}
}
#package Viewer::Network;
sub do_idle {
	my $self = shift;
	my $model = $self->{model};
	$self->start_timeout() unless $model->snmp_doone();
}
#package Viewer::Network;
sub do_timeout {
	my $self = shift;
	my $model = $self->{model};
	if ($model->snmp_doone()) {
		$self->start_quick();
	} else {
		$self->start_timeout();
	}
}

# ------------------------------------------
package Viewer::Driver; our @ISA = qw(Viewer);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

# ------------------------------------------
package Table::Window; our @ISA = qw(Table);
use strict; use warnings; use Carp;
use Data::Dumper; use DBI;
# ------------------------------------------
#package Table::Window;
our %schema = (
	table=>'winds',
	keys=>[ qw/windid/ ],
	cols=>[ qw/windid xpos ypos width height/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS winds (
	windid INT,
	xpos INT,
	ypos INT,
	width INT,
	height INT,
	PRIMARY KEY(windid)
);},
);

# ------------------------------------------
package Table::Page; our @ISA = qw(Table);
use strict; use warnings; use Carp;
use Data::Dumper; use DBI;
# ------------------------------------------
#package Table::Page;
our %schema = (
	table=>'pages',
	keys=>[ qw/windid pageid/ ],
	cols=>[ qw/windid pageid type id xoffset yoffset scale/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS winds (
	windid INT,
	pageid INT,
	type TEXT,	-- type of base object (e.g. Network)
	id INT,		-- id of base object within model
	xoffset INT,	-- xoffset of scrolled window
	yoffset INT,	-- yoffset of scrolled window
	scale REAL,	-- scale of canvas
	PRIMARY KEY(windid,pageid),
	FOREIGN KEY(windid) REFERENCES winds(windid)
);},
);

# ------------------------------------------
package View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package View;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	$type =~ s/::.*$//;
	return $type;
}
#package View;
sub isvisible {
	my $self = shift;
	return $self->{viewer}->isvisible($self->kind);
}
#package View;
sub isanchor { return Glib::FALSE }
#package View;
sub getmodels { return {} }
#package View;
sub find {
	my $type = shift;
	my ($viewer,$data) = @_;
	my $view = $viewer->findview($data);
	delete $view->{new} if $view;
	return $view;
}
#package View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	$viewer = $self unless defined $viewer;
	$self->{viewer} = $viewer;
	$self->{data} = $data;
	$self->{pos} = [0,0,0,0];
	$viewer->setview($data,$self);
	$self->{new} = 1;
	$self->{fresh} = 1;
	$self->{siz}{t} = [0,0,0,0];
	$self->{pad} = [3.0,3.0];
	return $self;
}
#package View;
sub fini {
	my $self = shift;
	my $type = ref $self;
	my $kind = $type->kind();
	my $viewer = delete $self->{viewer};
	my $data = delete $self->{data};
	$viewer->putview($data);
	delete $self->{pos};
	delete $self->{siz};
}
#package View;
sub pos { return @{shift->{pos}} }
#package View;
sub siz {
	my $self = shift;
	my ($h,$w,$mh,$mw) = @{$self->{siz}{t}};
	$h += 2.0*$self->{pad}[0]; # if $h;
	$w += 2.0*$self->{pad}[1]; # if $w;
	return ($h,$w,$mh,$mw);
}
#package View;
sub gethandw {
	my $self = shift;
	unless (exists $self->{handw}) {
		if ($self->isvisible) {
			if (my $sub = $self->can('recalc')) {
				$self->{handw} = [ &$sub($self,@_) ];
			} else {
				$self->{handw} = [ 0, 0, 0, 0 ];
			}
		} else {
			$self->{handw} = [ 0, 0, 0, 0 ];
		}
	}
	return @{$self->{handw}};
}
#package View;
sub resize {
	my $self = shift;
	if ($self->isvisible) {
		$self->{siz}{t} = [ $self->recalc(@_) ];
	} else {
		$self->{siz}{t} = [ 0, 0, 0, 0 ];
	}
	return $self->siz;
}
#package View;
sub place_view {
	my $self = shift;
	return unless $self->{set};
	$self->{placing} = 1;
	my $type = ref $self;
	$self->_forw($type,undef,'placing',@_);
	delete $self->{placing};
}
#package View;
sub move_view {
	my $self = shift;
	my ($x,$y,$xo,$yo) = @_;
	my ($X,$Y,$XO,$YO) = $self->pos;
	$x = $X unless defined $x;
	$y = $Y unless defined $y;
	$xo = $XO unless defined $xo;
	$yo = $YO unless defined $yo;
	#return unless ($x != $X or $y != $Y or $xo != $XO or $yo != $YO);
	#Carp::carp "****** Moving $self ($x,$y,$xo,$yo) component is negative or zero" if $x <= 0 or $y <= 0;
	$self->{pos} = [$x,$y,$xo,$yo];
	$self->{set} = 1;
	$self->{moving} = 1;
	my $type = ref $self;
	$self->_forw($type,undef,'moving',@_);
	delete $self->{moving};
}
#package View;
sub show_view {
	my $self = shift;
	$self->{showing} = 1;
	my $type = ref $self;
	$self->_forw($type,undef,'showing',@_);
	delete $self->{showing};
}
#package View;
sub populate {
	my $self = shift;
	my $type = ref $self;
	if (delete $self->{fresh}) {
		$self->_forw($type,undef,'popu',@_);
	}
}
#package View;
sub xform {
	my ($type,$view) = @_;
	bless $view,$type;
}

# ------------------------------------------
package Group::View; our @ISA = qw(View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Group::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	my $layer = $self->{layer} = $viewer->getlayer($self->kind);
	my $grp = $self->{grp} = {pnts=>[0,0]};
	$grp->{item} = new Goo::Canvas::Group($layer,
			visibility=>'invisible',
			);
}
#package Group::View;
sub fini {
	my $self = shift;
	my $layer = delete $self->{layer};
	foreach (qw/grp/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $layer->find_child($itm->{item});
			if (defined $numb) {
				$layer->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $layer";
			}
		}
	}
}
#package Group::View;
sub makevisible {
	my $self = shift;
	my $vis = 'invisible';
	$vis = 'visible' if $self->{set} and $self->isvisible;
	$self->{grp}{item}->set(visibility=>$vis);
	return $vis;
}

# ------------------------------------------
package Clickable::View; our @ISA = qw(Group::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Clickable::View;
sub init {
	my $self = shift;
	my $group = $self->{grp}{item};
	$group->set('pointer-events','visible');
	$group->signal_connect('enter-notify-event'=>sub{
			my ($item,$targ,$ev,$self) = @_;
			$self->setcolor('cyan');
		},$self);
	$group->signal_connect('leave-notify-event'=>sub{
			my ($item,$targ,$ev,$self) = @_;
			$self->setcolor($self->mycolor);
		},$self);
	$group->signal_connect('button-press-event'=>sub{
			my ($item,$targ,$ev,$self) = @_;
			if ($ev->button == 3) {
				if (my $sub = $self->can('fillmenu')) {
					my $menu = new Gtk2::Menu;
					if (&$sub($self,$menu)) {
						$menu->show_all;
						$menu->popup(undef,undef,undef,undef,$ev->button,$ev->time);
						return Gtk2::EVENT_STOP;
					}
					$menu->destroy;
				}
			}
			elsif ($ev->button == 2) {
				$self->{data}->show;
				return Gtk2::EVENT_STOP;
			}
			elsif ($ev->button == 1) {
				$self->{data}->show;
				return Gtk2::EVENT_STOP;
			}
			return Gtk2::EVENT_PROPAGATE;
		},$self);
}

# ------------------------------------------
package Node::View; our @ISA = qw(ThreeColumn::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Node::View;
sub mycolor { return 'black' }
#package Node::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my $nod = $self->{nod} = {pnts=>[-8.0,-4.5,16,9]};
	$nod->{item} = new Goo::Canvas::Rect(
		$group,@{$nod->{pnts}},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	my $txt = $self->{txt} = {pnts=>[0,0,20]};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},'center',
#'antialias'=>'subpixel',
		alignment=>'center',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$txt->{item}->raise($nod->{item});
	$self->{pad} = [1.0,1.0];
}
#package Node::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	foreach (qw/nod txt/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $group->find_child($itm->{item});
			if (defined $numb) {
				$group->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $group";
			}
		}
	}
}
#package Node::View;
sub setcolor {
	my ($self,$color) = @_;
	$self->SUPER::setcolor($color);
	$self->{nod}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
}
#package Node::View;
sub gettxt {
	my $self = shift;
	return sprintf '(%s%d)',ref($self->{data}),$self->{data}{no};
}
#package Node::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my $color = $self->mycolor;
	my ($x,$y) = $self->pos;
	my $txt = $self->{txt};
	$txt->{pnts}[0] = $x;
	$txt->{pnts}[1] = $y;
	$txt->{item}->set(x=>$x,y=>$y,text=>$self->gettxt(),'fill-color'=>$color);
	my $nod = $self->{nod};
	$x-=$nod->{pnts}[2]/2.0;
	$y-=$nod->{pnts}[3]/2.0;
	$nod->{pnts}[0] = $x;
	$nod->{pnts}[1] = $y;
	$nod->{item}->set(x=>$x,y=>$y,'stroke-color'=>$color);
}
#package Node::View;
sub makevisible {
	my $self = shift;
	my $vis = $self->SUPER::makevisible(@_);
	$self->{nod}{item}->set(visibility=>$vis);
	$self->{txt}{item}->set(visibility=>$vis);
	return $vis;
}
#package Node::View;
sub showing { shift->makevisible() }
#package Node::View;
sub parent_propagate {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private'))) {
		my ($x,$y,$xofs,$yofs) = $self->pos;
		my ($X,$Y) = $parent->pos;
		my $sub = $tag.'_view';
		my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
		$self->$sub(@pos);
	}
}
#package Node::View;
sub parent_walk {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private'))) {
		my $sub = $tag.'_view';
		$self->$sub();
	}
}
#package Node::View;
sub parent_moving {
	my $self = shift;
	$self->Node::View::parent_propagate('move',@_);
}
#package Node::View;
sub parent_placing {
	my $self = shift;
	$self->Node::View::parent_walk('place',@_);
}
#package Node::View;
sub parent_showing {
	my $self = shift;
	$self->Node::View::parent_walk('show',@_);
}

# ------------------------------------------
package Bus::View; our @ISA = qw(Clickable::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Bus::View;
sub mycolor { return 'black' }
#package Bus::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my $bus = $self->{bus} = {pnts=>[0,-10,0,10]};
	$bus->{item} = new Goo::Canvas::Polyline(
		$group,Glib::FALSE,$bus->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.6,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	my $txt = $self->{txt} = {pnts=>[0,0,12]};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},'west',
#'antialias'=>'subpixel',
		alignment=>'left',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$txt->{item}->raise($bus->{item});
	my $txb = $self->{txb} = {pnts=>[0,0,12]};
	$txb->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txb->{pnts}},'west',
#'antialias'=>'subpixel',
		alignment=>'left',
		font=>'Sans 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$txb->{item}->raise($bus->{item});
	$self->{pad} = [1.0,1.0];
}
#package Bus::View;
sub setcolor {
	my ($self,$color) = @_;
	$self->{bus}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
	$self->{txb}{item}->set('fill-color'=>$color);
}
#package Bus::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	foreach (qw/bus txt txb/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $group->find_child($itm->{item});
			if (defined $numb) {
				$group->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $group";
			}
		}
	}
}
#package Bus::View;
sub gettxt {
	my $self = shift;
	return sprintf '(%s%d)',ref($self->{data}),$self->{data}{no};
}
#package Bus::View;
sub recalc {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	my ($hmrg,$wmrg);
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	$hbox = 15.0 if $hbox < 15.0;
	$wbox = 0.8;
	$hmrg = ($self->{txt}{pnts}[2]+$self->{txb}{pnts}[2])/2.0;
	$wmrg = 0;
	return ($hbox,$wbox,$hmrg,$wmrg);
}
#package Bus::View;
sub layout { }
#package Bus::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my $color = $self->mycolor;
	my ($x,$y) = $self->pos;
	my ($h,$w) = @{$self->{siz}{t}};
	my $bus = $self->{bus};
	$bus->{pnts}[0] = $x;
	$bus->{pnts}[1] = $y-$h/2.0;
	$bus->{pnts}[2] = $x;
	$bus->{pnts}[3] = $y+$h/2.0;
	$bus->{item}->set(points=>Goo::Canvas::Points->new($bus->{pnts}),'stroke-color'=>$color);
	my ($X,$Y) = ($x,$y+$h/2.0+$self->{pad}[0]);
	my $txt = $self->{txt};
	$txt->{pnts}[0] = $X;
	$txt->{pnts}[1] = $Y;
	$txt->{item}->set(text=>$self->gettxt(),'fill-color'=>$color);
	$txt->{item}->set_simple_transform($X,$Y,1.0,90.0);
	($X,$Y) = ($x,$y-$h/2.0-$self->{pad}[0]);
	my $txb = $self->{txb};
	$txb->{pnts}[0] = $X;
	$txb->{pnts}[1] = $Y;
	$txb->{item}->set(text=>$self->gettxt(),'fill-color'=>$color);
	$txb->{item}->set_simple_transform($X,$Y,1.0,-90.0);
}
#package Bus::View;
sub makevisible {
	my $self = shift;
	my $vis = $self->SUPER::makevisible(@_);
	$self->{bus}{item}->set(visibility=>$vis);
	$self->{txt}{item}->set(visibility=>$vis);
	$self->{txb}{item}->set(visibility=>$vis);
	return $vis;
}
#package Bus::View;
sub showing { shift->makevisible() }
#package Bus::View;
sub parent_propagate {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private'))) {
		my ($x,$y,$xofs,$yofs) = $self->pos;
		my ($X,$Y) = $parent->pos;
		my $sub = $tag.'_view';
		my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
		$self->$sub(@pos);
	}
}
#package Bus::View;
sub parent_walk {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private'))) {
		my $sub = $tag.'_view';
		$self->$sub();
	}
}
#package Bus::View;
sub parent_moving {
	my $self = shift;
	$self->Bus::View::parent_propagate('move',@_);
}
#package Bus::View;
sub parent_placing {
	my $self = shift;
	$self->Bus::View::parent_walk('place',@_);
}
#package Bus::View;
sub parent_showing {
	my $self = shift;
	$self->Bus::View::parent_walk('show',@_);
}

# ------------------------------------------
package BoxAndLink::View; our @ISA = qw(Clickable::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package BoxAndLink::View;
sub mycolor { return 'black' }
#package BoxAndLink::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my $box = $self->{box} = {pnts=>[-1,-1,2,2]};
	$box->{item} = new Goo::Canvas::Rect(
		$group,@{$box->{pnts}},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	my $lnk = $self->{lnk} = {pnts=>[0,0,5,0,10,0]};
	$lnk->{item} = new Goo::Canvas::Polyline(
		$group,Glib::FALSE,$lnk->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.2,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	$lnk->{item}->lower($box->{item});
	my $txt = $self->{txt} = {pnts=>[5,0,20,'sw',alignment=>'left']};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},
#'antialias'=>'subpixel',
		font=>'Monospace 1',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$txt->{item}->raise($box->{item});
	$self->{pad} = [0,0];
}
#package BoxAndLink::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	foreach (qw/box lnk txt/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $group->find_child($itm->{item});
			if (defined $numb) {
				$group->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $group";
			}
		}
	}
}
#package BoxAndLink::View;
sub setcolor {
	my ($self,$color) = @_;
	$self->{box}{item}->set('stroke-color'=>$color);
	$self->{lnk}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
}
#package BoxAndLink::View;
sub recalc {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	my ($hmrg,$wmrg);
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	my $h = $self->{box}{pnts}[2];
	my $w = $self->{box}{pnts}[3]+$self->{txt}{pnts}[2];
	$hbox = $h; # if $hbox < $h; $hbox = 2.0*$h if $hbox > 2.0*$h;
	$wbox = $w; # if $wbox < $w; $wbox = 2.0*$w if $wbox > 2.0*$w;
	$hmrg = 0;
	$wmrg = 0;
	return ($hbox,$wbox,$hmrg,$wmrg);
}
#package BoxAndLink::View;
sub layout {
	my $self = shift;
	return;
	my ($X,$Y) = $self->pos;
	my ($btype,$ntype) = $self->types;
	my $bview = $self->parent($btype) if $btype;
	if ($bview) {
		my ($x,$y) = $bview->pos;
		# horizontal aligment with bus
		my $lnk = $self->{lnk};
		my $txt = $self->{txt};
		$lnk->{pnts}[3] = $Y;
		$txt->{pnts}[1] = $Y;
		my @pos = ($x,$Y);
		$self->move_view(@pos);
		($X,$Y) = $self->pos;
	}
	my $nview = $self->parent($ntype) if $ntype;
	if ($nview) {
		unless ($bview) {
			# when there is no bus, just hang the
			# box near the point but toward the 
			# outside of the diagram.
			$X = ($X < 0) ? -20 : 20;
			my @pos = ($X,$Y);
			$self->move_view(@pos);
		}
		my ($x,$y) = $nview->pos;
		my $w = $nview->{nod}{pnts}[2]/2.0;
		my $lnk = $self->{lnk};
		my $txt = $self->{txt};
		if ($x < $X) {
			$lnk->{pnts}[2] = $x + $w + 0.5;
			$txt->{pnts}[0] = $x + $w + 1.0;
			$txt->{pnts}[3] = 'sw';
			$txt->{pnts}[5] = 'left';
		} else {
			$lnk->{pnts}[2] = $x - $w - 0.5;
			$txt->{pnts}[0] = $x - $w - 1.0;
			$txt->{pnts}[3] = 'se';
			$txt->{pnts}[5] = 'right';
		}
		$lnk->[4] = $x;
		$lnk->[5] = $y;
	}
}
#package BoxAndLink::View;
sub gettxt {
	my $self = shift;
	return sprintf '(%s%d)',ref($self->{data}),$self->{data}{no};
}
#package BoxAndLink::View;
sub types { return () }
#package BoxAndLink::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my $color = $self->mycolor;
	my ($x,$y) = $self->pos;
	my $lnk = $self->{lnk};
	my $txt = $self->{txt};
	$lnk->{pnts}[0] = $x;
	$lnk->{pnts}[1] = $y;
	$lnk->{pnts}[3] = $y;
	$txt->{pnts}[1] = $y;
	my ($btype,$ntype) = $self->types;
	if (my $nview = $self->parent($ntype)) {
		my ($X,$Y) = $nview->pos;
		my $w = $nview->{nod}{pnts}[2]/2.0;
		if ($x > $X) {
			$lnk->{pnts}[2] = $X + $w + 0.5;
			$txt->{pnts}[0] = $X + $w + 1.0;
			$txt->{pnts}[3] = 'sw';
			$txt->{pnts}[5] = 'left';
		} else {
			$lnk->{pnts}[2] = $X - $w - 0.5;
			$txt->{pnts}[0] = $X - $w - 1.0;
			$txt->{pnts}[3] = 'se';
			$txt->{pnts}[5] = 'right';
		}
		$lnk->{pnts}[4] = $X;
		$lnk->{pnts}[5] = $Y;
	}
	$lnk->{item}->set(points=>Goo::Canvas::Points->new($lnk->{pnts}),'stroke-color'=>$color);
	$txt->{item}->set(
		x=>$txt->{pnts}[0],
		y=>$txt->{pnts}[1],
		anchor=>$txt->{pnts}[3],
		alignment=>$txt->{pnts}[5],
		text=>$self->gettxt(),
		'fill-color'=>$color);
	my $box = $self->{box};
	$x-=$box->{pnts}[3]/2.0;
	$y-=$box->{pnts}[2]/2.0;
	$box->{pnts}[0] = $x;
	$box->{pnts}[1] = $y;
	$box->{item}->set(x=>$x,y=>$y,'stroke-color'=>$color);
}
#package BoxAndLink::View;
sub makevisible {
	my $self = shift;
	my $vis = $self->SUPER::makevisible(@_);
	$self->{box}{item}->set(visibility=>$vis);
	$self->{lnk}{item}->set(visibility=>$vis);
	$self->{txt}{item}->set(visibility=>$vis);
	return $vis;
}
#package BoxAndLink::View;
sub showing { shift->makevisible() }
#package BoxAndLink::View;
sub parent_propagate {
	my ($self,$tag,$parent,@args) = @_;
	my ($btype,$ntype) = $self->types;
	my $kind = $parent->kind;
	my ($nview,$bview);
	if ($kind eq $btype) {
		$bview = $parent;
		$nview = $self->parent($ntype);
	}
	elsif ($kind eq $ntype) {
		$nview = $parent;
		$bview = $self->parent($btype);
	}
	if ($nview and $bview and $nview->{set} and $bview->{set}) {
		my ($x,$y,$xofs,$yofs) = $self->pos;
		my ($bx,$by) = $bview->pos;
		my ($lx,$ly) = $nview->pos;
		my $sub = $tag.'_view';
		my @pos = ($bx+$xofs,$ly+$yofs,$xofs,$yofs);
		$self->$sub(@pos);
	}
}
#package BoxAndLink::View;
sub parent_walk {
	my ($self,$tag,$parent,@args) = @_;
	my ($btype,$ntype) = $self->types;
	my $kind = $parent->kind;
	my ($nview,$bview);
	if ($kind eq $btype) {
		$bview = $parent;
		$nview = $self->parent($ntype);
	}
	elsif ($kind eq $ntype) {
		$nview = $parent;
		$bview = $self->parent($btype);
	}
	if ($nview and $bview and $nview->{set} and $bview->{set}) {
		my $sub = $tag.'_view';
		$self->$sub();
	}
}
#package BoxAndLink::View;
sub parent_moving {
	my $self = shift;
	$self->BoxAndLink::View::parent_propagate('move',@_);
}
#package BoxAndLink::View;
sub parent_placing {
	my $self = shift;
	$self->BoxAndLink::View::parent_walk('place',@_);
}
#package BoxAndLink::View;
sub parent_showing {
	my $self = shift;
	$self->BoxAndLink::View::parent_walk('show',@_);
}

# ------------------------------------------
package Root::View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Root::View;
sub child {
	my $self = shift;
	return $self->{viewer}->findview($self->{data}->child(@_));
}
#package Root::View;
sub members {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->members(@_));
}
#package Root::View;
sub members_sorted {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->members_sorted(@_));
}
#package Root::View;
sub members_sortbytype {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->members_sortbytype(@_));
}
#package Root::View;
sub children {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->children(@_));
}
#package Root::View;
sub children_sorted {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->children_sorted(@_));
}
#package Root::View;
sub children_sortbytype {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->children_sortbytype(@_));
}
#package Root::View;
sub offspring {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->offspring(@_));
}
#package Root::View;
sub offspring_sorted {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->offspring_sorted(@_));
}
#package Root::View;
sub offspring_sortbytype {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->offspring_sortbytype(@_));
}
#package Root::View;
sub getoffspring {
	my $self = shift;
	return $self->{viewer}->getviews($self->{data}->offspring(@_));
}
#package Root::View;
sub popu {
	my $self = shift;
	foreach my $off ($self->getoffspring()) { $off->populate() }
}
#package Root::View;
sub propagate {
	my $self = shift;
	my $tag = shift;
	my $name = "parent_$tag";
	if ($self->can('ctypes')) {
		foreach my $off ($self->offspring_sorted([$self->ctypes])) {
			next if $off->{$tag};
			$off->$name($self,@_);
		}
	}
}
#package Root::View;
sub moving { shift->Root::View::propagate('moving',@_) }
#package Root::View;
sub placing { shift->Root::View::propagate('placing',@_) }
#package Root::View;
sub showing { shift->Root::View::propagate('showing',@_) }

# ------------------------------------------
package Leaf::View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Leaf::View;
sub parent {
	my ($self,$type) = @_;
	return $self->{viewer}->findview($self->{data}->parent($type));
}
#package Leaf::View;
sub parents {
	my $self = shift;
	return $self->{viewer}->findviews($self->{data}->parents);
}
#package Leaf::View;
sub parent_anchor { return Glib::FALSE }
#package Leaf::View;
sub propagate {
	my $self = shift;
	my $tag = shift;
	foreach my $parent ($self->parents) {
		next if $parent->{$tag};
		if (my $sub = $parent->can("child_$tag")) { &$sub($parent,$self,@_) }
	}
}
#package Leaf::View;
sub moving_old { shift->Leaf::View::propagate('moving',@_) }
#package Leaf::View;
sub placing_old { shift->Leaf::View::propagate('placing',@_) }
#package Leaf::View;
sub showing_old { shift->Leaf::View::propagate('showing',@_) }

# ------------------------------------------
package Tree::View; our @ISA = qw(Root::View Leaf::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

# ------------------------------------------
package Point::View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Point::View;
sub pathsx {
	my ($self,$side,$kind) = @_;
	return $self->{viewer}->findviews($self->{data}->pathsx($side,$kind));
}
#package Point::View;
sub pathsa {
	my ($self,$kind) = @_;
	return $self->{viewer}->findviews($self->{data}->pathsa($kind));
}
#package Point::View;
sub pathsb {
	my ($self,$kind) = @_;
	return $self->{viewer}->findviews($self->{data}->pathsb($kind));
}
#package Point::View;
sub paths {
	my ($self,$kind) = @_;
	return $self->{viewer}->findviews($self->{data}->paths($kind));
}
#package Point::View;
sub getpaths {
	my ($self,$kind) = @_;
	return $self->{viewer}->getviews($self->{data}->paths($kind));
}
#package Point::View;
sub popu {
	my $self = shift;
	foreach my $path ($self->getpaths()) { $path->populate() }
}
#package Point::View;
sub propagate {
	my $self = shift;
	my $tag = shift;
	foreach my $path ($self->pathsa()) {
		next if $path->{$tag};
		if (my $sub = $path->can("obja_$tag")) { &$sub($path,$self,@_) }
	}
	foreach my $path ($self->pathsb()) {
		next if $path->{$tag};
		if (my $sub = $path->can("objb_$tag")) { &$sub($path,$self,@_) }
	}
}
#package Point::View;
sub moving_old { shift->Point::View::propagate('moving',@_) }
#package Point::View;
sub placing_old { shift->Point::View::propagate('placing',@_) }
#package Point::View;
sub showing_old { shift->Point::View::propagate('showing',@_) }

# ------------------------------------------
package Path::View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Path::View;
sub obj {
	my $self = shift;
	my $side = shift;
	return $self->{viewer}->findview($self->{data}->obj($side));
}
#package Path::View;
sub objo {
	my $self = shift;
	my $side = shift;
	$side = (($side eq 'a') ? 'b' : 'a');
	return $self->obj($side);
}
#package Path::View;
sub obja { return shift->obj('a') }
#package Path::View;
sub objb { return shift->obj('b') }
#package Path::View;
sub objs {
	my $self = shift;
	return $self->obja,$self->objb;
}

# ------------------------------------------
package MibInfo; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ------------------------------------------
#package MibInfo;
sub init {
	my ($self,$dialog,$view,$table,$label) = @_;
	$self->{dialog} = $dialog;
	$self->{view} = $view;
	$self->{table} = $table;
	$self->{label} = $label;
	lock $Model::SNMP::lockvar;
	$self->{m} = $SNMP::MIB{$label};
}
#package MibInfo;
sub widthof {
	my $text = shift;
	return length($text)*8;
}
#package MibInfo;
sub strip {
	my ($msg) = @_;
	if (defined $msg) {
		$msg =~ s/^(\s|\n)*//s;
		$msg =~ s/(\s|\n)*$//s;
		$msg =~ s/\t/        /g;
		$msg =~ s/\n[ ]+\n/\n\n/g;
		while ($msg =~ /\n\n\n/) {
			$msg =~ s/\n\n\n/\n/g;
		}
		my $lead = undef;
		my $match;
		while ($msg =~ /(\n[ ]+)[^\n]/g) {
			$match = $1;
			if (defined $lead) {
				if (length($lead) > length($match)) {
					$lead = $match;
				}
			} else {
				$lead = $match;
			}
		}
		if (defined $lead) {
			$msg =~ s/$lead/\n/g;
		}
		$msg =~ s/^(\n|\s)*$//sg;
		chomp $msg;
	} else {
		$msg = '';
	}
	return $msg;
}
#package MibInfo;
sub reflow {
	my ($msg,$width) = @_;
	$width = 400 unless defined $width;
	my @lines;
	my @result = ();
	$msg =~ s/\n[ ]+\n/\n\n/sg;
	if ($msg =~ /[ ]\n[ ]*/s) {
		$msg =~ s/[ ]\n[ ]*/ /sg;
		@lines = split(/\n/,$msg);
	} elsif ($msg =~ /\n\n/s) {
		$msg =~ s/\.\n(?!\n)/. \n/sg;
		$msg =~ s/\n(?!\n)/ \n/sg;
		$msg =~ s/\n[ ]+\n/\n\n/sg;
		$msg =~ s/[ ]\n[ ]*/ /sg;
		@lines = split(/\n/,$msg);
	} else {
		$msg =~ s/\.\n/. \n/sg;
		$msg =~ s/\n[ ]*/ /sg;
		# FIXME - check for lead change
		@lines = split(/\n/,$msg);
	}
	foreach (@lines) {
		my ($indent,$lead,$measure) = ('','');
		$indent = $1 if s/^( *)//;
		$lead = $1 if s/^(([0-9]+\.|-|·)[ ]+)//;
		my $rest = $width - MibInfo::widthof($indent) - MibInfo::widthof($lead);
		while (($measure = MibInfo::widthof($_)) > $rest) {
			my ($pos,$where,$break) = (-1,-1,-1);
			while (($break = index($_,' ',$pos+1)) >= 0) {
				my $portion = MibInfo::widthof(substr($_,0,$break));
				last if $portion >= $rest;
				$where = $break unless $break == $pos+1;
				$pos = $break;
			}
			$where = $break if $where == 0;
			$where = length($_) if $where <= 0;
			push @result,$indent.$lead.substr($_,0,$where,'');
			s/^[ ]*//;
			$lead =~ s/./ /g;
			$rest = $width - MibInfo::widthof($indent) - MibInfo::widthof($lead);
		}
		push @result,$indent.$lead.$_;
	}
	@result = grep(!/^\s*$/,@result) if @result > 60; # turf blank lines
	$msg = join("\n",@result);
	return $msg;
}
#package MibInfo;
sub rehang {
	my ($msg,$width,$hang) = @_;
	$width = 400 unless defined $width;
	$hang = '        ' unless defined $hang;
	$msg =~ s/\n[ ]+\n/\n\n/sg;
	$msg =~ s/[ ]\n[ ]*/ /sg;
	my @result = ();
	my @lines = split(/\n/,$msg);
	foreach (@lines) {
		my ($indent,$lead) = ('','');
		$indent = $1 if s/^( *)//;
		my $rest = $width - MibInfo::widthof($indent) - MibInfo::widthof($lead);
		while ((my $measure = MibInfo::widthof($_)) > $rest) {
			my ($pos,$where,$break) = (-1,-1,-1);
			while (($break = index($_,' ',$pos + 1)) >= 0) {
				my $portion = MibInfo::widthof(substr($_,0,$break));
				last if $portion >= $rest;
				$where = $break unless $break == $pos + 1;
				$pos = $break;
			}
			$where = $break if $where == 0;
			$where = length($_) if $where <= 0;
			push @result,$indent.$lead.substr($_,0,$where,'');
			s/^[ ]*//;
			$lead = $hang;
			$rest = $width - MibInfo::widthof($indent) - MibInfo::widthof($lead);
		}
		push @result,$indent.$lead.$_;
	}
	$msg = join("\n",@result);
	return $msg;
}
#package MibInfo;
sub description {
	my ($self,$width) = @_;
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my $msg = MibInfo::strip($m->{description});
	$msg = MibInfo::reflow($msg,$width) if defined $width;
	$msg = "$m->{moduleID}::$m->{label}($m->{subID}): $m->{objectID}\n\n".$msg;
	return $msg;
}
#package MibInfo;
sub TCDescription {
	my ($self,$width) = @_;
	lock $Model::SNMP::lockvar;
	my $msg = MibInfo::strip($self->{m}{TCDescription});
	$msg = MibInfo::reflow($msg,$width) if defined $width;
	return $msg;
}
#package MibInfo;
sub reference {
	my ($self,$width) = @_;
	lock $Model::SNMP::lockvar;
	my $msg = MibInfo::strip($self->{m}{reference});
	$msg = MibInfo::rehang($msg,$width) if defined $width;
	return $msg;
}

# ------------------------------------------
package MibLabel; our @ISA = qw(MibInfo);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
# ------------------------------------------
#package MibLabel;
our %prefixes = (
	system=>'sys',
	ipAddr=>'ipAdEnt',
	ifX=>'if',
	lldpLocalSystemData=>'lldpLoc',
	lldpConfiguration=>'lldp',
	lldpXMedLocalData=>'lldpXMedLoc',
	lldpStatistics=>'lldpStats',
);
#package MibLabel;
sub init {
	my ($self,$dialog,$view,$table,$label) = @_;
	my $prefix = $table;
	$prefix =~ s/Data$//;
	$prefix =~ s/Scalars$//;
	$prefix =~ s/Table$//;
	$prefix =~ s/Entry$//;
	$prefix = $prefixes{$prefix} if exists $prefixes{$prefix};
	my $short = $label; $short =~ s/^$prefix//;
	my $f = $self->{f} = new Gtk2::Frame;
	my $l = $self->{l} = new Gtk2::Label($short);
	$l->set_alignment(1.0,0.5);
	$f->set_shadow_type('etched-out');
	$f->add($l);
	return $self;
}
#package MibLabel;
sub add_tooltip {
	my ($self,$tips) = @_;
	my $tip = $self->description(450);
	$tips->set_tip($self->{l},$tip) if $tip;
}
#package MibLabel;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}

# ------------------------------------------
package MibUnits; our @ISA = qw(MibInfo);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ------------------------------------------
#package MibUnits;
sub init {
	my ($self,$dialog,$view,$table,$label) = @_;
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my $units = '';
	$units = $m->{type} if $m->{type};
	$units = $m->{textualConvention} if $m->{textualConvention};
	$units = $m->{units} if $m->{units};
	my $f = $self->{f} = new Gtk2::Frame;
	my $u = $self->{u} = new Gtk2::Label($units);
	$u->set_alignment(0.0,0.5);
	$f->set_shadow_type('etched-out');
	$f->add($u);
	return $self;
}
#package MibUnits;
sub add_tooltip {
	my ($self,$tips) = @_;
	my $tip = $self->TCDescription(450);
	$tips->set_tip($self->{u},$tip) if $tip;
}
#package MibUnits;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}

# ------------------------------------------
package MibNode; our @ISA = qw(MibInfo);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ------------------------------------------
#package MibNode;
sub init {
	my ($self,$dialog,$view,$table,$label,$val) = @_;
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	$self->{val} = $val;
	my ($f,$e);
	if ((defined $m->{syntax} and $m->{syntax} eq 'TruthValue') or (defined $m->{type} and $m->{type} eq 'BOOLEAN')) {
		$$val = 0 unless defined $$val;
		$e = $self->{e} = new Gtk2::ToggleButton();
		$e->set_active(($$val eq 'true(1)' or $$val eq '1') ? TRUE : FALSE);
		$f = $self->{f} = $e;
		if ($m->{syntax} eq 'TruthValue') {
			bless $self,'MibNode::Truth';
		} else {
			bless $self,'MibNode::Boolean';
		}
	}
	elsif (defined $m->{type} and $m->{type} eq 'BITS') {
		$$val = '' unless defined $$val;
		my @rows = ();
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			push @rows, [ $tag, $m->{enums}{$tag} ];
		}
		$f = $self->{f} = new Gtk2::Frame;
		$e = $self->{e} = new Gtk2::SimpleList(tag=>'text',value=>'int');
		$e->set_headers_visible(FALSE);
		$e->set_grid_lines('horizontal');
		@{$e->{data}} = @rows;
		$e->get_selection->set_mode('multiple');
		my @sels = ();
		my @bits = ();
		foreach (values %{$m->{enums}}) { $bits[$_] = '0'; }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits);

		my $use = MibNode::List->snmpval($m,$$val);
		$bits = unpack('B*',$use).$bits;

		my $i = 0;
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			if (substr($bits,$m->{enums}{$tag},1) eq '1') {
				push @sels,$i;
			}
			$i++;
		}
		$e->select(@sels);
		$f->set_shadow_type('in');
		$f->add($e);
		bless $self,'MibNode::List';
	}
	elsif (defined $m->{enums} and $m->{enums} and scalar keys %{$m->{enums}} > 0) {
		my $use = MibNode::Combo->snmpval($m,$$val);
		$e = $self->{e} = Gtk2::ComboBox->new_text;
		$e->set_title($label);
		my $i = 0;
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			$e->append_text("$tag($m->{enums}{$tag})");
			$e->set_active($i) if $use eq $tag or ($use =~ /^\d+$/ and $use == $m->{enums}{$tag});
			$i++;
		}
		if (defined $m->{access} and $m->{access} =~ /Write|Create/) {
			$e->set('button-sensitivity'=>'on');
		} else {
			$e->set('button-sensitivity'=>'off');
		}
		$f = $self->{f} = $e;
		bless $self,'MibNode::Combo';
	}
	else {
		$$val = '' unless defined $$val;
		my $b = $self->{b} = Gtk2::EntryBuffer->new($$val);
		$e = $self->{e} = Gtk2::Entry->new_with_buffer($b);
		$e->set_editable((defined $m->{access} and $m->{access} =~ /Write|Create/) ? TRUE : FALSE);
		$e->set(visibility=>FALSE) if defined $m->{type} and $m->{type} eq 'PASSWORD';
		$f = $self->{f} = $e;
		bless $self,'MibNode::Entry';
	}
	return $self;
}
#package MibNode;
sub add_to_table {
	my ($self,$table,$row,$col) = @_;
	$table->attach($self->{f},$col,$col+1,$row,$row+1,'fill','fill',0,0);
}
#package MibNode;
sub revert {
	my $self = shift;
	my $val = $self->{val};
	$self->setval($$val);
}
#package MibNode;
sub store {
	my $self = shift;
	my $val = $self->{val};
	$$val = $self->getval;
}
#package MibNode;
sub classof {
	my ($m) = @_;
	if ((defined $m->{syntax} and $m->{syntax} eq 'TruthValue') or (defined $m->{type} and $m->{type} eq 'BOOLEAN')) {
		if ($m->{syntax} eq 'TruthValue') {
			return 'MibNode::Truth';
		} else {
			return 'MibNode::Boolean';
		}
	} elsif (defined $m->{type} and $m->{type} eq 'BITS') {
		return 'MibNode::List';
	} elsif (defined $m->{enums} and $m->{enums} and scalar keys %{$m->{enums}} > 0) {
		return 'MibNode::Combo';
	} else {
		return 'MibNode::Entry';
	}
}
#package MibNode;
sub convert {
	my ($m,$val) = @_;
	my $class = MibNode::classof($m);
	$val = "$class"->snmpval($m,$val);
	return $val;
}

# ----------------------------------
package MibNode::Truth; our @ISA = qw(MibNode);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
# ----------------------------------
#package MibNode::Truth;
sub snmpval {
	my ($type,$m,$val) = @_;
	if (not defined $val) {
		return MibNode::Truth->snmpval($m,$m->{defaultValue}) if exists
			$m->{defaultValue} and defined $m->{defaultValue};
		$val = 2;
	} elsif ($val=~/^\d+$/) {
		$val = 2 unless $val == 1;
	} else {
		$val = 1 if $val=~/true|yes|on/i;
		$val = 2 if $val=~/false|no|off/i;
	}
	return ($val);
}
#package MibNode::Truth;
sub getval {
	return shift->{e}->get_active ? 1 : 2;
}
#package MibNode::Truth;
sub setval {
	shift->{e}->set_active(shift == 1 ? TRUE : FALSE);
}

# ----------------------------------
package MibNode::Boolean; our @ISA = qw(MibNode);
use strict; use warnings; use Carp;
use Glib qw/TRUE FALSE/; use Gtk2; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
# ----------------------------------
#package MibNode::Boolean;
sub snmpval {
	my ($type,$m,$val) = @_;
	if ($val=~/^\d+$/) {
		$val = 0 unless $val == 1;
	} else {
		$val = 1 if $val=~/true|yes|on/i;
		$val = 0 if $val=~/false|no|off/i;
	}
	return ($val);
}
#package MibNode::Boolean;
sub getval {
	return shift->{e}->get_active ? 1 : 0;
}
#package MibNode::Boolean;
sub setval {
	shift->{e}->set_active(shift == 1 ? Glib::TRUE : Glib::FALSE);
}

# ----------------------------------
package MibNode::List; our @ISA = qw(MibNode);
use strict; use warnings; use Carp;
use Glib qw/TRUE FALSE/; use Gtk2; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ----------------------------------
sub snmpval {
	my ($type,$m,$val) = @_;
	if (not defined $val) {
		# try default
		return MibNode::List->snmpval($m,$m->{defaultValue}) if exists $m->{defaultValue} and defined $m->{defaultValue};
		# do all zeros
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0; }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits).'0000000';
		$bits = substr($bits,0,8*int(length($bits)/8));
		$val = pack('B*',$bits);
	} elsif ($val=~/^((\w+\b)(\n?\w+\b)*)?$/) {
		# found line feed array
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0; }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits).'0000000';
		$bits = substr($bits,0,8*int(length($bits)/8));
		foreach (split(/\n/,$val)) {
			if (defined $m->{enums}{$_}) {
				substr($bits,$m->{enums}{$_},1) = '1';
			} elsif (/^\d+$/) {
				substr($bits,$_,1) = '1';
			}
		}
		$val = pack('B*',$bits);
	} elsif ($val=~/^\s*{\s*((\w+\b)(\s*[,]?\s*\w+\b)*)?\s*}\s*$/) {
		# found parethentical list
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0; }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits).'0000000';
		$bits = substr($bits,0,8*int(length($bits)/8));
		$val=~s/^\s*{\s*//; $val=~s/\s*}\s*$//;
		foreach (split(/\b\s*[,]?\s*/,$val)) {
			if (defined $m->{enums}{$_}) {
				substr($bits,$m->{enums}{$_},1) = '1';
			} elsif (/^\d+$/) {
				substr($bits,$_,1) = '1';
			}
		}
		$val = pack('B*',$bits);
	} elsif ($val=~/^"([0-9a-fA-F][0-9a-fA-f])*\s*"$/) {
		# found hexadecimal string
		$val=~s/^"//; $val=~s/\s*"$//;
		$val = pack('H*',$val);
	} elsif ($val=~/^[0-9]+$/) {
		# found number
		$val = pack('N',$val);
	}
	return ($val);
}
sub getval {
	my $self = shift;
	my $e = $self->{e};
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my @bits = ();
	foreach (values %{$m->{enums}}) { $bits[$_] = 0; }
	foreach ($e->get_selected_indices) {
		$bits[$e->{data}[$_][1]] = 1;
	}
	my $bits = '';
	for (my $i = 0; $i < @bits; $i++) {
		$bits .= $bits[$i] == 1 ? '1' : '0';
	}
	return pack('B*',$bits);
}
sub setval {
	my ($self,$val) = @_;
	my $e = $self->{e};
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my @sels = ();
	my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = '0'; }
	@bits = map {'0'} @bits;
	my $bits = join('',@bits);
	$bits = unpack('B*',$val).$bits;
	my $i = 0;
	foreach (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
		if (substr($bits,$m->{enums}{$_},1) eq '1') {
			push @sels,$i;
		}
		$i++;
	}
	$e->select(@sels);
}

# ----------------------------------
package MibNode::Combo; our @ISA = qw(MibNode);
use strict; use warnings; use Carp;
use Glib qw/TRUE FALSE/; use Gtk2; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ----------------------------------
#package MibNode::Combo;
sub snmpval {
	my ($type,$m,$val) = @_;
	if ($m->{type} eq 'OCTETSTR') {
		if (not defined $val) {
			return MibNode::Combo->snmpval($m,$m->{defaultValue}) if
				exists $m->{defaultValue} and
				defined $m->{defaultValue};
			$val = '';
		} elsif ($val =~ /^\d+$/) {
			my %vals = map {$m->{enums}{$_}=>$_} keys %{$m->{enums}};
			$val = $vals{$val} if exists $vals{$val};
		} elsif ($val =~ /^(\w+)\((\d+)\)$/) {
			my ($tag,$num) = ($1,$2);
			my %vals = map {$m->{enums}{$_}=>$_} keys %{$m->{enums}};
			$val = $vals{$num} if exists $vals{$num};
			$val = $tag if exists $m->{enums}{$tag};
		}
	} elsif ($m->{type} eq 'INTEGER') {
		if (not defined $val) {
			return MibNode::Combo->snmpval($m,$m->{defaultValue}) if
				exists $m->{defaultValue} and
				defined $m->{defaultValue};
			$val = 0;
		} elsif ($val =~ /^(\w+)\((\d+)\)$/) {
			my ($tag,$num) = ($1,$2);
			$val = $num;
		} elsif ($val !~ /^\d+$/) {
			$val = $m->{enums}{$val} if exists $m->{enums}{$val};
		}
	}
	return $val;
}
#package MibNode::Combo;
sub getval {
	return shift->{e}->get_active_text;
}
#package MibNode::Combo;
sub setval {
	my ($self,$val) = @_;
	my $e = $self->{e};
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my $i = 0;
	my %rows = map {$m->{enums}{$_}=>$i++} keys %{$m->{enums}};
	$e->set_active($rows{$val});
}

# ----------------------------------
package MibNode::Entry; our @ISA = qw(MibNode);
use strict; use warnings; use Carp;
use Glib qw/TRUE FALSE/; use Gtk2; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
# ----------------------------------
#package MibNode::Entry;
sub snmpval {
	my ($type,$m,$val) = @_;
	if ($m->{type} eq 'OCTETSTR') {
		if (not defined $val) {
			return MibNode::Entry->snmpval($m,$m->{defaultValue}) if
				exists $m->{defaultValue} and
				defined $m->{defaultValue};
			$val = '';
		} elsif ($val =~ /^"([^"]*)"$/) {
			$val = $1;
		} elsif ($val =~ /^\\"([^"]*)\\"$/) {
			$val = $1;
		}
	} elsif ($m->{type} =~ /INTEGER|UNSIGNED|COUNTER|GAUGE/) {
		if (not defined $val) {
			return MibNode::Entry->snmpval($m,$m->{defaultValue}) if
				exists $m->{defaultValue} and
				defined $m->{defaultValue};
			$val = 0;
		} elsif ($val =~ /^0x/) {
			$val = int($val);
		} elsif ($val =~ /^'(([0-9a-fA-F][0-9a-fA-F])+)'H$/) {
			$val = int("0x$1");
		}
	}
	return $val;
}
#package MibNode::Entry;
sub getval {
	return shift->{e}->get_text;
}
#package MibNode::Entry;
sub setval {
	shift->{e}->set_text(shift);
}

# ------------------------------------------
package MibEntry; our @ISA = qw(Base Gtk2::Window);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
use threads::shared;
# ------------------------------------------
#package MibEntry;
sub new {
	my ($type,$view,$table) = @_;
	my $self = new Gtk2::Window('toplevel');
	bless $self,$type;
	my $data = $view->{data}{data}{$table};
	my @rows = $self->getrows($table);
	my @cols = $self->getcols($table,$data);
	$self->{view} = $view;
	$self->{table} = $table;
	$self->{data} = $data;
	$self->{rows} = \@rows;
	my $fr = new Gtk2::Frame($table);
	my $t = new Gtk2::Table(scalar(@rows),scalar(@cols)+2,FALSE);
	$t->set_col_spacings(0);
	$t->set_row_spacings(0);
	my $sw = new Gtk2::ScrolledWindow;
	$sw->set_policy((scalar(@cols)>2 ? 'automatic' : 'never'),'automatic');
	$sw->add_with_viewport($t);
	$fr->add($sw);
	my $tt = $self->{tooltips} = new Gtk2::Tooltips;
	my $i = 0;
	foreach my $row (@rows) {
		my $j = 0;
		my $lab = $self->{labels}{$row} = MibLabel->new($self,$view,$table,$row);
		$lab->add_to_table($t,$i,$j); $j++;
		$lab->add_tooltip($tt);
		foreach my $col (@cols) {
			$col->{$row} = undef unless exists $col->{$row};
			my $val = \$col->{$row};
			my $ent = $self->{entries}{$row}{$j} = MibNode->new($self,$view,$table,$row,$val);
			$ent->add_to_table($t,$i,$j); $j++;
		}
		my $uni = $self->{units}{$row} = MibUnits->new($self,$view,$table,$row);
		$uni->add_to_table($t,$i,$j); $j++;
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
	$vb->pack_start($fr,TRUE,TRUE,0);
	$bb->set_border_width(5);
	$vb->pack_start($bb,FALSE,FALSE,0);
	$self->set_type_hint('normal');
	$self->set_default_size(-1,600);
	$self->set_opacity(0.5);
	$self->set_position('mouse');
	$self->{deleted} = FALSE;
	$self->signal_connect('delete-event'=>sub{
		my ($self,$ev) = @_;
		$self->hide_all;
		return Gtk2::EVENT_STOP;
	});
	$self->add($vb);
	return $self;
}
#package MibEntry;
sub lexical_oids {
	my (@indexes) = @_;
	my %tuples = ();
	foreach my $index (@indexes) {
		$tuples{$index} = pack('S*',split(/\./,$index));
	}
	my @sort = sort {$tuples{$a} cmp $tuples{$b}} keys %tuples;
	return @sort;
}
#package MibEntry;
sub getcols {
	my ($self,$table,$data) = @_;
	my @cols = ();
	if ($table =~ /Table$/) {
		@cols = map {$data->{$_}} lexical_oids(keys %$data);
	} else {
		@cols = ( $data );
	}
	return @cols;
}
#package MibEntry;
sub getrows {
	my ($self,$table) = @_;
	my $label = $table;
	if ($label =~ s/Data$//) { }
	elsif ($label =~ s/Scalars$//) { }
	elsif ($label =~ s/Table$/Entry/) { }
	lock $Model::SNMP::lockvar;
	my $mib = $SNMP::MIB{$label};
	my @rows = @{$mib->{indexes}};
	my %inds = map {$_=>1} @rows;
	foreach my $row (sort {$a->{subID}<=>$b->{subID}} @{$mib->{children}}) {
		my $name = $row->{label};
		next if $inds{$name};
		next unless $row->{access} and $row->{access} =~ /Read|Write|Create/;
		push @rows, $name;
	}
	return @rows;
}
#package MibEntry;
sub refresh_view {
	my $self = shift;
	$self->enable_view unless $self->{active};
	$self->revert;
	return $self;
}
#package MibEntry;
sub enable_view {
	my $self = shift;
	$self->{active} = TRUE;
	return $self;
}
#package MibEntry;
sub disable_view {
	my $self = shift;
	$self->{active} = FALSE;
	return $self;
}
#package MibEntry;
sub store {
	my $self = shift;
	foreach (@{$self->{rows}}) { $self->{entries}{$_}->store }
}
#package MibEntry;
sub revert {
	my $self = shift;
	foreach (@{$self->{rows}}) { $self->{entries}{$_}->revert }
}
#package MibEntry;
sub closeButton {
	my ($self,$ev) = @_;
	$self->hide_all;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub applyButton {
	my ($self,$ev) = @_;
	$self->store;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub connectButton {
	my ($self,$ev) = @_;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub refreshButton {
	my ($self,$ev) = @_;
	$self->revert;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub revertButton {
	my ($self,$ev) = @_;
	$self->revert;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub cancelButton {
	my ($self,$ev) = @_;
	$self->hid_all;
	return Gtk2::EVENT_STOP;
}
#package MibEntry;
sub okButton {
	my ($self,$ev) = @_;
	$self->store;
	$self->hide_all;
	return Gtk2::EVENT_STOP;
}

# ------------------------------------------
package Datum::View; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use SNMP;
# ------------------------------------------
#package Datum::View;
sub fini {
	my $self = shift;
	foreach my $dia (values %{$self->{mibentry}}) { $dia->destroy }
	delete $self->{mibentry};
}
#package Datum::View;
sub fillmenu {
	my ($self,$menu) = @_;
	my $gotone = 0;
	if (my $data = $self->{data}{data}) {
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
							MibEntry->new($self,$table);
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
package ThreeColumn::View; our @ISA = qw(Clickable::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package ThreeColumn::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @-;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my $tbox = $self->{tbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$tbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$tbox->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $lbox = $self->{lbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$lbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$lbox->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $cbox = $self->{cbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$cbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$cbox->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $rbox = $self->{rbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$rbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$rbox->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
}
#package ThreeColumn::View;
sub setcolor {
	my ($self,$color) = @_;
	$self->{tbox}{item}->set('stroke-color'=>$color);
	$self->{lbox}{item}->set('stroke-color'=>$color);
	$self->{cbox}{item}->set('stroke-color'=>$color);
	$self->{rbox}{item}->set('stroke-color'=>$color);
}
#package ThreeColumn::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	foreach (qw/tbox lbox cbox rbox/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $group->find_child($itm->{item});
			if (defined $numb) {
				$group->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $group";
			}
		}
	}
}
#package ThreeColumn::View;
sub makevisible {
	my $self = shift;
	my $vis = $self->SUPER::makevisible(@_);
	if (0) {
		$self->{tbox}{item}->set(visibility=>$vis);
		$self->{lbox}{item}->set(visibility=>$vis);
		$self->{cbox}{item}->set(visibility=>$vis);
		$self->{rbox}{item}->set(visibility=>$vis);
	}
	return $vis;
}
#package ThreeColumn::View;
sub showing { shift->makevisible() }
#package ThreeColumn::View;
sub placing {
	my $self = shift;
	#return unless $self->{set};
	my $color = $self->mycolor;
	my ($x,$y) = $self->pos;
	my $siz = $self->{siz};
	my ($th,$tw) = map {$_/2.0} @{$siz->{t}};
	my ($tx,$ty) = ($x,$y);
	my $tbox = $self->{tbox};
	$tbox->{pnts} = [$tx-$tw,$ty-$th,$tx+$tw,$ty-$th,$tx+$tw,$ty+$th,$tx-$tw,$ty+$th];
	$tbox->{item}->set(points=>Goo::Canvas::Points->new($tbox->{pnts}),'stroke-color'=>$color);
	my ($ch,$cw) = (0,0);
	if ($siz->{c}{total}) {
		($ch,$cw) = map {$_/2.0} @{$siz->{c}{total}};
		my ($cx,$cy) = ($x,$y);
		my $cbox = $self->{cbox};
		$cbox->{pnts} = [$cx-$cw,$cy-$ch,$cx+$cw,$cy-$ch,$cx+$cw,$cy+$ch,$cx-$cw,$cy+$ch];
		$cbox->{item}->set(points=>Goo::Canvas::Points->new($cbox->{pnts}),'stroke-color'=>$color);
	}
	if ($siz->{l}{total}) {
		my ($lh,$lw) = map {$_/2.0} @{$siz->{l}{total}};
		my ($lx,$ly) = ($x-$cw-$lw,$y);
		my $lbox = $self->{lbox};
		$lbox->{pnts} = [$lx-$lw,$ly-$lh,$lx+$lw,$ly-$lh,$lx+$lw,$ly+$lh,$lx-$lw,$ly+$lh];
		$lbox->{item}->set(points=>Goo::Canvas::Points->new($lbox->{pnts}),'stroke-color'=>$color);
	}
	if ($siz->{r}{total}) {
		my ($rh,$rw) = map {$_/2.0} @{$siz->{r}{total}};
		my ($rx,$ry) = ($x+$cw+$rw,$y);
		my $rbox = $self->{rbox};
		$rbox->{pnts} = [$rx-$rw,$ry-$rh,$rx+$rw,$ry-$rh,$rx+$rw,$ry+$rh,$rx-$rw,$ry+$rh];
		$rbox->{item}->set(points=>Goo::Canvas::Points->new($rbox->{pnts}),'stroke-color'=>$color);
	}
}
#package ThreeColumn::View;

# ------------------------------------------
package Subnetwork::View; our @ISA = qw(ThreeColumn::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Subnetwork::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	$self->{pad} = [1.0,1.0];
	foreach (qw/tbox lbox cbox rbox/) {
		if (my $itm = delete $self->{$_}) {
			my $numb = $group->find_child($itm->{item});
			if (defined $numb) {
				$group->remove_child($numb);
			} else {
				warn "****** Could not find child $itm->{item} in group $group";
			}
		}
	}
}
#package Subnetwork::View;
sub getsize {
	my $self = shift;
	my ($hbox,$wbox,$geom) = @_;
	foreach my $side (keys %$geom) {
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $view (@{$geom->{$side}{$kind}}) {
				my $hsiz = $side eq 'c' ? 0 : $hbox;
				$view->{want} = [$hsiz,$wbox];
			}
		}
	}
}
#package Subnetwork::View;
sub sizecalc {
	my $self = shift;
	my ($geom,$size) = @_;
	my $tsiz = $size->{t} = [0,0,0,0];
	$size->{l}{total} = [0,0,0,0];
	$size->{c}{total} = [0,0,0,0];
	$size->{r}{total} = [0,0,0,0];
	my ($smax,$mmax) = (0,0);
	foreach my $side (keys %$geom) {
		my $ssiz = $size->{$side}{total} = [0,0,0,0];
		if ($side eq 'c') {
			foreach my $kind (keys %{$geom->{$side}}) {
				my $ksiz = $size->{$side}{$kind} = [0,0,0,0];
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($hbox,$wbox) = @{$view->{want}};
					my ($h,$w,$mh,$mw) = $view->resize($hbox,$wbox);
					$ksiz->[0] = $h if $h > $ksiz->[0];
					$ksiz->[1] += $w;
					$ksiz->[2] = $mh if $mh > $ksiz->[2] and $h;
					$ksiz->[3] += $mw if $w;
				}
				$ssiz->[0] = $ksiz->[0] if $ksiz->[0] > $ssiz->[0];
				$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
				$ssiz->[2] = $ksiz->[2] if $ksiz->[2] > $ssiz->[2] and $ksiz->[0];
				$ssiz->[3] = $ksiz->[3] if $ksiz->[3] > $ssiz->[3] and $ksiz->[1];
			}
			$tsiz->[1] += $ssiz->[1];
			$tsiz->[3] += $ssiz->[3] if $ssiz->[1];
		} else {
			foreach my $kind (keys %{$geom->{$side}}) {
				my $ksiz = $size->{$side}{$kind} = [0,0,0,0];
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($hbox,$wbox) = @{$view->{want}};
					my ($h,$w,$mh,$mw) = $view->resize($hbox,$wbox);
					$ksiz->[0] += $h;
					$ksiz->[1] = $w if $w > $ksiz->[1];
					$ksiz->[2] += $mh if $h;
					$ksiz->[3] = $mw if $mw > $ksiz->[3] and $w;
				}
				$ssiz->[0] += $ksiz->[0];
				$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
				$ssiz->[2] += $ksiz->[2] if $ksiz->[0];
				$ssiz->[3] = $ksiz->[3] if $ksiz->[3] > $ssiz->[3] and $ksiz->[1];
			}
			$smax = $ssiz->[1] if $ssiz->[1] > $smax;
			$mmax = $ssiz->[3] if $ssiz->[3] > $mmax and $ssiz->[1];
		}
		$tsiz->[0] = $ssiz->[0] if $ssiz->[0] > $tsiz->[0];
		$tsiz->[2] = $ssiz->[2] if $ssiz->[2] > $tsiz->[2] and $ssiz->[0];
	}
	$size->{l}{total}[1] = $size->{r}{total}[1] = $smax;
	$size->{l}{total}[3] = $size->{r}{total}[3] = $mmax if $smax;
	$tsiz->[1] += $smax + $smax;
	$tsiz->[3] += $mmax + $mmax if $smax;
}
#package Subnetwork::View;
sub shrink {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my ($hall,$wall) = @{$size->{t}};
	my ($hfac,$wfac) = ($hbox/$hall,$wbox/$wall);
	foreach my $side (keys %$geom) {
		if ($side eq 'c') {
			my $hsiz = $hbox;
			foreach my $kind (keys %{$geom->{$side}}) {
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $view->siz;
					my $wsiz = $w*$wfac;
					$view->{want} = [$hsiz,$wsiz];
				}
			}
		} else {
			my $wsiz = ($size->{$side}{total}[1]+2.0*$size->{$side}{total}[3])*$wfac;
			foreach my $kind (keys %{$geom->{$side}}) {
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $view->siz;
					my $hsiz = $h*$hfac;
					$view->{want} = [$hsiz,$wsiz];
				}
			}
		}
	}
}
#package Subnetwork::View;
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	my $geom = $self->{geo} = $self->getgeom;
	my $size = $self->{siz} = {};
	$self->getsize($hbox,$wbox,$geom);
	$self->sizecalc($geom,$size);
	$self->rebalance($hbox,$wbox,$geom,$size);
	$self->sizecalc($geom,$size);
	if ($size->{t}[0]+$size->{t}[2]*2.0 > $hbox or $size->{t}[1]+$size->{t}[3]*2.0 > $wbox) {
		$self->shrink($hbox,$wbox,$geom,$size);
		$self->sizecalc($geom,$size);
	}
	my $hsiz = $size->{t}[0];
	foreach my $kind (keys %{$geom->{c}}) {
		foreach my $view (@{$geom->{c}{$kind}}) {
			my ($h,$wsiz) = $view->siz;
			$view->resize($hsiz,$wsiz);
		}
	}
	return $self->siz;
}
#package Subnetwork::View;
sub layout {
	my $self = shift;
	my $kind = $self->kind;
	my $geom = delete $self->{geo};
	my $size = $self->{siz};
	#warn "****** Laying out $self:\n", Data::Dumper->new([$geom])->Maxdepth(3)->Dump, Data::Dumper->new([$size])->Dump;
	my ($X,$Y) = $self->pos;
	foreach my $side (keys %$geom) {
		if ($side eq 'c') {
			my $yofs = 0;
			foreach my $type (keys %{$geom->{$side}}) {
				my ($H,$W,$MH,$MW) = @{$size->{$side}{$type}};
				my $x0 = -$W/2.0;
				my $x = $x0;
				foreach my $view (@{$geom->{$side}{$type}}) {
					my ($h,$w,$mh,$mw) = $view->siz;
					my $xofs = $x + $w/2.0 + $mw;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					$view->move_view(@pos);
					$view->layout();
					$x += $w + 2.0*$mw;
				}
			}
		} else {
			my ($H,$W,$MH,$MW) = @{$size->{$side}{total}};
			my $y = -$H/2.0;
			my $x = $size->{c}{total}[1]/2.0+$size->{c}{total}[3];
			foreach my $type (keys %{$geom->{$side}}) {
				foreach my $view (@{$geom->{$side}{$type}}) {
					my ($h,$w,$mh,$mw) = $view->siz;
					my $xofs = $x + $w/2.0 + $mw;
					$xofs = -$xofs if $side eq 'l';
					my $yofs = $y + $h/2.0 + $mw;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					$view->move_view(@pos);
					$view->layout();
					$y += $h + 2.0*$mh;
				}
			}
		}
	}
}

# ------------------------------------------
package Network::View; our @ISA = qw(Subnetwork::View Point::View Root::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Network::View;
sub mycolor { return 'grey' }
#package Network::View;
sub ctypes { return qw/Private Host Vlan Lan Subnet/ }
#package Network::View;
sub isanchor { return Glib::TRUE }
#package Network::View;
sub geom {
	my $self = shift;
	my %geom = ();
	foreach my $net ($self->children('Private')) {
		if ($net->isa('Private::Here::View')) {
			push @{$geom{l}{Private}}, $net;
		} else {
			push @{$geom{r}{Private}}, $net;
		}
	}
	foreach my $host ($self->offspring_sortbytype('Host',[
				Item::IPV4ADDRKEY,
				Item::IPV6ADDRKEY])) {
		next if ($host->parent('Private'));
		if ($host->isa('Host::Ip::Here::View')) {
			push @{$geom{l}{Host}}, $host;
		} else {
			push @{$geom{r}{Host}}, $host;
		}
	}
	foreach my $form (qw/Vlan Lan Subnet/) {
		foreach my $bus ($self->offsprint_sortbytype($form,[
					Item::MACKEY,
					Item::VLANKEY,
					Item::IPV4MASKKEY,
					Item::IPV6MASKKEY])) {
			push @{$geom{c}{$form}},$bus;
		}
	}
	return \%geom;
}
#package Network::View;
sub getgeom {
	my $self = shift;
	my %geom = ();
	foreach my $net ($self->children('Private')) {
		if ($net->isa('Private::Here::View')) {
			push @{$geom{l}{Private}},$net;
			$net->{col} = -2;
		} else {
			push @{$geom{r}{Private}},$net;
			$net->{col} = +2;
		}
	}
	foreach my $host ($self->offspring_sortbytype('Host',[
				Item::IPV4ADDRKEY,
				Item::IPV6ADDRKEY])) {
		next if ($host->parent('Private'));
		if ($host->isa('Host::Ip::Here::View')) {
			push @{$geom{l}{Host}},$host;
			$host->{col} = -1;
		} else {
			push @{$geom{r}{Host}},$host;
			$host->{col} = +1;
		}
	}
	foreach my $form (qw/Lan Vlan Subnet/) {
		foreach my $bus ($self->offspring_sortbytype($form,[
					Item::MACKEY,
					Item::VLANKEY,
					Item::IPV4MASKKEY,
					Item::IPV6MASKKEY])) {
			push @{$geom{c}{$form}},$bus;
			$bus->{col} = 0;
		}
	}
	return \%geom;
}
#package Network::View;
sub rebalance {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my %views = (l=>[],r=>[]);
	foreach my $side (qw/l r/) {
		foreach my $kind (qw/Private Host/) {
			if ($geom->{$side}{$kind}) {
				push @{$views{$side}}, @{$geom->{$side}{$kind}};
			}
		}
	}
	while (@{$views{r}} > 1) {
		my $view = $views{r}[-1];
		my ($h,$w) = $view->siz;
		last if ($size->{l}{total}[0] + $h > $size->{r}{total}[0] - $h);
		$size->{l}{total}[0] += $h;
		$size->{r}{total}[0] -= $h;
		push @{$views{l}},pop @{$views{r}};
		my $kind = $view->kind;
		push @{$geom->{l}{$kind}}, pop @{$geom->{r}{$kind}};
		$view->{col} = -$view->{col};
	}
}

# ------------------------------------------
package Private::View; our @ISA = qw(Subnetwork::View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Private::View;
sub mycolor { return 'orange' }
#package Private::View;
sub ctypes { return qw/Host Vlan Lan Subnet/ }
#package Private::View;
sub geom {
	my $self = shift;
	my %geom = ();
	foreach my $host ($self->offspring_sortbytype('Host',[
				Item::IPV4ADDRKEY,
				Item::IPV6ADDRKEY])) {
		if ($host->parent('Network')) {
			push @{$geom{l}{Host}}, $host;
		}
		elsif ($host->parent('Private')) {
			push @{$geom{r}{Host}}, $host;
		}
	}
	foreach my $form (qw/Vlan Lan Subnet/) {
		foreach my $bus ($self->offspring_sortbytype($form,[
					Item::MACKEY,
					Item::VLANKEY,
					Item::IPV4MASKKEY,
					Item::IPV6MASKKEY])) {
			unless ($bus->parent('Network')) {
				push @{$geom{c}{$form}},$bus;
			}
		}
	}
	return \%geom;
}
#package Private::View;
sub getgeom {
	my $self = shift;
	my %geom = ();
	my $col = $self->{col};
	my $l = $col > 0 ? 'l' : 'r';
	my $r = $col > 0 ? 'r' : 'l';
	foreach my $host ($self->offspring_sortbytype('Host',[
				Item::IPV4ADDRKEY,
				Item::IPV6ADDRKEY])) {
		if ($host->parent('Network')) {
			push @{$geom{$l}{Host}},$host;
			$host->{col} = ($col > 0) ? $col-1 : $col+1;
		}
		elsif ($host->parent('Private')) {
			push @{$geom{$r}{Host}},$host;
			$host->{col} = ($col > 0) ? $col+1 : $col-1;
		}
	}
	foreach my $form (qw/Lan Vlan Subnet/) {
		foreach my $bus ($self->offspring_sortbytype($form,[
					Item::MACKEY,
					Item::VLANKEY,
					Item::IPV4MASKKEY,
					Item::IPV6MASKKEY])) {
			unless ($bus->parent('Network')) {
				push @{$geom{c}{$form}},$bus;
				$bus->{col} = $col;
			}
		}
	}
	return \%geom;
}
#package Private::View;
sub rebalance {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my $col = $self->{col};
	my $l = $col > 0 ? 'l' : 'r';
	my $r = $col > 0 ? 'r' : 'l';
	my %views = ($l=>[],$r=>[]);
	foreach my $side (qw/l r/) {
		foreach my $kind (qw/Host/) {
			if ($geom->{$side}{$kind}) {
				push @{$views{$side}}, @{$geom->{$side}{$kind}};
			}
		}
	}
	while (@{$views{$r}} > 1) {
		my $view = $views{$r}[-1];
		my ($h,$w) = $view->siz;
		last if ($size->{$l}{total}[0] + $h > $size->{$r}{total}[0] - $h);
		$size->{$l}{total}[0] += $h;
		$size->{$r}{total}[0] -= $h;
		push @{$views{$l}}, pop @{$views{$r}};
		my $kind = $view->kind;
		push @{$geom->{$l}{$kind}}, pop @{$geom->{$r}{$kind}};
		$view->{col} = ($col > 0) ? $col-1 : $col+1;
	}
}
#package Private::View;
sub parent_propagate {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Network') {
		my ($x,$y,$xofs,$yofs) = $self->pos;
		my ($X,$Y) = $parent->pos;
		my $sub = $tag.'_view';
		my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
		$self->$sub(@pos);
	}
}
#package Private::View;
sub parent_walk {
	my ($self,$tag,$parent) = @_;
	my $kind = $parent->kind;
	if ($kind eq 'Network') {
		my $sub = $tag.'_view';
		$self->$sub();
	}
}
#package Private::View;
sub parent_moving {
	my $self = shift;
	$self->Private::View::parent_propagate('move',@_);
}
#package Private::View;
sub parent_placing {
	my $self = shift;
	$self->Private::View::parent_walk('place',@_);
}
#package Private::View;
sub parent_showing {
	my $self = shift;
	$self->Private::View::parent_walk('show',@_);
}

# ------------------------------------------
package Private::Here::View; our @ISA = qw(Private::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
sub xformed {
	my ($type,$self) = @_;
	bless $self,$type;
}

# ------------------------------------------
package Local::View; our @ISA = qw(View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Local::View;
sub repos {
	my ($type,$viewer) = @_;
	$type = ref $type if ref $type;
	my $kind = $type->kind();
}

# ------------------------------------------
package Local::Here::View; our @ISA = qw(Local::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

# ------------------------------------------
package Host::View; our @ISA = qw(Node::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Host::View;
sub mycolor { return 'red' }
#package Host::View;
sub ctypes { return qw/Vprt Port Address/ }
#package Host::View;
sub getgeom {
	my $self = shift;
	my ($x,$y) = $self->pos;
	my %geom = ();
	my $col = $self->{col};
	foreach my $vprt ($self->offspring_sortbytype('Vprt',[
				Item::INTKEY,
				Item::VLANKEY])) {
		my $vlan = $vprt->parent('Vlan');
		if ($vlan and exists $vlan->{col} and ($vlan->parent('Network') or $vlan->parent('Private'))) {
			if ($vlan->{col} < $col)
			{ push @{$geom{l}{Vprt}},$vprt } else
			{ push @{$geom{r}{Vprt}},$vprt }
		}
	}
	foreach my $port ($self->offspring_sortbytype('Port',[
				Item::INTKEY,
				Item::MACKEY])) {
		my $plan = $port->parent('Lan');
		if ($plan and exists $plan->{col} and ($plan->parent('Network') or $plan->parent('Private'))) {
			if ($plan->{col} < $col)
			{ push @{$geom{l}{Port}},$port } else
			{ push @{$geom{r}{Port}},$port }
		}
	}
	foreach my $addr ($self->offspring_sortbytype('Address',[
				Item::IPV4ADDRKEY,
				Item::IPV6ADDRKEY])) {
		my $subn = $addr->parent('Subnet');
		if ($subn and exists $subn->{col} and ($subn->parent('Network') or $subn->parent('Private'))) {
			if ($subn->{col} < $col)
			{ push @{$geom{l}{Address}},$addr } else
			{ push @{$geom{r}{Address}},$addr }
		}
	}
	return \%geom;
}
#package Host::View;
sub getsize {
	my $self = shift;
	my ($hbox,$wbox,$geom) = @_;
	$self->{want} = [$hbox,$wbox];
	foreach my $side (keys %$geom) {
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $view (@{$geom->{$side}{$kind}}) {
				$view->{want} = [$hbox,$wbox];
			}
		}
	}
}
#package Host::View;
sub sizecalc {
	my $self = shift;
	my ($geom,$size) = @_;
	my $pnts = $self->{nod}{pnts};
	$size->{c}{Host} = [$pnts->[3],$pnts->[2],0,0];
	$size->{c}{total} = [@{$size->{c}{Host}},0,0];
	my $tsiz = $size->{t} = [@{$size->{c}{Host}},0,0];
	$size->{l}{total} = [0,0,0,0];
	$size->{r}{total} = [0,0,0,0];
	my $smax = 0;
	foreach my $side (keys %$geom) {
		my $ssiz = $size->{$side}{total} = [0,0,0,0];
		foreach my $kind (keys %{$geom->{$side}}) {
			my $ksiz = $size->{$side}{$kind} = [0,0,0,0];
			foreach my $view (@{$geom->{$side}{$kind}}) {
				my ($hbox,$wbox) = @{$view->{want}};
				my ($h,$w) = $view->resize($hbox,$wbox);
				$ksiz->[0] += $h;
				$ksiz->[1] = $w if $w > $ksiz->[1];
			}
			$ssiz->[0] = $ksiz->[0] if $ksiz->[0] > $ssiz->[0];
			$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
		}
		$tsiz->[0] = $ssiz->[0] if $ssiz->[0] > $tsiz->[0];
		$smax = $ssiz->[1] if $ssiz->[1] > $smax;
	}
	$size->{l}{total}[1] = $smax;
	$size->{r}{total}[1] = $smax;
	$tsiz->[1] += $smax + $smax;
}
#package Host::View;
sub shrink {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my ($hall,$wall) = @{$size->{t}};
	my ($hfac,$wfac) = ($hbox/$hall,$wbox/$wall);
	foreach my $side (keys %$geom) {
		my $wsiz = $size->{$side}{total}[1]*$wfac;
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $view (@{$geom->{$side}{$kind}}) {
				my ($h,$v) = $view->siz;
				my $hsiz = $h*$hfac;
				$view->{want} = [$hsiz,$wsiz];
			}
		}
	}
}
#package Host::View;
sub geom {
	my $self = shift;
	my %geom = ();
	my @buses = qw/Vlan Lan Subnet/;
	foreach my $kind (qw/Vprt Port Address/) {
		my $ptype = shift @buses;
		foreach my $box ($self->offspring($kind)) {
			if (my $bus = $box->parent($ptype)) {
				if ($bus->parent('Network')) {
					push @{$geom{l}{$kind}}, $box;
				}
				elsif ($bus->parent('Private')) {
					push @{$geom{r}{$kind}}, $box;
				}
			} else {
				push @{$geom{r}{$kind}}, $box;
			}
		}
	}
	return \%geom;
}
#package Host::View;
sub recalc {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	my ($hmrg,$wmrg);
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	# The height is simply the maximum of the sum of the heights of all
	# boxes on either size and the height of the node itself.
	# The width is simply the maximum width of the boxes on the left plus
	# the width of the node plus the maximum width of the boxes on the
	# right.
	my $geom = $self->geom();
	$hbox = $self->{nod}{pnts}[3]; # height of node alone
	$wbox = $self->{nod}{pnts}[2]; # width of node alone
	my %wmax = (l=>0,r=>0);
	foreach my $side (qw/l r/) {
		foreach my $kind (keys %{$geom->{$side}}) {
			my $wmax = \$wmax{$side};
			my $hsum = 0;
			foreach my $box (@{$geom->{$side}{$kind}}) {
				my ($h,$w) = $box->gethandw($hbox,$wbox);
				$hsum += $h;
				$$wmax = $w if $$wmax < $w;
			}
			$hbox = $hsum if $hbox < $hsum;
		}
	}
	$wbox += $wmax{l} + $wmax{r};
	$hmrg = 0;
	$wmrg = 0;
	return ($hbox,$wbox,$hmrg,$wmrg);
}
#package Host::View;
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	my $geom = $self->{geo} = $self->getgeom;
	my $size = $self->{siz} = {};
	$self->getsize($hbox,$wbox,$geom);
	$self->sizecalc($geom,$size);
	if ($size->{t}[0] > $hbox or $size->{t}[1] > $wbox) {
		$self->shrink($hbox,$wbox,$geom,$size);
		$self->sizecalc($geom,$size);
	}
	return $self->siz;
}
#package Host::View;
sub layout {
	my $self = shift;
	my $geom = delete $self->{geo};
	my $size = $self->{siz};
	my ($X,$Y) = $self->pos;
	foreach my $side (keys %$geom) {
		my $x0 = $size->{c}{total}[1]/2.0 + $size->{$side}{total}[1]/2.0;
		$x0 = -$x0 if $side eq 'l';
		foreach my $kind (keys %{$geom->{$side}}) {
			my @views = @{$geom->{$side}{$kind}};
			if (@views) {
				my ($H,$W) = @{$size->{$side}{$kind}};
				my $y0 = -$H/2.0;
				my $y = $y0;
				foreach my $view (@views) {
					my ($btype,$ntype) = $view->types;
					my ($x,$yold,$xofs) = $view->pos;
					($x,$xofs) = ($x0,0) unless $view->parent($btype);
					my ($h,$w) = $view->siz;
					my $yofs = $y + $h/2.0;
					my @pos = ($x,$Y+$yofs,$xofs,$yofs);
					$view->move_view(@pos);
					$view->layout();
					$y += $h;
				}
			}
		}
	}
}

# ------------------------------------------
package Host::Ip::View; our @ISA = qw(Host::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Host::Ip::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{data}{data};
	my $sysd = $data->{systemData};
	my $color = ($sysd and exists $sysd->{sysName}) ? 'black' : 'darkgrey';
	return $color;
}
#package Host::Ip::View;
sub gettxt {
	my $self = shift;
	my @lines = ();
	my $data = $self->{data}{data};
	my $sysd = $data->{systemData};
	my $name = $sysd->{sysName} if $sysd;
	push @lines, $name if $name;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if ($self->{data}{key}{p}{$kind}) {
			push @keys,sort keys %{$self->{data}{key}{p}{$kind}};
		}
	}
	my $key = $keys[0];
	push @lines, Item::showkey($key) if $key;
	return $self->SUPER::gettxt() unless $key or $name;
	return join("\n",@lines);
}
#package Host::Ip::View;
sub xformed {
	my ($type,$self) = @_;
	bless $self,$type;
	$self->setcolor($self->mycolor);
}

# ------------------------------------------
package Host::Ip::Here::View; our @ISA = qw(Host::Ip::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Host::Ip::Here::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{data}{data};
	my $sysd = $data->{systemData};
	my $color = ($sysd and exists $sysd->{sysName}) ? 'magenta' : 'darkgrey';
	return $color;
}
#package Host::Ip::Here::View;
sub xformed {
	my ($type,$self) = @_;
	bless $self,$type;
	$self->setcolor($self->mycolor);
}

# ------------------------------------------
package Subnet::View; our @ISA = qw(Bus::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Subnet::View;
sub mycolor { return 'brown' }
#package Subnet::View;
sub ctypes { return qw/Address/ }
#package Subnet::View;
sub gettxt {
	my $self = shift;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if ($self->{data}{key}{p}{$kind}) {
			foreach (keys %{$self->{data}{key}{p}{$kind}}) {
				push @keys, $_ if Item::keytype($_) == Item::IPV4MASKKEY();
			}
		}
	}
	my @sorted = sort @keys;
	my $key = $sorted[0];
	return $self->SUPER::gettxt() unless $key;
	return Item::showkey($key);
}

# ------------------------------------------
package Lan::View; our @ISA = qw(Bus::View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Lan::View;
sub mycolor { return 'black' }
#package Lan::View;
sub ctypes { return qw/Port/ }

# ------------------------------------------
package Vlan::View; our @ISA = qw(Bus::View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Vlan::View;
sub mycolor { return 'blue' }
#package Vlan::View;
sub ctypes { return qw/Vprt/ }

# ------------------------------------------
package Address::View; our @ISA = qw(BoxAndLink::View Point::View Leaf::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Address::View;
sub init { shift->{txt}{pnts}[2] = 11 }
#package Address::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{data}{data};
	return 'brown' if exists $data->{ipAddrEntry};
	return 'brown' if exists $data->{ipAddressEntry};
	return 'darkgrey';
}
#package Address::View;
sub types { return qw{Subnet Host} }
#package Address::View;
sub gettxt {
	my $self = shift;
	if ($self->{data}{key}{p}{Host}) {
		foreach my $key (sort keys %{$self->{data}{key}{p}{Host}}) {
			return Item::showkey($key) if Item::keytype($key) == Item::IPV4ADDRKEY();
		}
	}
	return $self->SUPER::gettxt();
}

# ------------------------------------------
package Port::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Port::View;
sub init { shift->{txt}{pnts}[2] = 13 }
#package Port::View;
sub mycolor { return 'black' }
#package Port::View;
sub types { return qw{Lan Host} }
#package Port::View;
sub gettxt {
	my $self = shift;
	my $txt;
	my $data = $self->{data}{data};
	my $lldp = $data->{lldpLocPortEntry};
	$txt = $lldp->{lldpLocPortDesc} if $lldp;
	return $txt if $txt and $txt =~ /^eth/;
	my $ifen = $data->{ifEntry};
	$txt = $ifen->{ifDescr} if $ifen;
	return $txt if $txt and $txt =~ /^eth/;
	if ($self->{data}{key}{n}) {
		foreach my $key (sort @{$self->{data}{key}{n}}) {
			return Item::showkey($key) if Item::keytype($key) == Item::MACKEY();
		}
	}
	return $self->SUPER::gettxt();
}

# ------------------------------------------
package Vprt::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Vprt::View;
sub init { shift->{txt}{pnts}[2] = 17 }
#package Vprt::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{data}{data};
	return 'blue' if exists $data->{ifEntry};
	return 'blue' if exists $data->{lldpStatsTxPortEntry};
	return 'blue' if exists $data->{lldpStatsRxPortEntry};
	return 'blue' if exists $data->{lldpLocPortEntry};
	return 'darkgrey';
}
#package Vprt::View;
sub types { return qw{Vlan Host} }
#package Vprt::View;
sub gettxt {
	my $self = shift;
	my $txt;
	my $data = $self->{data}{data};
	my $lldp = $data->{lldpLocPortEntry};
	$txt = $lldp->{lldpLocPortDesc} if $lldp;
	return $txt if $txt and $txt =~ /^eth/;
	my $ifen = $data->{ifEntry};
	$txt = $ifen->{ifDescr} if $ifen;
	return $txt if $txt and $txt =~ /^eth/;
	if ($self->{data}{key}{n}) {
		foreach my $key (sort @{$self->{data}{key}{n}}) {
			return Item::showkey($key) if Item::keytype($key) == Item::MACKEY();
		}
	}
	return $self->SUPER::gettxt();
}

# ------------------------------------------
package Route::View; our @ISA = qw(View Path::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

# ------------------------------------------
package Driv::View; our @ISA = qw(View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Driv::View;
sub isanchor { return Glib::TRUE }

# ------------------------------------------
package Card::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Card::View;
sub mycolor { return 'black' }
#package Card::View;
sub init {
	my $self = shift;
	my ($viewer,$data) = @_;
	my $color = $self->mycolor;
	my $group = $self->{grp}{item};
	my ($x,$y) = (-107.0/2.0,-95.0/2.0);
	my $otl = $self->{otl} = {pnts=>[
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
		]};
	$otl->{item} = new Goo::Canvas::Polyline(
			$group,Glib::FALSE,$otl->{pnts},
#'antialias'=>'subpixel',
#'line-join'=>'round',
#'line-cap'=>'round',
			'line-width'=>0.4,
#'line-cap'=>'round',
			'stroke-color'=>$color,
			'fill-color'=>'SeaGreen',
			'pointer-events'=>'visible',
	);
}

# ------------------------------------------
package Span::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Span::View;

# ------------------------------------------
package Chan::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Chan::View;

# ------------------------------------------
package Xcon::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Xcon::View;

# ------------------------------------------
package Window; our @ISA = qw(Gtk2::Window);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Window;
our $windidn = 0;
our @windows = ();
our $quitting = 0;
#package Window;
sub new {
	my ($type,$db,$windid) = @_;
	my $self = new Gtk2::Window('toplevel');
	bless $self,$type;
	if (defined $windid) {
		if ($windidn <= $windid) {
			$windidn = $windid+1;
		}
	} else {
		$windid = $windidn;
		$windidn++;
	}
	$self->{windid} = $windid;
	#$self->Store::_init('winds',$windid);
	my $mb = new Gtk2::MenuBar();
	my $me = new Gtk2::Menu();
	my $mi;
	$mi = new_from_stock Gtk2::ImageMenuItem('gtk-new',undef);
	$mi->signal_connect('activate'=>\&Window::menuSnmpNew,$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-open',undef);
	$mi->signal_connect('activate'=>\&Window::menuSnmpOpen,$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-close',undef);
	$mi->signal_connect('activate'=>\&Window::menuSnmpClose,$self);
	$me->append($mi);
	$mi = new_from_stock Gtk2::ImageMenuItem('gkt-quit',undef);
	$mi->signal_connect('activate'=>\&Window::menuSnmpQuit,$self);
	$me->append($mi);
	my $mc = new Gtk2::MenuItem('_SNMP');
	$mc->set_submenu($me);
	$mb->append($mc);
	my $nb = new Gtk2::Notebook();
	$nb->popup_disable;
	my $vb = new Gtk2::VBox();
	$vb->set_spacing(0);
	$vb->pack_start($mb,Glib::FALSE,Glib::FALSE,0);
	$vb->pack_start($nb,Glib::TRUE,Glib::TRUE,0);
	$self->set_type_hint('normal');
	$self->set_default_size(1000,800);
#	if (0) {
#		my $read = $self->read;
#		if ($read) {
#			$self->move($self->{xpos},$self->{ypos}) if defined $self->{xpos};
#			$self->resize($self->{width},$self->{height}) if defined $self->{width};
#			# If the window is in the database, then we need to read whatever notebook
#			# pages there are are.
#		}
#	}
	$self->set_opacity(0.5);
	$self->set_position('mouse');
	$self->signal_connect('delete-event'=>sub{
			my ($self,$ev) = @_;
			my %windows = map {$_=>$_} @windows;
			delete $windows{$self};
			@windows = (values %windows);
			if ($quitting or scalar @windows == 0) {
				#$self->save_all;
			} else {
				#$self->remove;
			}
			if (scalar @windows == 0) {
				$quitting = 1;
				Gtk2->main_quit;
			}
			return Gtk2::EVENT_PROPAGATE;
		});
	$self->add($vb);
	$self->show_all;
	$self->{nb} = $nb;
	push @windows, $self;
#	if (0) {
#		$self->save unless $read;
#	}
	return $self;
}
#package Window;
sub menuSnmpNew {
	my ($mi,$self) = @_;
}
#package Window;
sub menuSnmpOpen {
	my ($mi,$self) = @_;
}
#package Window;
sub menuSnmpClose {
	my ($mi,$self) = @_;
	my %windows = map {$_=>$_} @windows;
	delete $windows{$self};
	@windows = (values %windows);
	#$self->remove;
	$self->destroy;
}
#package Window;
sub menuSnmpQuit {
	my ($mi,$self) = @_;
	$quitting = 1;
	foreach (@windows) {
		#$_->save_all
	}
	Gtk2->main_quit;
}
#package Window;
sub add_viewer {
	my ($self,$viewer) = @_;
	my $nb = $self->{nb};
	$nb->append_page($viewer,'Viewer '.$viewer->{viewid});
	$nb->set_tab_detachable($viewer,Glib::TRUE);
	$nb->set_tab_reorderable($viewer,Glib::TRUE);
	$self->show_all;
}
#package Window;
sub new_network {
	my ($self,$network) = @_;
	my $windid = $self->{windid};
	my $nb = $self->{nb};
	my $pageid = $nb->get_n_pages;
	my $viewer = new Viewer::Network($windid,$pageid,$network);
	$self->add_viewer($viewer);
	return $viewer;
}

1;

__END__
