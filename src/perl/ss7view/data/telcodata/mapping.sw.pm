package mapping;

our %mapping = (
	'NPA'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'NXX'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'X'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'State'=>sub{
	},
	'Company'=>sub{
	},
	'OCN'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'Rate Center'=>sub{
	},
	'Switch'=>sub{
		my ($dat,$fld,$val) = @_;
		if ($val and length($val) == 11) {
			$dat->{SWCLLI} = $val;
			unless (exists $skipcllis{$val}) {
				$dat->{WCCLLI} = substr($val,0,8);
				$dat->{PLCLLI} = substr($val,0,6);
				$dat->{STCLLI} = substr($val,4,2);
				$dat->{CTCLLI} = substr($val,0,4);
				my $cs = $dat->{STCLLI};
				unless (exists $::cllicc{$cs}) {
					print STDERR "E: $dat->{NPA}-$dat->{NXX}($dat->{X}) CLLI '$val' has invalid state code '$cs'\n";
				}
				$dat->{WCCC} = $::cllicc{$cs} if exists $::cllicc{$cs};
				$dat->{WCST} = $::cllist{$cs} if exists $::cllist{$cs};
				$dat->{WCRG} = $::cllirg{$cs} if exists $::cllirg{$cs};
			}
		}
	},
	'Date'=>sub{
	},
	'Prefix Type'=>sub{
	},
	'Switch Name'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{NAME} = $val if length($val);
	},
	'Switch Type'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{TYPE} = $val if length($val);
	},
	'LATA'=>sub{
		my ($dat,$fld,$val) = @_;
		if ($val =~ /\(([0-9]+)\)/) { $dat->{LATA} = $1 }
	},
	'Tandem'=>sub{
	},
);

