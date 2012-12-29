# ------------------------------------------
package UNIVERSAL;
use warnings; use Carp;
# ------------------------------------------
#package UNIVERSAL;
sub _forw {
	my $self = shift;
	my $type = shift;
	my $only = shift;
	my $each = shift;
	if ($only and exists *{"$type\::"}->{$only}) {
		my $sub = "$type\::$only";
		$self->$sub(@_);
	} elsif ($each) {
		if (exists *{"$type\::"}->{ISA}) {
			foreach my $clas (@{*{"$type\::"}->{ISA}}) {
				$self->_forw($clas,$only,$each,@_);
			}
		}
		if (exists *{"$type\::"}->{$each}) {
			my $sub = "$type\::$each";
			$self->$sub(@_);
		}
	}
}
#package UNIVERSAL;
sub _fwcd {
	my $self = shift;
	my $type = shift;
	my $only = shift;
	my $each = shift;
	my $want = shift;
	my $have = $want;
	if ($only and exists *{"$type\::"}->{$only}) {
		my $sub = "$type\::$only";
		$have = $self->$sub(@_);
	} elsif ($each) {
		if (exists *{"$type\::"}->{ISA}) {
			foreach my $clas (@{*{"$type\::"}->{ISA}}) {
				$have = $self->_fwcd($clas,$only,$each,$want,@_);
				last unless $have eq $want;
			}
		}
		if ($have eq $want) {
			if (exists *{"$type\::"}->{$each}) {
				my $sub = "$type\::$each";
				$have = $self->$sub(@_);
			}
		}
	}
	return $have;
}
#package UNIVERSAL;
sub _init {
	my $self = shift;
	my $type = shift;
	$self->_forw($type,'only','init',@_);
}
#package UNIVERSAL;
sub _new {
	my $clas = shift;
	my $type = shift;
	my @isas = ();
	@isas = @{*{"$clas\::"}->{ISA}} if exists *{"$clas\::"}->{ISA};
	my $self;
	if (my $isa = shift @isas) {
		$self = $isa->_make($type,@_);
	} else {
		$self = {};
		bless $self,$type;
	}
	return $self;
}
#package UNIVERSAL;
sub _make {
	my $clas = shift;
	my $type = shift;
	my $self;
	if (exists *{"$clas\::"}->{make}) {
		$self = &{*{"$clas\::"}->{make}}($type,@_);
	} else {
		$self = $clas->_new($type,@_);
	}
	return $self;
}
#package UNIVERSAL;
sub new {
	my $type = shift;
	my $self = $type->_make($type,@_);
	$self->_init($type,@_);
	return $self;
}
#package UNIVERSAL;
sub _revs {
	my $self = shift;
	my $type = shift;
	my $only = shift;
	my $each = shift;
	if ($only and exists *{"$type\::"}->{$only}) {
		my $sub = "$type\::$only";
		$self->$sub(@_);
	} elsif ($each) {
		if (exists *{"$type\::"}->{$each}) {
			my $sub = "$type\::$each";
			$self->$sub(@_);
		}
		if (exists *{"$type\::"}->{ISA}) {
			foreach my $clas (reverse @{*{"$type\::"}->{ISA}}) {
				$self->_revs($clas,$only,$each,@_);
			}
		}
	}
}
#package UNIVERSAL;
sub _rvcd {
	my $self = shift;
	my $type = shift;
	my $only = shift;
	my $each = shift;
	my $want = shift;
	my $have = $want;
	if ($only and exists *{"$type\::"}->{$only}) {
		my $sub = "$type\::$only";
		$have = $self->$sub(@_);
	} elsif ($each) {
		if (exists *{"$type\::"}->{$each}) {
			my $sub = "$type\::$each";
			$have = $self->$sub(@_);
		}
		if ($have eq $want) {
			if (exists *{"$type\::"}->{ISA}) {
				foreach my $clas (reverse @{*{"$type\::"}->{ISA}}) {
					$have = $self->_rvcd($clas,$only,$each,$want,@_);
					last unless $have eq $want;
				}
			}
		}
	}
	return $have;
}
#package UNIVERSAL;
sub _fini {
	my $self = shift;
	my $type = shift;
	$self->_revs($type,'cull','fini',@_);
}
#package UNIVERSAL;
sub put {
	my $self = shift;
	my $type = ref $self;
	$self->_fini($type,@_);
}
#package UNIVERSAL;
sub get {
	my $type = shift;
	my $self;
	if ($type->can('find')) {
		if ($self = $type->find(@_)) {
			if ($self->can('link')) {
				$self->link(@_);
			}
			return $self;
		}
	}
	$self = $type->new(@_);
	return $self;
}
#package UNIVERSAL;
sub _dump {
	my $self = shift;
	my $type = shift;
	$self->_forw($type,'info','dump',@_);
}
#package UNIVERSAL;
sub show {
	my $self = shift;
	my $type = ref $self;
	##warn "Bad object of type $type, $self\n" if $self->{bad};
	print STDERR "\nShowing item $self\n-------------------------\n";
	$self->_dump($type,@_);
}
#package UNIVERSAL;
sub _tree {
	my $self = shift;
	my $type = shift;
	$self->_forw($type,undef,'tree',@_);
}
#package UNIVERSAL;
sub walk {
	my $self = shift;
	my ($crumb,$indent,$suffix) = @_;
	my $type = ref $self;
	$suffix .= ' ...' if exists $self->{crumb} and $self->{crumb} == $crumb;
	print STDERR "$indent$self$suffix\n";
	$self->_tree($type,@_);
	$self->{crumb} = $crumb;
}

# ------------------------------------------
package Base;
use strict; use warnings; use Carp;
# ------------------------------------------
##package Base;
#our %use = ();
#our %put = ();
#our %dst = ();
#our $init_count = 0;
#our $fini_count = 0;
#our $dest_count = 0;
##package Base;
#sub showstats {
#	print STDERR "Base object stats:\n------------------\n";
#	printf STDERR "%6d object init calls\n", $Base::init_count;
#	printf STDERR "%6d object fini calls\n", $Base::fini_count;
#	printf STDERR "%6d object dest calls\n", $Base::dest_count;
#	print STDERR "------------------\n";
#	printf STDERR "%6d objects use\n",scalar(values %Base::use);
#	printf STDERR "%6d objects put\n",scalar(values %Base::put);
#	printf STDERR "%6d objects dst\n",scalar(values %Base::dst);
#	print STDERR "------------------\n";
#}
##package Base;
#sub init {
#	my $self = shift;
#	$use{$self} = 1;
#	$init_count++;
#	Base::showstats() unless ($init_count % 10);
#}
##package Base;
#sub fini {
#	my $self = shift;
#	delete $Base::use{$self};
#	$put{$self} = 1;
#	$fini_count++;
#	Base::showstats() unless ($fini_count % 10);
#}
##package Base;
#sub DESTROY {
#	my $self = shift;
#	delete $Base::put{$self};
#	$dst{$self} = 1;
#	$dest_count++;
#	Base::showstats() unless ($dest_count % 10);
#}

# ------------------------------------------
package Root; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Data::Dumper;
# ------------------------------------------
#package Root;
sub init {
	my $self = shift;
	$self->{child} = {} unless exists $self->{child};
	$self->{members} = {} unless exists $self->{members};
	$self->{children} = {} unless exists $self->{children};
}
#package Root;
sub dump {
	my $self = shift;
	foreach my $form (keys %{$self->{child}}) {
		my @keys = keys %{$self->{child}{$form}};
		if (@keys) {
			print STDERR "$self has children of type $form:\n";
			foreach my $key (@keys) {
				my $child = $self->{child}{$form}{$key};
				print STDERR "\t$form child $child has key ",Item::showkey($key),"\n";
			}
		}
	}
	foreach my $form (keys %{$self->{members}}) {
		my @members = values %{$self->{members}{$form}};
		if (@members) {
			print STDERR "$self has members of type $form:\n";
			foreach my $memb (@members) {
				print STDERR "\t$form member $memb\n";
			}
		}
	}
}
#package Root;
sub fini {
	my $self = shift;
#	return unless exists $self->{members};
#	foreach my $form (values %{$self->{members}}) {
#		foreach (values %$form) { $_->put(@_) }
#	}
	delete $self->{child};
	delete $self->{members};
	delete $self->{children};
}
#package Root;
sub child {
	my ($self,$form,$key) = @_;
	Carp::confess "XXXXXX child of $self requested without key" unless defined $key;
	return undef unless exists $self->{child}{$form}{$key};
	return $self->{child}{$form}{$key};
}
#package Root;
sub members {
	my ($self,$form) = @_;
	my @members = ();
	if (defined $form) {
		push @members, values %{$self->{members}{$form}} if $self->{members}{$form};
	} else {
		foreach $form (keys %{$self->{members}}) {
			push @members, values %{$self->{members}{$form}};
		}
	}
	return @members;
}
#package Root;
sub members_sorted {
	my ($self,$forms) = @_;
	$forms = [ keys %{$self->{members}} ] unless defined $forms;
	$forms = [ $forms ] unless ref($forms) eq 'ARRAY';
	my @results = ();
	foreach my $form (@$forms) {
		if (my $membs = $self->{members}{$form}) {
			push @results,sort {-$a->{no} <=> -$b->{no}} values %$membs;
		}
	}
	return @results;
}
#package Root;
sub children {
	my ($self,$form) = @_;
	my @children = ();
	if (defined $form) {
		push @children, values %{$self->{children}{$form}} if $self->{children}{$form};
	} else {
		foreach $form (keys %{$self->{children}}) {
			push @children, values %{$self->{children}{$form}};
		}
	}
	return @children;
}
#package Root;
sub children_sorted {
	my ($self,$forms,$min,$max) = @_;
	$max = $min unless defined $max;
	$min = $max unless defined $min;
	$forms = [ keys %{$self->{child}} ] unless defined $forms;
	$forms = [ $forms ] unless ref($forms) eq 'ARRAY';
	my @results = ();
	{
		my @pairs = ();
		foreach my $form (@$forms) {
			if (my $kids = $self->{child}{$form}) {
				foreach my $key (keys %$kids) {
					push @pairs, [ $key, $kids->{$key} ];
				}
			}
		}
		my %child = ();
		foreach my $result (sort {$a->[0] cmp $b->[0]} @pairs) {
			next if defined $min and defined $max and
				($min > length($result->[0]) or length($result->[0]) > $max);
			unless (exists $child{$result->[1]}) {
				push @results, $result;
				$child{$result->[1]} = 1;
			}
		}
		foreach my $result (sort {$a->[0] cmp $b->[0]} @pairs) {
			unless (exists $child{$result->[1]}) {
				push @results, $result;
				$child{$result->[1]} = 1;
			}
		}
	}
	@results = map {$_->[1]} @results;
	return @results;
}
#package Root;
sub offspring {
	my $self = shift;
	return ($self->children(@_),$self->members(@_));
}
#package Root;
sub offspring_sorted {
	my $self = shift;
	return ($self->children_sorted(@_),$self->members_sorted(@_));
}
#package Root;
sub tree {
	my ($self,$crumb,$indent,$suffix) = @_;
	return if exists $self->{crumb} and $self->{crumb} == $crumb;
	foreach my $form (sort keys %{$self->{children}}) {
		if (exists $self->{child}{$form}) {
			my %children = ();
			foreach my $key (keys %{$self->{child}{$form}}) {
				my $child = $self->{child}{$form}{$key};
				$children{$child}{child} = $child;
				push @{$children{$child}{keys}}, $key;
			}
			foreach my $ckey (keys %children) {
				my $child = $children{$ckey}{child};
				my $keys = join(',',map {Item::showkey($_)} sort @{$children{$ckey}{keys}});
				$keys = " with keys $keys";
				$child->walk($crumb,$indent."\t->>",$keys);
			}
		}
		if (exists $self->{members}{$form}) {
			foreach my $member (values %{$self->{members}{$form}}) {
				$member->walk($crumb,$indent."\t-->",' without key');
			}
		}
	}

}
#package Root;
sub getchild {
	my $self = shift;
	my ($type,$key,$ckey,$link) = @_;
	my $form = $type->kind;
	my $child = $self->child($form,$key);
	if ($child) {
		$child->link($self->{model},$ckey,$link)
			if $link and ref $link eq 'ARRAY';
	} else {
		$link = [] unless $link;
		unshift @$link, [ref $self,$self,$key];
		$child = $type->get($self->{model},$ckey,$link);
	}
	return $child;
}
#package Root;
sub addchild {
	my $self = shift;
	my ($child,$key) = @_;
	my $form = $child->kind;
	my $kind = $self->kind;
	my $result = 0;
	$child->{parent}{$kind} = $self;
	if ($key) {
		my @keys = ref $key eq 'ARRAY' ? @$key : ($key);
		foreach $key (@keys) {
			next unless defined $key;
			my $othr = $self->{child}{$form}{$key};
			if ($othr and $othr ne $child) {
				Carp::confess sprintf('%s(%s) and %s(%s) object mismatch',$othr,$othr->kind,$child,$child->kind)
				unless $othr->isa(ref $child) or $child->isa(ref $othr);
				# Whoops.  Another child exists at that key so must
				# be the same, merge them.
				#warn "Adding child: child merge required: $othr instead of $child at key ".Item::showkey($key);
				my $reason = sprintf "Child index '%s' added to %s belongs to %s",Item::showkey($key),$child,$othr;
				$child->mergefrom($othr,$reason);
				my $became = $self->{child}{$form}{$key};
				Carp::confess "Adding child: child merge failed: $became should have become $child for key ".Item::showkey($key)
				if $became and $became ne $child;
			} elsif (not $othr) {
				$self->{child}{$form}{$key} = $child;
				$child->{key}{p}{$kind}{$key} = $key;
				$result = 1;
				#if (($self->isa('Network') or $self->isa('Private')) and $child->isa('Point::Station')) {
				#	Carp::cluck "Adding $child to $self with key ",Item::showkey($key); 
				#}
			}
			delete $self->{members}{$form}{$child->no}; # insurance
		}
	} else {
		unless (exists $child->{key}{p}{$kind} and scalar values %{$child->{key}{p}{$kind}}) {
			unless (exists $self->{members}{$form}{$child->no}) {
				$self->{members}{$form}{$child->no} = $child;
				$result = 1;
			}
		}
	}
	if (exists $self->{children}{$form}{$child->no}) {
		Viewer->changed($self,$child) if $result;
	} else {
		$self->{children}{$form}{$child->no} = $child;
		Viewer->added_child($self,$child);
	}
	return $result;
}
#package Root;
sub delchild {
	my $self = shift;
	my ($child,$key) = @_;
	my $kind = $self->kind;
	my $form = $child->kind;
	my $result = undef;
	$key = exists $child->{key}{p}{$kind} ? [ values %{$child->{key}{p}{$kind}} ] : [] unless $key;
	my @keys = ref $key eq 'ARRAY' ? @$key : ($key);
	if (@keys) {
		foreach my $k (@keys) {
			delete $child->{key}{p}{$kind}{$k};
			if (my $othr = delete $self->{child}{$form}{$k}) {
				Carp::confess "Deleted child $othr unexpected, should be $child"
				if $othr ne $child;
				#warn "Deleted child $othr for key ".Item::showkey($k);
				$result = $child;
			} else {
				Carp::cluck "No entry for child with key ".Item::showkey($k);
			}
		}
		delete $self->{members}{$form}{$child->no}; # insurance
		unless (scalar values %{$child->{key}{p}{$kind}}) {
			delete $child->{key}{p}{$kind};
			delete $child->{parent}{$kind};
		}
	} else {
		if (my $othr = delete $self->{members}{$form}{$child->no}) {
			#warn "Deleted member $othr";
			$result = $child;
		} else {
			Carp::cluck "No entry for child $child";
		}
		delete $child->{key}{p}{$kind};
		delete $child->{parent}{$kind};
	}
	delete $self->{children}{$form}{$child->no};
	return $result;
}
#package Root;
sub chek {
	my $self = shift;
	foreach my $form (keys %{$self->{children}}) {
		foreach my $child (values %{$self->{children}{$form}}) {
			#$self->show();
			Carp::confess "Merge failed, Root $self still has children $child";
		}
	}
	foreach my $form (keys %{$self->{child}}) {
		foreach my $child (values %{$self->{child}{$form}}) {
			#$self->show();
			Carp::confess "Merge failed, Root $self still has child $child";
		}
		delete $self->{child}{$form};
	}
	foreach my $form (keys %{$self->{members}}) {
		foreach my $member (values %{$self->{members}{$form}}) {
			#$self->show();
			Carp::confess "Merge failed, Root $self still has member $member";
		}
		delete $self->{members}{$form};
	}
}
#package Root;
sub mark {
	my ($self,$othr,$merge) = @_;
	foreach my $form (keys %{$othr->{child}}) {
		my %children = ();
		foreach my $key (keys %{$othr->{child}{$form}}) {
			my $child = $othr->{child}{$form}{$key};
			$children{$child}{child} = $child;
			push @{$children{$child}{keys}}, $key;
		}
		foreach my $ckey (keys %children) {
			foreach my $key (@{$children{$ckey}{keys}}) {
				if (my $schild = $self->{child}{$form}{$key}) {
					my $ochild = $children{$ckey}{child};
					my $reason = sprintf "Merging %s from %s child %s and %s have same key %s",$self,$othr,$schild,$ochild,Item::showkey($ckey);
					$schild->mergemark($ochild,$merge,$reason);
				}
			}
		}
	}
	return 1;
}
#package Root;
sub them {
	my ($self,@others) = @_;
	my $kind = $self->kind;
	foreach my $othr (@others) {
		foreach my $form (keys %{$othr->{children}}) {
			foreach my $ckey (keys %{$othr->{children}{$form}}) {
				my $child = delete $othr->{children}{$form}{$ckey};
				my $targ = $child->target;
				if (exists $child->{key}{p}{$kind}) {
					foreach my $key (keys %{$child->{key}{p}{$kind}}) {
						delete $othr->{child}{$form}{$key};
						$self->{child}{$form}{$key} = $targ;
						delete $child->{key}{p}{$kind}{$key};
						$targ->{key}{p}{$kind}{$key} = $key;
						delete $self->{members}{$form}{$targ->no};
					}
					delete $child->{key}{p}{$kind} unless $child eq $targ;
				}
				if (my $member = delete $othr->{members}{$form}{$child->no}) {
					unless (exists $targ->{key}{p}{$kind} and scalar values %{$targ->{key}{p}{$kind}}) {
						$self->{members}{$form}{$targ->no} = $targ;
					}
				}
				$self->{children}{$form}{$targ->no} = $targ;
				delete $child->{parent}{$kind};
				$targ->{parent}{$kind} = $self;
			}
			delete $othr->{members}{$form};
			delete $othr->{child}{$form};
			delete $othr->{children}{$form};
		}
	}
}
#package Root;
sub mfix {
	my ($self,$abort) = @_;
	my $kind = $self->kind;
	my $fixed = 0;
	foreach my $form (keys %{$self->{children}}) {
		foreach my $key (keys %{$self->{children}{$form}}) {
			my $member = $self->{children}{$form}{$key};
			if (my $became = $member->{target}) {
				#warn "Root $self: Fixing reference to bad member $member which became $became";
				$self->{children}{$form}{$key} = $became;
				$became->{parent}{$kind} = $self;
				$fixed = 1;
			}
		}
	}
	foreach my $form (keys %{$self->{members}}) {
		foreach my $key (keys %{$self->{members}{$form}}) {
			my $member = $self->{members}{$form}{$key};
			if (my $became = $member->{target}) {
				#warn "Root $self: Fixing reference to bad member $member which became $became";
				$self->{members}{$form}{$key} = $became;
				$became->{parent}{$kind} = $self;
				$fixed = 1;
			}
		}
	}
	foreach my $form (keys %{$self->{child}}) {
		foreach my $key (keys %{$self->{child}{$form}}) {
			my $child = $self->{child}{$form}{$key};
			if (my $became = $child->{target}) {
				#warn "Root $self: Fixing reference to bad child $child which became $became";
				$self->{child}{$form}{$key} = $became;
				$became->{key}{p}{$kind}{$key} = $key;
				$fixed = 1;
			}
		}
	}
	my %children = ();
	foreach my $form (keys %{$self->{members}}) {
		foreach my $key (keys %{$self->{members}{$form}}) {
			my $child = $self->{members}{$form}{$key};
			$children{$form}{$child}{child} = $child;
			$children{$form}{$child}{ents} = {};
			if (exists $child->{key}{p}{$kind}) {
				$children{$form}{$child}{keys} = $child->{key}{p}{$kind};
			} else {
				$children{$form}{$child}{keys} = {};
			}
		}
	}
	foreach my $form (keys %{$self->{child}}) {
		foreach my $key (keys %{$self->{child}{$form}}) {
			my $child = $self->{child}{$form}{$key};
			$children{$form}{$child}{child} = $child;
			$children{$form}{$child}{ents}{$key} = $key;
			if (exists $child->{key}{p}{$kind}) {
				$children{$form}{$child}{keys} = $child->{key}{p}{$kind};
			} else {
				$children{$form}{$child}{keys} = {};
			}
		}
	}
	foreach my $form (keys %children) {
		foreach my $ckey (keys %{$children{$form}}) {
			my $entry = $children{$form}{$ckey};
			my $child = $entry->{child};
			my %ents = %{$entry->{ents}};
			my %keys = %{$entry->{keys}};
			if (scalar values %ents or scalar values %keys and exists $self->{members}{$form}{$child->no}) {
				if ($fixed) {
					#warn "Root $self: Fixing reference to member $child that has keys entries";
				}
			}
			foreach my $ent (keys %ents) {
				next if exists $keys{$ent};
				#warn "Root $self: Fixing child $child missing key ".Item::showkey($ent);
				$child->{key}{p}{$kind}{$ent} = $ent;
				$keys{$ent} = $ent;
				$fixed = 1;
			}
			foreach my $key (keys %keys) {
				next if exists $ents{$key};
				#warn "Root $self: Fixing child $child extra key ".Item::showkey($key);
				if (my $exists = $self->{child}{$form}{$key}) {
					#warn "Root $self: Fixing child $child extra key ".Item::showkey($key)." belongs to $exists";
					delete $child->{key}{p}{$kind}{$key};
				} else {
					#warn "Root $self: Fixing child $child extra key ".Item::showkey($key)." adding parent entry";
					$self->{child}{$form}{$key} = $child;
				}
				$fixed = 1;
			}
			unless (exists $self->{children}{$form}{$child->no}) {
				#warn "Root $self: Fixing missing $child";
				$self->{children}{$form}{$child->no} = $child;
			}
		}
	}
	foreach my $form (keys %{$self->{child}}) {
		foreach my $child (values %{$self->{child}{$form}}) {
			if (exists $self->{children}{$form}{$child->no}) {
				unless ($self->{children}{$form}{$child->no} eq $child) {
					#warn "Root $self: Fixing wrong children setting for $child";
					$self->{children}{$form}{$child->no} = $child;
					$fixed = 1;
				}
			} else {
				#warn "Root $self: Fixing missing children setting for $child";
				$self->{children}{$form}{$child->no} = $child;
				#$fixed = 1;
			}
		}
	}
	foreach my $form (keys %{$self->{members}}) {
		my @children = values %{$self->{members}{$form}};
		foreach my $child (@children) {
			if (exists $child->{key}{p}{$kind} and scalar values %{$child->{key}{p}{$kind}}) {
				#warn "Root $self: Fixing extra member $child that has keys";
				delete $self->{members}{$form}{$child->no};
				#$fixed = 1;
			}
		}
	}
	foreach my $form (keys %{$self->{children}}) {
		foreach my $child (values %{$self->{children}{$form}}) {
			unless (exists $child->{key}{p}{$kind} and scalar values %{$child->{key}{p}{$kind}}) {
				if (exists $self->{members}{$form}{$child->no}) {
					unless ($self->{members}{$form}{$child->no} eq $child) {
						#warn "Root $self: Fixing wrong member setting for $child";
						$self->{members}{$form}{$child->no} = $child;
						$fixed = 1;
					}
				} else {
					#warn "Root $self: Fixing missing member $child";
					$self->{members}{$form}{$child->no} = $child;
					#$fixed = 1;
				}
			}
		}
	}
#	$self->show if $fixed;
	Carp::croak if $fixed and $abort;
}

# ------------------------------------------
package Leaf; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Data::Dumper;
# ------------------------------------------
#package Leaf;
sub addtoparent {
	my $self = shift;
	my ($parent,$key) = @_;
	return unless defined $parent;
#	if ($key) {
#		warn "Adding child $self to parent $parent with key ".Item::showkey($key);
#	} else {
#		warn "Adding member $self to group $parent";
#	}
	Carp::confess "Parent $parent is not a Root" unless $parent->isa('Root');
	my $kind = $parent->kind;
	my $othr = $self->{parent}{$kind};
	$othr = $othr->target if $othr;
	if ($othr and $othr ne $parent) {
		Carp::confess sprintf('%s($s) and %s(%s) object mismatch',$othr,$othr->kind,$parent,$parent->kind)
		unless $othr->isa(ref $parent) or $parent->isa(ref $othr);
		# Whoops.  We have already been added to another parent,
		# that parent and this must be the same so merge them.
		#warn "Adding child: parent merge required: parent $othr instead of $parent at key ".Item::showkey($key);
		my $reason = sprintf "Child %s cannot be %s child for two parents %s and %s",$self,$kind,$parent,$othr;
		$parent->mergefrom($othr,$reason);
		my $became = $self->{parent}{$kind};
		Carp::confess "Adding child: parent merge failed: $became should have become $parent for key ".Item::showkey($key)
		if $became and $became ne $parent;
		# we don't have to add the child to the parent, the merge should have done this
	} else {
		$parent->addchild($self,$key);
	}
}
#package Leaf;
sub parent {
	my ($self,$type) = @_;
	return $self->{parent}{$type->kind()};
}
#package Leaf;
sub parents {
	my $self = shift;
	return values %{$self->{parent}};
}
#package Leaf;
sub delfromparent {
	my $self = shift;
	my ($parent) = @_;
	my $kind = $parent->kind;
	#warn "Deleting $kind children from $parent";
	Carp::confess "parent changed" if ref $parent and $parent ne $self->{parent}{$kind};
	my $key = [ values %{$self->{key}{p}{$kind}} ];
	$parent->delchild($self,$key);
	return $key;
}
#package Leaf;
sub link {
	my $self = shift;
	my ($model,$key,$links) = @_;
	return unless defined $links and ref $links eq 'ARRAY';
	foreach my $link (@$links) {
		next unless defined $link and ref $link eq 'ARRAY';
		my ($type,$pkey,$index,@args) = @$link;
		if (defined $pkey) {
			while (ref($pkey) eq 'REF') { $pkey = $$pkey }
		}
		next unless $type or ref $pkey;
		$type = ref $pkey unless defined $type;
		next unless $type;
		my $kind = $type->kind;
		Carp::confess ref($pkey)." should be $type"
		if ref $pkey and $pkey->kind ne $kind;
		Carp::confess "$type is not a Root"
		unless $type->isa('Root');
		$pkey = $pkey->target if ref $pkey;
		my $oldparent = $self->{parent}{$kind};
		my $newparent = ref $pkey ? $pkey
			: ($oldparent ? $oldparent : $type->get($model,$pkey,@args));
		$newparent->add_key($pkey) if not ref $pkey;
		$self = $self->target;
		$newparent = $newparent->target;
		$self->addtoparent($newparent,$index);
		$newparent = $newparent->target;
		if ($newparent->can('link') and scalar @args) {
			$newparent->link($model,$pkey,@args);
		}
	}
}
#package Leaf;
sub init {
	my $self = shift;
	$self->link(@_);
}
#package Leaf;
sub myckey {
	my $self = shift;
	my $kind = shift;
	my @keys = sort @{$self->{key}{p}{$kind}} if exists $self->{key}{p}{$kind};
	my $key = $keys[0];
	$key = makekey($self->{no}) unless $key or not exists $self->{no};
	return $key;
}
#package Leaf;
sub myckeys {
	my $self = shift;
	my $kind = shift;
	$kind = [ keys %{$self->{key}{p}} ] unless defined $kind;
	$kind = [ $kind ] unless ref $kind eq 'ARRAY';
	my %keys = ();
	foreach my $k (@$kind) { map {$keys{$_}=$_} keys %{$self->{key}{p}{$k}} }
	my @keys = (sort keys %keys);
	return \@keys;
}
#package Leaf;
sub dump {
	my $self = shift;
	my $form = $self->kind;
	foreach my $kind (keys %{$self->{parent}}) {
		my $parent = $self->{parent}{$kind};
		print STDERR "$self has $kind parent $parent with indexes:\n";
		if ($self->{key}{p}{$kind}) {
			foreach my $key (values %{$self->{key}{p}{$kind}}) {
				print STDERR "\t$form ",Item::showkey($key),"\n";
			}
		}
	}
}
#package Leaf;
sub chek {
	my $self = shift;
	foreach my $kind (keys %{$self->{parent}}) {
		if (my $parent = $self->{parent}{$kind}) {
			#$self->show();
			Carp::confess "Merge failed, Leaf $self still has $kind parent $parent";
		}
		delete $self->{parent}{$kind};
	}
	foreach my $kind (keys %{$self->{key}{p}}) {
		foreach my $key (values %{$self->{key}{p}{$kind}}) {
			#$self->show();
			Carp::confess "Merge failed, Leaf $self still has $kind key ".Item::showkey($key);
		}
		delete $self->{key}{p}{$kind};
	}
}
#package Leaf;
sub mark {
	my ($self,$othr,$merge) = @_;
	foreach my $kind (%{$othr->{parent}}) {
		my $sparent = $self->{parent}{$kind};
		my $oparent = $othr->{parent}{$kind};
		if ($oparent and $sparent and $oparent ne $sparent) {
			my $reason = sprintf "Merging %s from %s have different parents %s and %s",$self,$othr,$sparent,$oparent;
			$sparent->mergemark($oparent,$merge,$reason);
		}
	}
	return 1;
}
#package Leaf;
sub them {
	my ($self,@others) = @_;
	my $form = $self->kind;
	foreach my $kind (keys %{$self->{parent}}) {
		my $sparent = $self->{parent}{$kind};
		my $tparent = $sparent->target();
		if ($sparent ne $tparent) {
			$self->{parent}{$kind} = $tparent;
			if (exists $self->{key}{p}{$kind}) {
				foreach my $key (values %{$self->{key}{p}{$kind}}) {
					#warn "Moving $form child $self from $kind parent $sparent to parent $tparent for key ".Item::showkey($key);
					delete $sparent->{child}{$form}{$key};
					$tparent->{child}{$form}{$key} = $self;
				}
			}
			if (delete $sparent->{members}{$form}{$self->no}) {
				#warn "Moving $form member $self from $kind parent $sparent to parent $tparent";
				$tparent->{members}{$form}{$self->no} = $self;
			}
			delete $sparent->{children}{$form}{$self->no};
			$tparent->{children}{$form}{$self->no} = $self;
			$tparent->Root::mfix();
		}
	}
	foreach my $othr (@others) {
		foreach my $kind (keys %{$othr->{parent}}) {
			my $oparent = delete $othr->{parent}{$kind};
			next unless $oparent;
			my $tparent = $oparent->target();
			#warn "Adding $kind parent $tparent to $self" unless exists $self->{parent}{$kind};
			$self->{parent}{$kind} = $tparent;
			if (exists $othr->{key}{p}{$kind}) {
				foreach my $key (values %{$othr->{key}{p}{$kind}}) {
					#warn "Merging $form child $othr of $kind parent $oparent to child $self of parent $tparent for key ".Item::showkey($key);
					delete $oparent->{child}{$form}{$key};
					#warn "Deleting $form child $othr $kind key ".Item::showkey($key);
					delete $othr->{key}{p}{$kind}{$key};
					$self->{key}{p}{$kind}{$key} = $key;
					$tparent->{child}{$form}{$key} = $self;
					delete $tparent->{members}{$form}{$self->no};
				}
				delete $othr->{key}{p}{$kind};
			}
			if (delete $oparent->{members}{$form}{$othr->no}) {
				unless (exists $self->{key}{p}{$kind} and scalar values %{$self->{key}{p}{kind}}) {
					#warn "Merging $form member $othr of $kind parent $oparent to member $self of parent $tparent";
					$tparent->{members}{$form}{$self->no} = $self;
				}
			}
			delete $oparent->{children}{$form}{$othr->no};
			$tparent->{children}{$form}{$self->no} = $self;
			$tparent->Root::mfix();
		}
	}
}

# ------------------------------------------
package Tree; our @ISA = qw(Root Leaf);
use strict; use warnings; use Carp;
# ------------------------------------------

# ------------------------------------------
package Model; our @ISA = qw(Base);
use strict; use warnings; use Carp;
# ------------------------------------------
#package Model;
sub init {
	my $self = shift;
	$self->{numbs} = {};
	$self->{items} = {};
	$self->{lists} = {};
	my $glob = $self->{net}{PUBLIC} = Network->new($self,undef);
	my $priv = $self->{net}{PRIVATE} = Private::Here->new($self,undef,[['Network',$glob,undef]]);
	my $locl = $self->{net}{LOOPBACK} = Local::Here->new($self,undef,[['Private::Here',$priv,undef]]);
}
#package Model;
sub fini {
	my $self = shift;
	delete $self->{numbs};
	delete $self->{items};
	delete $self->{lists};
	delete($self->{net}{LOOPBACK})->put();
	delete($self->{net}{PRIVATE})->put();
	delete($self->{net}{PUBLIC})->put();
}
#package Model;
sub getno {
	my ($self,$kind) = @_;
	return --$self->{numbs}{$kind};
}
#package Model;
sub item {
	my $self = shift;
	my ($kind,$ref) = @_;
	return undef unless defined $ref;
	my $result = undef;
	my @keys = ref $ref eq 'ARRAY' ? @$ref : ( $ref );
	foreach my $key (@keys) {
		#warn "Searching $kind key ".Item::showkey($key);
		if (exists $self->{items}{$kind}{$key}) {
			$result = $self->{items}{$kind}{$key};
			last if $result;
		}
	}
	#warn "Found $result" if $result;
	return $result;
}
#package Model;
sub items {
	my ($self,$kind) = @_;
	$kind = [keys %{$self->{items}}] unless defined $kind;
	$kind = [$kind] unless ref $kind eq 'ARRAY';
	my @items = ();
	foreach (@$kind) {
		push @items, values %{$self->{lists}{$_}} if $self->{lists}{$_};
	}
	return @items;
}
#package Model;
sub showstats {
	my $self = shift;
	print STDERR "Model stats:\n------------\n";
	Carp::confess "$self->{lists} not a HASH" unless ref $self->{lists} eq 'HASH';
	foreach my $kind (sort keys %{$self->{lists}}) {
		next unless $self->{lists}{$kind};
		Carp::confess "$self->{lists}{$kind} not a HASH" unless ref $self->{lists}{$kind} eq 'HASH';
		my $count = scalar(values %{$self->{lists}{$kind}});
		printf STDERR "%6d lists of kind %s\n", $count, $kind;
	}
	print STDERR "------------\n";
	foreach my $kind (sort keys %{$self->{items}}) {
		my $count = scalar(values %{$self->{items}{$kind}});
		printf STDERR "%6d items of kind %s\n", $count, $kind;
	}
	print STDERR "------------\n";
}
#package Model;
our $bread = 0;
#package Model;
sub walktree {
	my $self = shift;
	print STDERR "\nNetwork tree:\n-------------\n";
	my $crumb = $bread++;
	$self->walk($crumb,'','');
	print STDERR "-------------\n\n";
}

# ------------------------------------------
package Model::SNMP; our @ISA = qw(Model);
use strict; use warnings; use Carp; use Data::Dumper;
use threads; use threads::shared; use Thread; use Thread::Queue;
use SNMP; use Net::IP;
# ------------------------------------------
#package Model::SNMP;
our $tables = [
	'systemBulk',
#	'systemScalars',
#	'sysORTable',
	'interfacesBulk',
	'ifMIBObjectsBulk',
#	'ifTable',
	'ipBulk',
#	'ipAddrTable',
#	'ipAddressTable',
#	'ipRouteTable',
	'atBulk',
#	'atTable',
#	'ipNetToPhysicalTable',
	'lldpObjectsBulk',
#	'lldpLocalSystemDataScalars',
#	'lldpConfigurationScalars',
#	'lldpPortConfigTable',
#	'lldpConfigManAddrTable',
#	'lldpStatsTxPortTable',
#	'lldpStatsRxPortTable',
#	'lldpLocPortTable',
#	'lldpLocManAddrTable',
#	'lldpRemTable',
#	'lldpRemManAddrTable',
#	'lldpRemUnknownTLVTable',
#	'lldpRemOrgDefInfoTable',

#	'lldpXdot1ObjectsBulk',
#	'lldpXdot1ConfigPortVlanTable',
#	'lldpXdot1ConfigVlanNameTable',
#	'lldpXdot1ConfigProtoVlanTable',
#	'lldpXdot1ConfigProtocolTable',
#	'lldpXdot1LocTable',
#	'lldpXdot1LocProtoVlanTable',
#	'lldpXdot1LocVlanNameTable',
#	'lldpXdot1LocProtocolTable',
#	'lldpXdot1RemTable',
#	'lldpXdot1RemProtoVlanTable',
#	'lldpXdot1RemVlanNameTable',
#	'lldpXdot1RemProtocolTable',

#	'lldpXdot3ObjectsBulk',
#	'lldpXdot3PortConfigTable',
#	'lldpXdot3LocPortTable',
#	'lldpXdot3LocPowerTable',
#	'lldpXdot3LocLinkAggTable',
#	'lldpXdot3LocMaxFrameSizeTable',
#	'lldpXdot3RemPortTable',
#	'lldpXdot3RemPowerTable',
#	'lldpXdot3RemLinkAggTable',
#	'lldpXdot3RemMaxFrameSizeTable',

#	'lldpXMedObjectsBulk',
#	'lldpXMedLocalDataScalars',
#	'lldpXMedPortConfigTable',
#	'lldpXMedLocMediaPolicyTable',
#	'lldpXMedLocLocationTable',
#	'lldpXMedLocXPoEPSEPortTable',
#	'lldpXMedRemCapabilitiesTable',
#	'lldpXMedRemMediaPolicyTable',
#	'lldpXMedRemInventoryTable',
#	'lldpXMedRemLocationTable',
#	'lldpXMedRemXPoETable',
#	'lldpXMedRemXPoEPSETable',
#	'lldpXMedRemXPoEPDTable',
];
#package Model::SNMP;
sub init {
	my $self = shift;
	$self->launch_thread();
}
#package Model::SNMP;
sub fini {
	my $self = shift;
	delete ($self->{thread}{host})->kill('TERM');
	delete $self->{queue}{host}{request};
	delete $self->{queue}{host}{results};
}
#package Model::SNMP;
sub launch_thread {
	my $self = shift;
	my $rq :shared = Thread::Queue->new; $self->{queue}{host}{request} = $rq;
	my $wq :shared = Thread::Queue->new; $self->{queue}{host}{results} = $wq;
	#warn "Launching thread...";
	#my $thread = Thread->new(\&Model::SNMP::main_thread,$rq,$wq);
	my $thread = threads->create('Model::SNMP::main_thread',$rq,$wq);
	#warn "Detaching thread...";
	$thread->detach();
	$self->{thread}{host} = $thread;
}
#package Model::SNMP;
sub main_thread {
	my ($rq,$wq) = @_;
	while (my $item = $rq->dequeue()) {
		#my $worker = Thread->new(\&Model::SNMP::worker_thread,$rq,$wq,$item);
		my $worker = threads->create('Model::SNMP::worker_thread',$rq,$wq,$item);
		$worker->detach();
	}
}
#package Model::SNMP;
sub worker_thread {
	my ($rq,$wq,$item) = @_;
	my ($session,$error) = Net::SNMP->session(%$item);
	if (defined $session and $session) {
		foreach my $table (@$tables) {
			my ($oid,$result,$error);
			my $key = $table;
			if ($key =~ /Table$/) {
				$oid = sub_dot(SNMP::translateObj($key));
				$result = $session->get_table(-baseoid=>$oid,-maxrepetitions=>4);
				$error = $session->error() unless defined $result;
				unless (defined $result) {
					if ($error =~ /No response/i) { last }
					if ($error =~ /Requested table is empty/i) { next }
				}
				$wq->enqueue([$item,$table,$result,$error]);
			}
			elsif ($key =~ /Scalars$/) {
				$key =~ s/Scalars$//;
				$oid = sub_dot(SNMP::translateObj($key));
				$result = $session->get_bulk_request(-nonrepeaters=>0,-maxrepetitions=>8,-varbindlist=>[$oid]);
				$error = $session->error() unless defined $result;
				unless (defined $result) {
					if ($error =~ /No response/) { last }
				}
				$wq->enqueue([$item,$table,$result,$error]);
			}
			elsif ($key =~ /Bulk$/) {
				$key =~ s/Bulk$//;
				$oid = sub_dot(SNMP::translateObj($key));
				$result = $session->get_table(-baseoid=>$oid,-maxrepetitions=>4);
				$error = $session->error() unless defined $result;
				unless (defined $result) {
					if ($error =~ /No response/) { last }
				}
				$wq->enqueue([$item,$table,$result,$error]);
			}
		}
	} else {
		$wq->enqueue([$item,undef,$error]);
	}
}
#package Model::SNMP;
sub lexical_oids {
	my (@indexes) = @_;
	my %tuples = ();
	foreach my $index (@indexes) {
		$tuples{$index} = pack('S*',split(/\./,$index));
	}
	my @sort = sort {$tuples{$a} cmp $tuples{$b}} keys %tuples;
	return @sort;
}
#package Model::SNMP;
sub lexical_sort {
	my (@tables) = @_;
	my %tuples = ();
	foreach my $table (@tables) {
		my $label = $table;
		if ($label =~ s/Data$//) { }
		elsif ($label =~ s/Scalars$//) { }
		elsif ($label =~ s/Bulk$//) { }
		my $mib = $SNMP::MIB{$label};
		next unless $mib;
		my $oid = $mib->{objectID};
		unless (defined $oid) {
			Carp::cluck "Undefined objectID for label '$label' of table '$table'";
			next;
		}
		$oid =~ s/^\.//;
		my $key = pack('S*',split(/\./,$oid));
		$tuples{$table} = $key;
	}
	my @sort = sort {$tuples{$a} cmp $tuples{$b}} keys %tuples;
	return @sort;
}
#package Model::SNMP;
sub snmp_process {
	my $self = shift;
	my $item = shift;
	my ($snmpargs,$table,$result,$error) = @$item;
	if ($error) {
		warn "Errored result:\nRaw SNMP result:\n",Dumper($item),"\n";
		return;
	}
	my $xlated;
	if ($table =~ /Scalars$/) {
		$xlated = xlate_scalars(@$item);
	}
	elsif ($table =~ /Table$/) {
		$xlated = xlate_table(@$item);
	}
	elsif ($table =~ /Bulk$/) {
		$xlated = xlate_bulk(@$item);
	}
	return unless $xlated;
	my ($hkey,$type,$net) = $self->keytype($snmpargs->{'-hostname'});
	my $name = netname($type);
	if ($net = $self->{net}{$type}) {
		#warn "Found net $name/$type for ".Item::showkey($hkey);
	} else {
		warn "No net $name/$type for ".Item::showkey($hkey);
		return;
	}
	my $host = $net->getchild('Point::Host',$hkey,undef);
	foreach my $group (lexical_sort(keys %$xlated)) {
		my $entries = $group;
		if ($entries =~ s/Scalars$/Data/) {}
		elsif ($entries =~ s/Table$/Entry/) {}

		if ($host->can($group)) {
			warn "Host ".Item::showkey($hkey)." processing $table ($group)";
			$host->$group($self,$snmpargs,$group,$xlated->{$group},$error,$hkey,$type,$net,$name);
		} elsif ($host->can($entries)) {
			warn "Host ".Item::showkey($hkey)." processing $table ($entries)";
			if ($group =~ /Scalars$/) {
				$host->addrow($group,$xlated->{$group}{0});
			} elsif ($group =~ /Table$/) {
				$host->addtab($group,$xlated->{$group});
			}
			foreach my $index (lexical_oids(keys %{$xlated->{$group}})) {
				$host->$entries($index,$xlated->{$group}{$index},$self,$snmpargs,$group,$xlated->{$group},$error,$hkey,$type,$net,$name);
			}
		} else {
			if ($group =~ /Scalars$/) {
				warn "Host ".Item::showkey($hkey)." including $table ($group)";
				$host->addrow($group,$xlated->{$group}{0});
			} elsif ($group =~ /Table$/) {
				warn "Host ".Item::showkey($hkey)." including $table ($group)";
				$host->addtab($group,$xlated->{$group});
			} else {
				warn "Host ".Item::showkey($hkey)." cannot process $table ($group)";
			}
		}
	}
	Viewer->refresh();
	#Model::walktree($host);
}
#package Model::SNMP;
sub snmp_doone {
	my $self = shift;
	my $q = $self->{queue}{host}{results};
	my $result = 0;
	if (my $item = $q->dequeue_nb()) {
		#warn "There are still ",$q->pending()," pending results\n" if $q->pending();
		$self->snmp_process($item);
		$result++;
	}
	return $result;
}
#package Model::SNMP;
sub snmp_result {
	my $self = shift;
	my $q = $self->{queue}{host}{results};
	my $result = 0;
	while (my $item = $q->dequeue_nb()) {
		#warn "There are still ",$q->pending()," pending results\n" if $q->pending();
		$self->snmp_process($item);
		$result++;
	}
	return $result;
}
#package Model::SNMP;
sub snmp_results {
	my $self = shift;
	my $count = 5;
	while (1) {
		$count = 5 if $self->snmp_result();
		sleep 1;
		last unless --$count;
	}
}
#package Model::SNMP;
sub sub_dot {
	my $oid = shift;
	$oid =~ s/^\.+//;
	return $oid;
}
#package Model::SNMP;
sub add_dot {
	my $oid = shift;
	$oid = sub_dot($oid);
	return ".$oid";
}
#package Model::SNMP;
sub xlate_bulk {
	my ($snmpargs,$bulk,$result,$error) = @_;
	if (0) {
		print STDERR "\nBulk $bulk raw result:\n";
		print STDERR Dumper($result),"\n";
		foreach my $key (keys %$result) {
			my $tag = SNMP::translateObj(add_dot($key));
			my ($label,$index) = split(/\./,$tag,2);
			my $entry = '(unknown)';
			if (my $mib = $SNMP::MIB{$label}) {
				if (my $parent = $mib->{parent}) {
					$entry = $parent->{label};
				}
			}
			print STDERR "$key --> $entry\::$label $index\n";
		}
		print STDERR "\n";
	}
	return undef unless defined $result;
	my $indexes = {};
	my $xlate = {};
	foreach my $key (keys %$result) {
		my $val = $result->{$key};
		my $tag = SNMP::translateObj(add_dot($key));
		my ($label,$index) = split(/\./,$tag,2);
		($label,$index) = ('sysUpTime',0) if $label eq 'sysUpTimeInstance';
		Carp::cluck "Couldn't find index in $key, label is $label" unless length($index);
		next unless length($index);
		my $mib = $SNMP::MIB{$label};
		my $entry = $mib->{parent};
		my $table;
		if ($entry->{indexes} and scalar(@{$entry->{indexes}}) > 0) {
			$table = $entry->{parent}{label};
			$indexes->{$table} = [ @{$entry->{indexes}} ] unless exists $indexes->{$table};
		} else {
			$table = $entry->{label}.'Scalars';
			$indexes->{$table} = [] unless exists $indexes->{$table};
		}
		if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
			$val =~ s/^0x//;
			$val = pack('H*',$val);
			$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
		}
		if ($mib->{enums}) {
			foreach my $k (keys %{$mib->{enums}}) {
				$val = "$k($val)" if $mib->{enums}{$k} eq $val;
			}
		}
		if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
			if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
				$val =~ s/://g;
				$val = join('.',unpack('C*',pack('H*',$val)));
			} else {
				while ($val =~ s/00:/:/) { }
				while ($val =~ s/:::+/::/) { }
				$val =~ s/^:([^:])/::$1/;
				$val =~ s/^0+//;
				while ($val =~ s/:0/:/) { }
			}
		}
		if ($mib->{type} =~ /OBJECTID|OPAQUE/) {
			$val = SNMP::translateObj(add_dot($val));
		}
		$xlate->{$table}{$index}{$label} = $val;
	}
	foreach my $table (keys %$xlate) {
		my @indexes = @{$indexes->{$table}};
		foreach my $index (keys %{$xlate->{$table}}) {
			my @inds = split(/\./,$index);
			foreach my $i (@indexes) {
				my $mib = $SNMP::MIB{$i};
				next if exists $xlate->{$table}{$index}{$i};
				if ($mib->{type} =~ /INTEGER|UNSIGNED|COUNTER|GAUGE|TICKS/) {
					my $val = shift @inds;
					if (defined $val) {
						if (my $enums = $SNMP::MIB{$i}{enums}) {
							foreach my $k (keys %$enums) {
								$val = "$k($val)" if $enums->{$k} eq $val;
							}
						}
						$xlate->{$table}{$index}{$i} = $val;
					} else {
						Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
						Carp::cluck "processing index '$index' of table '$table' for instance $i";
						print STDERR "$table raw result was:\n";
						print STDERR Dumper($result),"\n";
						print STDERR "$table translated result is currently:\n";
						print STDERR Dumper($xlate),"\n";
						@inds = ();
						#$xlate->{$table}{$index}{$i} = undef;
					}
				} elsif ($mib->{type} =~ /OCTETSTR/) {
					my @vals = ();
					my $len = shift @inds;
					if (defined $len and $len <= scalar(@inds)) {
						while ($len>0) { push @vals, shift @inds; $len-- } 
						my $val = join(':',map {sprintf('%02x',$_)} @vals);
						if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
							$val =~ s/^0x//;
							$val = pack('H*',$val);
							$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
						}
						if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
							if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
								$val =~ s/://g;
								$val = join('.',unpack('C*',pack('H*',$val)));
							} else {
								while ($val =~ s/00:/:/) { }
								while ($val =~ s/:::+/::/) { }
								$val =~ s/^:([^:])/::$1/;
								$val =~ s/^0+//;
								while ($val =~ s/:0/:/) { }
							}
						}
						$xlate->{$table}{$index}{$i} = $val;
					} else {
						Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
						Carp::cluck "processing index '$index' of table '$table' for instance $i";
						print STDERR "$table raw result was:\n";
						print STDERR Dumper($result),"\n";
						print STDERR "$table translated result is currently:\n";
						print STDERR Dumper($xlate),"\n";
						@inds = ();
						#$xlate->{$table}{$index}{$i} = undef;
					}
				} elsif ($mib->{type} =~ /OBJECTID|OPAQUE/) {
					my @vals = ();
					my $len = shift @inds;
					if (defined $len and $len <= scalar(@inds)) {
						while ($len>0) { push @vals, shift @inds; $len-- }
						my $val = join('.',@vals);
						$val = SNMP::translateObj(add_dot($val));
						$xlate->{$table}{$index}{$i} = join('.',@vals);
					} else {
						Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
						Carp::cluck "processing index '$index' of table '$table' for instance $i";
						print STDERR "$table raw result was:\n";
						print STDERR Dumper($result),"\n";
						print STDERR "$table translated result is currently:\n";
						print STDERR Dumper($xlate),"\n";
						@inds = ();
						#$xlate->{$table}{$index}{$i} = undef;
					}
				} elsif ($mib->{type} =~ /IPADDR|NETADDR/) {
					my @vals = ();
					my $len = 4;
					if ($len <= scalar(@inds)) {
						while ($len>0) { push @vals, shift @inds; $len-- }
						$xlate->{$table}{$index}{$i} = join('.',@vals);
					} else {
						Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
						Carp::cluck "processing index '$index' of table '$table' for instance $i";
						print STDERR "$table raw result was:\n";
						print STDERR Dumper($result),"\n";
						print STDERR "$table translated result is currently:\n";
						print STDERR Dumper($xlate),"\n";
						@inds = ();
						#$xlate->{$table}{$index}{$i} = undef;
					}
				} else {
					Carp::cluck "Unhandled index type for $i is $mib->{type}\n";
				}
			}
		}
	}
	return $xlate;
}
#package Model::SNMP;
sub xlate_table {
	my ($snmpargs,$table,$result,$error) = @_;
	if (0) {
		print STDERR "\nTable $table raw result:\n";
		print STDERR Dumper($result),"\n";
		foreach my $key (keys %$result) {
			my $tag = SNMP::translateObj(add_dot($key));
			my ($label,$index) = split(/\./,$tag,2);
			my $entry = '(unknown)';
			if (my $mib = $SNMP::MIB{$label}) {
				if (my $parent = $mib->{parent}) {
					$entry = $parent->{label};
				}
			}
			print STDERR "$key --> $entry\::$label $index\n";
		}
		print STDERR "\n";
	}
	return undef unless defined $result;
	my $xlate = {};
	foreach my $key (keys %$result) {
		my $val = $result->{$key};
		my $tag = SNMP::translateObj(add_dot($key));
		my ($label,$index) = split(/\./,$tag,2);
		my $mib = $SNMP::MIB{$label};
		if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
			$val =~ s/^0x//;
			$val = pack('H*',$val);
			$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
		}
		if ($mib->{enums}) {
			foreach my $k (keys %{$mib->{enums}}) {
				$val = "$k($val)" if $mib->{enums}{$k} eq $val;
			}
		}
		if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
			if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
				$val =~ s/://g;
				$val = join('.',unpack('C*',pack('H*',$val)));
			} else {
				while ($val =~ s/00:/:/) { }
				while ($val =~ s/:::+/::/) { }
				$val =~ s/^:([^:])/::$1/;
				$val =~ s/^0+//;
				while ($val =~ s/:0/:/) { }
			}
		}
		if ($mib->{type} =~ /OBJECTID|OPAQUE/) {
			$val = SNMP::translateObj(add_dot($val));
		}
		$xlate->{$table}{$index}{$label} = $val;
	}
	my $entry = $SNMP::MIB{$table}{children}[0];
	my @indexes = @{$entry->{indexes}};
	foreach my $index (keys %{$xlate->{$table}}) {
		my @inds = split(/\./,$index);
		foreach my $i (@indexes) {
			my $mib = $SNMP::MIB{$i};
			next if exists $xlate->{$table}{$index}{$i};
			if ($mib->{type} =~ /INTEGER|UNSIGNED|COUNTER|GAUGE|TICKS/) {
				my $val = shift @inds;
				if (defined $val) {
					if (my $enums = $SNMP::MIB{$i}{enums}) {
						foreach my $k (keys %$enums) {
							$val = "$k($val)" if $enums->{$k} eq $val;
						}
					}
					$xlate->{$table}{$index}{$i} = $val;
				} else {
					Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
					Carp::cluck "processing index '$index' of table '$table' for instance $i";
					print STDERR "$table raw result was:\n";
					print STDERR Dumper($result),"\n";
					print STDERR "$table translated result is currently:\n";
					print STDERR Dumper($xlate),"\n";
					@inds = ();
					$xlate->{$table}{$index}{$i} = undef;
				}
			} elsif ($mib->{type} =~ /OCTETSTR/) {
				my @vals = ();
				my $len = shift @inds;
				if (defined $len and $len <= scalar(@inds)) {
					while ($len>0) { push @vals, shift @inds; $len-- } 
					my $val = join(':',map {sprintf('%02x',$_)} @vals);
					if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
						$val =~ s/^0x//;
						$val = pack('H*',$val);
						$val = join(':',map {sprintf('%02x',$_)} unpack('C*',$val));
					}
					if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
						if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
							$val =~ s/://g;
							$val = join('.',unpack('C*',pack('H*',$val)));
						} else {
							while ($val =~ s/00:/:/) { }
							while ($val =~ s/:::+/::/) { }
							$val =~ s/^:([^:])/::$1/;
							$val =~ s/^0+//;
							while ($val =~ s/:0/:/) { }
						}
					}
					$xlate->{$table}{$index}{$i} = $val;
				} else {
					Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
					Carp::cluck "processing index '$index' of table '$table' for instance $i";
					print STDERR "$table raw result was:\n";
					print STDERR Dumper($result),"\n";
					print STDERR "$table translated result is currently:\n";
					print STDERR Dumper($xlate),"\n";
					@inds = ();
					$xlate->{$table}{$index}{$i} = undef;
				}
			} elsif ($mib->{type} =~ /OBJECTID|OPAQUE/) {
				my @vals = ();
				my $len = shift @inds;
				if (defined $len and $len <= scalar(@inds)) {
					while ($len>0) { push @vals, shift @inds; $len-- }
					my $val = join('.',@vals);
					$val = SNMP::translateObj(add_dot($val));
					$xlate->{$table}{$index}{$i} = join('.',@vals);
				} else {
					Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
					Carp::cluck "processing index '$index' of table '$table' for instance $i";
					print STDERR "$table raw result was:\n";
					print STDERR Dumper($result),"\n";
					print STDERR "$table translated result is currently:\n";
					print STDERR Dumper($xlate),"\n";
					@inds = ();
					$xlate->{$table}{$index}{$i} = undef;
				}
			} elsif ($mib->{type} =~ /IPADDR|NETADDR/) {
				my @vals = ();
				my $len = 4;
				if ($len <= scalar(@inds)) {
					while ($len>0) { push @vals, shift @inds; $len-- }
					$xlate->{$table}{$index}{$i} = join('.',@vals);
				} else {
					Carp::carp "Ran out of indexes in [",join('.',@inds),"] looking for ",$mib->{type};
					Carp::cluck "processing index '$index' of table '$table' for instance $i";
					print STDERR "$table raw result was:\n";
					print STDERR Dumper($result),"\n";
					print STDERR "$table translated result is currently:\n";
					print STDERR Dumper($xlate),"\n";
					@inds = ();
					$xlate->{$table}{$index}{$i} = undef;
				}
			} else {
				Carp::cluck "Unhandled index type for $i is $mib->{type}\n";
			}
		}
	}
	return $xlate;
}
#package Model::SNMP;
sub xlate_scalars {
	my ($snmpargs,$scalars,$result,$error) = @_;
	if (0) {
		print STDERR "\nScalars $scalars raw result:\n";
		print STDERR Dumper($result),"\n";
		foreach my $key (keys %$result) {
			my $tag = SNMP::translateObj(add_dot($key));
			my ($label,$index) = split(/\./,$tag,2);
			my $entry = '(unknown)';
			if (my $mib = $SNMP::MIB{$label}) {
				if (my $parent = $mib->{parent}) {
					$entry = $parent->{label};
				}
			}
			print STDERR "$key --> $entry\::$label $index\n";
		}
		print STDERR "\n";
	}
	return undef unless defined $result;
	my $xlate = {};
	foreach my $key (keys %$result) {
		my $val = $result->{$key};
		my $tag = SNMP::translateObj(add_dot($key));
		my ($label,$index) = split(/\./,$tag,2);
		($label,$index) = ('sysUpTime',0) if $label eq 'sysUpTimeInstance';
		my $mib = $SNMP::MIB{$label};
		if ($val =~ /^0x([0-9a-fA-F]{2})+$/) {
			$val =~ s/^0x//;
			$val = pack('H*',$val);
			$val = join('.',map {sprintf('%02x',$_)} unpack('C*',$val));
		}
		if ($mib->{enums}) {
			foreach my $k (keys %{$mib->{enums}}) {
				$val = "$k($val)" if $mib->{enums}{$k} eq $val;
			}
		}
		if ($mib->{syntax} eq 'InetAddress' and $val =~ /:/) {
			if ($val =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
				$val =~ s/://g;
				$val = join('.',unpack('C*',pack('H*',$val)));
			} else {
				while ($val =~ s/00:/:/) { }
				while ($val =~ s/:::+/::/) { }
				$val =~ s/^:([^:])/::$1/;
				$val =~ s/^0+//;
				while ($val =~ s/:0/:/) { }
			}
		}
		if ($mib->{type} =~ /OBJECTID|OPAQUE/) {
			$val = SNMP::translateObj(add_dot($val));
		}
		$xlate->{$scalars}{$index}{$label} = $val;
	}
	return $xlate;
}
#package Model::SNMP;
sub netname {
	my $type = shift;
	my %hash = (
		PUBLIC=>'Network',
		PRIVATE=>'Private',
		LOOPBACK=>'Local',
		RESERVED=>'Network',
	);
	return $hash{$type};
}
#package Model::SNMP;
sub typeok {
	my ($type,$ipt) = @_;
	my %hash = (
		PUBLIC=>{PUBLIC=>1,PRIVATE=>0,LOOPBACK=>0,RESERVED=>0},
		PRIVATE=>{PUBLIC=>1,PRIVATE=>1,LOOPBACK=>0,RESERVERD=>0},
		LOOPBACK=>{PUBLIC=>1,PRIVATE=>1,LOOPBACK=>1,RESERVED=>0},
		RESERVED=>{PUBLIC=>0,PRIVATE=>0,LOOPBACK=>0,RESERVED=>0},
	);
	return $hash{$type}{$ipt};
}
#package Model::SNMP;
sub iptype {
	my $key = shift;
	my $hnip = Item::showkey($key);
	return 'PUBLIC' if $hnip =~ /\/0$/;
	my $ip = new Net::IP($hnip) or Carp::confess "can't decode key '$hnip'";
	#print STDERR "SNMP host IP  :",$ip->ip,"\n";
	#print STDERR "SNMP host Ver :",$ip->version,"\n";
	#print STDERR "SNMP host Sho :",$ip->short,"\n";
	#print STDERR "SNMP host Bin :",$ip->binip,"\n";
	#print STDERR "SNMP host Int :",$ip->intip,"\n";
	#print STDERR "SNMP host Hex :",$ip->hexip,"\n";
	#print STDERR "SNMP host Mask:",$ip->mask,"\n";
	#print STDERR "SNMP host Last:",$ip->last_ip,"\n";
	#print STDERR "SNMP host Pfx :",$ip->prefix,"\n";
	#print STDERR "SNMP host Len :",$ip->prefixlen,"\n";
	#print STDERR "SNMP host Size:",$ip->size,"\n";
	#print STDERR "SNMP host Type:",$ip->iptype,"\n";
	#print STDERR "SNMP host Rev :",$ip->reverse_ip,"\n";
	my $type = $ip->iptype; $type = 'LOOPBACK' if $type eq 'PRIVATE' and $hnip =~ /^127\./;
	# LOOPBACK, PRIVATE, RESERVED, PUBLIC for IPv4
	return $type;
}
#package Model::SNMP;
sub keytype {
	my ($self,$host) = @_;
	my $key = Item::makekey($host);
	return $key,iptype($key);
}
#package Model::SNMP;
our %already_queried = ();
#package Model::SNMP;
sub query {
	my $self = shift;
	my ($ipa) = @_;
	if (exists $already_queried{$ipa}) {
		Carp::confess "Already queried ",Item::showkey($ipa);
	}
	$already_queried{$ipa} = Item::showkey($ipa);
	my $snmpargs = {
		-hostname=>Item::showkey($ipa),
		-version=>'snmpv2c',
		-community=>'public',
	};
	my $q = $self->{queue}{host}{request};
	$q->enqueue($snmpargs);
	#warn "There are now ",$q->pending()," pending requests";
}
#package Model::SNMP;

# ------------------------------------------
package Subnetwork; our @ISA = qw(Point Root);
use strict; use warnings; use Carp;
# ------------------------------------------
#package Subnetwork;
sub subnetbyip {
	my ($self,$ipa) = @_;
	if ($ipa) {
		my $kind = $self->kind;
		#print STDERR "Lookup up subnet of $self for IP ",Item::showkey($ipa),"\n";
		foreach my $subnet ($self->children('Subnet')) {
			#print STDERR "Checking subnet $subnet\n";
			foreach my $key (sort keys %{$subnet->{key}{p}{$kind}}) {
				next unless length($key) == 5;
				#print STDERR "Checking prefix ",Item::showkey($key),"\n";
				my ($addr,$len) = unpack('NC',$key);
				next if $len == 0 or $addr == 0;
				my $mask = 0xffffffff << (32-$len);
				my $kasm = ~$mask;
				my $beg = pack('N',$addr & $mask);
				my $end = pack('N',$addr | $kasm);
				#print STDERR "Beg address is ",Item::showkey($beg),"\n";
				#print STDERR "End address is ",Item::showkey($end),"\n";
				return ($subnet,$key) if $beg le $ipa and $ipa le $end;
				#print STDERR "IP ",Item::showkey($ipa)," not in range from ",Item::showkey($beg)," to ",Item::showkey($end),"\n";
			}
		}
	}
	return (undef,undef);
}

# ------------------------------------------
package Network; our @ISA = qw(Subnetwork);
use strict; use warnings; use Carp;
# ------------------------------------------
# There is only really one network object.  It is the collection of the single
# global network plus all the private and local subnetworks.  This is the root
# of all other objects in the network.
# ------------------------------------------
#package Network;
sub addchild {
	my $self = shift;
	my ($child,$key,$result) = @_;
	$result = $self->SUPER::addchild(@_);
	if ($result and $key and length($key) == 4 and Model::SNMP::iptype($key) ne 'RESERVED' and $child->kind eq 'Point') {
		my $host = $self->child('Point',$key);
		unless ($host and $host->{queried}) {
			$self->{model}->query($key);
		}
	}
	return $result;
}

# -------------------------------------
package Private; our @ISA = qw(Subnetwork Leaf);
use strict; use warnings; use Carp;
# -------------------------------------
#package Private;
sub them {
	my ($self,@others) = @_;
	$self->{model}{net}{PRIVATE} = $self->{model}{net}{PRIVATE}->target();
}
# -------------------------------------
package Private::Here; our @ISA = qw(Private);
use strict; use warnings; use Carp;
# -------------------------------------
#package Private::Here;
sub addchild {
	my $self = shift;
	my ($child,$key,$result) = @_;
	$result = $self->SUPER::addchild(@_);
	if ($result and $key and length($key) == 4 and Model::SNMP::iptype($key) ne 'RESERVED' and $child->kind eq 'Point') {
		my $host = $self->child('Point',$key);
		unless ($host and $host->{queried}) {
			$self->{model}->query($key);
		}
	}
	return $result;
}

# -------------------------------------
package Local; our @ISA = qw(Subnetwork Leaf);
use strict; use warnings; use Carp;
# -------------------------------------
#package Local;
sub them {
	my ($self,@others) = @_;
	$self->{model}{net}{LOOPBACK} = $self->{model}{net}{LOOPBACK}->target();
}
# -------------------------------------
package Local::Here; our @ISA = qw(Local);
use strict; use warnings; use Carp;
# -------------------------------------
#package Local::Here;
sub addchild {
	my $self = shift;
	my ($child,$key,$result) = @_;
	$result = $self->SUPER::addchild(@_);
	if ($result and $key and length($key) == 4 and Model::SNMP::iptype($key) ne 'RESERVED' and $child->kind eq 'Point') {
		my $host = $self->child('Point',$key);
		unless ($host and $host->{queried}) {
			$self->{model}->query($key);
		}
	}
	return $result;
}

# -------------------------------------
package Subnet; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------

# -------------------------------------
package Datum; our @ISA = qw(Base);
use strict; use warnings; use Carp;
use Data::Dumper;
# -------------------------------------
#package Datum;
sub init {
	my $self = shift;
	$self->{data} = {} unless exists $self->{data};
}
#package Datum;
sub fini {
	my $self = shift;
	delete $self->{data};
}
#package Datum;
sub dump {
	my $self = shift;
	print STDERR Dumper($self->{data});
}
#package Datum;
sub them {
	my ($self,@others) = @_;
	foreach my $othr (@others) {
		foreach my $table (keys %{$othr->{data}}) {
			foreach my $col (keys %{$othr->{data}{$table}}) {
				$self->{data}{$table}{$col} = $othr->{data}{$table}{$col};
			}
		}
	}
}
#package Datum;
sub addrow {
	my $self = shift;
	my ($table,$row) = @_;
	if ($table =~ /Table$/) {
		$table =~ s/Table$/Entry/;
	}
	elsif ($table =~ /Scalars$/) {
		$table =~ s/Scalars$/Data/;
	}
	$self->{data}{$table} = $row;
}
#package Datum;
sub addrow_to_table {
	my $self = shift;
	my ($table,$index,$row) = @_;
	$self->{data}{$table}{$index} = $row;
}
#package Datum;
sub addtab {
	my $self = shift;
	my ($table,$tab) = @_;
	$self->{data}{$table} = $tab;
}

# ------------------------------------------
package Item; our @ISA = qw(Base);
use strict; use warnings; use Carp;
# ------------------------------------------
#package Item;
sub makekey {
	my $key = shift;
	unless (defined $key) {
		Carp::cluck "passed null key";
		#return '';
		return undef;
	}
	my ($made,@parts) = ($key);
	if ($key =~ /^\.{3}\/?$/) {
		$made = undef;
	} elsif ($key =~ /^-?\d+$/) {
		$made = pack('s',$key);
	} elsif ($key =~ /^0x([0-9a-fA-F]{2})+$/) {
		$key =~ s/^0x//;
		$made = pack('H*',$key);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*$/) {
		$key =~ s/://g;
		$made = pack('H*',$key);
	} elsif ($key =~ /^\d+(\.\d+){3}$/) {
		@parts = split(/\./,$key);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$made = pack('C*',@parts);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){3}$/) {
		$key =~ s/://g;
		$made = pack('H*',$key);
	} elsif ($key =~ /^\d+(\.\d+){0,3}\/\d+$/) {
		my ($pfx,$len) = split(/\//,$key);
		@parts = split(/\./,$pfx);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$made = pack('C*',@parts,$len);
	} elsif ($key =~ /^\d+(\.\d+){0,3}\/\d+(\.\d+){1,3}$/) {
		my ($add,$msk) = split(/\//,$key);
		@parts = split(/\./,$add);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$add = pack('C*',@parts);
		$add = unpack('N',$add);
		@parts = split(/\./,$msk);
		for (my $i=0;$i<4;$i++) { $parts[$i] = 0 unless $parts[$i] }
		$msk = pack('C*',@parts);
		$msk = unpack('N',$msk);
		my $pfx = $add & $msk;
		my $len = 32;
		while (($msk & 1) == 0 and $len > 0) { $len--; $msk >>= 1 }
		$made = pack('NC',$pfx,$len);
	} elsif ($key =~ /^[0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*[(]\d+[)]$/) {
		my ($hwa,$vlan) = split(/[()]/,$key);
		$hwa =~ s/://g;
		$made = pack('H*',$hwa).pack('n',$vlan);
	} elsif ($key eq "\0\0\0\0\0\0") {
		$made = undef;
	}
	return $made;
}
#package Item;
sub showkey {
	my $key = shift;
	unless (defined $key) {
		#cluck "passed null key";
		return '(null)';
	}
	my ($show,@parts);
	if ($key =~ /^-?\d+$/) {
		$show = $key;
	} elsif (length($key) == 2) { # item number
		$show = unpack('s',$key);
	} elsif (length($key) == 4) { # IP address
		@parts = unpack('C*',$key);
		$show = join('.',@parts);
	} elsif (length($key) == 5) { # IP prefix
		@parts = unpack('C*',$key);
		$show = join('/',join('.',@parts[0..3]),$parts[4]);
	} elsif (length($key) == 6) { # HW address
		@parts = unpack('C*',$key);
		$show = join(':',map {sprintf('%02x',$_)} @parts);
	} elsif (length($key) == 8) { # HW address + VLAN
		@parts = unpack('C6n',$key);
		$show = join(':',map {sprintf('%02x',$_)} @parts[0..5])."($parts[6])";
	} elsif (length($key) == 0) {
		$show = '';
	} else {
		@parts = unpack('C*',$key);
		$show = "0x".join('',map {sprintf('%02x',$_)} @parts);
	}
	return $show;
}
#package Item;
sub kind {
	my $type = shift;
	$type = ref $type if ref $type;
	$type =~ s/::.*//;
	return $type;
}
#package Item;
sub mykey {
	my $self = shift;
	my $key = $self->{key}{n}[0];
	$key = makekey($self->{no}) unless $key or not exists $self->{no};
	return $key;
}
#package Item;
sub mykeys {
	my $self = shift;
	return [] unless exists $self->{key}{n};
	return $self->{key}{n};
}
#package Item;
sub allkeys {
	my $self = shift;
	my @keys = ();
	my $key = $self->key(@_);
	push @keys, $key if defined $key;
	return \@keys;
}
#package Item;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($model) = @_;
	$self->{views} = [];
	$self->{model} = $model;
	$self->{key}{n} = $type->allkeys(@_);
	my @keys = @{$self->{key}{n}};
	if (@keys) {
		#warn "Creating item $self with key ".Item::showkey($keys[0]);
	} else {
		#warn "Creating item $self with no external key.";
	}
	my $kind = $self->kind;
	$self->{no} = $model->getno($kind);
	push @keys, Item::makekey($self->{no});
	foreach my $key (@keys) {
		$model->{items}{$kind}{$key} = $self;
	}
	$model->{lists}{$kind}{0-$self->{no}} = $self;
	$self->{new} = 1;
}
#package Item;
sub identify {
	my $self = shift;
	my @keys = ();
	push @keys,map {Item::showkey($_)} sort @{$self->{key}{n}} if $self->{key}{n};
	if ($self->{no}) {
		unshift @keys,$self->{no};
	} elsif ($self->{bad}) {
		unshift @keys,'[bad]';
	} else {
		unshift @keys,'[null]';
	}
	return $self->kind.'('.join(',',@keys).')';
}
#package Item;
sub dump {
	my $self = shift;
	print STDERR "$self has identity ",$self->Item::identify(),"\n";
	print STDERR "$self has number ",$self->no,"\n";
	my @keys = (); @keys = @{$self->{key}{n}} if $self->{key}{n};
	my $keys = ''; $keys = "\n\tkey ".join("\n\tkey ",map {Item::showkey($_)} sort @keys) if @keys;
	print STDERR "$self has keys:",$keys,"\n";
}
#package Item;
sub find {
	my $type = shift;
	my ($model) = @_;
	my $self = $model->item($type->kind,$type->allkeys(@_));
	delete $self->{new} if $self and exists $self->{new};
	return $self;
}
#package Item;
sub fini {
	my $self = shift;
	my $model = $self->{model};
	my $kind = $self->kind;
	my $keys = $self->{key}{n};
	push @$keys, Item::makekey($self->{no});
	foreach my $key (@$keys) {
		delete $model->{items}{$kind}{$key};
	}
	delete $model->{lists}{$kind}{0-$self->{no}};
	Viewer->removed($self);
	delete $self->{key}{n};
	delete $self->{no};
	delete $self->{views};
	$self->{bad} = 1;
}
#package Item;
sub add_key {
	my $self = shift;
	my ($key) = @_;
	return unless $key;
	my @keys = ref $key eq 'ARRAY' ? @$key : ($key);
	my %toadd = ();
	foreach $key (@keys) {
		next if ref $key;
		next unless $key;
		$toadd{$key} = 1;
	}
	foreach (@{$self->{key}{n}}) {
		delete $toadd{$_} if exists $toadd{$_};
	}
	@keys = keys %toadd;
	my $kind = $self->kind;
	foreach $key (@keys) {
		my $othr = $self->{model}->item($kind,$key);
		if (defined $othr) {
			next if $othr eq $self;
			my $reason = sprintf "Key '%s' added to %s belongs to %s",Item::showkey($key),$self,$othr;
			$self->mergefrom($othr,$reason);
		} else {
			push @{$self->{key}{n}}, $key;
			$self->{model}{items}{$kind}{$key} = $self;
		}
	}
}
#package Item;
sub mergechek {
	my $self = shift;
	my $type = ref $self;
	$self->_revs($type,undef,'chek',@_);
}
#package Item;
sub mergemark {
	my $self = shift;
	my $type = ref $self;
	my $want = 1;
	$self->_fwcd($type,undef,'mark',$want,@_);
}
#package Item;
sub mergethem {
	my $self = shift;
	my $type = ref $self;
	$self->_forw($type,undef,'them',@_);
}
#package Item;
sub mergefix {
	my $self = shift;
	my $type = ref $self;
	$self->_forw($type,undef,'mfix',@_);
}
#package Item;
sub showlist {
	my ($self,$merge) = @_;
	my %processed = ();
	print STDERR "\nTargets to process:\n-------------------\n";
	foreach (@{$merge->{targets}}) {
		my $kind = $_->kind;
		my $no = $_->no;
		my $list = $merge->{$kind}{$no};
		next if exists $processed{$list};
		$processed{$list} = $list;
		print STDERR '[',join(',',@$list),']',"\n";
	}
	print STDERR "-------------------\n";
}
#package Item;
sub mergefrom {
	my ($self,$othr,$reason) = @_;
	my @list = ();
	# Pass #1: mark for merge
	{
		my %merge = ( targets=>[] );
		$self->mergemark($othr,\%merge,$reason);
		#$self->showlist(\%merge);
		#print STDERR "\n";
		my %processed = ();
		foreach my $target (@{$merge{targets}}) {
			my $kind = $target->kind;
			my $no = $target->no;
			my $list = $merge{$kind}{$no};
			next if exists $processed{$list};
			push @list, $list;
			$processed{$list} = $list;
		}
	}
	# Pass #2: test the merge
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		foreach (@others) { $_->mergefix(1) }
	}
	# Pass #3: perform the merge
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		foreach (@others) { $_->{target} = $targ }
	}
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		Carp::confess "Target $targ is bad" if exists $targ->{target};
		$targ->mergethem(@others);
	}
	# Pass #4: fix internal state
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		$targ->mergefix();
	}
	# Pass #5: check the merge
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		foreach (@others) { $_->mergechek() }
	}
	# Pass #6: release merged objects
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		foreach (@others) { $_->put() }
	}
	# Pass #7: notify of merge
	foreach my $list (@list) {
		my ($targ,@others) = @$list;
		Viewer->merged($targ);
	}
}
#package Item;
sub target {
	my $self = shift;
	while (exists $self->{target}) { $self = $self->{target} }
	return $self;
}
#package Item;
sub chek {
	my $self = shift;
	if (exists $self->{key}{n}) {
		foreach (@{$self->{key}{n}}) {
			#$self->show;
			Carp::confess "Item $self still has model key ".Item::showkey($_);
		}
	}
}
#package Item;
sub no {
	my $self = shift;
	Carp::confess "Requesting number for deleted object $self" if $self->{bad} or not exists $self->{no};
	return $self->{no};
}
#package Item;
sub mark {
	my ($self,$othr,$merge) = @_;
	Carp::confess "This object $othr is not a reference" unless ref $othr;
	Carp::confess "That object $self is not a reference" unless ref $self;
	Carp::confess "This object $othr is bad" if $othr->{bad};
	Carp::confess "That object $self is bad" if $self->{bad};
	Carp::confess "Objects $othr and $self are not of same kind" if $othr->kind ne $self->kind;
	my $kind = $self->kind;
	my $sno = $self->no;
	my $ono = $othr->no;
	return 0 if $self eq $othr;
	return 0 if exists $merge->{$kind}{$sno} and exists $merge->{$kind}{$ono} and $merge->{$kind}{$sno} eq $merge->{$kind}{$ono};
	#warn "Marking merge of $othr into $self";
	my @smerge = exists $merge->{$kind}{$sno} ? @{$merge->{$kind}{$sno}} : ($self);
	my @omerge = exists $merge->{$kind}{$ono} ? @{$merge->{$kind}{$ono}} : ($othr);
	my $tmerge = ();
	push @$tmerge, @smerge;
	push @$tmerge, @omerge;
	foreach (@$tmerge) { $merge->{$_->kind}{$_->no} = $tmerge }
	push @{$merge->{targets}}, $self;
	return 1;
}
#package Item;
sub them {
	my ($self,@others) = @_;
	#warn "Merging ".join(',',@others)." into $self";
	my $model = $self->{model};
	my $s_kind = $self->kind;
	foreach my $othr (@others) {
		if ($othr->isa(ref $self) and not $self->isa(ref $othr)) {
			my $type = ref($othr);
			$type->xform($self);
		}
		my $o_kind = $othr->kind;
		while (my $key = shift @{$othr->{key}{n}}) {
			next if $key eq Item::makekey($othr->no);
			#warn "Merging $othr key ".Item::showkey($key)." to $self";
			push @{$self->{key}{n}}, $key;
			delete $model->{items}{$o_kind}{$key};
			$model->{items}{$s_kind}{$key} = $self;
		}
	}
}
#package Item;
sub xform {
	my ($type,$self) = @_;
	my $oldtype = ref $self;
	bless $self,$type;
	Viewer->xformed($self);
}

# ------------------------------------------
package Point; our @ISA = qw(Item);
use strict; use warnings; use Carp;
# ------------------------------------------
#package Point;
sub key {
	my $type = shift; $type = ref $type if ref $type;
	my ($model,$item) = @_;
	$item = $item->[0] if ref $item eq 'ARRAY';
	$item = $item->mykey() if ref $item;
	$item = Item::makekey($item) if $item;
	return $item;
}
#package Point;
sub allkeys {
	my $type = shift; $type = ref $type if ref $type;
	my ($model,$item) = @_;
	my @keys = ref $item eq 'ARRAY' ? @$item : ($item);
	my @rslt = ();
	foreach $item (@keys) {
		Carp::confess "XXXXXX wrong item $item"
		if ref $item eq 'ARRAY' or ref $item eq 'HASH' or ref $item eq 'SCALAR';
		$item = $item->mykey() if ref $item;
		$item = Item::makekey($item) if $item;
		push @rslt, $item if $item;
	}
	return \@rslt;
}
#package Point;
sub init {
	my $self= shift;
	$self->{end} = { a => {}, b => {} };
}
#package Point;
sub fini {
	my $self = shift;
#	foreach my $side (values %{$self->{end}}) {
#		foreach my $kind (values %$side) {
#			foreach my $edge (values %$kind) {
#				$edge->put(@_);
#			}
#		}
#	}
	delete $self->{end};
}
#package Point;
sub mypkey {
	my $self = shift;
	my $kind = shift;
}
#package Point;
sub mypkeys {
	my $self = shift;
}
#package Point;
sub dump {
	my $self = shift;
	print STDERR "$self attaches to the following paths:\n";
	foreach my $side (qw/a b/) {
		foreach my $kind (keys %{$self->{end}{$side}}) {
			foreach my $key (keys %{$self->{end}{$side}{$kind}}) {
				my $path = $self->{end}{$side}{$kind}{$key};
				print STDERR "\t$path side $side kind $kind with key ",Item::showkey($key),"\n";
			}
		}
	}
}
#package Point;
sub chek {
	my $self = shift;
	foreach my $side (qw{a b}) {
		foreach my $kind (keys %{$self->{end}{$side}}) {
			foreach my $obj (values %{$self->{end}{$side}{$kind}}) {
				#$self->show();
				Carp::confess "Point $self still attached to Path $obj";
			}
		}
	}
}
#package Point;
sub mark {
	my ($self,$othr,$merge) = @_;
	# basically if other ends of paths from merged objects are to the same object or objects
	# that are to be merged together, then the path must be merged as well.
	foreach my $oside (qw/a b/) {
		my $oopos = ($oside eq 'a' ? 'b' : 'a');
		#warn "Checking $othr side $oside";
		foreach my $kind (keys %{$othr->{end}{$oside}}) {
			#warn "Checking $othr side $oside kind $kind";
			foreach my $okey (keys %{$othr->{end}{$oside}{$kind}}) {
				my $opath = $othr->{end}{$oside}{$kind}{$okey};
				#warn "Checking $othr side $oside kind $kind path $opath";
				foreach my $sside (qw/a b/) {
					my $sopos = (($sside eq 'a') ? 'b' : 'a');
					#warn "Checking $othr side $oside kind $kind path $opath side $sside";
					foreach my $skey (keys %{$self->{end}{$sside}{$kind}}) {
						my $spath = $self->{end}{$sside}{$kind}{$skey};
						#warn "Checking $othr side $oside kind $kind path $opath side $sside path $spath";
						my $otrg = $opath->obj($oopos)->target();
						my $strg = $spath->obj($sopos)->target();
						unless ($otrg eq $strg) {
							my $reason = sprintf "Merging %s from %s have different ends %s and %s with same key %s",$self,$othr,$strg,$otrg,Item::showkey($skey);
							$spath->mergemark($opath,$merge,$reason);
						}
					}
				}
			}
		}
	}
	return 1;
}
#package Point;
sub them {
	my ($self,@others) = @_;
	my %sides = ( a=>'b', b=>'a' );
	foreach my $othr (@others) {
		foreach my $oside (qw/a b/) {
			my $oopos = ($oside eq 'a' ? 'b' : 'a');
			foreach my $kind (keys %{$othr->{end}{$oside}}) {
				foreach my $opath (values %{$othr->{end}{$oside}{$kind}}) {
					my $otarg = $opath->target();
					my $objl = $opath->{obj}{$oside};
					Carp::confess "Object $objl should be $othr or $self" unless $objl eq $othr or $objl eq $self;
					my $objr = $opath->{obj}{$oopos};
					#warn "Deleting $objl reference to Path $opath side $oside kind $kind with key ".Item::showkey($objr->no);
					delete $objl->{end}{$oside}{$kind}{$objr->no};
					#warn "Deleting $objr reference to Path $opath side $oopos kind $kind with key ".Item::showkey($objl->no);
					delete $objr->{end}{$oopos}{$kind}{$objl->no};
					my $trgl = $objl->target();
					Carp::confess "Object $trgl should be $self" unless $trgl eq $self;
					my $trgr = $objr->target();
					#warn "Adding $trgl reference to Path $otarg side $oside kind $kind with key ".Item::showkey($trgr->no);
					$trgl->{end}{$oside}{$kind}{$trgr->no} = $otarg;
					#warn "Adding $trgr reference to Path $otarg side $oopos kind $kind with key ".Item::showkey($trgl->no);
					$trgr->{end}{$oopos}{$kind}{$trgl->no} = $otarg;
					$otarg->{obj}{$oside} = $trgl;
					$otarg->{obj}{$oopos} = $trgr;
				}
			}
		}
	}

}
#package Point;
sub mfix {
	my ($self,$abort) = @_;
	my $fixed = 0;
	foreach my $side (qw/a b/) {
		foreach my $kind (keys %{$self->{end}{$side}}) {
			foreach my $key (keys %{$self->{end}{$side}{$kind}}) {
				my $edge = $self->{end}{$side}{$kind}{$key};
				if (my $became = $edge->{target}) {
					#warn "Point $self: Fixing reference to bad edge $edge which became $became";
					delete $self->{end}{$side}{$kind}{$key};
					$self->{end}{$side}{$kind}{$became->objo($side)->no} = $became;
					$fixed = 1;
					$edge = $became;
				}
				unless ($edge->obj($side) eq $self) {
					#warn "Path $edge: Fixing reference to $side object";
					$edge->{obj}{$side} = $self;
					$fixed = 1;
				}
			}
		}
	}
#	$self->show if $fixed;
	Carp::croak if $fixed and $abort;
}
#package Point
sub tree {
	my ($self,$crumb,$indent,$suffix) = @_;
	return if exists $self->{crumb} and $self->{crumb} == $crumb;
	foreach my $side (qw/a b/) {
		foreach my $kind (keys %{$self->{end}{$side}}) {
			foreach my $key (keys %{$self->{end}{$side}{$kind}}) {
				my $edge = $self->{end}{$side}{$kind}{$key};
				my $suffix;
				if ($side eq 'a') {
					$suffix = " to object $edge->{obj}{b}";
				} else {
					$suffix = " from object $edge->{obj}{a}";
				}
				$edge->walk($crumb,$indent."\t==>",$suffix);
			}
		}
	}
}
#package Point
sub pathsx {
	my ($self,$side,$kind) = @_;
	my @paths = ();
	if (defined $kind) {
		push @paths, values %{$self->{end}{$side}{$kind}} if $self->{end}{$side}{$kind};
	} else {
		foreach $kind (keys %{$self->{end}{$side}}) {
			push @paths, values %{$self->{end}{$side}{$kind}};
		}
	}
	return @paths;
}
#package Point
sub pathsa {
	my ($self,$kind) = @_;
	return $self->pathsx('a',$kind);
}
#package Point
sub pathsb {
	my ($self,$kind) = @_;
	return $self->pathsx('b',$kind);
}
#package Point
sub paths {
	my ($self,$kind) = @_;
	return ($self->pathsx('a',$kind),$self->pathsx('b',$kind));
}

# ------------------------------------------
package Path; our @ISA = qw(Item);
use strict; use warnings; use Carp;
# ------------------------------------------
#package Path;
sub typpair { return 'Point','Point'; }
#package Path;
sub key {
	my $type = shift;
	my ($model,$enda,$endb) = @_;
	my ($typa,$typb) = $type->typpair();
	$enda = $typa->key($model,$enda);
	$endb = $typb->key($model,$endb);
	return $enda.$endb if $enda and $endb;
	return undef;
}
#package Path;
sub gobj {
	my $type = shift;
	my ($model,$obja,$objb,$linka,$linkb,@args) = @_;
	my ($typea,$typeb) = ($type->typpair(@_));
	unless (ref $obja and ref $obja ne 'ARRAY') {
		$obja = $typea->get($model,$obja,$linka,@args);
	} else { Carp::confess "Object $obja must be of type $typea" unless $obja->isa($typea); }
	unless (ref $objb and ref $objb ne 'ARRAY') {
		$objb = $typeb->get($model,$objb,$linkb,@args);
	} else { Carp::confess "Object $objb must be of type $typeb" unless $objb->isa($typeb); }
	return $obja,$objb;
}
#package Path;
sub init {
	my $self = shift;
	my $type = ref $self;
	my ($obja,$objb) = ($type->gobj(@_));
	$self->{obj}{a} = $obja;
	$self->{obj}{b} = $objb;
	$obja->{end}{a}{$self->kind}{$objb->no} = $self;
	$objb->{end}{b}{$self->kind}{$obja->no} = $self;
	$self->{duplex} = 0;
	#warn "Created Path $self from $obja to $objb";
	Viewer->added_path($obja,$self);
	Viewer->added_path($objb,$self);
}
#package Path;
sub dump {
	my $self = shift;
	print STDERR "$self object A is $self->{obj}{a} with identity ",$self->{obj}{a}->identify(),"\n";
	print STDERR "$self object B is $self->{obj}{b} with identity ",$self->{obj}{b}->identify(),"\n";
}
#package Path;
sub fini {
	my $self = shift;
	my ($obja,$objb) = ($self->{obj}{a},$self->{obj}{b});
	if ($obja and $objb) {
		delete $obja->{end}{a}{$self->kind}{$objb->no};
		delete $objb->{end}{b}{$self->kind}{$obja->no};
	}
	foreach (qw(key obj)) { delete $self->{$_} }
}
#package Path;
sub identify {
	my $self = shift;
	my $type = ref $self;
	my $id = "$type ";
	$id .= $self->obja->shortid;
	$id .= $self->{duplex} ? ', ' : ' -> ';
	$id .= $self->objb->shortid;
	my $cid = $self->can('childid');
	$cid = ' : '.$cid if $cid and $cid = &{$cid}($self);
	$id .= $cid;
	return ($self->{id} = $id);
}
#package Path;
sub shortid {
	my $self = shift;
	my $conn = $self->{duplex} ? '::' : '->';
	my $cid = $self->can('childid');
	$cid = ' : '.$cid if $cid and $cid = &{$cid}($self);
	return $self->obja->shortid.$conn.$self->objb->shortid.$cid;
}
#package Path;
sub chek {
	my $self = shift;
	if (my $obj = $self->{obj}{a}) {
		#$self->show();
		Carp::confess "Path $self still attached from $obj";
	}
	if (my $obj = $self->{obj}{b}) {
		#$self->show();
		Carp::confess "Path $self still attached to $obj";
	}
}
#package Path;
sub mark {
	my ($self,$othr,$merge) = @_;
	return 1;
}
#package Path;
sub them {
	my ($self,@others) = @_;
	my $kind = $self->kind;
	foreach my $othr (@others) {
		my $obja = delete $othr->{obj}{a};
		my $objb = delete $othr->{obj}{b};
		#warn "Deleting $obja reference to Path $othr side a kind $kind with key ".Item::showkey($objb->no);
		delete $obja->{end}{a}{$kind}{$objb->no};
		#warn "Deleting $objb reference to Path $othr side b kind $kind with key ".Item::showkey($obja->no);
		delete $objb->{end}{b}{$kind}{$obja->no};
		my $trga = $self->{obj}{a} = $obja->target();
		my $trgb = $self->{obj}{b} = $objb->target();
		#warn "Adding $trga reference to Path $self side a kind $kind with key ".Item::showkey($trgb->no);
		$trga->{end}{a}{$kind}{$trgb->no} = $self if $trga ne $obja;
		#warn "Adding $trgb reference to Path $self side b kind $kind with key ".Item::showkey($trga->no);
		$trgb->{end}{b}{$kind}{$trga->no} = $self if $trgb ne $objb;
	}
}
#package Path;
sub mfix {
	my ($self,$abort) = @_;
	my $fixed = 0;
	my ($obja,$objb) = ($self->{obj}{a},$self->{obj}{b});
	my ($beca,$becb) = ($obja->{target},$objb->{target});
	my ($fina,$finb) = ($obja,$objb);
	if ($beca) {
		#warn "Path $self: Fixing reference to bad node $obja which became $beca";
		$fina = $beca;
		$fixed = 1;
	}
	if ($becb) {
		#warn "Path $self: Fixing reference to bad node $objb which became $becb";
		$finb = $becb;
		$fixed = 1;
	}
	if ($fixed) {
		$self->{obj}{a} = $fina;
		$self->{obj}{b} = $finb;
		my $kind = $self->kind;
		foreach (keys %{$fina->{end}{a}{$kind}}) {
			delete $fina->{end}{a}{$kind}{$_} if $fina->{end}{a}{$kind}{$_} eq $self;
		}
		$fina->{end}{a}{$kind}{$finb->no} = $self;
		foreach (keys %{$finb->{end}{b}{$kind}}) {
			delete $finb->{end}{b}{$kind}{$_} if $finb->{end}{b}{$kind}{$_} eq $self;
		}
		$finb->{end}{b}{$kind}{$fina->no} = $self;
	}
	Carp::confess "Path $self, object A $self->{obj}{a} is bad" if $self->{obj}{a}{bad};
	Carp::confess "Path $self, object B $self->{obj}{b} is bad" if $self->{obj}{b}{bad};
#	$self->show if $fixed;
	Carp::croak if $fixed and $abort;
}
#package Path;
sub obj {
	my $self = shift;
	my $side = shift;
	Carp::confess "Requested object of deleted path $self" if $self->{bad};
	my $obj = $self->{obj}{$side};
	Carp::confess "Returning reference to delete object $obj of path $self" if $self->{bad};
	return $obj;
}
#package Path;
sub objo {
	my $self = shift;
	my $side = shift;
	$side = (($side eq 'a') ? 'b' : 'a');
	return $self->obj($side);
}
#package Path;
sub obja { return shift->obj('a') }
#package Path;
sub objb { return shift->obj('b') }
#package Path;
sub objs {
	my $self = shift;
	return $self->obja,$self->objb;
}

# -------------------------------------
package Path::Duplex; our @ISA = qw(Path);
use strict; use warnings; use Carp;
# -------------------------------------
# A Path::Duplex is a bidirectional concept.  There are two directional keys
# used to index the Path.
# -------------------------------------
#package Path::Duplex;
sub init {
	my $self = shift;
	$self->{duplex} = 1;
}
#package Path::Duplex;
sub allkeys {
	my $type = shift;
	my ($model,$enda,$endb) = @_;
	my @keys = ();
	my $keya = $type->key($model,$enda,$endb); push @keys, $keya if $keya;
	my $keyb = $type->key($model,$endb,$enda); push @keys, $keyb if $keyb;
	return \@keys;
}

# -------------------------------------
package Traceable; our @ISA = qw(Base);
use strict; use warnings; use Carp;
# -------------------------------------
#package Traceable;
sub init {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->{trace}{$which} = [];
	$self->{tracing}{$which} = $what;
}
#package Traceable;
sub fini {
	my $self = shift;
	my ($which,$what) = ($self->what);
	$self->dotrace('put',${$self->{tracing}{$which}});
	delete $self->{trace}{$which};
	delete $self->{tracing}{$which};
}
#package Traceable;
sub trace {
	my ($self,$which,$sub) = @_;
	my $index = scalar @{$self->{trace}{$which}};
	push @{$self->{trace}{$which}}, $sub;
	return $index;
}
#package Traceable;
sub untrace {
	my ($self,$which,$index) = @_;
	if ($self->{trace}{$which}->[$index]) {
		$self->{trace}{$which}->[$index] = undef;
		while (!defined $self->{trace}{$which}->[0]) {
			shift @{$self->{trace}{$which}};
		}
		while (!defined $self->{trace}{$which}->[-1]) {
			pop @{$self->{trace}{$which}};
		}
		return 1;
	}
}
#package Traceable;
sub dotrace {
	my ($self,$which,$what) = @_;
	foreach (@{$self->{trace}{$which}}) {
		my $val = ${$self->{tracing}{$which}};
		if (ref $_ eq 'SUB') {
			&{$_}($what,$val);
		} elsif (ref $_ eq 'ARRAY') {
			my $sub = shift @{$_};
			&{$sub}($what,$val,@{$_});
		}
	}
}

# -------------------------------------
package Showable; our @ISA = qw(Traceable);
use strict; use warnings; use Carp;
# -------------------------------------
#package Showable;
sub init {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->{show}{$which} = undef;
}
#package Showable;
sub fini {
	my $self = shift;
	my ($which,$what) = $self->what;
#	$self->{show}{$which}->put(@_) if $self->{show}{$which};
	delete $self->{show}{$which};
}
#package Showable;
sub update {
	my $self = shift;
	my ($which,$what) = $self->what;
	$self->Traceable::dotrace($which,'changed');
	$self->{show}{$which}->update(@_) if $self->{show}{$which};
}

# -------------------------------------
package Point::Station; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# Stations are an unidentified IP host or gateway.  We know that there are two
# stations on the edge of a single ethernet, we just do no know which stations
# yet.  A bare Point::Station object is an unknown IP host or gateway.  It just
# acts as a place holder for when the station is bound to an actual IP host or
# gateway.  Stations need a put method so that the ethernet can put these
# stations once they are no longer necessary.  Stations are created in reference
# to an ethernet, the edge of the ethernet on which they exist.
#
# When a Point::Station is created, we know possibly one of its Ethernet
# hardware addresses (Port), but we have no idea which IP addreses belong to it
# unti an ARP or LLDP packet is processed.  When we are live (we are viewing
# captured files or performing capture on the monitoring point itself), we can
# query ARP, LLDP or SNMP for further information.  In particualr LLDP can help
# us find the management IP address associated with the station so that we can
# query it directly with SNMP.
# -------------------------------------
#package Point::Station;
sub add_key {
	my ($self,$key) = @_;
	if ($key and length($key) == 4) {
		Point::Host->xform($self);
		$self->add_key($key);
	} else {
		$self->SUPER::add_key($key);
	}
}
#package Point::Station;
#sub mark {
#	my ($self,$othr,$merge,$reason) = @_;
#	#Carp::cluck $reason;
#	return 1;
#}

# -------------------------------------
package Point::Host; our @ISA = qw(Point::Station);
use strict; use warnings; use Carp;
use Data::Dumper;
# -------------------------------------
#package Point::Host;
use constant {
	dump_tables => 0,
};
#package Point::Host;
sub systemData {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$scalars,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nScalars $scalars: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	$self->{queried} = 1;
	$self->addrow($scalars,$row);
}
#package Point::Host;
sub ifEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{ifIndex};
	my $hwa = $row->{ifPhysAddress};
	return unless $hwa or $row->{ifType} eq 'softwareLoopback(24)';
	my $dsc = $row->{ifDescr};
	my $vid = ''; if ($dsc =~ /\d+\.(\d+)(:\d+)?$/) { $vid = $1 }
	my $vla = undef;
	if (defined $hwa) {
		$vla = $hwa = Item::makekey($hwa);
		$vla .= pack('n',$vid) if defined $vla and length($vid);
	}
	my $vprt = $self->getchild('Vprt',$idx,$vla,[
		['Port',$hwa,undef,[
			['Point::Host',$self,undef],
			['Lan',undef,undef]]],
		['Vlan',undef,undef],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub ifXEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{ifIndex};
	return unless $idx;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub ipAddrEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{ipAdEntIfIndex};
	return unless defined $idx;
	return if not $row->{ipAdEntAddr} or $row->{ipAdEntAddr} eq '...';
	my $pfx = Item::makekey($row->{ipAdEntAddr}.'/'.$row->{ipAdEntNetMask});
	my $ipa = Item::makekey($row->{ipAdEntAddr});
	my $ipt = Model::SNMP::iptype($ipa); return if $ipt eq 'RESERVED';
	my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
	my $nam = Model::SNMP::netname($ipt);
	$self->link($model,undef,[[$nam,$net,$ipa]]);
	$net = $self->parent($nam) unless $net;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt = $self->getchild('Vprt',$idx,undef,[
		[$nam,$net,$ipa],
		['Point::Host',$self,$ipa],
		['Port',undef,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$self,undef],
			['Lan',undef,undef,[
				[$nam,$net,$ipa]]]]],
		['Vlan',undef,undef,[
			[$nam,$net,$ipa]]],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	$net = $net->target;
	my $sub = $net->getchild('Subnet',$pfx,undef,[
		['Vlan',$vlan,undef],
		]);
	my $addr = $net->getchild('Address',$ipa,undef,[
		['Point::Host',$self,$ipa],
		['Subnet',$sub,undef],
		]);
	$addr->addrow($table,$row);
	$sub = $sub->target;
	$sub->addrow($table,$row);
}
#package Point::Host;
sub ipAddressEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	return unless $row->{ipAddressRowStatus} eq 'active(1)';
	return unless $row->{ipAddressAddrType} eq 'ipv4(1)';
	return unless $row->{ipAddressType} eq 'unicast(1)';
	my $idx = $row->{ipAddressIfIndex};
	my $ipa = Item::makekey($row->{ipAddressAddr});
	my $ipt = Model::SNMP::iptype($ipa);
	return if $ipt eq 'RESERVED';
	my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
	my $nam = Model::SNMP::netname($ipt);
	$self->link($model,undef,[[$nam,$net,$ipa]]);
	$net = $self->parent($nam) unless $net;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt = $self->getchild('Vprt',$idx,undef,[
		[$nam,$net,$ipa],
		['Point::Host',$self,$ipa],
		['Port',undef,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$self,undef],
			['Lan',undef,undef,[
				[$nam,$net,$ipa]]]]],
		['Vlan',undef,undef,[
			[$nam,$net,$ipa]]],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	$net = $net->target;
	# FIXME: there is a way to get the prefix as well
	my ($sub,$pfx) = $net->subnetbyip($ipa);
	warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
	my $addr = $net->getchild('Address',$ipa,undef,[
		['Point::Host',$self,$ipa],
		['Subnet',$sub,undef,[
			[$nam,$net,$pfx],
			['Vlan',$vlan,undef]]],
		]);
	$addr->addrow($table,$row);
}
#package Point::Host;
sub atEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{atIfIndex};
	my $mac = Item::makekey($row->{atPhysAddress}) if $row->{atPhysAddress};
	return unless $mac;
	my $ipa = Item::makekey($row->{atNetAddress}) if $row->{atNetAddress};
	return unless $ipa;
	my $ipt = Model::SNMP::iptype($ipa);
	return if $ipt eq 'RESERVED';
	my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
	my $nam = Model::SNMP::netname($ipt);
	$self->link($model,undef,[[$nam,$net,undef]]);
	$net = $self->parent($nam) unless $net;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt = $self->getchild('Vprt',$idx,undef,[
		['Port',undef,undef,[
			[$nam,$net,undef],
			['Point::Host',$self,undef],
			['Lan',undef,undef,[
				[$nam,$net,undef]]]]],
		['Vlan',undef,undef,[
			[$nam,$net,undef]]],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	$vprt = $vprt->target;
	my $host = Point::Host->get($model,$mac,[[$nam,$net,$ipa]]);
	$net = $host->parent($nam) unless $net;
	my $oprt = $vlan->getchild('Vprt',$mac,undef,[
		[$nam,$net,$ipa],
		['Point::Host',$host,$ipa],
		['Port',$mac,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$host,undef],
			['Lan',$plan,undef,[
				[$nam,$net,$ipa]]]]],
		['Vlan',$vlan,undef,[
			[$nam,$net,$ipa]]],
		]);
	$net = $net->target;
	my ($sub,$pfx) = $net->subnetbyip($ipa);
	warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
	my $addr = $net->getchild('Address',$ipa,undef,[
		['Point::Host',$host,$ipa],
		['Subnet',$sub,undef,[
			[$nam,$net,$pfx],
			['Vlan',$vlan,undef]]],
		]);
	$addr->addrow($table,$row);
}
#package Point::Host;
sub ipNetToMediaEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		return unless $row->{ipNetToMediaType} ne 'invalid(2)';
		# TODO: should also check ipNetToPhysicalType
		my $idx = $row->{ipNetToMediaIfIndex};
		my $mac = Item::makekey($row->{ipNetToMediaPhysAddress});
		return unless $mac;
		my $ipa = Item::makekey($row->{ipNetToMediaNetAddress});
		my $ipt = Model::SNMP::iptype($ipa);
		return if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		$vprt = $self->getchild('Vprt',$idx,undef,[
			['Port',undef,undef,[
				[$nam,$net,undef],
				['Point::Host',$self,undef],
				['Lan',undef,undef,[
					[$nam,$net,undef]]]]],
			['Vlan',undef,undef,[
				[$nam,$net,undef]]],
			]);
		my $vlan = $vprt->parent('Vlan');
		my $plan = $vprt->parent('Port')->parent('Lan');
		$vlan->link($model,undef,[['Lan',$plan,undef]]);
		my $host = Point::Host->get($model,$mac,[[$nam,$net,$ipa]]);
		$net = $host->parent($nam) unless $net;
		$vprt = $vprt->target;
		my $oprt = $vlan->getchild('Vprt',$mac,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$host,$ipa],
			['Port',$mac,undef,[
				[$nam,$net,$ipa],
				['Point::Host',$host,undef],
				['Lan',$plan,undef,[
					[$nam,$net,$ipa]]]]],
			['Vlan',$vlan,undef,[
				[$nam,$net,$ipa]]],
			]);
		$net = $net->target;
		my ($sub,$pfx) = $net->subnetbyip($ipa);
		warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
		my $addr = $net->getchild('Address',$ipa,undef,[
			['Point::Host',$host,$ipa],
			['Subnet',$sub,undef,[
				[$nam,$net,$pfx],
				['Vlan',$vlan,undef]]],
			]);
		$addr->addrow($table,$row);
}
#package Point::Host;
sub ipNetToPhysicalEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	return unless $row->{ipNetToPhysicalRowStatus} eq 'active(1)';
	return unless $row->{ipNetToPhysicalNetAddressType} eq 'ipv4(1)';
	return unless $row->{ipNetToPhysicalState} eq 'reachable(1)';
	return unless $row->{ipNetToPhysicalType} ne 'invalid(2)';
	# TODO: should also check ipNetToPhysicalType
	my $idx = $row->{ipNetToPhysicalIfIndex};
	my $mac = Item::makekey($row->{ipNetToPhysicalPhysAddress});
	return unless $mac;
	my $ipa = Item::makekey($row->{ipNetToPhysicalNetAddress});
	my $ipt = Model::SNMP::iptype($ipa);
	return if $ipt eq 'RESERVED';
	my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
	my $nam = Model::SNMP::netname($ipt);
	$self->link($model,undef,[[$nam,$net,undef]]);
	$net = $self->parent($nam) unless $net;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt = $self->getchild('Vprt',$idx,undef,[
		['Port',undef,undef,[
			[$nam,$net,undef],
			['Point::Host',$self,undef],
			['Lan',undef,undef,[
				[$nam,$net,undef]]]]],
		['Vlan',undef,undef,[
			[$nam,$net,undef]]],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	my $host = Point::Host->get($model,$mac,[[$nam,$net,$ipa]]);
	$net = $host->parent($nam) unless $net;
	$vprt = $vprt->target;
	my $oprt = $vlan->getchild('Vprt',$mac,undef,[
		[$nam,$net,$ipa],
		['Point::Host',$host,$ipa],
		['Port',$mac,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$host,undef],
			['Lan',$plan,undef,[
				[$nam,$net,$ipa]]]]],
		['Vlan',$vlan,undef,[
			[$nam,$net,$ipa]]],
		]);
	$net = $net->target;
	my ($sub,$pfx) = $net->subnetbyip($ipa);
	warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
	my $addr = $net->getchild('Address',$ipa,undef,[
		['Point::Host',$host,$ipa],
		['Subnet',$sub,undef,[
			[$nam,$net,$pfx],
			['Vlan',$vlan,undef]]],
		]);
	$addr->addrow($table,$row);
}
#package Point::Host;
sub ipRouteEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	while (1) {
		last unless $row->{ipRouteType} eq 'direct(3)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipRouteIfIndex};
		last unless $row->{ipRouteDest} and $row->{ipRouteMask};
		my $pfx = Item::makekey($row->{ipRouteDest}.'/'.$row->{ipRouteMask});
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		$vprt = $self->getchild('Vprt',$idx,undef,[
			['Port',undef,undef,[
				[$nam,$net,undef],
				['Point::Host',$self,undef],
				['Lan',undef,undef,[
					[$nam,$net,undef]]]]],
			['Vlan',undef,undef,[
				[$nam,$net,undef]]],
			]);
		my $vlan = $vprt->parent('Vlan');
		my $plan = $vprt->parent('Port')->parent('Lan');
		$vlan->link($model,undef,[['Lan',$plan,undef]]);
		$net = $net->target;
		last if unpack('C',substr($pfx,4,1)) == 32;
		my $sub = $net->getchild('Subnet',$pfx,undef,[
			['Vlan',$vlan,undef],
			]);
		last;
	}
	while (1) {
		last unless $row->{ipRouteType} eq 'indirect(4)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipRouteIfIndex};
		my $ipa = Item::makekey($row->{ipRouteNextHop});
		unless ($row->{ipRouteDest} and $row->{ipRouteMask}) {
			Carp::confess "XXXXXX Missing fields:\n",Data::Dumper->new([$row])->Dump;
		}
		my $pfx = Item::makekey($row->{ipRouteDest}.'/'.$row->{ipRouteMask});
		last if unpack('C',substr($pfx,4,1)) == 32;
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$net = $self->parent($nam) unless $net;
		last unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		my $sub = $net->getchild('Subnet',$pfx,undef);
		my $rte = Route->get($model,$vprt->target,$sub->target,$ipa);
		$rte->addrow($table,$row);
		last;
	}
	while (1) {
		last unless $row->{ipRouteType} eq 'indirect(4)';
		last unless $row->{ipRouteNextHop};
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipRouteIfIndex};
		my $ipa = Item::makekey($row->{ipRouteNextHop});
		my $ipt = Model::SNMP::iptype($ipa); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		my $host = $net->getchild('Point::Host',$ipa);
		my ($sub,$pfx) = $net->subnetbyip($ipa);
		warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
		my $addr = $net->getchild('Address',$ipa,undef,[
			['Point::Host',$host,$ipa],
			['Subnet',$sub,undef,[
				[$nam,$net,$pfx],
				['Vlan',undef,undef]]],
			]);
		last;
	}
}
#package Point::Host;
sub ipCidrRouteEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	while (1) {
		last unless $row->{ipCidrRouteType} eq 'local(3)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipCidrRouteIfIndex};
		last unless $row->{ipCidrRouteDest} and $row->{ipCidrRouteMask};
		my $pfx = Item::makekey($row->{ipCidrRouteDest}.'/'.$row->{ipCidrRouteMask});
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		$vprt = $self->getchild('Vprt',$idx,undef,[
			['Port',undef,undef,[
				[$nam,$net,undef],
				['Point::Host',$self,undef],
				['Lan',undef,undef,[
					[$nam,$net,undef]]]]],
			['Vlan',undef,undef,[
				[$nam,$net,undef]]],
			]);
		my $vlan = $vprt->parent('Vlan');
		my $plan = $vprt->parent('Port')->parent('Lan');
		$vlan->link($model,undef,[['Lan',$plan,undef]]);
		last if unpack('C',substr($pfx,4,1)) == 32;
		$net = $net->target;
		my $sub = $net->getchild('Subnet',$pfx,undef,[
			['Vlan',$vlan,undef],
			]);
		last;
	}
	while (1) {
		last unless $row->{ipCidrRouteType} eq 'remote(4)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipCidrRouteIfIndex};
		my $ipa = Item::makekey($row->{ipCidrRouteNextHop});
		unless ($row->{ipCidrRouteDest} and $row->{ipCidrRouteMask}) {
			Carp::confess "XXXXXX Missing fields:\n",Data::Dumper->new([$row])->Dump;
		}
		my $pfx = Item::makekey($row->{ipCidrRouteDest}.'/'.$row->{ipCidrRouteMask});
		last if unpack('C',substr($pfx,4,1)) == 32;
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$net = $self->parent($nam) unless $net;
		last unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		my $sub = $net->getchild('Subnet',$pfx,undef);
		my $rte = Route->get($model,$vprt->target,$sub->target,$ipa);
		$rte->addrow($table,$row);
		last;
	}
	while (1) {
		last unless $row->{ipCidrRouteType} eq 'remote(4)';
		last unless $row->{ipCidrRouteNextHop};
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		my $idx = $row->{ipCidrRouteIfIndex};
		my $ipa = Item::makekey($row->{ipCidrRouteNextHop});
		my $ipt = Model::SNMP::iptype($ipa); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		my $host = $net->getchild('Point::Host',$ipa);
		my ($sub,$pfx) = $net->subnetbyip($ipa);
		warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
		my $addr = $net->getchild('Address',$ipa,undef,[
			['Point::Host',$host,$ipa],
			['Subnet',$sub,undef,[
				[$nam,$net,$pfx],
				['Vlan',undef,undef]]],
			]);
		last;
	}
}
#package Point::Host;
sub inetCidrRouteEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	while (1) {
		last unless $row->{inetCidrRouteType} eq 'local(3)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		last unless $row->{inetCidrRouteDestType} eq 'ipv4(1)';
		my $idx = $row->{inetCidrRouteIfIndex};
		last unless defined $row->{inetCidrRouteDest} and defined $row->{inetCidrRoutePfxLen};
		my $pfx = Item::makekey($row->{inetCidrRouteDest}.'/'.$row->{inetCidrRoutePfxLen});
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		$vprt = $self->getchild('Vprt',$idx,undef,[
			['Port',undef,undef,[
				[$nam,$net,undef],
				['Point::Host',$self,undef],
				['Lan',undef,undef,[
					[$nam,$net,undef]]]]],
			['Vlan',undef,undef,[
				[$nam,$net,undef]]],
			]);
		my $vlan = $vprt->parent('Vlan');
		my $plan = $vprt->parent('Port')->parent('Lan');
		$vlan->link($model,undef,[['Lan',$plan,undef]]);
		last if unpack('C',substr($pfx,4,1)) == 32;
		$net = $net->target;
		my $sub = $net->getchild('Subnet',$pfx,undef,[
			['Vlan',$vlan,undef],
			]);
		last;
	}
	while (1) {
		last unless $row->{inetCidrRouteType} eq 'remote(4)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		last unless $row->{inetCidrRouteDestType} eq 'ipv4(1)';
		last unless $row->{inetCidrRouteNextHopType} eq 'ipv4(1)';
		my $idx = $row->{inetCidrRouteIfIndex};
		my $ipa = Item::makekey($row->{inetCidrRouteNextHop});
		unless ($row->{inetCidrRouteDest} and defined $row->{inetCidrRoutePfxLen}) {
			Carp::confess "XXXXXX Missing fields:\n",Data::Dumper->new([$row])->Dump;
		}
		my $pfx = Item::makekey($row->{inetCidrRouteDest}.'/'.$row->{inetCidrRoutePfxLen});
		last if unpack('C',substr($pfx,4,1)) == 32;
		my $ipt = Model::SNMP::iptype($pfx); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$net = $self->parent($nam) unless $net;
		last unless $net;
		last unless $idx;
		my $vprt = $self->getchild('Vprt',$idx,undef);
		my $sub = $net->getchild('Subnet',$pfx,undef);
		my $rte = Route->get($model,$vprt->target,$sub->target,$ipa);
		$rte->addrow($table,$row);
		last;
	}
	while (1) {
		last unless $row->{inetCidrRouteType} eq 'remote(4)';
		if (dump_tables) {
			print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
			print STDERR Dumper($row),"\n";
		}
		last unless $row->{inetCidrRouteDestType} eq 'ipv4(1)';
		last unless $row->{inetCidrRouteNextHopType} eq 'ipv4(1)';
		my $idx = $row->{inetCidrRouteIfIndex};
		my $ipa = Item::makekey($row->{inetCidrRouteNextHop});
		my $ipt = Model::SNMP::iptype($ipa); last if $ipt eq 'RESERVED';
		my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
		my $nam = Model::SNMP::netname($ipt);
		$self->link($model,undef,[[$nam,$net,undef]]);
		$net = $self->parent($nam) unless $net;
		my $host = $net->getchild('Point::Host',$ipa);
		my ($sub,$pfx) = $net->subnetbyip($ipa);
		warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
		my $addr = $net->getchild('Address',$ipa,undef,[
			['Point::Host',$host,$ipa],
			['Subnet',$sub,undef,[
				[$nam,$net,$pfx],
				['Vlan',undef,undef]]],
			]);
		last;
	}
}
#package Point::Host;
sub ipSystemStatsEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	return unless $row->{ipSystemStatsIPVersion} eq 'ipv4(1)';
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	$self->addrow($table,$row);
}
#package Point::Host;
sub ipIfStatsEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	return unless $row->{ipIfStatsIPVersion} eq 'ipv4(1)';
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	$self->addrow($table,$row);
}
#package Point::Host;
sub lldpPortConfigEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{lldpPortConfigPortNum};
	return unless $idx;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	return unless $vprt;
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub lldpStatsTxPortEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{lldpStatsTxPortNum};
	return unless $idx;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	return unless $vprt;
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub lldpStatsRxPortEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{lldpStatsRxPortNum};
	return unless $idx;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	return unless $vprt;
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub lldpLocPortEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	my $idx = $row->{lldpLocPortNum};
	return unless $idx;
	my $hwa = Item::makekey($row->{lldpLocPortIdSubtype})
		if $row->{lldpLocPortIdSubtype} eq 'macAddress(3)';
	my $vprt = $self->getchild('Vprt',$idx,undef);
	$vprt->addrow($table,$row);
}
#package Point::Host;
sub lldpLocManAddrEntry {
	my $self = shift;
	my ($index,$row,$model,$snmpargs,$table,$xlated,$error,$hkey,$type,$snet,$name) = @_;
	if (dump_tables) {
		print STDERR "\nTable $table row: for ",Item::showkey($hkey),"\n";
		print STDERR Dumper($row),"\n";
	}
	return; # XXX FIXME
	my ($idx,$ipa);
	if ($row->{lldpLocManAddrIfSubtype} eq 'ifIndex(2)') {
		$idx = $row->{lldpLocManAddrIfId};
	}
	return unless $idx;
	if ($row->{lldpLocManAddrSubtype} eq 'ipV4(1)') {
		$ipa = Item::makekey($row->{lldpLocManAddr});
		return unless $ipa;
		$row->{lldpLocManAddr} = Item::showkey($ipa);
	}
	return unless $ipa;
	my $ipt = Model::SNMP::iptype($ipa);
	return if $ipt eq 'RESERVED';
	my $net = $model->{net}{$ipt} if Model::SNMP::typeok($type,$ipt);
	my $nam = Model::SNMP::netname($ipt);
	$self->link($model,undef,[[$nam,$net,$ipa]]);
	$net = $self->parent($nam) unless $net;
	my $vprt = $self->getchild('Vprt',$idx,undef);
	my $addr;
	$vprt = $self->getchild('Vprt',$idx,undef,[
		['Port',undef,undef,[
			[$nam,$net,$ipa],
			['Point::Host',$self,undef],
			['Lan',undef,undef,[
				[$nam,$net,$ipa]]]]],
		['Vlan',undef,undef,[
			[$nam,$net,$ipa]]],
		]);
	my $vlan = $vprt->parent('Vlan');
	my $plan = $vprt->parent('Port')->parent('Lan');
	$vlan->link($model,undef,[['Lan',$plan,undef]]);
	$net = $net->target;
	my ($sub,$pfx) = $net->subnetbyip($ipa);
	warn "Can't find subnet of $net for address ",Item::showkey($ipa) unless $sub;
	$addr = $net->getchild('Address',$ipa,undef,[
		['Point::Host',$self,$ipa],
		['Subnet',$sub,undef,[
			[$nam,$net,$pfx],
			['Vlan',$vlan,undef]]],
		]);
	$addr->addrow($table,$row);
	$vprt->target->addrow($table,$row);
	$self->target->addrow($table,$row);
}

# -------------------------------------
package Point::Host::Here; our @ISA = qw(Point::Host);
use strict; use warnings; use Carp;
# -------------------------------------

# -------------------------------------
package Lan; our @ISA = qw(Point Tree);
use strict; use warnings; use Carp;
# -------------------------------------
# An Lan is a relationship between any number of Port packages.  All ports that
# share a hardware address with a Lan are on that LAN.
#
# When we add a packet to a Lan both hardware addresses are discovered to also
# belong to the same Lan.  The Lan was looked up by source hardware address, so
# we add the destination hardware address when it is available.  A destination
# hardware addresses is not available when the packet comes from a Linux cooked
# header capture.  When another Lan exists that already has the destination
# hardware address as one of its addresses, the two Lan packages will be merged.
# In this way membership of hardware addresses (Port and Point::Station
# packages) to Lan packages is discovered.
# -------------------------------------

# -------------------------------------
package Vlan; our @ISA = qw(Point Tree);
use strict; use warnings; use Carp;
# -------------------------------------
# A Vlan is an IEEE 802.1P/Q virtual LAN.  It is a relationship between any
# number of Vprt packages.  All virtual ports that share a hardware address and
# VLAN id with a Vlan are on that VLAN.
#
# When we add a package to a Vlan both hardware addresses are discovered to also
# belong to the same Vlan.  The Vlan was looked up by source hardware address
# and VLAN id, so we add the destination hardware address when it is available.
# A destination hardware address is not available when the packet comes from a
# Linux cooked header capture.  When another Vlan exists that already has this
# destination hardware address as on of its addresses, the two Vlan packages
# will be merged.  In this way membership of hardware addresses (Port and
# Point::Station packages) to Vlan packages is discovered.
#
# To avoid unnecessary complexity, we deal with Vlan packages only.  The parent
# Lan package is just for automatically tracking membership of Vlan to Lan
# relationships.
# -------------------------------------

# -------------------------------------
package Address; our @ISA = qw(Point Leaf Datum);
use strict; use warnings; use Carp;
# -------------------------------------

# -------------------------------------
package Route; our @ISA = qw(Path Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# A route is an IP routing path from a Port to a Subnet via a gateway
# (Point::Router).
# -------------------------------------
#package Route;
sub typpair { return 'Vprt','Subnet'; }
#package Route;
sub dump {
	my $self = shift;
	print STDERR "$self is from vprt ",$self->obja," to subnet ",$self->objb,"\n";
}

# -------------------------------------
package Loopback; our @ISA = qw(Point Leaf);
use strict; use warnings; use Carp;
# -------------------------------------
# A Loopback object represents the loopback port.
# -------------------------------------
#package Loopback;

# -------------------------------------
package Port; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# A Port is a LAN link layer interface.  Fundamentally, a Port belongs to the
# Lan to which it is attached  (with which it shares its hardware address key).
# Also, it fundamentally belongs to the Point::Station to which it is attached
# (and also shares its hardware address key).  The Lan to which a Port belongs
# can be found by simply lookup up the Lan with the Port hardware address.  The
# same is true of the Point::Station.
# -------------------------------------
#package Port;
sub add_key {
	my $self = shift;
	my ($key) = @_;
	return unless $key;
	my @keys = ref $key eq 'ARRAY' ? @$key : ($key);
	my %keys = ();
	foreach $key (@keys) {
		next if ref $key;
		next unless $key;
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 6;
	}
	foreach $key (@{$self->{key}{n}}) {
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 6;
	}
	if (scalar(values(%keys)) > 1) {
		Carp::carp "Attempt to add non-unique key to $self:";
		Carp::carp "Keys added are ",join(',',map {Item::showkey($_)} @keys);
		$self->show;
		Carp::confess;
	}
	$self->SUPER::add_key(@_);
}
#package Port;
sub mergefrom {
	my $self = shift;
	my ($othr,$reason) = @_;
	my %keys = ();
	foreach my $key (@{$self->{key}{n}},@{$othr->{key}{n}}) {
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 6;
	}
	if (scalar(values(%keys)) > 1) {
		Carp::carp "Attempt to merge $self and $othr with non-unique keys:";
		Carp::carp "Reason was $reason";
		$self->show;
		$othr->show;
		Carp::confess;
	}
	$self->SUPER::mergefrom(@_);
}

# -------------------------------------
package Vprt; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# A Vprt is a VLAN link layer interface.  Fundamentally, a Vprt belongs to the
# Vlan to which it is attached (with which it shares its hardware address and
# VLAN id key).  Also, a Vprt is a child of a Port.  The Vlan to which a Vprt
# belongs can be found by simply lookup up the Vlan with the Vprt hardware
# address and VLAN id.
#
# To avoid unnecessary complexity, we deal with Vprt packages only.  The parent
# Port package is just for automatically tracking membership of Vprt to Port
# relationships.
# -------------------------------------
#package Vprt;
sub add_key {
	my $self = shift;
	my ($key) = @_;
	return unless $key;
	my @keys = ref $key eq 'ARRAY' ? @$key : ($key);
	my %keys = ();
	foreach $key (@keys) {
		next if ref $key;
		next unless $key;
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 8;
	}
	foreach $key (@{$self->{key}{n}}) {
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 8;
	}
	if (scalar(values(%keys)) > 1) {
		Carp::carp "Attempt to add non-unique key to $self:";
		Carp::carp "Keys added are ",join(',',map {Item::showkey($_)} @keys);
		$self->show;
		Carp::confess;
	}
	$self->SUPER::add_key(@_);
}
#package Vprt;
sub mergefrom {
	my $self = shift;
	my ($othr,$reason) = @_;
	my %keys = ();
	foreach my $key (@{$self->{key}{n}},@{$othr->{key}{n}}) {
		my $len = length($key);
		$keys{$key} = 1 if $len >= 6 and $len <= 8;
	}
	if (scalar(values(%keys)) > 1) {
		Carp::carp "Attempt to merge $self and $othr with non-unique keys:";
		Carp::carp "Reason was $reason";
		$self->show;
		$othr->show;
		Carp::confess;
	}
	$self->SUPER::mergefrom(@_);
}
#package Vprt;
sub addrow {
	my $self = shift;
	my ($table,$row) = @_;
	$self->SUPER::addrow(@_);
	my $vla = $self->{key}{n}[0];
	if (defined $vla and length($vla) == 6) {
		if (my $port = $self->parent('Port')) {
			$port->addrow($table,$row);
		}
	}
}

# -------------------------------------
package Driv; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# An MX driver belongs to a specific host.  The index used is the MX driver
# name.  There is really only one driver for now.  There is really nothing more
# required to model these objects.
# -------------------------------------

# -------------------------------------
package Card; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# An MX card belongs to a driver on a specific host.  The index used in the
# card number.  There is really nothing more required to model these objects.
# -------------------------------------

# -------------------------------------
package Span; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# An MX span belongs to a card.  The index used in ths span number.  There is
# really nothing more required to model these objects.
# -------------------------------------

# -------------------------------------
package Chan; our @ISA = qw(Point Tree Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# An MX channel belongs to a span.  The index used is the channel number.  There
# is really nothing more required to model these objects.
# -------------------------------------

# -------------------------------------
package Xcon; our @ISA = qw(Point Leaf Datum);
use strict; use warnings; use Carp;
# -------------------------------------
# An MX cross-connect belongs to a driver.  The index used is a ppa pair: mx
# card, span and channel triple pair.  There is really nothing more required to
# model these objects.
# -------------------------------------

1;

__END__
