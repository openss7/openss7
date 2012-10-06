require threads;
require threads::shared;
require Thread;
#use Thread qw/:DEFAULT async yield/;
require Thread::Queue;
use strict;
use warnings;
require Time::HiRes;
#use Time::HiRes qw(gettimeofday tv_interval);
require Date::Parse;

# ------------------------------------------
package Viewers; our @ISA = qw(Base);
# ------------------------------------------
#package Viewers;
our @viewers = ();
our $changed = 0;
#package Viewers;
sub rem_viewer {
	my $type = shift;
	my ($viewer) = @_;
	@viewers = map {$_ eq $viewer ? () : $_} @viewers;
}
#package Viewers;
sub add_viewer {
	my $type = shift;
	my ($viewer) = @_;
	$type->rem_viewer($viewer);
	push @viewers, $viewer;
}
#package Viewers;
sub changed {
	my $type = shift;
	$changed = 1;
}
#package Viewers;
sub refresh {
	my $type = shift;
	return unless $changed;
	$changed = 0;
	foreach my $viewer (@viewers) {
		$viewer->layout if delete $viewer->{relayout};
	}
}
#package Viewers;
sub added_child {
	my ($type,$pdata,$cdata) = @_;
	foreach my $viewer (@viewers) {
		if (my $pview = $viewer->findview($pdata)) {
			if (exists $viewer->{views}{$cdata->kind}) {
				my $cview = $viewer->getview($cdata);
				$pview->add_child($cview);
				$viewer->changed;
			}
		}
	}
}
#package Viewers;
sub added_path {
	my ($type,$ndata,$pdata) = @_;
	foreach my $viewer (@viewers) {
		if (my $nview = $viewer->findview($ndata)) {
			if (exists $viewer->{views}{$pdata->kind}) {
				my $pview = $viewer->getview($pdata);
				$nview->add_path($pview);
				$viewer->changed;
			}
		}
	}
}
#package Viewers;
sub labeled {
	my ($type,$item) = @_;
	foreach my $viewer (@viewers) {
		if (my $view = $viewer->findview($item)) {
			$view->label;
			$viewer->changed;
		}
	}
}
#package Viewers;
sub removed {
	my ($type,$item) = @_;
	foreach my $viewer (@viewers) {
		if (my $view = $viewer->findview($item)) {
			$view->remove;
			$view->put;
			$viewer->changed;
		}
	}
}
#package Viewers;
sub merged {
	my ($type,$item,@others) = @_;
	foreach my $viewer (@viewers) {
		if (exists $viewer->{views}{$item->kind}) {
			my @oviews = $viewer->findviews(@others);
			if (@oviews) {
				my $view = $viewer->getview($item);
				$view->merge(@oviews);
				$viewer->changed;
			}
		}
	}
}
#package Viewers;
sub rekeyed {
	my ($type,$pdata,$cdata) = @_;
	foreach my $viewer (@viewers) {
		if (my $pview = $viewer->findview($pdata)) {
			if (my $cview = $viewer->findview($cdata)) {
				$pview->rekey($cview);
				$viewer->changed;
			}
		}
	}
}
#package Viewers;
sub xformed {
	my ($type,$item) = @_;
	foreach my $viewer (@viewers) {
		if (my $view = $viewer->findview($item)) {
			$viewer->viewtype($item)->xform($view);
			$viewer->changed;
		}
	}
}

# ------------------------------------------
package Gui; our @ISA = qw(Base);
# ------------------------------------------
#package Gui;
sub init {
	my $self = shift;
	my ($db,$model) = @_;
	$self->{db} = $db;
	$self->{model} = $model;
	my $type = ref $self;
	my $gtk = $self->{gtk} = ($type =~ s/(Gtk|Tk|Html|Qt|Fltk)$//) ? $& : '';
	my @kinds = qw/Window Viewer Canvas/;
	$self->{kinds} = \@kinds;
	$self->{gobjs} = {map {$_=>{}} @kinds};
	$self->{numbs} = {map {$_=>0 } @kinds};
	$db->begin_work;
	foreach my $kind (@kinds) {
		my $table = $self->table($kind);
		if (my $dat = $table->getall) {
			foreach my $id (keys %$dat) {
				my $sub = $dat->{$id}{sub};
				my $type = $kind;
				$type = join('::',$type,$sub) if $sub;
				$type = join('::',$type,$gtk) if $gtk;
				$type->get($self,$id);
			}
		}
	}
	$db->commit;
}
#package Gui;
sub fini {
	my $self = shift;
	delete $self->{db};
	# TODO: more, but gui packages are never destroyed
}
#package Gui;
sub table {
	my ($self,$kind) = @_;
	my $tab = 'Table::'.$kind;
	my $db = $self->{db};
	return $tab->get($db,$kind);
}
#package Gui;
sub lookup {
	my ($self,$type,$id) = @_;
	my $kind = $type->kind;
	return $self->table($kind)->select({id=>$id});
}
#package Gui;
sub do_quick {
	my $self = shift;
	my $model = $self->{model};
	if ($model->snmp_doone()) {
		$self->start_quick();
	} else {
		$self->start_timeout();
	}
}
#package Gui;
sub do_idle {
	my $self = shift;
	my $model = $self->{model};
	$self->start_timeout() unless $model->snmp_doone();
}
#package Gui;
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
package Gui::Typed; our @ISA = qw(Base);
# ------------------------------------------
#package Gui::Typed;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	my $gui = ($type =~ s/(Gtk|Tk|Html|Qt)$//) ? $& : '';
	my ($kind,$subtype) = split(/::/,$type,2);
	return $kind;
}
#package Gui::Typed;
sub subtype {
	my $type = shift;
	$type = ref $type if ref $type;
	my $gui = ($type =~ s/(Gtk|Tk|Html|Qt)$//) ? $& : '';
	my ($kind,$subtype) = split(/::/,$type,2);
	return $subtype;
}
#package Gui::Typed;
sub gui {
	my $type = shift;
	$type = ref $type if ref $type;
	my $gui = ($type =~ s/(Gtk|Tk|Html|Qt)$//) ? $& : '';
	my ($kind,$subtype) = split(/::/,$type,2);
	return $gui;
}

# ------------------------------------------
package Gui::Derived; our @ISA = qw(Gui::Typed);
# ------------------------------------------
#package Gui::Derived;
sub impl {
	my ($self,$clas,$func) = (shift,shift,shift);
	my $gtk = $self->{gtk};
	$clas = join('::',$clas,$gtk);
	unless ($self->isa($clas)) {
		if (my $sub = $clas->can($func)) {
			return &$sub($self,@_);
		}
		Carp::confess "$clas has no function $func";
	} else {
		Carp::confess "$self is not a $clas";
	}
}
#package Gui::Derived;
sub init {
	my $self = shift;
	my ($gui) = @_;
	$self->{gtk} = $gui->{gtk};
}

# ------------------------------------------
package Gui::Object; our @ISA = qw(Base);
# ------------------------------------------
# A Gui::Object is a database backed GUI object.  This base class hides the handling
# of the database from the derived object.
# ------------------------------------------
#package Gui::Object;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	my ($kind,$subtype) = split(/::/,$type,2);
	return $kind;
}
#package Gui::Object;
sub subtype {
	my $type = shift;
	$type = ref $type if ref $type;
	my ($kind,$subtype) = split(/::/,$type,2);
	return $subtype;
}
#package Gui::Object;
#package Gui::Object;
sub find {
	my $type = shift;
	my $gui = shift;
	my $id = shift;
	return $id unless defined $id;
	return $id if ref $id and $id->isa($type);
	my $kind = $type->kind;
	return $gui->{gobjs}{$kind}{$id};
}
#package Gui::Object;
sub init {
	my $self = shift;
	my ($gui,$id) = @_;
	my $kind = $self->kind;
	if (defined $id) {
		if ($gui->{numbs}{$kind} < $id) {
			$gui->{numbs}{$kind} = $id;
		}
		if (my $row = $self->lookup(@_)) {
			$self->{row} = $row;
			$self->{readme} = 1;
		}
	} else {
		$id = ++$gui->{numbs}{$kind};
	}
	my $row = $self->{row} = $self->{row} ? $self->{row} : {};
	$self->{gui} = $gui;
	$self->{no} = $row->{id} = $id;
	$self->{sub} = $row->{sub} = $self->subtype;
	$self->{table} = $gui->table($kind);
	$gui->{gobjs}{$kind}{id} = $self;
}
#package Gui::Object;
sub post {
	my $self = shift;
	my ($gui,$id) = @_;
	if (delete $self->{readme}) {
		$self->saveme;
	} else {
		$self->saveit;
	}
}
#package Gui::Object;
sub put {
	my $self = shift;
	$self->SUPER::put(@_);
}
#package Gui::Object;
sub rel {
	my $self = shift;
	$self->{preserve} = 1;
	$self->SUPER::put(@_);
}
#package Gui::Object;
sub prep {
	my $self = shift;
	if ($self->{preserve}) {
		$self->saveme;
	} else {
		$self->unsave;
	}
}
#package Gui::Object;
sub fini {
	my $self = shift;
	my $gui = delete $self->{gui};
	delete $self->{table};
	my $kind = $self->kind;
	my $id = delete $self->{no};
	$gui->{numbs}{$kind}-- if $id == $gui->{numbs}{$kind};
	delete $gui->{gobjs}{$kind}{$id};
}
#package Gui::Object;
sub tab { return "Table\::".shift->kind }
#package Gui::Object;
sub table_command {
	my ($self,$cmd) = @_;
	return if $self->{reading};
	return $self->{table}->$cmd($self->{row});
}
#package Gui::Object;
sub saveit { return shift->table_command('insert') }
#package Gui::Object;
sub saveme { return shift->table_command('update') }
#package Gui::Object;
sub unsave { return shift->table_command('delete') }
#package Gui::Object;
sub lookup {
	my ($type,$gui,$id) = @_;
	return $gui->lookup($type,$id);
}

# ------------------------------------------
package Table::Viewer; our @ISA = qw(Table);
# ------------------------------------------
#package Table::Viewer;
our %schema = (
	table=>'Viewer',
	keys=>[ qw/id/ ],
	cols=>[ qw/id name sub type obj vis/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS Viewer (
	id INT,		-- id of Viewer
	name TEXT,	-- name of Viewer (possibly user assigned)
	sub TEXT,	-- subtype of viewer (e.g. Network, Ss7)
	type TEXT,	-- type of base object (e.g. Network)
	obj INT,	-- base object id in model
	vis TEXT,	-- visibility matrix
	PRIMARY KEY(id)
);},
);

# ------------------------------------------
package Viewer; our @ISA = qw(Gui::Object);
# ------------------------------------------
# Unlike most other view objects, a Viewer object can exist without the
# GUI being active whatsover.  It contains a model of what is to be
# displayed within the GUI (canvas mostly, but dialogs as well).  Only
# when a Canvas object is attached to the viewer will the model of the
# viewer actually be displayed (viewed).  Because canvas are normally
# associated with a viewer at creation time, a pageid or reference to a
# Canvas object can optionally be specified in the constructor.  A viewer is
# a database-backed GUI object.
# ------------------------------------------
#package Viewer;
sub kinds { return () }
#package Viewer;
sub stack { return () }
#package Viewer;
sub init {
	my $self = shift;
	my ($gui,$id,$base) = @_;
	my $row = $self->{row};
	unless ($base) {
		my $btyp = $row->{type};
		$btyp = $self->subtype unless $btyp;
		my $model = $self->{gui}{model};
		$base = $btyp->get($model,Item::makekey($row->{obj}));
	}
	$row->{name} = $base->kind unless $row->{name};
	$row->{type} = $base->kind;
	$row->{obj} = $base->{no};
	$self->{base} = $base;
	$self->{model} = $base->{model}; # no real need for multiple models
	$self->{canvas} = {};
	$self->{views} = {map {$_=>{}} $self->kinds};
	my $view = $self->{view} = $self->getbase($base);
	$self->get_vis;
}
#package Viewer;
sub post {
	my $self = shift;
	Viewers->add_viewer($self);
}
#package Viewer;
sub prep {
	my $self = shift;
	Viewers->rem_viewer($self);
}
#package Viewer;
sub fini {
	my $self = shift;
	my @kinds = values %{$self->{views}};
	foreach my $kind (@kinds) {
		my @views = values %$kind;
		foreach my $view (@views) {
			$view->put;
		}
	}
	delete $self->{views};
	delete $self->{canvas};
	delete $self->{model};
	delete $self->{obj};
	delete $self->{base};
}
#package Viewer;
sub changed {
	my $self = shift;
	$self->{relayout} = 1;
	Viewers->changed;
}
#package Viewer;
sub add_page {
	my $self = shift;
	my ($canvas) = @_;
	$self->{canvas}{$canvas->{no}} = $canvas;
}
#package Viewer;
sub del_page {
	my $self = shift;
	my ($canvas) = @_;
	delete $self->{canvas}{$canvas->{no}};
}
#package Viewer;
sub setview {
	my ($self,$item,$view) = @_;
	$self->{views}{$item->kind}{$item->{no}} = $view;
}
#package Viewer;
sub putview {
	my ($self,$item) = @_;
	delete $self->{views}{$item->kind}{$item->{no}};
}
#package Viewer;
sub viewtype {
	my $self = shift;
	my $item = shift;
	my $type = ref($item);
	my $gui = $self->{gui};
	$type .= '::'.$self->{sub} if $self->{sub};
	$type .= '::View';
	#$type .= '::'.$gui->{gtk} if $gui->{gtk};
	return $type;
}
#package Viewer;
sub getbase {
	my ($self,$item) = @_;
	return $self->viewtype($item)->get($self,$item);
}
#package Viewer;
sub getview {
	my ($self,$item) = @_;
	return $item unless defined $item;
	return $self->viewtype($item)->get($self,$item);
}
#package Viewer;
sub getviews {
	my $self = shift;
	return map {$self->getview($_)} @_;
}
#package Viewer;
sub findview {
	my ($self,$item) = @_;
	return $item unless defined $item;
	return $self->{views}{$item->kind}{$item->{no}};
}
#package Viewer;
sub findviews {
	my $self = shift;
	return map {$_?$_:()} (map {$self->findview($_)} @_);
}
#package Viewer;
sub get_vis {
	my $self = shift;
	my $row = $self->{row};
	my @vis = $self->vis_def;
	if (my $vis = $row->{vis}) {
		my @vals = split(//,$vis) if defined $vis and length($vis);
		for (my $i=0;$i<@vals;$i++) {
			$vis[$i] = $vals[$i] if $vals[$i] =~ /^[X_]$/;
		}
	}
	$self->{visibility} = {};
	my @t = $self->vis_tog;
	for (my $i=0;$i<@t;$i++) {
		$self->{visibility}{$t[$i]} = ($vis[$i] and $vis[$i] eq 'X');
	}
	$row->{vis} = join('',@vis);
}
#package Viewer;
sub set_vis {
	my $self = shift;
	$self->{row}{vis} = join('',map {$self->{visibility}{$_}?'X':'_'} $self->vis_sel);
}
#package Viewer;
sub vis_sel { return qw// };
#package Viewer;
sub selected {
	my ($self,$kind) = @_;
	foreach my $view ($self->vis_sel) {
		my $vis = ($view eq $kind);
		my @pair = split(/\//,$view);
		$self->{visibility}{$pair[0]} = $vis;
		$self->{visibility}{$pair[1]} = $vis;
	}
	$self->set_vis;
	$self->saveme;
	$self->layout;
}
#package Viewer;
sub vis_def { return qw// };
#package Viewer;
sub vis_tog { return qw// };
#package Viewer;
sub toggled {
	my ($self,$kind) = @_;
	$self->{visibility}{$kind} = not $self->{visibility}{$kind};
	$self->set_vis;
	$self->saveme;
	$self->layout;
}
#package Viewer;
sub isvisible {
	my ($self,$kind) = @_;
	return $self->{visibility}{$kind};
}
#package Viewer;
sub layout {
	my $self = shift;
	Carp::confess "$self\->layout must be implemented by derived class";
}

# ------------------------------------------
package Viewer::Network; our @ISA = qw(Viewer);
# ------------------------------------------
# Columns in the display are laid out as follows:
#   - local host
#   - private subnets of the local host
#   - hosts directly accessible via private subnets of the local host
#   - public subnets
#   - hosts not directly accessible via private subnets of the local host but
#     which are directly accessible via public subnets.
#   - private subnets of foreign hosts
#   - hosts not directly accessible via private subnets of the local host and
#     not directly accessible via public subnets
#   - unknown subnets
#   - unknown hosts
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
sub kinds { return qw/Host Vlan Lan Subnet Vprt Port Address Network Private Route/ }
#package Viewer::Network;
sub stack { return qw/Network Private Route Subnet Address Vlan Vprt Lan Port Host/ }
#package Viewer::Network;
sub vis_sel { return qw{Subnet/Address Vlan/Vprt Lan/Port} }
#package Viewer::Network;
sub vis_tog { return qw/Host Vlan Lan Subnet Vprt Port Address Network Private/ }
#package Viewer::Network;
sub vis_def { return qw/X _ _ X _ _ X X X/ }
#package Viewer::Network;
sub layout {
	my $self = shift;
	foreach my $canvas (values %{$self->{canvas}}) {
		$canvas->layout;
		$canvas->refresh;
	}
}

# ------------------------------------------
package Viewer::Driv; our @ISA = qw(Viewer);
# ------------------------------------------

# ------------------------------------------
package Table::Canvas; our @ISA = qw(Table);
# ------------------------------------------
#package Table::Canvas;
our %schema = (
	table=>'Canvas',
	keys=>[ qw/id/ ],
	cols=>[ qw/id Window seq Viewer type xoffset yoffset scale/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS Canvas (
	id INT,
	Window INT,	-- id of Window object
	seq INT,	-- sequence of page in notebook;
	Viewer INT,	-- id of base object within model
	type TEXT,	-- subtype of viewer object (e.g. Network)
	xoffset INT,	-- xoffset of scrolled window
	yoffset INT,	-- yoffset of scrolled window
	scale REAL,	-- scale of canvas
	PRIMARY KEY(id,Window),
	FOREIGN KEY(Window) REFERENCES Window(id)
);},
);

# ------------------------------------------
package Canvas; our @ISA = qw(Gui::Derived Gui::Object);
# ------------------------------------------
# A canvas is a notebook page.  Each canvas belongs to a Notebook which belongs to a
# Window.  The page should contain more than just a canvas.  It should contain
# its own menu-bar/tool-bar and may contain a scrolled pane that contains the
# canvas.  We keep track of canvas seperately by pageid because they can be torn
# out of a notebook and their attributes are saved in a backing database.
# ------------------------------------------
#package Canvas;
sub init {
	my $self = shift;
	my ($gui,$id,$window,$viewer) = @_;
	my $row = $self->{row};
	if ($self->{readme}) {
		my $wtyp = join('::','Window',$self->{gtk});
		my $vtyp = join('::','Viewer',$row->{type},$self->{gtk});
		$window = $wtyp->get($gui,$row->{Window});
		$viewer = $vtyp->get($gui,$row->{Viewer});
	}
	$self->{window} = $window;
	$self->{viewer} = $viewer;
	$row->{name} = $viewer->subtype unless $row->{name};
	$row->{type} = $viewer->subtype;
	$row->{Viewer} = $viewer->{no};
	$row->{Window} = $window->{no};
	$self->{draws} = {};
}
#package Canvas;
sub post {
	my $self = shift;
	my ($gui,$id,$window,$viewer) = @_;
	$viewer = $self->{viewer} unless $viewer;
	$self->{draw} = $self->getview($viewer->{view});
	$self->{viewer}->add_page($self);
}
#package Canvas;
sub prep {
	my $self = shift;
	$self->{viewer}->del_page($self);
}
#package Canvas;
sub fini {
	my $self = shift;
	my @draws = values %{$self->{draws}};
	foreach my $draw (@draws) {
		$draw->put;
	}
	delete $self->{draws};
	delete $self->{window};
	delete $self->{viewer};
	delete $self->{draw};
}
#package Canvas;
sub setdraw {
	my ($self,$view,$draw) = @_;
	$self->{draws}{$draw->{no}} = $draw;
	$view->{draws}{$self->{no}} = $draw;
}
#package Canvas;
sub putdraw {
	my ($self,$view,$draw) = @_;
	delete $self->{draws}{$draw->{no}};
	delete $view->{draws}{$self->{no}};
}
#package Canvas;
sub finddraw {
	my ($self,$view) = @_;
	return $view->{draws}{$self->{no}};
}
#package Canvas;
sub drawtype {
	my ($self,$type) = @_;
	$type = ref $type if ref $type;
	$type =~ s/:View/:Draw/;
	return $type;
}
#package Canvas;
sub getview {
	my ($self,$view) = @_;
	return $view unless defined $view;
	return $self->drawtype($view)->get($self,$view);
}
#package Canvas;
sub getdraw {
	my ($self,$view) = @_;
	return $view unless defined $view;
	return $self->drawtype($view)->get($self,$view);
}
#package Canvas;
sub getdraws {
	my $self = shift;
	return map {$self->getdraw($_)} @_;
}
#package Canvas;
sub isvisible { shift->{viewer}->isvisible(@_) }
#package Canvas;
sub layout {
	my $self = shift;
	my $draw = $self->{draw};
	my ($hbox,$wbox) = $self->getdims;
	$draw->resize($hbox,$wbox);
	$draw->layout($hbox,$wbox);
	$draw->place_draw;
}

# ------------------------------------------
package Table::View; our @ISA = qw(Table);
# ------------------------------------------
#package Table::View;
our %schema = (
	table=>'View',
	keys=>[ qw/id type obj/ ],
	cols=>[ qw/id type obj state hidden/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS View (
	id INT,		-- viewer identifier
	type TEXT,	-- model object type
	obj INT,	-- model object number
	state INT,	-- view state
	hidden BOOLEAN,	-- whether hidden
	PRIMARY KEY(id,type,obj),
	FOREIGN KEY(id) REFERENCES Viewer(id)
);},
);

# ------------------------------------------
package View; our @ISA = qw(Base);
# ------------------------------------------
#package View;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	my ($kind,$subtype) = split(/::/,$type,2);
	return $kind;
}
#package View;
sub subtype {
	my $type = shift;
	$type = ref $type if ref $type;
	my ($kind,$subtype) = split(/::/,$type,2);
	return $subtype;
}
#package View;
sub isvisible {
	my $self = shift;
	return $self->{viewer}->isvisible($self->kind);
}
#package View;
sub getmodels { return {} }
#package View;
sub find {
	my $type = shift;
	my ($viewer,$item) = @_;
	my $view = $viewer->findview($item);
	delete $view->{new} if $view;
	return $view;
}
#package View;
sub init {
	my $self = shift;
	$self->{new} = 1;
	my ($viewer,$item) = @_;
	if (my $row = $self->lookup(@_)) {
		$self->{row} = $row;
		$self->{readme} = 1;
	}
	my $row = $self->{row} = $self->{row} ? $self->{row} : {};
	$row->{id} = $viewer->{no};
	$row->{type} = $item->kind;
	$row->{obj} = $item->{no};
	$self->{viewer} = $viewer;
	$self->{item} = $item;
	$self->{draws} = {};
	$self->{table} = $viewer->{gui}->table('View');
	$viewer->setview($item,$self);
	$self->{cookie} = "ViewCookie($item)";
}
#package View;
sub post {
	my $self = shift;
	my ($viewer,$item) = @_;
	if (delete $self->{readme}) {
		$self->saveme;
	} else {
		$self->saveit;
	}
	$self->populate;
}
#package View;
sub put {
	my $self = shift;
	$self->SUPER::put(@_);
}
#package View;
sub rel {
	my $self = shift;
	$self->{preserve} = 1;
	$self->SUPER::put(@_);
}
#package View;
sub prep {
	my $self = shift;
	if ($self->{preserve}) {
		$self->saveme;
	} else {
		$self->unsave;
	}
}
#package View;
sub fini {
	my $self = shift;
	$self->{bad} = 1;
	my $type = ref $self;
	my $kind = $type->kind;
	my @draws = values %{$self->{draws}};
	foreach (@draws) { $_->put }
	delete $self->{draws};
	my $viewer = $self->{viewer};
	my $item = $self->{item};
	$viewer->putview($item);
	delete $self->{viewer};
	$self->{cookie} .= 'DELETED';
	delete $self->{item};
	delete $self->{pos};
	delete $self->{siz};
	delete $self->{geom};
	delete $self->{geo};
	delete $self->{table};
}
#package View;
sub table_command {
	my ($self,$cmd) = @_;
	return if $self->{reading};
	return $self->{table}->$cmd($self->{row});
}
#package View;
sub saveit { return shift->table_command('insert') }
#package View;
sub saveme { return shift->table_command('update') }
#package View;
sub unsave { return shift->table_command('delete') }
#package View;
sub lookup {
	my ($type,$viewer,$item) = @_;
	my $row = {};
	$row->{id} = $viewer->{no};
	$row->{type} = $item->kind;
	$row->{obj} = $item->{no};
	return $viewer->{gui}->table('View')->select($row);
}
#package View;
sub tag {
	my $self = shift;
	my $row = $self->{row};
	return $row->{type}.$row->{obj};
}
#package View;
#sub pos { return @{shift->{pos}} }
#package View;
#sub siz {
#	my $self = shift;
#	my ($h,$w,$mh,$mw) = @{$self->{siz}{t}};
#	$h += 2.0*$self->{pad}[0]; # if $h;
#	$w += 2.0*$self->{pad}[1]; # if $w;
#	return ($h,$w,$mh,$mw);
#}
#package View;
#sub gethandw {
#	my $self = shift;
#	unless (exists $self->{handw}) {
#		if ($self->isvisible) {
#			if (my $sub = $self->can('recalc')) {
#				$self->{handw} = [ &$sub($self,@_) ];
#			} else {
#				$self->{handw} = [ 0, 0, 0, 0 ];
#			}
#		} else {
#			$self->{handw} = [ 0, 0, 0, 0 ];
#		}
#	}
#	return @{$self->{handw}};
#}
#package View;
sub mycolor { return 'black' }
#package View;
sub gettxt { shift->{text} = '' }
#package View;
sub telldraw {
	my $self = shift;
	my $func = shift;
	foreach (values %{$self->{draws}}) { $_->$func(@_) }
}
#package View;
sub setcolor { shift->telldraw('setcolor',@_) }
#package View;
sub settext { shift->telldraw('settext',@_) }
#package View;
sub passon {
	my $self = shift;
	my $func = shift;
	my $type = ref $self;
	$self->_forw($type,undef,$func,@_);
}
#package View;
sub populate { shift->passon('filling',@_) }
#package View;
sub add_child { shift->passon('adding',@_) }
#package View;
sub add_path { shift->passon('adding',@_) }
#package View;
sub label { shift->passon('labeling',@_) }
#package View;
sub remove { shift->passon('removing',@_) }
#package View;
sub merge { shift->passon('merging',@_) }
#package View;
sub merging {
	my ($self,@others) = @_;
	return if $self->{fixed};
	foreach my $othr (@others) {
		next unless $othr->{fixed};
		# TODO: copy fixed parameters to $self
		# Note that $othr will be deleted separately
	}
}
#package View;
sub rekey { shift->passon('rekeying',@_) }
#package View;
sub xform { shift->passon('xforming',@_) }
#package View;
sub xforming {
	my ($type,$self) = @_;
	bless $self,$type;
	$type =~ s/:View/:Draw/;
	foreach my $draw (values %{$self->{draws}}) {
		$type->xform($draw);
	}
}

# ------------------------------------------
package Draw; our @ISA = qw(Gui::Derived);
# ------------------------------------------
#package Draw;
our $drawnumber = 0;
#package Draw;
sub isvisible { $_[0]->{set} and $_[0]->{view}->isvisible }
#package Draw;
sub find {
	my $type = shift;
	my ($canvas,$view) = @_;
	my $draw = $canvas->finddraw($view);
	delete $draw->{new} if $draw;
	return $draw;
}
#package Draw;
sub init {
	my $self = shift;
	my ($canvas,$view) = @_;
	$drawnumber++;
	$self->{no} = $drawnumber;
	$self->{new} = 1;
	$self->{canvas} = $canvas;
	$self->{view} = $view;
	$canvas->setdraw($view,$self);
	$self->{pos} = [0,0,0,0];
	$self->{siz} = {t=>[0,0,0,0]};
	$self->{pad} = [3.0,3.0];
}
#package Draw;
sub fini {
	my $self = shift;
	my $type = ref $self;
	my $kind = $type->kind;
	my $canvas = delete $self->{canvas};
	my $view = delete $self->{view};
	$canvas->putdraw($view,$self);
	delete $self->{pos};
	delete $self->{siz};
	delete $self->{geo};
	delete $self->{geom};
}
#package Draw;
sub pos { return @{shift->{pos}} }
#package Draw;
sub siz {
	my $self = shift;
	my ($h,$w,$mh,$mw) = @{$self->{siz}{t}};
	$h += 2.0*$self->{pad}[0]; # if $h;
	$w += 2.0*$self->{pad}[1]; # if $w;
	return ($h,$w,$mh,$mw);
}
#package Draw;
sub copygeom {
	my $self = shift;
	my $geom = shift;
	my $canvas = $self->{canvas};
	my $copy = {};
	foreach my $s (keys %$geom) {
		my $side = $geom->{$s};
		my $scpy = $copy->{$s} = {};
		foreach my $k (keys %$side) {
			my $kind = $side->{$k};
			my @draws = $canvas->getdraws(@$kind);
			$scpy->{$k} = [ @draws ];
			foreach my $draw (@draws) {
				delete $draw->{col};
				$draw->{col} = $draw->{view}{col}
					if exists $draw->{view}{col};
			}
		}
	}
	return $copy;
}
#package Draw;
sub geomcopy {
	my $self = shift;
	my $func = shift;
	if (my $sub = $self->{view}->can($func)) {
		return $self->copygeom(&$sub($self->{view}));
	}
	return {};
}
#package Draw;
our $recursion = 0;
#package Draw;
sub geom { shift->geomcopy('geom') }
#package Draw;
sub getgeom { shift->geomcopy('getgeom') }
#package Draw;
#sub gethandw {
#	my $self = shift;
#	unless (exists $self->{handw}) {
#		if ($self->{view}->isvisible) {
#			if (my $sub = $self->can('recalc')) {
#				$self->{handw} = [ &$sub($self,@_) ];
#			} else {
#				$self->{handw} = [ 0, 0, 0, 0 ];
#			}
#		} else {
#			$self->{handw} = [ 0, 0, 0, 0 ];
#		}
#	}
#	return @{$self->{handw}};
#}
#package Draw;
sub resize {
	my $self = shift;
	if ($self->{view}->isvisible) {
		$self->{siz}{t} = [ $self->recalc(@_) ];
	} else {
		$self->{siz}{t} = [ 0, 0, 0, 0 ];
	}
	return $self->siz;
}
#package Draw;
sub passalong {
	my $self = shift;
	my $verb = shift;
	my $type = ref $self;
	$self->{$verb} = 1;
	$self->_forw($type,undef,$verb,@_);
	delete $self->{$verb};
}
#package Draw;
sub place_draw {
	my $self = shift;
	Carp::confess "Placing child of Local" if $self->kind ne 'Host' and $self->{view}{item}->can('parent') and $self->{view}{item}->parent('Local');
	$self->passalong('placing',@_);
	delete $self->{changed};
}
#package Draw;
sub move_draw {
	my $self = shift;
	Carp::confess "Moving child of Local" if $self->kind ne 'Host' and $self->{view}{item}->can('parent') and $self->{view}{item}->parent('Local');
	$self->{set} = 1;
	my ($x,$y,$xo,$yo) = @_;
	my ($X,$Y,$XO,$YO) = $self->pos;
	$x = $X unless defined $x;
	$y = $Y unless defined $y;
	$xo = $XO unless defined $xo;
	$yo = $YO unless defined $yo;
#return if ($x == $X and $y == $Y and $xo == $XO and $yo == $YO);
	$self->{pos} = [$x,$y,$xo,$yo];
	$self->{changed} = 1;
	$self->passalong('moving',@_);
}
#package Draw;
sub mycolor { shift->{view}->mycolor }
#package Draw;
sub gettxt { shift->{view}->gettxt }
#package Draw;
sub xform {
	my ($type,$self) = @_;
	bless $self,$type;
}

# ------------------------------------------
package Group::View; our @ISA = qw(View);
# ------------------------------------------

# ------------------------------------------
package Group::Draw; our @ISA = qw(Draw);
# ------------------------------------------
#package Group::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_) }
#package Group::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_) }
#package Group::Draw;
sub placing { shift->impl(__PACKAGE__,'placing',@_) }

# ------------------------------------------
package Node::View; our @ISA = qw(ThreeColumn::View);
# ------------------------------------------
#package Node::View;
sub gettxt {
	my $self = shift;
	$self->{text} = sprintf '(%s%d)',ref($self->{item}),$self->{item}{no};
}

# ------------------------------------------
package Node::Draw; our @ISA = qw(ThreeColumn::Draw);
# ------------------------------------------
#package Node::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_) }
#package Node::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_) }
#package Node::Draw;
sub setcolor { shift->impl(__PACKAGE__,'setcolor',@_) }
#package Node::Draw;
sub placing {
	my $self = shift;
	my ($forced) = @_;
#return unless $self->{changed} or $forced;
	my ($x,$y) = $self->pos;
	my $txt = $self->{txt};
	$txt->{pnts}[0] = $x;
	$txt->{pnts}[1] = $y;
	my $nod = $self->{nod};
	$x-=$nod->{pnts}[2]/2.0;
	$y-=$nod->{pnts}[3]/2.0;
	$nod->{pnts}[0] = $x;
	$nod->{pnts}[1] = $y;
	$self->impl(__PACKAGE__,'placing',@_);
}
#package Node::Draw;
sub myparent {
	my ($self,$parent) = @_;
	my $kind = $parent->kind;
	return ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private')));
}

# ------------------------------------------
package Bus::View; our @ISA = qw(Group::View);
# ------------------------------------------
#package Bus::View;
sub gettxt {
	my $self = shift;
	$self->{text} = sprintf '(%s%d)',ref($self->{item}),$self->{item}{no};
}
#package Bus::View;
sub adding {
	my ($self,$child) = @_;
	my $kind = $child->kind;
	foreach ($self->ctypes) {
		if ($_ eq $kind) {
			$self->{viewer}->changed;
			last;
		}
	}
}
#package Bus::View;
sub rekeying { shift->adding(@_) }

# ------------------------------------------
package Bus::Draw; our @ISA = qw(Group::Draw);
# ------------------------------------------
#package Bus::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_); }
#package Bus::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_); }
#package Bus::Draw;
sub setcolor { shift->impl(__PACKAGE__,'setcolor',@_) }
#package Bus::Draw;
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
#package Bus::Draw;
sub layout { }
#package Bus::Draw;
sub myparent {
	my ($self,$parent) = @_;
	my $kind = $parent->kind;
	return ($kind eq 'Private' or ($kind eq 'Network' and not $self->parent('Private')));
}
#package Bus::Draw;
sub placing {
	my $self = shift;
	my ($forced) = @_;
#return unless $self->{changed} or $forced;
	my ($x,$y) = $self->pos;
	my ($h,$w) = @{$self->{siz}{t}};
	my $bus = $self->{bus}{pnts};
	$bus->[0] = $x;
	$bus->[1] = $y-$h/2.0;
	$bus->[2] = $x;
	$bus->[3] = $y+$h/2.0;
	my ($X,$Y) = ($x,$y+$h/2.0+$self->{pad}[0]);
	my $txt = $self->{txt}{pnts};
	$txt->[0] = $X;
	$txt->[1] = $Y;
	($X,$Y) = ($x,$y-$h/2.0-$self->{pad}[0]);
	my $txb = $self->{txb}{pnts};
	$txb->[0] = $X;
	$txb->[1] = $Y;
	$self->impl(__PACKAGE__,'placing',@_);
}

# ------------------------------------------
package BoxAndLink::View; our @ISA = qw(Group::View);
# ------------------------------------------
#package BoxAndLink::View;
sub gettxt {
	my $self = shift;
	$self->{text} = sprintf '(%s%d)',ref($self->{item}),$self->{item}{no};
}
#package BoxAndLink::View;
sub types { return () }

# ------------------------------------------
package BoxAndLink::Draw; our @ISA = qw(Group::Draw);
# ------------------------------------------
#package BoxAndLink::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_) }
#package BoxAndLink::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_) }
#package BoxAndLink::Draw;
sub setcolor { shift->impl(__PACKAGE__,'setcolor',@_) }
#package BoxAndLink::Draw;
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
#package BoxAndLink::Draw;
sub layout {
	my $self = shift;
	return;
	my ($X,$Y) = $self->pos;
	my ($btype,$ntype) = $self->{view}->types;
	my $bdraw = $self->parent($btype) if $btype;
	if ($bdraw) {
		my ($x,$y) = $bdraw->pos;
		# horizontal aligment with bus
		my $lnk = $self->{lnk}{pnts};
		my $txt = $self->{txt}{pnts};
		$lnk->[3] = $Y;
		$txt->[1] = $Y;
		my @pos = ($x,$Y);
		$self->move_draw(@pos);
		($X,$Y) = $self->pos;
	}
	my $ndraw = $self->parent($ntype) if $ntype;
	if ($ndraw) {
		unless ($bdraw) {
			# when there is no bus, just hang the
			# box near the point but toward the 
			# outside of the diagram.
			$X = ($X < 0) ? -20 : 20;
			my @pos = ($X,$Y);
			$self->move_draw(@pos);
		}
		my ($x,$y) = $ndraw->pos;
		my $w = $ndraw->{nod}{pnts}[2]/2.0;
		my $lnk = $self->{lnk}{pnts};
		my $txt = $self->{txt}{pnts};
		if ($x < $X) {
			$lnk->[2] = $x + $w + 0.5;
			$txt->[0] = $x + $w + 1.0;
			$txt->[3] = 'sw';
			$txt->[5] = 'left';
		} else {
			$lnk->[2] = $x - $w - 0.5;
			$txt->[0] = $x - $w - 1.0;
			$txt->[3] = 'se';
			$txt->[5] = 'right';
		}
		$lnk->[4] = $x;
		$lnk->[5] = $y;
		$self->{changed} = 1;
	}
}
#package BoxAndLink::Draw;
sub placing {
	my $self = shift;
	my ($forced) = @_;
#return unless $self->{changed} or $forced;
	my ($x,$y) = $self->pos;
	my $lnk = $self->{lnk}{pnts};
	my $txt = $self->{txt}{pnts};
	my ($btype,$ntype) = $self->{view}->types;
	if (my $bview = $self->parent($btype)) {
		my ($X,$Y) = $bview->pos;
		print STDERR "Updating $self x=$x to $X ...\n" if $x != $X;
		$x = $X;
	}
	$lnk->[0] = $x;
	$lnk->[1] = $y;
	$lnk->[3] = $y;
	$txt->[1] = $y;
	if (my $nview = $self->parent($ntype)) {
		my ($X,$Y) = $nview->pos;
		my $w = $nview->{nod}{pnts}[2]/2.0;
		if ($x > $X) {
			$lnk->[2] = $X + $w + 0.5;
			$txt->[0] = $X + $w + 1.0;
			$txt->[3] = 'sw';
			$txt->[5] = 'left';
		} else {
			$lnk->[2] = $X - $w - 0.5;
			$txt->[0] = $X - $w - 1.0;
			$txt->[3] = 'se';
			$txt->[5] = 'right';
		}
		$lnk->[4] = $X;
		$lnk->[5] = $Y;
	}
	my $box = $self->{box}{pnts};
	$x-=$box->[3]/2.0;
	$y-=$box->[2]/2.0;
	$box->[0] = $x;
	$box->[1] = $y;
	$self->impl(__PACKAGE__,'placing',@_);
}
#package BoxAndLink::Draw;
sub myparent {
	my ($self,$parent) = @_;
	my $kind = $parent->kind;
	my ($btype,$ntype) = $self->{view}->types;
	if ($kind eq $btype) {
		return not $parent->{view}{item}->parent('Local');
	}
	elsif ($kind eq $ntype) {
		return not $self->{view}{item}->parent('Local');
	}
	return ($kind eq $btype or $kind eq $ntype);
}
#package BoxAndLink::Draw;
sub parent_moving {
	my ($self,$parent) = @_;
	my ($btype,$ntype) = $self->{view}->types;
	my $kind = $parent->kind;
	my ($x,$y,$xofs,$yofs) = $self->pos;
	if ($kind eq $btype) {
		if (not $parent->{view}{item}->parent('Local')) {
			my ($X,$Y) = $parent->pos;
			my @pos = ($X+$xofs,$y,$xofs,$yofs);
#print STDERR "Parent moving $self x=$x to $pos[0] ...\n";
			$self->move_draw(@pos);
		}
	}
	elsif ($kind eq $ntype) {
		if (not $self->{view}{item}->parent('Local')) {
			my ($X,$Y) = $parent->pos;
			my @pos = ($x,$Y+$yofs,$xofs,$yofs);
#print STDERR "Parent moving $self y=$y to $pos[1] ...\n";
			$self->move_draw(@pos);
		}
	} else {
		Carp::cluck "Ignoring parent $parent of $self";
	}
}

# ------------------------------------------
package Root::View; our @ISA = qw(Base);
use Data::Dumper;
# ------------------------------------------
#package Root::View;
sub child {
	my $self = shift;
	my $child = $self->{item}->child(@_);
	return $self->{viewer}->findview($child);
}
#package Root::View;
sub members {
	my $self = shift;
	my @members = $self->{item}->members(@_);
	return $self->{viewer}->findviews(@members);
}
#package Root::View;
sub members_sorted {
	my $self = shift;
	my @members = $self->{item}->members_sorted(@_);
	return $self->{viewer}->findviews(@members);
}
#package Root::View;
sub members_sortbytype {
	my $self = shift;
	my @members = $self->{item}->members_sortbytype(@_);
	return $self->{viewer}->findviews(@members);
}
#package Root::View;
sub children {
	my $self = shift;
	my @children = $self->{item}->children(@_);
	return $self->{viewer}->findviews(@children);
}
#package Root::View;
sub children_sorted {
	my $self = shift;
	my @children = $self->{item}->children_sorted(@_);
	return $self->{viewer}->findviews(@children);
}
#package Root::View;
sub children_sortbytype {
	my $self = shift;
	my @children = $self->{item}->children_sortbytype(@_);
	return $self->{viewer}->findviews(@children);
}
#package Root::View;
sub offspring {
	my $self = shift;
	my @offspring = $self->{item}->offspring(@_);
	return $self->{viewer}->findviews(@offspring);
}
#package Root::View;
sub offspring_sorted {
	my $self = shift;
	my @offspring = $self->{item}->offspring_sorted(@_);
	return $self->{viewer}->findviews(@offspring);
}
#package Root::View;
sub offspring_sortbytype {
	my $self = shift;
	my @offspring = $self->{item}->offspring_sortbytype(@_);
	return $self->{viewer}->findviews(@offspring);
}
#package Root::View;
sub getoffspring {
	my $self = shift;
	my @offspring = $self->{item}->offspring(@_);
	return $self->{viewer}->getviews(@offspring);
}
#package Root::View;
sub ctypes { return () }
#package Root::View;
sub filling {
	my $self = shift;
	my @offspring = $self->getoffspring([$self->ctypes]);
}

# ------------------------------------------
package Root::Draw; our @ISA = qw(Base);
# ------------------------------------------
#package Root::Draw;
sub child {
	my $self = shift;
	return $self->{canvas}->getdraw($self->{view}->child(@_));
}
#package Root::Draw;
sub members {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->members(@_));
}
#package Root::Draw;
sub members_sorted {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->members_sorted(@_));
}
#package Root::Draw;
sub members_sortbytype {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->members_sortbytype(@_));
}
#package Root::Draw;
sub children {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->children(@_));
}
#package Root::Draw;
sub children_sorted {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->children_sorted(@_));
}
#package Root::Draw;
sub children_sortbytype {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->children_sortbytype(@_));
}
#package Root::Draw;
sub offspring {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->offspring(@_));
}
#package Root::Draw;
sub offspring_sorted {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->offspring_sorted(@_));
}
#package Root::Draw;
sub offspring_sortbytype {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->offspring_sortbytype(@_));
}
#package Root::Draw;
sub getoffspring {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->offspring(@_));
}
#package Root::Draw;
sub moving {
	my $self = shift;
	foreach my $off ($self->offspring_sorted([$self->{view}->ctypes])) {
		next if $off->{moving};
		$off->parent_moving($self,@_);
	}
}
#package Root::Draw;
sub placing {
	my $self = shift;
	foreach my $off ($self->offspring_sorted([$self->{view}->ctypes])) {
		next if $off->{placing};
		$off->parent_placing($self,@_);
	}
}

# ------------------------------------------
package Leaf::View; our @ISA = qw(Base);
# ------------------------------------------
#package Leaf::View;
sub parent {
	my $self = shift;
	my $parent = $self->{item}->parent(@_);
	return $self->{viewer}->findview($parent);
}
#package Leaf::View;
sub parents {
	my $self = shift;
	my @parents = $self->{item}->parents(@_);
	return $self->{viewer}->findviews(@parents);
}
#package Leaf::View;
sub getparents {
	my $self = shift;
	my @parents = $self->{item}->parents(@_);
	return $self->{viewer}->getviews(@parents);
}
#package Leaf::View;
sub types { return () }
#package Leaf::View;
sub filling {
	my $self = shift;
	my @parents = $self->getparents([$self->types]);
}


# ------------------------------------------
package Leaf::Draw; our @ISA = qw(Base);
# ------------------------------------------
#package Leaf::Draw;
sub parent {
	my $self = shift;
	return $self->{canvas}->getdraw($self->{view}->parent(@_));
}
#package Leaf::Draw;
sub parents {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->parents(@_));
}
#package Leaf::Draw;
sub myparent { return 1 }
#package Leaf::Draw;
sub parent_moving {
	my ($self,$parent) = @_;
	if ($self->myparent($parent)) {
		my ($x,$y,$xofs,$yofs) = $self->pos;
		my ($X,$Y) = $parent->pos;
		my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
		$self->move_draw(@pos);
	} else {
#Carp::cluck "Ignoring parent $parent of $self";
	}
}
#package Leaf::Draw;
sub parent_placing {
	my ($self,$parent) = (shift,shift);
	if ($self->myparent($parent)) {
		$self->place_draw(@_);
	} else {
#Carp::cluck "Ignoring parent $parent of $self";
	}
}

# ------------------------------------------
package Tree::View; our @ISA = qw(Root::View Leaf::View);
# ------------------------------------------

# ------------------------------------------
package Tree::Draw; our @ISA = qw(Root::Draw Leaf::Draw);
# ------------------------------------------

# ------------------------------------------
package Point::View; our @ISA = qw(Base);
# ------------------------------------------
#package Point::View;
sub pathsx {
	my $self = shift;
	my @paths = $self->{item}->pathsx(@_);
	return $self->{viewer}->findviews(@paths);
}
#package Point::View;
sub pathsa {
	my $self = shift;
	my @paths = $self->{item}->pathsa(@_);
	return $self->{viewer}->findviews(@paths);
}
#package Point::View;
sub pathsb {
	my $self = shift;
	my @paths = $self->{item}->pathsb(@_);
	return $self->{viewer}->findviews(@paths);
}
#package Point::View;
sub paths {
	my $self = shift;
	my @paths = $self->{item}->paths(@_);
	return $self->{viewer}->findviews(@paths);
}
#package Point::View;
sub getpaths {
	my $self = shift;
	my @paths = $self->{item}->paths(@_);
	return $self->{viewer}->getviews(@paths);
}
#package Point::View;
sub ptypes { return () }
#package Point::View;
sub filling {
	my $self = shift;
	my @paths = $self->getpaths([$self->ptypes]);
}

# ------------------------------------------
package Point::Draw; our @ISA = qw(Base);
# ------------------------------------------
#package Point::Draw;
sub pathsx {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->pathsx(@_));
}
#package Point::Draw;
sub pathsa {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->pathsa(@_));
}
#package Point::Draw;
sub pathsb {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->pathsb(@_));
}
#package Point::Draw;
sub paths {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->paths(@_));
}
#package Point::Draw;
sub getpaths {
	my $self = shift;
	return $self->{canvas}->getdraws($self->{view}->paths(@_));
}
#package Point::Draw;
sub propagate {
	my ($self,$tag) = (shift,shift);
	foreach my $path ($self->pathsa) {
		next if $path->{$tag};
		if (my $sub = $path->can("obja_$tag")) { &$sub($path,$self,@_) }
	}
	foreach my $path ($self->pathsb) {
		next if $path->{$tag};
		if (my $sub = $path->can("objb_$tag")) { &$sub($path,$self,@_) }
	}
}
#package Point::Draw;
sub moving_old { shift->Point::Draw::propagate('moving',@_) }
#package Point::Draw;
sub placing_old { shift->Point::Draw::propagate('placing',@_) }

# ------------------------------------------
package Path::View; our @ISA = qw(Base);
# ------------------------------------------
#package Path::View;
sub obj {
	my $self = shift;
	my $side = shift;
	return $self->{viewer}->findview($self->{item}->obj($side));
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
package Path::Draw; our @ISA = qw(Base);
# ------------------------------------------
#package Path::Draw;
sub obj {
	my $self = shift;
	my $side = shift;
	return $self->{canvas}->getdraw($self->{view}->obj($side));
}
#package Path::Draw;
sub objo {
	my $self = shift;
	my $side = shift;
	$side = (($side eq 'a') ? 'b' : 'a');
	return $self->obj($side);
}
#package Path::Draw;
sub obja { return shift->obj('a') }
#package Path::Draw;
sub objb { return shift->obj('b') }
#package Path::Draw;
sub objs {
	my $self = shift;
	return $self->obja,$self->objb;
}

# ------------------------------------------
package MibInfo; our @ISA = qw(Base);
# ------------------------------------------
#package MibInfo;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label) = @_;
	$self->{dialog} = $dialog;
	$self->{view} = $view;
	$self->{table} = $table;
	$self->{label} = $label;
	lock $Model::SNMP::lockvar;
	$self->{m} = $SNMP::MIB{$label};
}
#package MibInfo;
sub strip {
	my ($self,$msg) = @_;
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
	my ($self,$msg,$width) = @_;
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
		my $rest = $width - $self->widthof($indent) - $self->widthof($lead);
		while (($measure = $self->widthof($_)) > $rest) {
			my ($pos,$where,$break) = (-1,-1,-1);
			while (($break = index($_,' ',$pos+1)) >= 0) {
				my $portion = $self->widthof(substr($_,0,$break));
				last if $portion >= $rest;
				$where = $break unless $break == $pos+1;
				$pos = $break;
			}
			$where = $break if $where == 0;
			$where = length($_) if $where <= 0;
			push @result,$indent.$lead.substr($_,0,$where,'');
			s/^[ ]*//;
			$lead =~ s/./ /g;
			$rest = $width - $self->widthof($indent) - $self->widthof($lead);
		}
		push @result,$indent.$lead.$_;
	}
	@result = grep(!/^\s*$/,@result) if @result > 60; # turf blank lines
	$msg = join("\n",@result);
	return $msg;
}
#package MibInfo;
sub rehang {
	my ($self,$msg,$width,$hang) = @_;
	$width = 400 unless defined $width;
	$hang = '        ' unless defined $hang;
	$msg =~ s/\n[ ]+\n/\n\n/sg;
	$msg =~ s/[ ]\n[ ]*/ /sg;
	my @result = ();
	my @lines = split(/\n/,$msg);
	foreach (@lines) {
		my ($indent,$lead) = ('','');
		$indent = $1 if s/^( *)//;
		my $rest = $width - $self->widthof($indent) - $self->widthof($lead);
		while ((my $measure = $self->widthof($_)) > $rest) {
			my ($pos,$where,$break) = (-1,-1,-1);
			while (($break = index($_,' ',$pos + 1)) >= 0) {
				my $portion = $self->widthof(substr($_,0,$break));
				last if $portion >= $rest;
				$where = $break unless $break == $pos + 1;
				$pos = $break;
			}
			$where = $break if $where == 0;
			$where = length($_) if $where <= 0;
			push @result,$indent.$lead.substr($_,0,$where,'');
			s/^[ ]*//;
			$lead = $hang;
			$rest = $width - $self->widthof($indent) - $self->widthof($lead);
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
	my $msg = $self->strip($m->{description});
	$msg = $self->reflow($msg,$width) if defined $width;
	$msg = "$m->{moduleID}::$m->{label}($m->{subID}): $m->{objectID}\n\n".$msg;
	return $msg;
}
#package MibInfo;
sub TCDescription {
	my ($self,$width) = @_;
	lock $Model::SNMP::lockvar;
	my $msg = $self->strip($self->{m}{TCDescription});
	$msg = $self->reflow($msg,$width) if defined $width;
	return $msg;
}
#package MibInfo;
sub reference {
	my ($self,$width) = @_;
	lock $Model::SNMP::lockvar;
	my $msg = $self->strip($self->{m}{reference});
	$msg = $self->rehang($msg,$width) if defined $width;
	return $msg;
}

# ------------------------------------------
package MibLabel; our @ISA = qw(MibInfo);
# ------------------------------------------
#package MibLabel;
my %prefixes = (
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
	my $self = shift;
	my ($dialog,$view,$table,$label) = @_;
	my $prefix = $table;
	$prefix =~ s/Data$//;
	$prefix =~ s/Scalars$//;
	$prefix =~ s/Table$//;
	$prefix =~ s/Entry$//;
	$prefix = $prefixes{$prefix} if exists $prefixes{$prefix};
	my $short = $label; $short =~ s/^$prefix//;
	$self->{prefix} = $prefix;
	$self->{short} = $short;
}

# ------------------------------------------
package MibUnits; our @ISA = qw(MibInfo);
# ------------------------------------------
#package MibUnits;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label) = @_;
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	my $units = '';
	$units = $m->{type} if $m->{type};
	$units = $m->{textualConvention} if $m->{textualConvention};
	$units = $m->{units} if $m->{units};
	$self->{units} = $units;
}

# ------------------------------------------
package MibNode; our @ISA = qw(MibInfo);
# ------------------------------------------
#package MibNode;
sub bless {
	my $self = shift;
	my $kind = shift;
	my ($node,@rest) = split(/::/,ref($self));
	my $type = "$node\::$kind\::".join('::',@rest);
	bless $self,$type;
	$self->init(@_);
	return $self;
}
#package MibNode;
sub init {
	my $self = shift;
	my ($dialog,$view,$table,$label,$val) = @_;
	lock $Model::SNMP::lockvar;
	my $m = $self->{m};
	$self->{editable} = (defined $m->{access} and $m->{access} =~ /Write|Create/);
	$self->{password} = (defined $m->{type} and $m->{type} eq 'PASSWORD');
	$self->{val} = $val;
	if ((defined $m->{syntax} and $m->{syntax} eq 'TruthValue') or (defined $m->{type} and $m->{type} eq 'BOOLEAN')) {
		$$val = 0 unless defined $$val;
		if ($m->{syntax} eq 'TruthValue') {
			$self->bless('Truth',@_);
		} else {
			$self->bless('Boolean',@_);
		}
	}
	elsif (defined $m->{type} and $m->{type} eq 'BITS') {
		$$val = '' unless defined $$val;
		my @rows = ();
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			push @rows, [ $tag, $m->{enums}{$tag} ];
		}
		$self->{rows} = \@rows;
		my @sels = ();
		my @bits = ();
		foreach (values %{$m->{enums}}) { $bits[$_] = '0' }
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
		$self->{sels} = \@sels;
		$self->bless('List',@_);
	}
	elsif (defined $m->{enums} and $m->{enums} and scalar keys %{$m->{enums}} > 0) {
		my $use = MibNode::Combo->snmpval($m,$$val);
		my $i = 0;
		my @tags = ();
		my @sels = ();
		foreach my $tag (sort {$m->{enums}{$a}<=>$m->{enums}{$b}} keys %{$m->{enums}}) {
			push @tags, "$tag($m->{enums}{$tag})";
			push @sels, $i if $use eq $tag or ($use =~ /^\d+$/ and $use == $m->{enums}{$tag});
			$i++;
		}
		$self->{tags} = \@tags;
		$self->{sels} = \@sels;
		$self->bless('Combo',@_);
	}
	else {
		$$val = '' unless defined $$val;
		$self->bless('Entry',@_);
	}
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

# ------------------------------------------
package MibNode::Truth; our @ISA = qw(MibNode);
# ------------------------------------------
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

# ----------------------------------
package MibNode::Boolean; our @ISA = qw(MibNode);
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

# ----------------------------------
package MibNode::List; our @ISA = qw(MibNode);
# ----------------------------------
#package MibNode::List;
sub snmpval {
	my ($type,$m,$val) = @_;
	if (not defined $val) {
		# try default
		return MibNode::List->snmpval($m,$m->{defaultValue}) if exists $m->{defaultValue} and defined $m->{defaultValue};
		# do all zeros
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0 }
		@bits = map {'0'} @bits;
		my $bits = join('',@bits).'0000000';
		$bits = substr($bits,0,8*int(length($bits)/8));
		$val = pack('B*',$bits);
	} elsif ($val=~/^((\w+\b)(\n?\w+\b)*)?$/) {
		# found line feed array
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0 }
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
		my @bits = (); foreach (values %{$m->{enums}}) { $bits[$_] = 0 }
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

# ----------------------------------
package MibNode::Combo; our @ISA = qw(MibNode);
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

# ----------------------------------
package MibNode::Entry; our @ISA = qw(MibNode);
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

# ------------------------------------------
package MibEntry; our @ISA = qw(Base);
# ------------------------------------------
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
	$self->{active} = 1;
	return $self;
}
#package MibEntry;
sub disable_view {
	my $self = shift;
	$self->{active} = 0;
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

# ------------------------------------------
package Datum::View; our @ISA = qw(Base);
# ------------------------------------------

# ------------------------------------------
package Datum::Draw; our @ISA = qw(Base);
# ------------------------------------------
#package Datum::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_) }
#package Datum::Draw;
sub fillmenu { return shift->impl(__PACKAGE__,'fillmenu',@_) }

# ------------------------------------------
package ThreeColumn::View; our @ISA = qw(Group::View);
# ------------------------------------------

# ------------------------------------------
package ThreeColumn::Draw; our @ISA = qw(Group::Draw);
# ------------------------------------------
#package ThreeColumn::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_) }
#package ThreeColumn::Draw;
sub fini { shift->impl(__PACKAGE__,'fini',@_) }
#package ThreeColumn::Draw;
sub setcolor { shift->impl(__PACKAGE__,'setcolor',@_) }
#package ThreeColumn::Draw;
sub placing {
	my $self = shift;
	my ($forced) = @_;
#return unless $self->{changed} or $forced;
	my ($x,$y) = $self->pos;
	my $siz = $self->{siz};
	my ($th,$tw) = map {$_/2.0} @{$siz->{t}};
	my ($tx,$ty) = ($x,$y);
	my $tbox = $self->{tbox}{pnts};
	@$tbox = ($tx-$tw,$ty-$th,$tx+$tw,$ty-$th,$tx+$tw,$ty+$th,$tx-$tw,$ty+$th);
	my ($ch,$cw) = (0,0);
	if ($siz->{c}{total}) {
		($ch,$cw) = map {$_/2.0} @{$siz->{c}{total}};
		my ($cx,$cy) = ($x,$y);
		my $cbox = $self->{cbox}{pnts};
		@$cbox = ($cx-$cw,$cy-$ch,$cx+$cw,$cy-$ch,$cx+$cw,$cy+$ch,$cx-$cw,$cy+$ch);
	}
	if ($siz->{l}{total}) {
		my ($lh,$lw) = map {$_/2.0} @{$siz->{l}{total}};
		my ($lx,$ly) = ($x-$cw-$lw,$y);
		my $lbox = $self->{lbox}{pnts};
		@$lbox = ($lx-$lw,$ly-$lh,$lx+$lw,$ly-$lh,$lx+$lw,$ly+$lh,$lx-$lw,$ly+$lh);
	}
	if ($siz->{r}{total}) {
		my ($rh,$rw) = map {$_/2.0} @{$siz->{r}{total}};
		my ($rx,$ry) = ($x+$cw+$rw,$y);
		my $rbox = $self->{rbox}{pnts};
		@$rbox = ($rx-$rw,$ry-$rh,$rx+$rw,$ry-$rh,$rx+$rw,$ry+$rh,$rx-$rw,$ry+$rh);
	}
	$self->impl(__PACKAGE__,'placing',@_);
}

# ------------------------------------------
package Subnetwork::Network::View; our @ISA = qw(ThreeColumn::View);
# ------------------------------------------

# ------------------------------------------
package Subnetwork::Network::Draw; our @ISA = qw(ThreeColumn::Draw);
# ------------------------------------------
#package Subnetwork::Network::Draw;
sub getsize {
	my $self = shift;
	my ($hbox,$wbox,$geom) = @_;
	foreach my $side (keys %$geom) {
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $draw (@{$geom->{$side}{$kind}}) {
				my $hsiz = $side eq 'c' ? 0 : $hbox;
				$draw->{want} = [$hsiz,$wbox];
			}
		}
	}
}
#package Subnetwork::Network::Draw;
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
				foreach my $draw (@{$geom->{$side}{$kind}}) {
					my ($hbox,$wbox) = @{$draw->{want}};
					my ($h,$w,$mh,$mw) = $draw->resize($hbox,$wbox);
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
				foreach my $draw (@{$geom->{$side}{$kind}}) {
					my ($hbox,$wbox) = @{$draw->{want}};
					my ($h,$w,$mh,$mw) = $draw->resize($hbox,$wbox);
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
#package Subnetwork::Network::Draw;
sub shrink {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my ($hall,$wall) = @{$size->{t}};
	my ($hfac,$wfac) = ($hbox/$hall,$wbox/$wall);
	foreach my $side (keys %$geom) {
		if ($side eq 'c') {
			my $hsiz = $hbox;
			foreach my $kind (keys %{$geom->{$side}}) {
				foreach my $draw (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $draw->siz;
					my $wsiz = $w*$wfac;
					$draw->{want} = [$hsiz,$wsiz];
				}
			}
		} else {
			my $wsiz = ($size->{$side}{total}[1]+2.0*$size->{$side}{total}[3])*$wfac;
			foreach my $kind (keys %{$geom->{$side}}) {
				foreach my $draw (@{$geom->{$side}{$kind}}) {
					my ($h,$w) = $draw->siz;
					my $hsiz = $h*$hfac;
					$draw->{want} = [$hsiz,$wsiz];
				}
			}
		}
	}
}
#package Subnetwork::Network::Draw;
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
		foreach my $draw (@{$geom->{c}{$kind}}) {
			my ($h,$wsiz) = $draw->siz;
			$draw->resize($hsiz,$wsiz);
		}
	}
	return $self->siz;
}
#package Subnetwork::Network::Draw;
sub layout {
	my $self = shift;
	my $kind = $self->kind;
	my $geom = delete $self->{geo};
	my $size = $self->{siz};
	my ($X,$Y) = $self->pos;
	foreach my $side (keys %$geom) {
		if ($side eq 'c') {
			my $yofs = 0;
			foreach my $type (keys %{$geom->{$side}}) {
				my ($H,$W,$MH,$MW) = @{$size->{$side}{$type}};
				my $x0 = -$W/2.0;
				my $x = $x0;
				foreach my $draw (@{$geom->{$side}{$type}}) {
					my ($h,$w,$mh,$mw) = $draw->siz;
					my $xofs = $x + $w/2.0 + $mw;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					$draw->move_draw(@pos);
					$draw->layout;
					$x += $w + 2.0*$mw;
				}
			}
		} else {
			my ($H,$W,$MH,$MW) = @{$size->{$side}{total}};
			my $y = -$H/2.0;
			my $x = $size->{c}{total}[1]/2.0+$size->{c}{total}[3];
			foreach my $type (keys %{$geom->{$side}}) {
				foreach my $draw (@{$geom->{$side}{$type}}) {
					my ($h,$w,$mh,$mw) = $draw->siz;
					my $xofs = $x + $w/2.0 + $mw;
					$xofs = -$xofs if $side eq 'l';
					my $yofs = $y + $h/2.0 + $mw;
					my @pos = ($X+$xofs,$Y+$yofs,$xofs,$yofs);
					$draw->move_draw(@pos);
					$draw->layout;
					$y += $h + 2.0*$mh;
				}
			}
		}
	}
}

# ------------------------------------------
package Network::Network::View; our @ISA = qw(Subnetwork::Network::View Point::View Root::View);
# ------------------------------------------
#package Network::Network::View;
sub mycolor { return 'grey' }
#package Network::Network::View;
sub ctypes { return qw/Private Host Vlan Lan Subnet/ }
#package Network::Network::View;
sub geom {
	my $self = shift;
	my $geom = $self->{geom};
	unless ($geom) {
		$geom = $self->{geom} = {};
		foreach my $net ($self->children('Private')) {
			if ($net->isa('Private::Here::Network::View')) {
				push @{$geom->{l}{Private}}, $net;
			} else {
				push @{$geom->{r}{Private}}, $net;
			}
		}
		foreach my $host ($self->offspring_sortbytype('Host',[
					Item::IPV4ADDRKEY,
					Item::IPV6ADDRKEY])) {
			next if ($host->parent('Private'));
			if ($host->isa('Host::Ip::Here::Network::View')) {
				push @{$geom->{l}{Host}}, $host;
			} else {
				push @{$geom->{r}{Host}}, $host;
			}
		}
		foreach my $form (qw/Vlan Lan Subnet/) {
			foreach my $bus ($self->offspring_sortbytype($form,[
						Item::MACKEY,
						Item::VLANKEY,
						Item::IPV4MASKKEY,
						Item::IPV6MASKKEY])) {
				push @{$geom->{c}{$form}},$bus;
			}
		}
	}
	return $geom;
}
#package Network::Network::View;
sub getgeom {
	my $self = shift;
	my $geom = $self->{geo};
	unless ($geom) {
		#$geom = $self->{geo} = {};
		$geom = {};
		foreach my $net ($self->children('Private')) {
			if ($net->isa('Private::Here::Network::View')) {
				push @{$geom->{l}{Private}},$net;
				$net->{col} = -2;
			} else {
				push @{$geom->{r}{Private}},$net;
				$net->{col} = +2;
			}
		}
		foreach my $host ($self->offspring_sortbytype('Host',[
					Item::IPV4ADDRKEY,
					Item::IPV6ADDRKEY])) {
			next if ($host->parent('Private'));
			if ($host->isa('Host::Ip::Here::Network::View')) {
				push @{$geom->{l}{Host}},$host;
				$host->{col} = -1;
			} else {
				push @{$geom->{r}{Host}},$host;
				$host->{col} = +1;
			}
		}
		foreach my $form (qw/Lan Vlan Subnet/) {
			foreach my $bus ($self->offspring_sortbytype($form,[
						Item::MACKEY,
						Item::VLANKEY,
						Item::IPV4MASKKEY,
						Item::IPV6MASKKEY])) {
				push @{$geom->{c}{$form}},$bus;
				$bus->{col} = 0;
			}
		}
	}
	return $geom;
}
#package Network::Network::View;
sub adding {
	my ($self,$child) = @_;
	return unless $self->{geo} or $self->{geom};
	my $kind = $child->kind;
	my %map = map {$_=>1} qw/Private Host Lan Vlan Subnet/;
	return unless $map{$kind};
	return if $kind eq 'Host' and $child->parent('Private');
	$self->{viewer}->changed if delete $self->{geom};
	$self->{viewer}->changed if delete $self->{geo};
}
#package Network::Network::View;
sub rekeying { shift->adding(@_) }

# ------------------------------------------
package Network::Network::Draw; our @ISA = qw(Subnetwork::Network::Draw Point::Draw Root::Draw);
# ------------------------------------------
#package Network::Network::Draw;
sub rebalance {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my %draws = (l=>[],r=>[]);
	foreach my $side (qw/l r/) {
		foreach my $kind (qw/Private Host/) {
			if ($geom->{$side}{$kind}) {
				push @{$draws{$side}}, @{$geom->{$side}{$kind}};
			}
		}
	}
	while (@{$draws{r}} > 1) {
		my $draw = $draws{r}[-1];
		my ($h,$w) = $draw->siz;
		last if ($size->{l}{total}[0] + $h > $size->{r}{total}[0] - $h);
		$size->{l}{total}[0] += $h;
		$size->{r}{total}[0] -= $h;
		push @{$draws{l}},pop @{$draws{r}};
		my $kind = $draw->kind;
		push @{$geom->{l}{$kind}}, pop @{$geom->{r}{$kind}};
		$draw->{col} = -$draw->{col};
	}
}

# ------------------------------------------
package Private::Network::View; our @ISA = qw(Subnetwork::Network::View Point::View Tree::View);
# ------------------------------------------
#package Private::Network::View;
sub mycolor { return 'orange' }
#package Private::Network::View;
sub ctypes { return qw/Host Vlan Lan Subnet/ }
#package Private::Network::View;
sub geom {
	my $self = shift;
	my $geom = $self->{geom};
	unless ($geom) {
		$geom = $self->{geom} = {};
		foreach my $host ($self->offspring_sortbytype('Host',[
					Item::IPV4ADDRKEY,
					Item::IPV6ADDRKEY])) {
			if ($host->parent('Network')) {
				push @{$geom->{l}{Host}}, $host;
			}
			elsif ($host->parent('Private')) {
				push @{$geom->{r}{Host}}, $host;
			}
		}
		foreach my $form (qw/Vlan Lan Subnet/) {
			foreach my $bus ($self->offspring_sortbytype($form,[
						Item::MACKEY,
						Item::VLANKEY,
						Item::IPV4MASKKEY,
						Item::IPV6MASKKEY])) {
				unless ($bus->parent('Network')) {
					push @{$geom->{c}{$form}},$bus;
				}
			}
		}
	}
	return $geom;
}
#package Private::Network::View;
sub getgeom {
	my $self = shift;
	my $geom = $self->{geo};
	unless ($geom) {
		#$geom = $self->{geo} = {};
		$geom = {};
		my $col = $self->{col};
		my $l = $col > 0 ? 'l' : 'r';
		my $r = $col > 0 ? 'r' : 'l';
		foreach my $host ($self->offspring_sortbytype('Host',[
					Item::IPV4ADDRKEY,
					Item::IPV6ADDRKEY])) {
			if ($host->parent('Network')) {
				push @{$geom->{$l}{Host}},$host;
				$host->{col} = ($col > 0) ? $col-1 : $col+1;
			}
			elsif ($host->parent('Private')) {
				push @{$geom->{$r}{Host}},$host;
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
					push @{$geom->{c}{$form}},$bus;
					$bus->{col} = $col;
				}
			}
		}
	}
	return $geom;
}
#package Private::Network::View;
sub adding {
	my ($self,$child) = @_;
	return unless $self->{geo} or $self->{geom};
	my $kind = $child->kind;
	my %map = map {$_=>1} qw/Private Host Lan Vlan Subnet/;
	return unless $map{$kind};
	if (my $net = $child->parent('Network')) {
		# Needs to be removed from Network and added here
		$self->{viewer}->changed if delete $net->{geom};
		$self->{viewer}->changed if delete $net->{geo};
	}
	$self->{viewer}->changed if delete $self->{geom};
	$self->{viewer}->changed if delete $self->{geo};
}
#package Private::Network::View;
sub rekeying { shift->adding(@_) }

# ------------------------------------------
package Private::Network::Draw; our @ISA = qw(Subnetwork::Network::Draw Point::Draw Tree::Draw);
# ------------------------------------------
#package Private::Network::Draw;
sub rebalance {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my $col = $self->{col};
	my $l = $col > 0 ? 'l' : 'r';
	my $r = $col > 0 ? 'r' : 'l';
	my %draws = ($l=>[],$r=>[]);
	foreach my $side (qw/l r/) {
		foreach my $kind (qw/Host/) {
			if ($geom->{$side}{$kind}) {
				push @{$draws{$side}}, @{$geom->{$side}{$kind}};
			}
		}
	}
	while (@{$draws{$r}} > 1) {
		my $draw = $draws{$r}[-1];
		my ($h,$w) = $draw->siz;
		last if ($size->{$l}{total}[0] + $h > $size->{$r}{total}[0] - $h);
		$size->{$l}{total}[0] += $h;
		$size->{$r}{total}[0] -= $h;
		push @{$draws{$l}}, pop @{$draws{$r}};
		my $kind = $draw->kind;
		push @{$geom->{$l}{$kind}}, pop @{$geom->{$r}{$kind}};
		$draw->{col} = ($col > 0) ? $col-1 : $col+1;
	}
}
#package Private::Network::Draw;
sub myparent {
	my ($self,$parent) = @_;
	return $parent->kind eq 'Network';
}

# ------------------------------------------
package Private::Here::Network::View; our @ISA = qw(Private::Network::View);
# ------------------------------------------

# ------------------------------------------
package Private::Here::Network::Draw; our @ISA = qw(Private::Network::Draw);
# ------------------------------------------

# ------------------------------------------
package Local::Network::View; our @ISA = qw(View Point::View Tree::View);
# ------------------------------------------

# ------------------------------------------
package Local::Network::Draw; our @ISA = qw(Draw Point::Draw Tree::Draw);
# ------------------------------------------

# ------------------------------------------
package Local::Here::Network::View; our @ISA = qw(Local::Network::View);
# ------------------------------------------

# ------------------------------------------
package Local::Here::Network::Draw; our @ISA = qw(Local::Network::Draw);
# ------------------------------------------

# ------------------------------------------
package Host::Network::View; our @ISA = qw(Node::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Host::Network::View;
sub mycolor { return 'red' }
#package Host::Network::View;
sub ctypes { return qw/Vprt Port Address/ }
#package Host::Network::View;
sub geom {
	my $self = shift;
	my $geom = $self->{geom};
	unless ($geom) {
		$geom = $self->{geom} = {};
		my @buses = qw/Vlan Lan Subnet/;
		foreach my $kind (qw/Vprt Port Address/) {
			my $ptype = shift @buses;
			foreach my $box ($self->offspring($kind)) {
				if (my $bus = $box->parent($ptype)) {
					if ($bus->parent('Network')) {
						push @{$geom->{l}{$kind}}, $box;
					}
					elsif ($bus->parent('Private')) {
						push @{$geom->{r}{$kind}}, $box;
					}
				} else {
					#push @{$geom->{r}{$kind}}, $box;
				}
			}
		}
	}
	return $geom;
}
#package Host::Network::View;
sub getgeom {
	my $self = shift;
	my $geom = $self->{geo};
	unless ($geom) {
		#$geom = $self->{geo} = {};
		$geom = {};
		my $col = $self->{col};
		foreach my $vprt ($self->offspring_sortbytype('Vprt',[
					Item::INTKEY,
					Item::VLANKEY])) {
			my $vlan = $vprt->parent('Vlan');
			if ($vlan and exists $vlan->{col} and ($vlan->parent('Network') or $vlan->parent('Private'))) {
				if ($vlan->{col} < $col)
				{ push @{$geom->{l}{Vprt}},$vprt } else
				{ push @{$geom->{r}{Vprt}},$vprt }
			}
		}
		foreach my $port ($self->offspring_sortbytype('Port',[
					Item::INTKEY,
					Item::MACKEY])) {
			my $plan = $port->parent('Lan');
			if ($plan and exists $plan->{col} and ($plan->parent('Network') or $plan->parent('Private'))) {
				if ($plan->{col} < $col)
				{ push @{$geom->{l}{Port}},$port } else
				{ push @{$geom->{r}{Port}},$port }
			}
		}
		foreach my $addr ($self->offspring_sortbytype('Address',[
					Item::IPV4ADDRKEY,
					Item::IPV6ADDRKEY])) {
			my $subn = $addr->parent('Subnet');
			if ($subn and exists $subn->{col} and ($subn->parent('Network') or $subn->parent('Private'))) {
				if ($subn->{col} < $col)
				{ push @{$geom->{l}{Address}},$addr } else
				{ push @{$geom->{r}{Address}},$addr }
			}
		}
	}
	return $geom;
}
#package Host::Network::View;
sub adding {
	my ($self,$child) = @_;
	return unless $self->{geo} or $self->{geom};
	my $kind = $child->kind;
	my %map = map {$_=>1} qw/Vprt Port Address/;
	return unless $map{$kind};
	$self->{viewer}->changed if delete $self->{geom};
	$self->{viewer}->changed if delete $self->{geo};
}
#package Host::Network::View;
sub rekeying { shift->adding(@_) }

# ------------------------------------------
package Host::Network::Draw; our @ISA = qw(Node::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Host::Network::Draw;
sub getsize {
	my $self = shift;
	my ($hbox,$wbox,$geom) = @_;
	$self->{want} = [$hbox,$wbox];
	foreach my $side (keys %$geom) {
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $draw (@{$geom->{$side}{$kind}}) {
				$draw->{want} = [$hbox,$wbox];
			}
		}
	}
}
#package Host::Network::Draw;
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
			foreach my $draw (@{$geom->{$side}{$kind}}) {
				my ($hbox,$wbox) = @{$draw->{want}};
				my ($h,$w) = $draw->resize($hbox,$wbox);
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
#package Host::Network::Draw;
sub shrink {
	my $self = shift;
	my ($hbox,$wbox,$geom,$size) = @_;
	my ($hall,$wall) = @{$size->{t}};
	my ($hfac,$wfac) = ($hbox/$hall,$wbox/$wall);
	foreach my $side (keys %$geom) {
		my $wsiz = $size->{$side}{total}[1]*$wfac;
		foreach my $kind (keys %{$geom->{$side}}) {
			foreach my $draw (@{$geom->{$side}{$kind}}) {
				my ($h,$v) = $draw->siz;
				my $hsiz = $h*$hfac;
				$draw->{want} = [$hsiz,$wsiz];
			}
		}
	}
}
#package Host::Network::Draw;
sub recalc {
	my $self = shift;
	my $canvas = $self->{canvas};
	my ($hbox,$wbox) = @_;
	my ($hmrg,$wmrg);
	$hbox = $hbox < 2.0*$self->{pad}[0] ? 0 : $hbox - 2.0*$self->{pad}[0];
	$wbox = $wbox < 2.0*$self->{pad}[1] ? 0 : $wbox - 2.0*$self->{pad}[1];
	# The height is simply the maximum of the sum of the heights of all
	# boxes on either size and the height of the node itself.
	# The width is simply the maximum width of the boxes on the left plus
	# the width of the node plus the maximum width of the boxes on the
	# right.
	my $geom = $self->geom;
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
#package Host::Network::Draw;
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
#package Host::Network::Draw;
sub layout {
	my $self = shift;
	my $geom = delete $self->{geo};
	my $size = $self->{siz};
	my ($X,$Y) = $self->pos;
	foreach my $side (keys %$geom) {
		my $x0 = $size->{c}{total}[1]/2.0 + $size->{$side}{total}[1]/2.0;
		$x0 = -$x0 if $side eq 'l';
		foreach my $kind (keys %{$geom->{$side}}) {
			my @draws = @{$geom->{$side}{$kind}};
			if (@draws) {
				my ($H,$W) = @{$size->{$side}{$kind}};
				my $y0 = -$H/2.0;
				my $y = $y0;
				foreach my $draw (@draws) {
					my ($btype,$ntype) = $draw->{view}->types;
					my ($x,$yold,$xofs) = $draw->pos;
					($x,$xofs) = ($x0,0) unless $draw->parent($btype);
					my ($h,$w) = $draw->siz;
					my $yofs = $y + $h/2.0;
					my @pos = ($x,$Y+$yofs,$xofs,$yofs);
					$draw->move_draw(@pos);
					$draw->layout;
					$y += $h;
				}
			}
		}
	}
}

# ------------------------------------------
package Host::Ip::Network::View; our @ISA = qw(Host::Network::View);
# ------------------------------------------
#package Host::Ip::Network::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{item}{data};
	my $sysd = $data->{systemData};
	my $color = ($sysd and exists $sysd->{sysName}) ? 'black' : 'darkgrey';
	return $color;
}
#package Host::Ip::Network::View;
sub gettxt {
	my $self = shift;
	my @lines = ();
	my $data = $self->{item}{data};
	my $sysd = $data->{systemData};
	my $name = $sysd->{sysName} if $sysd;
	push @lines, $name if $name;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if (exists $self->{item}{key}{p}{$kind}) {
			push @keys,sort keys %{$self->{item}{key}{p}{$kind}};
		}
	}
	my $key = $keys[0];
	push @lines, Item::showkey($key) if $key;
	return $self->SUPER::gettxt unless @lines;
	$self->{text} = join("\n",@lines);
}

# ------------------------------------------
package Host::Ip::Network::Draw; our @ISA = qw(Host::Network::Draw);
# ------------------------------------------
#package Host::Ip::Network::Draw;
sub xform {
	my ($type,$self) = @_;
	bless $self,$type;
	$self->setcolor($self->mycolor);
}

# ------------------------------------------
package Host::Ip::Here::Network::View; our @ISA = qw(Host::Ip::Network::View);
# ------------------------------------------
#package Host::Ip::Here::Network::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{item}{data};
	my $sysd = $data->{systemData};
	my $color = ($sysd and exists $sysd->{sysName}) ? 'magenta' : 'darkgrey';
	return $color;
}

# ------------------------------------------
package Host::Ip::Here::Network::Draw; our @ISA = qw(Host::Ip::Network::Draw);
# ------------------------------------------
#package Host::Ip::Here::Network::Draw;
sub xform {
	my ($type,$self) = @_;
	bless $self,$type;
	$self->setcolor($self->mycolor);
}

# ------------------------------------------
package Subnet::Network::View; our @ISA = qw(Bus::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Subnet::Network::View;
sub mycolor { return 'brown' }
#package Subnet::Network::View;
sub ctypes { return qw/Address/ }
#package Subnet::Network::View;
sub gettxt {
	my $self = shift;
	my @keys = ();
	foreach my $kind (qw/Network Private Local/) {
		if (exists $self->{item}{key}{p}{$kind}) {
			foreach (keys %{$self->{item}{key}{p}{$kind}}) {
				push @keys, $_ if Item::keytype($_) == Item::IPV4MASKKEY();
			}
		}
	}
	my @sorted = sort @keys;
	my $key = $sorted[0];
	return $self->SUPER::gettxt unless $key;
	$self->{text} = Item::showkey($key);
	return $self->{text} if $self->{text};
	return $self->SUPER::gettxt;
}

# ------------------------------------------
package Subnet::Network::Draw; our @ISA = qw(Bus::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------

# ------------------------------------------
package Lan::Network::View; our @ISA = qw(Bus::View Point::View Tree::View);
# ------------------------------------------
#package Lan::Network::View;
sub ctypes { return qw/Port/ }

# ------------------------------------------
package Lan::Network::Draw; our @ISA = qw(Bus::Draw Point::Draw Tree::Draw);
# ------------------------------------------

# ------------------------------------------
package Vlan::Network::View; our @ISA = qw(Bus::View Point::View Tree::View);
# ------------------------------------------
#package Vlan::Network::View;
sub mycolor { return 'blue' }
#package Vlan::Network::View;
sub ctypes { return qw/Vprt/ }

# ------------------------------------------
package Vlan::Network::Draw; our @ISA = qw(Bus::Draw Point::Draw Tree::Draw);
# ------------------------------------------

# ------------------------------------------
package Address::Network::View; our @ISA = qw(BoxAndLink::View Point::View Leaf::View Datum::View);
# ------------------------------------------
#package Address::Network::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{item}{data};
	return 'brown' if exists $data->{ipAddrEntry};
	return 'brown' if exists $data->{ipAddressEntry};
	return 'darkgrey';
}
#package Address::Network::View;
sub types { return qw{Subnet Host} }
#package Address::Network::View;
sub gettxt {
	my $self = shift;
	if (exists $self->{item}{key}{p}{Host}) {
		foreach my $key (sort keys %{$self->{item}{key}{p}{Host}}) {
			$self->{text} = Item::showkey($key) if Item::keytype($key) == Item::IPV4ADDRKEY();
			return $self->{text} if $self->{text};
			last;
		}
	}
	return $self->SUPER::gettxt;
}

# ------------------------------------------
package Address::Network::Draw; our @ISA = qw(BoxAndLink::Draw Point::Draw Leaf::Draw Datum::Draw);
# ------------------------------------------
#package Address::Network::Draw;
sub init { shift->{txt}{pnts}[2] = 11 }

# ------------------------------------------
package Port::Network::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Port::Network::View;
sub types { return qw{Lan Host} }
#package Port::Network::View;
sub gettxt {
	my $self = shift;
	my $txt;
	my $data = $self->{item}{data};
	my $lldp = $data->{lldpLocPortEntry};
	$txt = $lldp->{lldpLocPortDesc} if $lldp;
	return $txt if $txt and $txt =~ /^eth/;
	my $ifen = $data->{ifEntry};
	$txt = $ifen->{ifDescr} if $ifen;
	return $txt if $txt and $txt =~ /^eth/;
	if (exists $self->{item}{key}{n}) {
		foreach my $key (sort @{$self->{item}{key}{n}}) {
			$self->{text} = Item::showkey($key) if Item::keytype($key) == Item::MACKEY();
			return $self->{text} if $self->{text};
			last;
		}
	}
	return $self->SUPER::gettxt;
}

# ------------------------------------------
package Port::Network::Draw; our @ISA = qw(BoxAndLink::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Port::Network::Draw;
sub init { shift->{txt}{pnts}[2] = 13 }

# ------------------------------------------
package Vprt::Network::View; our @ISA = qw(BoxAndLink::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Vprt::Network::View;
sub mycolor {
	my $self = shift;
	my $data = $self->{item}{data};
	return 'blue' if exists $data->{ifEntry};
	return 'blue' if exists $data->{lldpStatsTxPortEntry};
	return 'blue' if exists $data->{lldpStatsRxPortEntry};
	return 'blue' if exists $data->{lldpLocPortEntry};
	return 'darkgrey';
}
#package Vprt::Network::View;
sub types { return qw{Vlan Host} }
#package Vprt::Network::View;
sub gettxt {
	my $self = shift;
	my $txt;
	my $data = $self->{item}{data};
	my $lldp = $data->{lldpLocPortEntry};
	$txt = $lldp->{lldpLocPortDesc} if $lldp;
	return $txt if $txt and $txt =~ /^eth/;
	my $ifen = $data->{ifEntry};
	$txt = $ifen->{ifDescr} if $ifen;
	return $txt if $txt and $txt =~ /^eth/;
	if (exists $self->{item}{key}{n}) {
		foreach my $key (sort @{$self->{item}{key}{n}}) {
			$self->{text} = Item::showkey($key) if Item::keytype($key) == Item::MACKEY();
			return $self->{text} if $self->{text};
			last;
		}
	}
	return $self->SUPER::gettxt;
}

# ------------------------------------------
package Vprt::Network::Draw; our @ISA = qw(BoxAndLink::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Vprt::Network::Draw;
sub init { shift->{txt}{pnts}[2] = 17 }

# ------------------------------------------
package Route::Network::View; our @ISA = qw(View Path::View Datum::View);
# ------------------------------------------

# ------------------------------------------
package Route::Network::Draw; our @ISA = qw(Draw Path::Draw Datum::Draw);
# ------------------------------------------

# ------------------------------------------
package Driv::Driv::View; our @ISA = qw(View Point::View Tree::View Datum::View);
# ------------------------------------------

# ------------------------------------------
package Driv::Driv::Draw; our @ISA = qw(Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------

# ------------------------------------------
package Card::Driv::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
# ------------------------------------------

# ------------------------------------------
package Card::Driv::Draw; our @ISA = qw(Group::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Card::Driv::Draw;
sub init { shift->impl(__PACKAGE__,'init',@_) }

# ------------------------------------------
package Span::Driv::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Span::Driv::View;

# ------------------------------------------
package Span::Driv::Draw; our @ISA = qw(Group::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Span::Driv::Draw;

# ------------------------------------------
package Chan::Driv::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Chan::Driv::View;

# ------------------------------------------
package Chan::Driv::Draw; our @ISA = qw(Group::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Chan::Driv::Draw;

# ------------------------------------------
package Xcon::Driv::View; our @ISA = qw(Group::View Point::View Tree::View Datum::View);
# ------------------------------------------
#package Xcon::Driv::View;

# ------------------------------------------
package Xcon::Driv::Draw; our @ISA = qw(Group::Draw Point::Draw Tree::Draw Datum::Draw);
# ------------------------------------------
#package Xcon::Driv::Draw;

# ------------------------------------------
package Table::Window; our @ISA = qw(Table);
# ------------------------------------------
#package Table::Window;
our %schema = (
	table=>'Window',
	keys=>[ qw/id/ ],
	cols=>[ qw/id xpos ypos width height/ ],
	tsql=>
q{CREATE TABLE IF NOT EXISTS Window (
	id INT,
	xpos INT,
	ypos INT,
	width INT,
	height INT,
	PRIMARY KEY(id)
);},
);

# ------------------------------------------
package Window; our @ISA = qw(Gui::Derived Gui::Object);
# ------------------------------------------
# A Window is a window manager toplevel window.  It contains a notebook that has
# canvas associated with it.  We keep track of windows separately because they
# can come and go an have attributes that are saved in a backing database.
# ------------------------------------------
#package Window;
our $quitting = 0;
#package Window;
sub menuSnmpNew {
	my $self = shift;
}
#package Window;
sub menuSnmpOpen {
	my $self = shift;
}
#package Window;
sub menuSnmpClose {
	my $self = shift;
	$self->destroy;
}
#package Window;
sub menuSnmpQuit {
	my $self = shift;
	$quitting = 1;
	$self->save_all;
	$self->destroy_all;
	$self->main_quit;
}
#package Window;
sub main_quit { shift->impl(__PACKAGE__,'main_quit',@_) }
#package Window;
sub Canvas { shift->impl(__PACKAGE__,'Canvas',@_) }
#package Window;
sub delete_event {
	my $self = shift;
	my $gui = $self->{gui};
	my @windows = values %{$gui->{gobjs}{Window}};
	if ($quitting or scalar(@windows) == 1) {
		$self->{preserve} = 1;
	}
	$self->put;
	if (not $quitting and scalar(@windows) == 1) {
		$quitting = 1;
		$self->main_quit;
	}
}
#package Window;
sub save_all {
	my $self = shift;
	my $gui = $self->{gui};
	foreach (values %{$gui->{gobjs}{Window}}) {
		$_->{preserve} = 1;
		$_->saveme;
	}
}
#package Window;
sub destroy_all {
	my $self = shift;
	my $gui = $self->{gui};
	foreach (values %{$gui->{gobjs}{Window}}) {
		$_->{preserve} = 1;
		$_->destroy;
	}
}
#package Window;
sub new_viewer {
	my ($self,$base) = @_;
	my $id = undef;
	my $gui = $self->{gui};
	my $vtyp = 'Viewer::'.$base->kind;
	my $viewer = $vtyp->new($gui,$id,$base);
	my $ctyp = 'Canvas::'.$self->{gtk};
	my $canvas = $ctyp->new($gui,$id,$self,$viewer);
	return $viewer;
}

# ------------------------------------------
package MyOptions;
# ------------------------------------------
#package MyOptions;

# ------------------------------------------
package MyPixmaps;
# ------------------------------------------

1;

__END__
