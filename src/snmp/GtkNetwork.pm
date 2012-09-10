# ------------------------------------------
package Viewer; our @ISA = qw(Base Gtk2::ScrolledWindow);
use strict; use warnings; use Carp;
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
		'units'=>'mm',
	);
	$self->add($canv);
	$self->{layers} = {};
	my $root = $self->{root} = new Goo::Canvas::Group();
	$root->set('pointer-events'=>'all');
	$root->{viewer} = $self;
	$canv->set_root_item($root);
	foreach my $layer (qw/Network Private Lan Port Vlan Vprt Blan Bprt Subnet Address Point/) {
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
	$view->populate();
	$self->layout();
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
#	$Viewer::added_child_count += 1;
#	unless ($Viewer::added_child_count % 10) {
#		warn "added_child called $Viewer::added_child_count times...";
#	}
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
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::added_path_count = 0;
#package Viewer;
sub added_path {
	my ($type,$ndata,$pdata) = @_;
#	$Viewer::added_path_count += 1;
#	unless ($Viewer::added_path_count % 10) {
#		warn "added_path called $Viewer::added_path_count times...";
#	}
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
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::removed_count = 0;
#package Viewer;
sub removed {
	my ($type,$data) = @_;
#	$Viewer::removed_count += 1;
#	unless ($Viewer::removed_count % 10) {
#		warn "removed called $Viewer::removed_count times...";
#	}
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			$view->put();
		}
		$viewer->{changed} = 1;
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::updated_count = 0;
#package Viewer;
sub updated {
	my ($type,$data) = @_;
#	$Viewer::updated_count += 1;
#	unless ($Viewer::updated_count % 10) {
#		warn "updated called $Viewer::updated_count times...";
#	}
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			if (my $sub = $view->can('update')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::merged_count = 0;
#package Viewer;
sub merged {
	my ($type,$data) = @_;
#	$Viewer::merged_count += 1;
#	unless ($Viewer::merged_count % 10) {
#		warn "merged called $Viewer::merged_count times...";
#	}
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->getview($data)) {
			$view->populate();
			if (my $sub = $view->can('reconfig')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::changed_count = 0;
#package Viewer;
sub changed {
	my ($type,@data) = @_;
#	$Viewer::changed_count += 1;
#	unless ($Viewer::changed_count % 10) {
#		warn "changed called $Viewer::changed_count times...";
#	}
	foreach my $viewer (values %viewers) {
		my @views = ($viewer->findviews(@data));
		foreach my $view (@views) {
			if (my $sub = $view->can('change')) {
				&$sub($view);
			}
		}
		$viewer->{changed} = 1;
		#$viewer->layout();
	}
}
#package Viewer;
$Viewer::xformed_count = 0;
#package Viewer;
sub xformed {
	my ($type,$data) = @_;
#	$Viewer::xformed_count += 1;
#	unless ($Viewer::xformed_count % 10) {
#		warn "xformed called $Viewer::xformed_count times...";
#	}
	foreach my $viewer (values %viewers) {
		if (my $view = $viewer->findview($data)) {
			my $type = ref($data).'::View';
			if (my $sub = $type->can('xformed')) {
				&$sub($type,$view);
			}
		}
		$viewer->{changed} = 1;
		#$viewer->layout();
	}
}

# ------------------------------------------
package Viewer::Network; our @ISA = qw(Viewer);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
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
# Note that local nets are displayed beneath the Point::Station.
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
	foreach (qw/Point Blan Vlan Lan Subnet Bprt Vprt Port Address Network Private/) {
		$self->{visibility}{$_} = Glib::TRUE;
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
	foreach my $view (qw{Blan/Bprt Vlan/Vprt Lan/Port Subnet/Address}) {
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

			foreach my $view (qw{Blan/Bprt Vlan/Vprt Lan/Port Subnet/Address}) {
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

			foreach my $kind (qw/Point Blan Vlan Lan Subnet Bprt Vprt Port Address Network Private/) {
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
	#printf STDERR "Laying out %s...\n",$self;
	my $view = $self->{view};
	my $canv = $self->{canv};
	my $hbox = $canv->get_screen->get_height_mm;
	my $wbox = $canv->get_screen->get_width_mm;
	my ($h,$w) = $view->resize($hbox,$wbox);
	$w = $view->{siz}{l}{total}[1]+$view->{siz}{c}{total}[1]/2.0+$view->{pad}[1];
	$h = $h/2.0;
	$view->move_view($w,$h,0,0);
	$view->layout($hbox,$wbox);
	$view->place_view($w,$h,0,0);
	$view->show_view($w,$h,0,0);
	$Viewer::Network::callcount += 1;
#	unless ($Viewer::Network::callcount % 10) {
#		warn "Layout called $Viewer::Network::callcount times...";
#	}
#	unless ($Viewer::Network::callcount % 100) {
#		$self->showstats;
#	}
	#printf STDERR "Laying out %s... ...done\n",$self;
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
	$tag = Glib::Timeout->add(100,$self->can('do_idle'),$self);
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
sub do_idle {
	my $self = shift;
	my $model = $self->{model};
	$self->start_timeout() unless $model->snmp_doone();
}
#package Viewer::Network;
sub do_timeout {
	my $self = shift;
	my $model = $self->{model};
	$self->start_quick() if $model->snmp_doone();
}

# ------------------------------------------
package Viewer::Driver; our @ISA = qw(Viewer);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

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
	$self->{siz}{t} = [0,0];
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
	my ($h,$w) = @{$self->{siz}{t}};
	$h += $self->{pad}[0]; # if $h;
	$w += $self->{pad}[1]; # if $w;
	return ($h,$w);
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
	if (delete $self->{fresh}) {
		my $type = ref $self;
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
	$grp->{item} = new Goo::Canvas::Group($layer,visibility=>'invisible');
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
			#Carp::carp "***** enter-notify-event (",join(',',@_),")";
			my ($item,$targ,$ev,$self) = @_;
			$self->setcolor('cyan');
		},$self);
	$group->signal_connect('leave-notify-event'=>sub{
			#Carp::carp "***** leave-notify-event (",join(',',@_),")";
			my ($item,$targ,$ev,$self) = @_;
			$self->setcolor($self->mycolor);
		},$self);
	$group->signal_connect('button-press-event'=>sub{
			Carp::carp "***** button-press-event (",join(',',@_),")";
			my ($item,$targ,$ev,$self) = @_;
			if ($ev->button == 3) {
				if (my $sub = $self->can('fillmenu')) {
					my $menu = new Gtk2::Menu;
					if (&$sub($self,$menu)) {
						$menu->show_all;
						$menu->popup(undef,undef,undef,undef,$ev->button,$ev->time);
						return Gtk2::EVENT_STOP;
					} else {
						Carp::carp "******** $self did not fillmenu";
					}
					$menu->destroy;
				} else {
					Carp::carp "******** $self cannot fillmenu";
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
	my $nod = $self->{nod} = {pnts=>[-6.0,-6.0,12,12]};
	$nod->{item} = new Goo::Canvas::Rect(
		$group,@{$nod->{pnts}},
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	my $txt = $self->{txt} = {pnts=>[0,0,20]};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},'center',
		alignment=>'center',
		font=>'Sans 1',
		#height=>6.0,
		#wrap=>'word-char',
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
sub gettxt { return ref(shift) }
#package Node::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my ($x,$y) = $self->pos;
	#Carp::carp "****** Placing $self ($x,$y) component is negative or zero" if $x <= 0 or $y <= 0;
	if (0) {
		my $grp = $self->{grp};
		my ($dx,$dy) = ($x-$grp->{pnts}[0],$y-$grp->{pnts}[1]);
		$grp->{item}->translate($dx,$dy);
		$grp->{pnts}[0] = $x;
		$grp->{pnts}[1] = $y;
#		$grp->{item}->set(x=>$x,y=>$y);
	} else {
		my $txt = $self->{txt};
		$txt->{pnts}[0] = $x;
		$txt->{pnts}[1] = $y;
		$txt->{item}->set(x=>$x,y=>$y,text=>$self->gettxt());
		my $nod = $self->{nod};
		$x-=$nod->{pnts}[3]/2.0;
		$y-=$nod->{pnts}[2]/2.0;
		$nod->{pnts}[0] = $x;
		$nod->{pnts}[1] = $y;
		#Carp::cluck;
		#warn "****** Setting $self ($x,$y)";
		$nod->{item}->set(x=>$x,y=>$y);
		#warn "****** Getting $self (",join(',',$nod->{item}->get('x','y')),")";
	}
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
		#warn "****** $tag (",join(',',@pos),") child $self for parent $parent";
		$self->$sub(@pos);
	}
}
#package Node::View;
sub parent_moving { shift->Node::View::parent_propagate('move',@_) }
#package Node::View;
sub parent_placing { shift->Node::View::parent_propagate('place',@_) }
#package Node::View;
sub parent_showing { shift->Node::View::parent_propagate('show',@_) }

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
		'line-width'=>0.6,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	my $txt = $self->{txt} = {pnts=>[0,0,20]};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},'west',
		alignment=>'left',
		font=>'Sans 1',
		#height=>6.0,
		#wrap=>'word-char',
		'fill-color'=>$color,
		'pointer-events'=>'visible',
	);
	$txt->{item}->raise($bus->{item});
	$self->{pad} = [1.0,1.0];
}
#package Bus::View;
sub setcolor {
	my ($self,$color) = @_;
	$self->{bus}{item}->set('stroke-color'=>$color);
	$self->{txt}{item}->set('fill-color'=>$color);
}
#package Bus::View;
sub fini {
	my $self = shift;
	my $group = $self->{grp}{item};
	foreach (qw/bus txt/) {
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
sub gettxt { return ref(shift) }
#package Bus::View;
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	if ($self->isvisible) {
		$hbox = $hbox < $self->{pad}[0] ? 0 : $hbox - $self->{pad}[0];
		$wbox = $wbox < $self->{pad}[1] ? 0 : $wbox - $self->{pad}[1];
		$hbox = 15.0 if $hbox < 15.0;
		$wbox = 0.8;
	} else {
		($hbox,$wbox) = (0,0);
	}
	$self->{siz}{t} = [$hbox,$wbox];
	return $self->siz;
}
#package Bus::View;
sub layout { }
#package Bus::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my ($x,$y) = $self->pos;
	#Carp::carp "****** Placing $self ($x,$y) component is negative or zero" if $x <= 0 or $y <= 0;
	if (0) {
		my $grp = $self->{grp};
		my ($dx,$dy) = ($x-$grp->{pnts}[0],$y-$grp->{pnts}[1]);
		$grp->{item}->translate($dx,$dy);
		$grp->{pnts}[0] = $x;
		$grp->{pnts}[1] = $y;
#		$grp->{item}->set(x=>$x,y=>$y);
	} else {
		my ($h,$w) = @{$self->{siz}{t}};
		my $bus = $self->{bus};
		$bus->{pnts}[0] = $x;
		$bus->{pnts}[1] = $y-$h/2.0;
		$bus->{pnts}[2] = $x;
		$bus->{pnts}[3] = $y+$h/2.0;
		$bus->{item}->set(points=>Goo::Canvas::Points->new($bus->{pnts}));
		my ($X,$Y) = ($x,$y+$h/2.0+$self->{pad}[0]);
		my $txt = $self->{txt};
		$txt->{pnts}[0] = $X;
		$txt->{pnts}[1] = $Y;
		$txt->{item}->set(text=>$self->gettxt());
		$txt->{item}->set_simple_transform($X,$Y,1.0,90.0);
	}
}
#package Bus::View;
sub makevisible {
	my $self = shift;
	my $vis = $self->SUPER::makevisible(@_);
	$self->{bus}{item}->set(visibility=>$vis);
	$self->{txt}{item}->set(visibility=>$vis);
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
		#warn "****** $tag (",join(',',@pos),") child $self for parent $parent";
		$self->$sub(@pos);
	}
}
#package Bus::View;
sub parent_moving { shift->Bus::View::parent_propagate('move',@_) }
#package Bus::View;
sub parent_placing { shift->Bus::View::parent_propagate('place',@_) }
#package Bus::View;
sub parent_showing { shift->Bus::View::parent_propagate('show',@_) }

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
		'line-width'=>0.4,
		'stroke-color'=>$color,
		'fill-color'=>'white',
		'pointer-events'=>'visible',
	);
	my $lnk = $self->{lnk} = {pnts=>[0,0,5,0,10,0]};
	$lnk->{item} = new Goo::Canvas::Polyline(
		$group,Glib::FALSE,$lnk->{pnts},
		'line-width'=>0.2,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
	);
	$lnk->{item}->lower($box->{item});
	my $txt = $self->{txt} = {pnts=>[5,0,20,'sw',alignment=>'left']};
	$txt->{item} = new Goo::Canvas::Text(
		$group,$self->gettxt(),@{$txt->{pnts}},
		font=>'Monospace 1',
		#height=>6.0,
		#wrap=>'word-char',
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
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	if ($self->isvisible) {
		$hbox = $hbox < $self->{pad}[0] ? 0 : $hbox - $self->{pad}[0];
		$wbox = $wbox < $self->{pad}[1] ? 0 : $wbox - $self->{pad}[1];
		my ($h,$w) = ($self->{box}{pnts}[2],$self->{box}{pnts}[3]+$self->{txt}{pnts}[2]);
		$hbox = $h; # if $hbox < $h; $hbox = 2.0*$h if $hbox > 2.0*$h;
		$wbox = $w; # if $wbox < $w; $wbox = 2.0*$w if $wbox > 2.0*$w;
	} else {
		($hbox,$wbox) = (0,0);
	}
	$self->{siz}{t} = [$hbox,$wbox];
	return $self->siz;
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
		my $lnk = $self->{lnk};
		my $txt = $self->{txt};
		if ($x < $X) {
			$lnk->{pnts}[2] = $x + 6.0 + 0.5;
			$txt->{pnts}[0] = $x + 6.0 + 1.0;
			$txt->{pnts}[3] = 'sw';
			$txt->{pnts}[5] = 'left';
		} else {
			$lnk->{pnts}[2] = $x - 6.0 - 0.5;
			$txt->{pnts}[0] = $x - 6.0 - 1.0;
			$txt->{pnts}[3] = 'se';
			$txt->{pnts}[5] = 'right';
		}
		$lnk->[4] = $x;
		$lnk->[5] = $y;
	}
}
#package BoxAndLink::View;
sub gettxt { return ref(shift) }
#package BoxAndLink::View;
sub types { return () }
#package BoxAndLink::View;
sub placing {
	my $self = shift;
	return unless $self->{set};
	my ($x,$y) = $self->pos;
	#Carp::carp "****** Placing $self ($x,$y) component is negative or zero" if $x <= 0 or $y <= 0;
	my $lnk = $self->{lnk};
	my $txt = $self->{txt};
	$lnk->{pnts}[0] = $x;
	$lnk->{pnts}[1] = $y;
	$lnk->{pnts}[3] = $y;
	$txt->{pnts}[1] = $y;
	my ($btype,$ntype) = $self->types;
	if (my $nview = $self->parent($ntype)) {
		my ($X,$Y) = $nview->pos;
		if ($x > $X) {
			$lnk->{pnts}[2] = $X + 6.0 + 0.5;
			$txt->{pnts}[0] = $X + 6.0 + 1.0;
			$txt->{pnts}[3] = 'sw';
			$txt->{pnts}[5] = 'left';
		} else {
			$lnk->{pnts}[2] = $X - 6.0 - 0.5;
			$txt->{pnts}[0] = $X - 6.0 - 1.0;
			$txt->{pnts}[3] = 'se';
			$txt->{pnts}[5] = 'right';
		}
		$lnk->{pnts}[4] = $X;
		$lnk->{pnts}[5] = $Y;
	}
	$lnk->{item}->set(points=>Goo::Canvas::Points->new($lnk->{pnts}));
	$txt->{item}->set(
		x=>$txt->{pnts}[0],
		y=>$txt->{pnts}[1],
		anchor=>$txt->{pnts}[3],
		alignment=>$txt->{pnts}[5],
		text=>$self->gettxt());
	my $box = $self->{box};
	$x-=$box->{pnts}[3]/2.0;
	$y-=$box->{pnts}[2]/2.0;
	$box->{pnts}[0] = $x;
	$box->{pnts}[1] = $y;
	$box->{item}->set(x=>$x,y=>$y);
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
sub parent_moving { shift->BoxAndLink::View::parent_propagate('move',@_) }
#package BoxAndLink::View;
sub parent_placing { shift->BoxAndLink::View::parent_propagate('place',@_) }
#package BoxAndLink::View;
sub parent_showing { shift->BoxAndLink::View::parent_propagate('show',@_) }

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
	foreach my $off ($self->offspring()) {
		next if $off->{$tag};
		if (my $sub = $off->can("parent_$tag")) { &$sub($off,$self,@_) };
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
sub moving { shift->Leaf::View::propagate('moving',@_) }
#package Leaf::View;
sub placing { shift->Leaf::View::propagate('placing',@_) }
#package Leaf::View;
sub showing { shift->Leaf::View::propagate('showing',@_) }

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
sub moving { shift->Point::View::propagate('moving',@_) }
#package Point::View;
sub placing { shift->Point::View::propagate('placing',@_) }
#package Point::View;
sub showing { shift->Point::View::propagate('showing',@_) }

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
package MibNode; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib qw/TRUE FALSE/; use Goo::Canvas;
use Gtk2::SimpleList; use SNMP;
# ------------------------------------------
# This is a grouping of three widgets providing a row in a Gtk2::Table to be used for inputting and
# displaying data.
# ------------------------------------------
#package MibNode;
sub new {
	my ($type,$dialog,$table,$label) = @_;
	my $self = {};
	bless $self,$type;
	my $data = $dialog->{data}{data}{$table};
	$self->{dialog} = $dialog;
	$self->{data} = $data;
	my $prefix = $table;
	$prefix =~ s/Table$//;
	my $short = $label;
	$short =~ s/^$prefix//;
	my $m = $SNMP::MIB{$label};
	$data->{label} = undef unless exists $data->{label};
	my $val = $self->{val} = \$data->{$label};
	my ($lf,$l,$ef,$e,$uf,$u);
	$lf = $self->{lf} = new Gtk2::Frame;
	$l = $self->{l} = new Gtk2::Label($short);
	$l->set_alignment(1.0,0.5);
	$lf->set_shadow_type('etched-out');
	$lf->add($l);
	if ((defined $m->{syntax} and $m->{syntax} eq 'TruthValue') or (defined $m->{type} and $m->{type} eq 'BOOLEAN')) {
		$$val = 0 unless defined $$val;
		$e = $self->{e} = new Gtk2::ToggleButton();
		$e->set_active($$val == 1 or $$val eq 'true' ? TRUE : FALSE);
		$ef = $self->{ef} = $e;
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
		$ef = $self->{ef} = new Gtk2::Frame;
		$e = $self->{e} = new Gtk2::SimpleList(tag=>'text',value=>'int');
		$e->set_headers_visible(FALSE);
		$e->set_grid_lines(FALSE);
		$e->get_selection->set_model('multiple');
		my @sels = ();
		my @bits = ();
		foreach (values %{$m->{enums}}) { $bits[$_] = '0'; }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits);

		my $use = MibNode::List->snmpval($m,$val);
		$bits = unpack('B*',$use).$bits;

		my $i = 0;
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			if (substr($bits,$m->{enums}{$tag},1) eq '1') {
				push @sels,$i;
			}
			$i++;
		}
		$e->select(@sels);
		$ef->set_shadow_type('in');
		$ef->add($e);
		bless $self,'MibNode::List';
	}
	elsif (defined $m->{enums} and $m->{enums} and scalar keys %{$m->{enums}} > 0) {
		$$val = 0 unless defined $$val;
		$e = $self->{e} = Gtk2::ComboBox->new_text;
		$e->set_title($label);
		my $i = 0;
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			$e->append_text($tag);
			$e->set_active($i) if $$val eq $tag or ($$val =~ /^d+$/ and $$val == $m->{enums}{$tag});
			$i++;
		}
#		$e->set_active_text($$val) if $m->{type} eq 'OCTETSTR';
		if (defined $m->{access} and $m->{access} =~ /Write|Create/) {
			$e->set('button-sensitivity'=>'on');
		} else {
			$e->set('button-sensitivity'=>'off');
		}
		$ef = $self->{ef} = $e;
		bless $self,'MibNode::Combo';
	}
	else {
		$$val = '' unless defined $$val;
		my $b = Gtk2::EntryBuffer->new($$val);
		$e = $self->{e} = Gtk2::Entry->new_with_buffer($b);
		$e->set_editable((defined $m->{access} and $m->{access} =~ /Write|Create/) ? TRUE : FALSE);
		$e->set(visibility=>FALSE) if defined $m->{type} and $m->{type} eq 'PASSWORD';
		$ef = $self->{ef} = $e;
		bless $self,'MibNode::Entry';
	}
	my $units = '';
	$units = $m->{type} if $m->{type};
	$units = $m->{syntax} if $m->{syntax};
	$units = $m->{textualConvention} if $m->{textualConvention};
	$units = $m->{units} if $m->{units};
	my $uf = new Gtk2::Frame;
	my $u = new Gtk2::Label($units);
	$u->set_alignment(0.0,0.5);
	$uf->set_shadow_type('etched-out');
	$uf->add($u);
	return $self;
}
#package MibNode;
sub add_to_table {
	my ($self,$table,$entryno) = @_;
	$table->attach($self->{lf},0,1,$entryno,$entryno+1,'fill','fill',0,0);
	$table->attach($self->{ef},1,2,$entryno,$entryno+1,'fill','fill',0,0);
	$table->attach($self->{uf},2,3,$entryno,$entryno+1,'fill','fill',0,0);
	return $self;
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
		}
	} elsif ($m->{type} eq 'INTEGER') {
		if (not defined $val) {
			return MibNode::Combo->snmpval($m,$m->{defaultValue}) if
				exists $m->{defaultValue} and
				defined $m->{defaultValue};
			$val = 0;
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
# ------------------------------------------
#package MibEntry;
sub new {
	my ($type,$view,$table) = @_;
	my $self = new Gtk2::Window('toplevel');
	bless $self,$type;
	my $data = $view->{data}{data}{$table};
	my @rows = $self->getrows($table,$data);
	$self->{view} = $view;
	$self->{table} = $table;
	$self->{data} = $data;
	$self->{rows} = \@rows;
	return undef unless scalar(@rows);
	my $fr = new Gtk2::Frame($table);
	my $v = new Gtk2::VBox(FALSE,0);
	my $sw = new Gtk2::ScrolledWindow;
	$sw->set_policy('never','automatic');
	$sw->add_with_viewport($v);
	$fr->add($sw);
	my $h = new Gtk2::HBox;
	$v->pack_start($h,TRUE,TRUE,0);
	my $t = new Gtk2::Table(scalar(@rows),3,FALSE);
	$t->set_col_spacings(0);
	$t->set_row_spacings(0);
	$h->pack_start($t,TRUE,TRUE,0);
	my $i = 0;
	foreach my $row (@rows) {
		my $ent = $self->{entries}{$row} = MibNode->new($self,$table,$row);
		$ent->add_to_table($t,$i);
		$i++;
	}
	my $bb = new Gtk2::HButtonBox;
	$bb->set_layout('end');
	$bb->set_spacing(10);
	my $b;
	$b = Gtk2::Button->new_from_stock('gtk-refresh');
	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->refresh_view; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-revert-to-saved');
	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->revert; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-apply');
	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->store; },$self);
	$bb->add($b);
	$b = Gtk2::Button->new_from_stock('gtk-close');
	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->hide; },$self);
	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-connect');
#	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->connectButton; },$self);
#	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-cancel');
#	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->cancelButton; },$self);
#	$bb->add($b);
#	$b = Gtk2::Button->new_from_stock('gtk-ok');
#	$b->signal_connect('clicked'=>sub{ Carp::carp join(',',@_); my ($b,$self) = @_; return $self->okButton; },$self);
#	$bb->add($b);
	my $vb = new Gtk2::VBox;
	$vb->set_spacing(0);
	$fr->set_border_width(5);
	$vb->pack_start($fr,TRUE,TRUE,0);
	$bb->set_bort_width(5);
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
sub getrows {
	my ($self,$table,$data) = @_;
	my $label = $table;
	$label =~ s/Scalars$//;
	$label =~ s/Table$/Entry/;
	my $mib = $SNMP::MIB{$label};
	my @ents = @{$mib->{indexes}};
	my %inds = map {$_=>1} @ents;
	foreach my $row (@{$mib->{children}}) {
		my $name = $row->{label};
		push @ents, $name unless $inds{$name};
	}
	my @rows = ();
	foreach my $row (@ents) {
		push @rows, $row if exists $data->{$row};
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
		foreach my $table (@{$Model::SNMP::tables}) {
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
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $lbox = $self->{lbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$lbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$lbox->{pnts},
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $cbox = $self->{cbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$cbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$cbox->{pnts},
		'line-width'=>0.1,
		'stroke-color'=>$color,
		'pointer-events'=>'visible',
		'visibility'=>'invisible',
	);
	my $rbox = $self->{rbox} = {pnts=>[0,0,0,0,0,0,0,0]};
	$rbox->{item} = new Goo::Canvas::Polyline(
		$group,Glib::TRUE,$rbox->{pnts},
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
	my ($x,$y) = $self->pos;
	my $siz = $self->{siz};
	my ($th,$tw) = map {$_/2.0} @{$siz->{t}};
	my ($tx,$ty) = ($x,$y);
	my $tbox = $self->{tbox};
	$tbox->{pnts} = [$tx-$tw,$ty-$th,$tx+$tw,$ty-$th,$tx+$tw,$ty+$th,$tx-$tw,$ty+$th];
	$tbox->{item}->set(points=>Goo::Canvas::Points->new($tbox->{pnts}));
	my ($ch,$cw) = (0,0);
	if ($siz->{c}{total}) {
		($ch,$cw) = map {$_/2.0} @{$siz->{c}{total}};
		my ($cx,$cy) = ($x,$y);
		my $cbox = $self->{cbox};
		$cbox->{pnts} = [$cx-$cw,$cy-$ch,$cx+$cw,$cy-$ch,$cx+$cw,$cy+$ch,$cx-$cw,$cy+$ch];
		$cbox->{item}->set(points=>Goo::Canvas::Points->new($cbox->{pnts}));
	}
	if ($siz->{l}{total}) {
		my ($lh,$lw) = map {$_/2.0} @{$siz->{l}{total}};
		my ($lx,$ly) = ($x-$cw-$lw,$y);
		my $lbox = $self->{lbox};
		$lbox->{pnts} = [$lx-$lw,$ly-$lh,$lx+$lw,$ly-$lh,$lx+$lw,$ly+$lh,$lx-$lw,$ly+$lh];
		$lbox->{item}->set(points=>Goo::Canvas::Points->new($lbox->{pnts}));
	}
	if ($siz->{r}{total}) {
		my ($rh,$rw) = map {$_/2.0} @{$siz->{r}{total}};
		my ($rx,$ry) = ($x+$cw+$rw,$y);
		my $rbox = $self->{rbox};
		$rbox->{pnts} = [$rx-$rw,$ry-$rh,$rx+$rw,$ry-$rh,$rx+$rw,$ry+$rh,$rx-$rw,$ry+$rh];
		$rbox->{item}->set(points=>Goo::Canvas::Points->new($rbox->{pnts}));
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
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	$hbox = $hbox < $self->{pad}[0] ? 0 : $hbox - $self->{pad}[0];
	$wbox = $wbox < $self->{pad}[1] ? 0 : $wbox - $self->{pad}[1];
	my $geom = $self->getgeom;
	my %size = ();
	my $tsiz = $size{t} = [0,0];
	$size{l}{total} = [0,0];
	$size{c}{total} = [0,0];
	$size{r}{total} = [0,0];
	my $smax = 0;
	foreach my $side (keys %$geom) {
		my $ssiz = $size{$side}{total} = [0,0];
		if ($side eq 'c') {
			foreach my $kind (keys %{$geom->{$side}}) {
				my $ksiz = $size{$side}{$kind} = [0,0];
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $view->resize(0,$wbox);
					$ksiz->[1] += $w;
					$ksiz->[0] = $h if $h > $ksiz->[0];
				}
				$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
				$ssiz->[0] = $ksiz->[0] if $ksiz->[0] > $ssiz->[0];
			}
			$tsiz->[1] += $ssiz->[1];
		} else {
			foreach my $kind (keys %{$geom->{$side}}) {
				my $ksiz = $size{$side}{$kind} = [0,0];
				foreach my $view (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $view->resize($hbox,$wbox);
					$ksiz->[0] += $h;
					$ksiz->[1] = $w if $w > $ksiz->[1];
				}
				$ssiz->[0] += $ksiz->[0];
				$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
			}
			$smax = $ssiz->[1] if $ssiz->[1] > $smax;
		}
		$tsiz->[0] = $ssiz->[0] if $ssiz->[0] > $tsiz->[0];
	}
	$size{l}{total}[1] = $smax;
	$size{r}{total}[1] = $smax;
	$tsiz->[1] += $smax + $smax;
	if ($tsiz->[0] > $hbox or $tsiz->[1] > $wbox) {
		my ($hall,$wall) = @$tsiz;
		my $hfac = $hbox/$hall;
		$tsiz = $size{t} = [0,0];
		$size{l}{total} = [0,0];
		$size{c}{total} = [0,0];
		$size{r}{total} = [0,0];
		$smax = 0;
		foreach my $side (keys %$geom) {
			my $wfac = $size{$side}{total}[1]/$wall;
			my $ssiz = $size{$side}{total} = [0,0];
			if ($side eq 'c') {
				foreach my $kind (keys %{$geom->{$side}}) {
					if (my $count = scalar(@{$geom->{$side}{$kind}})) {
						my $ksiz = $size{$side}{$kind} = [0,0];
						my $wsiz = $wbox*$wfac/$count;
						foreach my $view (@{$geom->{$side}{$kind}}) {
							my ($h,$w) = $view->resize(0,$wsiz);
							$ksiz->[1] += $w;
							$ksiz->[0] = $h if $h > $ksiz->[0];
						}
						$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
						$ssiz->[0] = $ksiz->[0] if $ksiz->[0] > $ssiz->[0];
					}
				}
				$tsiz->[1] += $ssiz->[1];
			} else {
				foreach my $kind (keys %{$geom->{$side}}) {
					my $ksiz = $size{$side}{$kind} = [0,0];
					foreach my $view (@{$geom->{$side}{$kind}}) {
						my ($h,$w) = $view->siz;
						($h,$w) = $view->resize($h*$hfac,$wbox*$wfac);
						$ksiz->[0] += $h;
						$ksiz->[1] = $w if $w > $ksiz->[1];
					}
					$ssiz->[0] += $ksiz->[0];
					$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
				}
				$smax = $ssiz->[1] if $ssiz->[1] > $smax;
			}
			$tsiz->[0] = $ssiz->[0] if $ssiz->[0] > $tsiz->[0];
		}
		$size{l}{total}[1] = $smax;
		$size{r}{total}[1] = $smax;
		$tsiz->[1] += $smax + $smax;
	}
	foreach my $kind (keys %{$geom->{c}}) {
		foreach my $view (@{$geom->{c}{$kind}}) {
			my ($h,$w) = $view->siz;
			$view->resize($tsiz->[0],$w);
		}
	}
	$self->{siz} = \%size;
	$self->{geo} = $geom;
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
				my ($H,$W) = @{$size->{$side}{$type}};
				my $x0 = -$W/2.0;
				my $x = $x0;
				foreach my $view (@{$geom->{$side}{$type}}) {
					my ($h,$w) = $view->siz;
					my $xofs = $x + $w/2.0;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					#warn "****** Placing $view at (",join(',',@pos),")";
					$view->move_view(@pos);
					$view->layout();
					$x += $w;
				}
			}
		} else {
			my ($H,$W) = @{$size->{$side}{total}};
			my $y = -$H/2.0;
			my $x = $size->{c}{total}[1]/2.0;
			foreach my $type (keys %{$geom->{$side}}) {
				foreach my $view (@{$geom->{$side}{$type}}) {
					my ($h,$w) = $view->siz;
					my $xofs = $x + $w/2.0;
					$xofs = -$xofs if $side eq 'l';
					my $yofs = $y + $h/2.0;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					#warn "****** Placing $view at (",join(',',@pos),")";
					$view->move_view(@pos);
					$view->layout();
					$y += $h;
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
sub isanchor { return Glib::TRUE }
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
	foreach my $host ($self->offspring_sorted('Point',4)) {
		next if ($host->parent('Private'));
		if ($host->isa('Point::Host::Here')) {
			push @{$geom{l}{Point}},$host;
			$host->{col} = -1;
		} else {
			push @{$geom{r}{Point}},$host;
			$host->{col} = +1;
		}
	}
	foreach my $form (qw/Lan Vlan Blan Subnet/) {
		foreach my $bus ($self->offspring_sorted($form,5,8)) {
			push @{$geom{c}{$form}},$bus;
			$bus->{col} = 0;
		}
	}
	return \%geom;
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
sub getgeom {
	my $self = shift;
	my %geom = ();
	my $col = $self->{col};
	my $l = $col > 0 ? 'l' : 'r';
	my $r = $col > 0 ? 'r' : 'l';
	foreach my $point ($self->offspring_sorted('Point',4,6)) {
		if ($point->parent('Network')) {
			push @{$geom{$l}{Point}},$point;
			$point->{col} = ($col > 0) ? $col-1 : $col+1;
		}
		elsif ($point->parent('Private')) {
			push @{$geom{$r}{Point}},$point;
			$point->{col} = ($col > 0) ? $col+1 : $col-1;
		}
	}
	foreach my $form (qw/Lan Vlan Blan Subnet/) {
		foreach my $bus ($self->offspring_sorted($form,5,8)) {
			unless ($bus->parent('Network')) {
				push @{$geom{c}{$form}},$bus;
				$bus->{col} = $col;
			}
		}
	}
	return \%geom;
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
		#warn "****** $tag (",join(',',@pos),") child $self for parent $parent";
		$self->$sub(@pos);
	}
}
#package Private::View;
sub parent_moving { shift->Private::View::parent_propagate('move',@_) }
#package Private::View;
sub parent_placing { shift->Private::View::parent_propagate('place',@_) }
#package Private::View;
sub parent_showing { shift->Private::View::parent_propagate('show',@_) }

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
package Point::Station::View; our @ISA = qw(Node::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
use Data::Dumper;
# ------------------------------------------
#package Point::Station::View;
sub mycolor { return 'red' }
#package Point::Station::View;
sub getgeom {
	my $self = shift;
	my ($x,$y) = $self->pos;
	my %geom = ();
	my $col = $self->{col};
	foreach my $vprt ($self->offspring_sorted('Vprt',6,8)) {
		my $vlan = $vprt->parent('Vlan');
		if ($vlan and exists $vlan->{col} and ($vlan->parent('Network') or $vlan->parent('Private'))) {
			if ($vlan->{col} < $col)
			{ push @{$geom{l}{Vprt}},$vprt } else
			{ push @{$geom{r}{Vprt}},$vprt }
		}
	}
	foreach my $bprt ($self->offspring_sorted('Bprt',6,8)) {
		my $blan = $bprt->parent('Blan');
		if ($blan and exists $blan->{col} and ($blan->parent('Network') or $blan->parent('Private'))) {
			if ($blan->{col} < $col)
			{ push @{$geom{l}{Bprt}},$bprt } else
			{ push @{$geom{r}{Bprt}},$bprt }
		}
	}
	foreach my $port ($self->offspring_sorted('Port',6)) {
		my $plan = $port->parent('Lan');
		if ($plan and exists $plan->{col} and ($plan->parent('Network') or $plan->parent('Private'))) {
			if ($plan->{col} < $col)
			{ push @{$geom{l}{Port}},$port } else
			{ push @{$geom{r}{Port}},$port }
		}
	}
	foreach my $addr ($self->offspring_sorted('Address',4)) {
		my $subn = $addr->parent('Subnet');
		if ($subn and exists $subn->{col} and ($subn->parent('Network') or $subn->parent('Private'))) {
			if ($subn->{col} < $col)
			{ push @{$geom{l}{Address}},$addr } else
			{ push @{$geom{r}{Address}},$addr }
		}
	}
	return \%geom;
}
#package Point::Station::View;
sub resize {
	my $self = shift;
	my ($hbox,$wbox) = @_;
	$hbox = $hbox < $self->{pad}[0] ? 0 : $hbox - $self->{pad}[0];
	$wbox = $wbox < $self->{pad}[1] ? 0 : $wbox - $self->{pad}[1];
	my $geom = $self->getgeom;
	my %size = ();
	my $pnts = $self->{nod}{pnts};
	$size{c}{Point} = [$pnts->[2],$pnts->[3]];
	$size{c}{total} = [@{$size{c}{Point}}];
	my $tsiz = $size{t} = [@{$size{c}{Point}}];
	$size{l}{total} = [0,0];
	$size{r}{total} = [0,0];
	my $smax = 0;
	foreach my $side (keys %$geom) {
		my $ssiz = $size{$side}{total} = [0,0];
		foreach my $kind (keys %{$geom->{$side}}) {
			my $ksiz = $size{$side}{$kind} = [0,0];
			foreach my $view (@{$geom->{$side}{$kind}}) {
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
	$size{l}{total}[1] = $smax;
	$size{r}{total}[1] = $smax;
	$tsiz->[1] += $smax + $smax;
	if ($tsiz->[0] > $hbox or $tsiz->[1] > $wbox) {
		my ($hall,$wall) = @$tsiz;
		$size{c}{Point} = [$pnts->[2],$pnts->[3]];
		$size{c}{total} = [@{$size{c}{Point}}];
		$tsiz = $size{t} = [@{$size{c}{Point}}];
		$size{l}{total} = [0,0];
		$size{r}{total} = [0,0];
		$smax = 0;
		foreach my $side (keys %$geom) {
			my $wfac = $size{$side}{total}[1]/$wall;
			my $ssiz = $size{$side}{total} = [0,0];
			foreach my $kind (keys %{$geom->{$side}}) {
				my @views = @{$geom->{$side}{$kind}};
				if (my $count = scalar(@views)) {
					my $hfac = $size{$side}{$kind}[0]/$hall;
					my $ksiz = $size{$side}{$kind} = [0,0];
					my $hsiz = $hbox*$hfac/$count;
					foreach my $view (@views) {
						my ($h,$w) = $view->resize($hbox*$hfac,$wbox*$wfac);
						$ksiz->[0] += $h;
						$ksiz->[1] = $w if $w > $ksiz->[1];
					}
					$ssiz->[0] = $ksiz->[0] if $ksiz->[0] > $ssiz->[0];
					$ssiz->[1] = $ksiz->[1] if $ksiz->[1] > $ssiz->[1];
				}
			}
			$tsiz->[0] = $ssiz->[0] if $ssiz->[0] > $tsiz->[0];
			$smax = $ssiz->[1] if $ssiz->[1] > $smax;
		}
		$size{l}{total}[1] = $smax;
		$size{r}{total}[1] = $smax;
		$tsiz->[1] += $smax + $smax;
	}
	$self->{siz} = \%size;
	$self->{geo} = $geom;
	return $self->siz;
}
#package Point::Station::View;
sub layout {
	my $self = shift;
	my $geom = delete $self->{geo};
	my $size = $self->{siz};
	#warn "****** Laying out $self:\n", Data::Dumper->new([$geom])->Maxdepth(3)->Dump, Data::Dumper->new([$size])->Dump;
	my ($X,$Y) = $self->pos;
	foreach my $side (keys %$geom) {
		my $x0 = $size->{c}{total}[1]/2.0 + $size->{$side}{total}[1]/2.0;
		$x0 = -$x0 if $side eq 'l';
		foreach my $kind (keys %{$geom->{$side}}) {
			my @views = @{$geom->{$side}{$kind}};
			if (@views) {
				my ($H,$W) = @{$size->{$side}{$kind}};
				#warn "****** Size ($H,$W) for side $side kind $kind of $self has zero component" unless $H and $W;
				my $y0 = -$H/2.0;
				my $y = $y0;
				foreach my $view (@views) {
					my ($btype,$ntype) = $view->types;
					my ($x,$yold,$xofs) = $view->pos;
					($x,$xofs) = ($x0,0) unless $view->parent($btype);
					my ($h,$w) = $view->siz;
					my $yofs = $y + $h/2.0;
					my @pos = ($x,$Y+$yofs,$xofs,$yofs);
					#warn "****** Placing $view at (",join(',',@pos),")";
					$view->move_view(@pos);
					$view->layout();
					$y += $h;
				}
			}
		}
	}
}

# ------------------------------------------
package Point::Host::View; our @ISA = qw(Point::Station::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Point::Host::View;
sub mycolor { return 'black' }
#package Point::Host::View;
sub gettxt {
	my $self = shift;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if ($self->{data}{key}{p}{$kind}) {
			push @keys,sort keys %{$self->{data}{key}{p}{$kind}};
		}
	}
	my $key = $keys[0];
	return $self->SUPER::gettxt() unless $key;
	return Item::showkey($key);
}
#package Point::Host::View;
sub xformed {
	my ($type,$self) = @_;
	bless $self,$type;
	$self->setcolor($self->mycolor);
}

# ------------------------------------------
package Point::Host::Here::View; our @ISA = qw(Point::Host::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Point::Host::Here::View;
sub mycolor { return 'magenta' }
#package Point::Host::Here::View;
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
sub gettxt {
	my $self = shift;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if ($self->{data}{key}{p}{$kind}) {
			foreach (keys %{$self->{data}{key}{p}{$kind}}) {
				push @keys, $_ if length($_) == 5;
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

# ------------------------------------------
package Vlan::View; our @ISA = qw(Bus::View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Vlan::View;
sub mycolor { return 'blue' }

# ------------------------------------------
package Blan::View; our @ISA = qw(Bus::View Point::View Leaf::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Blan::View;
sub mycolor { return 'green' }

# ------------------------------------------
package Address::View; our @ISA = qw(BoxAndLink::View Point::View Leaf::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Address::View;
sub init { shift->{txt}{pnts}[2] = 11 }
#package Address::View;
sub mycolor { return 'brown' }
#package Address::View;
sub types { return qw{Subnet Point} }
#package Address::View;
sub gettxt {
	my $self = shift;
	if ($self->{data}{key}{p}{Subnet}) {
		foreach my $key (sort keys %{$self->{data}{key}{p}{Subnet}}) {
			return Item::showkey($key) if length($key) == 4;
		}
	}
	return '(unknown)';
}

# ------------------------------------------
package Port::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Port::View;
sub init { shift->{txt}{pnts}[2] = 13 }
#package Port::View;
sub mycolor { return 'black' }
#package Port::View;
sub types { return qw{Lan Point} }
#package Port::View;
sub gettxt {
	my $self = shift;
	if ($self->{data}{key}{n}) {
		foreach my $key (sort @{$self->{data}{key}{n}}) {
			return Item::showkey($key) if length($key) == 6;
		}
	}
	return '(unknown)';
}

# ------------------------------------------
package Vprt::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Vprt::View;
sub init { shift->{txt}{pnts}[2] = 17 }
#package Vprt::View;
sub mycolor { return 'blue' }
#package Vprt::View;
sub types { return qw{Vlan Point} }
#package Vprt::View;
sub gettxt {
	my $self = shift;
	if ($self->{data}{key}{n}) {
		foreach my $key (sort @{$self->{data}{key}{n}}) {
			return Item::showkey($key) if length($key) >= 6;
		}
	}
	return '(unknown)';
}

# ------------------------------------------
package Bprt::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
#package Bprt::View;
sub init { shift->{txt}{pnts}[2] = 17 }
#package Bprt::View;
sub mycolor { return 'green' }
#package Bprt::View;
sub types { return qw{Blan Point} }
#package Bprt::View;
sub gettxt {
	my $self = shift;
	if ($self->{data}{key}{n}) {
		foreach my $key (sort @{$self->{data}{key}{n}}) {
			return Item::showkey($key) if length($key) >= 6;
		}
	}
	return '(unknown)';
}

# ------------------------------------------
package Route::View; our @ISA = qw(View Path::View Datum::View);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------

# ------------------------------------------
package Menu; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Gtk2; use Glib; use Goo::Canvas;
# ------------------------------------------
sub new {
	my ($type,$view) = @_;
	my $self = {};
	bless $self,$type;
	return $self;
}

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
	my ($type,$windid) = @_;
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
	$self->set_default_size(800,640);
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
