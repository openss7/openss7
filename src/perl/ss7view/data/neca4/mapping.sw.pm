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
	'rng'=>sub{
	},
	'lines'=>sub{
	},
	'total'=>sub{
	},
	'loc'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{NAME} = $val if length($val);
	},
	'state'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'clli'=>sub{
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
	'wcvh'=>sub{
		my ($dat,$fld,$val) = @_;
		if (length($val)) {
			my ($v,$h) = split(/,/,$val);
			if ($v and $h) {
				$dat->{VH} = $dat->{WCVH} = sprintf('%05d,%05d',$v,$h);
				$dat->{LL} = $dat->{WCLL} = join(',',::vh2ll($v,$h));
			}
		}
	},
	'ocn'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'lata'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'feat'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
	'sect'=>sub{
		my ($dat,$fld,$val) = @_;
		$dat->{"\U$fld\E"} = $val if length($val);
	},
);

