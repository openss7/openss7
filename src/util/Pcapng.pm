use strict;
use warnings;

# ------------------------------------------------
package Net::Pcapng; use strict; use warnings;
# ------------------------------------------------

use constant {
	OPT_ENDOFOPT	=>  0,
	OPT_COMMENT	=>  1,

	SHB_HARDWARE	=>  2,
	SHB_OS		=>  3,
	SHB_USERAPPL	=>  4,
	SHB_UUID	=> 0x5572,

	IDB_NAME	=>  2,
	IDB_DESCRIPTION	=>  3,
	IDB_IPV4ADDR	=>  4,
	IDB_IPV6ADDR	=>  5,
	IDB_MACADDR	=>  6,
	IDB_EUIADDR	=>  7,
	IDB_SPEED	=>  8,
	IDB_TSRESOL	=>  9,
	IDB_TZONE	=> 10,
	IDB_FILTER	=> 11,
	IDB_OS		=> 12,
	IDB_FCSLEN	=> 13,
	IDB_TSOFFSET	=> 14,
	IDB_PHYSADDR	=> 0x5572,
	IDB_UUID	=> 0x5573,
	IDB_IFINDEX	=> 0x5574,

	EPB_FLAGS	=>  2,
	EPB_HASH	=>  3,
	EPB_DROPCOUNT	=>  4,
	EPB_COMPCOUNT	=> 0x5572,

	OPB_FLAGS	=>  2,
	OPB_HASH	=>  3,

	ISB_STARTTIME	=>  2,
	ISB_ENDTIME	=>  3,
	ISB_IFRECV	=>  4,
	ISB_IFDROP	=>  5,
	ISB_FILTERACCEPT=>  6,
	ISB_OSDROP	=>  7,
	ISB_USRDELIV	=>  8,
	ISB_IFCOMP	=> 0x5572,

	PCAPNG_SHB_TYPE	=> 0x0a0d0d0a,
	PCAPNG_IDB_TYPE	=> 0x00000001,
	PCAPNG_OPB_TYPE	=> 0x00000002,
	PCAPNG_SPB_TYPE	=> 0x00000003,
	PCAPNG_NRB_TYPE	=> 0x00000004,
	PCAPNG_ISB_TYPE	=> 0x00000005,
	PCAPNG_EPB_TYPE	=> 0x00000006,
	PCAPNG_ITB_TYPE	=> 0x00000007,
	PCAPNG_AEB_TYPE	=> 0x00000008,

	PCAPNG_MAGIC	=> 0x1a2b3c4d,

	PCAPNG_FLAG_DIR_UNKNOWN	    => 0x0000000,
	PCAPNG_FLAG_DIR_INBOUND	    => 0x0000001,
	PCAPNG_FLAG_DIR_OUTBOUND    => 0x0000002,
	PCAPNG_FLAG_DIR_INVALID	    => 0x0000003,

	PCAPNG_FLAG_TYP_UNSPEC	    => 0x0000000,
	PCAPNG_FLAG_TYP_UNICAST	    => 0x0000004,
	PCAPNG_FLAG_TYP_MULTICAST   => 0x0000008,
	PCAPNG_FLAG_TYP_BROADCAST   => 0x000000c,
	PCAPNG_FLAG_TYP_PROMISCUOUS => 0x0000010,

	PCAPNG_FLAG_FCS_UNAVAIL	    => 0x0000000,
	PCAPNG_FLAG_FCS_2BYTES	    => 0x0000040,

	PCAPNG_FLAG_ERR_NONE	    => 0x0000000,
	PCAPNG_FLAG_ERR_SYMBOL	    => 0x8000000,
	PCAPNG_FLAG_ERR_PREAMBLE    => 0x4000000,
	PCAPNG_FLAG_ERR_START	    => 0x2000000,
	PCAPNG_FLAG_ERR_UNALIGNED   => 0x1000000,
	PCAPNG_FLAG_ERR_GAP	    => 0x0800000,
	PCAPNG_FLAG_ERR_SHORT	    => 0x0400000,
	PCAPNG_FLAG_ERR_LONG	    => 0x0200000,
	PCAPNG_FLAG_ERR_CRC	    => 0x0100000,
};


sub pcapng_size_opt {
	my ($buf,$len) = @_;
	if ($buf) {
		$len = ($len + 3) & ~0x3;
		return (4 + $len);
	}
	return (0);
}

sub pcapng_size_opt_64 {
	my $val = shift;
	if ($val) {
		return (4 + 8);
	}
	return (0);
}

sub pcapng_size_opt_32 {
	my $val = shift;
	if ($val) {
		return (4 + 4);
	}
	return (0);
}

sub pcapng_size_opt_16 {
	my $val = shift;
	if ($val) {
		return (4 + 4);
	}
	return (0);
}

sub pcapng_size_opt_8 {
	my $val = shift;
	if ($val) {
		return (4 + 4);
	}
	return (0);
}

sub pcapng_size_opt_str {
	my $str = shift;
	my $size = 0;
	if ($str) {
		my $len = length($str);
		if ($len > 0) {
			$len = ($len + 3) & ~0x3;
			$size += 4 + $len;
		}
	}
	return ($size);
}

sub pcapng_size_opt_var {
	my $var = shift;
	if ($var && $var->{value}) {
		my $len = $var->{length};
		if ($len > 0) {
			$len = ($len + 3) & ~0x3;
			return (4 + $len);
		}
	}
	return (0);
}

sub pcapng_size_opt_ts {
	my $ts = shift;
	if ($ts && ($ts->[0] || $ts->[1])) {
		return (4 + 8);
	}
	return (0);
}

sub pcapng_size_data {
	my $var = shift;
	if ($var && $var->{value}) {
		my $len = $var->{length};
		if ($len > 0) {
			$len = ($len + 3) & ~0x3;
			return ($len);
		}
	}
	return (0);
}

sub pcapng_size_datalen {
	my $len = shift;
	if ($len > 0) {
		$len = ($len + 3) & ~0x3;
		return ($len);
	}
	return (0);
}

sub pcapng_write_opt {
	my ($ptr,$code,$buf,$len) = @_;
	if ($buf) {
		my $plen = (($len + 3) & ~0x3) - $len;
		$$ptr .= pack('SS',$code,$len);
		$$ptr .= $buf;
		$$ptr .= substr("\x{0}\x{0}\x{0}\x{0}",0,$plen);
	}
}

sub pcapng_write_opt_str {
	my ($ptr,$code,$str) = @_;
	if ($str) {
		my $len = length($str);
		if ($len > 0) {
			pcapng_write_opt($ptr,$code,$str,$len);
		}
	}
}

sub pcapng_write_opt_var {
	my ($ptr,$code,$var) = @_;
	if ($var && $var->{value}) {
		my $len = $var->{length};
		if ($len > 0) {
			pcapng_write_opt($ptr,$code,$var->{value},$len);
		}
	}
}

sub pcapng_write_opt_64 {
	my ($ptr,$code,$val) = @_;
	if ($val) {
		pcapng_write_opt($ptr,$code,pack('Q',$val),8);
	}
}

sub pcapng_write_opt_32 {
	my ($ptr,$code,$val) = @_;
	if ($val) {
		pcapng_write_opt($ptr,$code,pack('L',$val),4);
	}
}

sub pcapng_write_opt_16 {
	my ($ptr,$code,$val) = @_;
	if ($val) {
		pcapng_write_opt($ptr,$code,pack('S',$val),2);
	}
}

sub pcapng_write_opt_8 {
	my ($ptr,$code,$val) = @_;
	if ($val) {
		pcapng_write_opt($ptr,$code,pack('C',$val),1);
	}
}

sub pcapng_write_opt_ts {
	my ($ptr,$code,$ts) = @_;
	if ($ts && ($ts->[0] || $ts->[1])) {
		pcapng_write_opt($ptr,$code,pack('LL',@$ts),8);
	}
}

sub pcapng_write_data {
	my ($ptr,$var) = @_;
	if ($var && $var->{value}) {
		my $len = $var->{length};
		if ($len > 0) {
			my $plen = (($len + 3) & ~0x3) - $len;
			$$ptr .= $var->{value};
			$$ptr .= substr("\x{0}\x{0}\x{0}\x{0}",0,$plen);
		}
	}
}

sub pcapng_write_datalen {
	my ($ptr,$buf,$len) = @_;
	if ($len > 0) {
		my $plen = (($len + 3) & ~0x3) - $len;
		$$ptr .= $buf;
		$$ptr .= substr("\x{0}\x{0}\x{0}\x{0}",0,$plen);
	}
}

sub pcapng_size_shb {
	my $shbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 4; #magic
	$size += 2; #major
	$size += 2; #minor
	$size += 8; #section length
	$size += pcapng_size_opt_str($shbi->{comment});
	$size += pcapng_size_opt_str($shbi->{hardware});
	$size += pcapng_size_opt_str($shbi->{os});
	$size += pcapng_size_opt_str($shbi->{userappl});
	$size += pcapng_size_opt($shbi->{uuid},16);
	$size += 4; #end of options
	$size += 4; #trailer
#	print STDERR "SHB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_shb {
	my ($f,$shbi) = @_;
	my $size = pcapng_size_shb($shbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_SHB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('L',PCAPNG_MAGIC);
	$buf .= pack('S',$shbi->{major});
	$buf .= pack('S',$shbi->{minor});
	$buf .= pack('q',-1); #section length
	pcapng_write_opt_str(\$buf,OPT_COMMENT,$shbi->{comment});
	pcapng_write_opt_str(\$buf,SHB_HARDWARE,$shbi->{hardware});
	pcapng_write_opt_str(\$buf,SHB_OS,$shbi->{os});
	pcapng_write_opt_str(\$buf,SHB_USERAPPL,$shbi->{userappl});
	pcapng_write_opt(\$buf,SHB_UUID,$shbi->{uuid},16);
	$buf .= pack('SS',0,0);
	$buf .= pack('L',$size);
	print $f $buf;
}

sub pcapng_size_idb {
	my $idbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 2; #linktype
	$size += 2; #reserved
	$size += 4; #snaplen
	$size += pcapng_size_opt_str($idbi->{comment});
	$size += pcapng_size_opt_str($idbi->{name});
	$size += pcapng_size_opt_str($idbi->{desc});
	$size += pcapng_size_opt($idbi->{ipv4addr},8);
	$size += pcapng_size_opt($idbi->{ipv6addr},17);
	$size += pcapng_size_opt($idbi->{macaddr},6);
	$size += pcapng_size_opt($idbi->{euiaddr},8);
	$size += pcapng_size_opt_64($idbi->{speed});
	$size += pcapng_size_opt_8($idbi->{tsresol});
	$size += pcapng_size_opt_32($idbi->{tzone});
	$size += pcapng_size_opt_var($idbi->{filter});
	$size += pcapng_size_opt_str($idbi->{os});
	$size += pcapng_size_opt_8($idbi->{fcslen});
	$size += pcapng_size_opt_ts($idbi->{tsoffset});
	$size += pcapng_size_opt_var($idbi->{physaddr});
	$size += pcapng_size_opt($idbi->{uuid},16);
	$size += pcapng_size_opt_32($idbi->{ifindex});
	$size += 4; #end of options
	$size += 4; #trailer
#	print STDERR "IDB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_idb {
	my ($f,$idbi) = @_;
	my $size = pcapng_size_idb($idbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_IDB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('S',$idbi->{linktype});
	$buf .= pack('S',$idbi->{reserved});
	$buf .= pack('L',$idbi->{snaplen});
	pcapng_write_opt_str(\$buf,OPT_COMMENT,$idbi->{comment});
	pcapng_write_opt_str(\$buf,IDB_NAME,$idbi->{name});
	pcapng_write_opt_str(\$buf,IDB_DESCRIPTION,$idbi->{desc});
	pcapng_write_opt(\$buf,IDB_IPV4ADDR,$idbi->{ipv4addr},8);
	pcapng_write_opt(\$buf,IDB_IPV6ADDR,$idbi->{ipv6addr},17);
	pcapng_write_opt(\$buf,IDB_MACADDR,$idbi->{macaddr},6);
	pcapng_write_opt(\$buf,IDB_EUIADDR,$idbi->{euiaddr},8);
	pcapng_write_opt_64(\$buf,IDB_SPEED,$idbi->{speed});
	pcapng_write_opt_8(\$buf,IDB_TSRESOL,$idbi->{tsresol});
	pcapng_write_opt_32(\$buf,IDB_TZONE,$idbi->{tzone});
	pcapng_write_opt_var(\$buf,IDB_FILTER,$idbi->{filter});
	pcapng_write_opt_str(\$buf,IDB_OS,$idbi->{os});
	pcapng_write_opt_8(\$buf,IDB_FCSLEN,$idbi->{fcslen});
	pcapng_write_opt_ts(\$buf,IDB_TSOFFSET,$idbi->{tsoffset});
	pcapng_write_opt_var(\$buf,IDB_PHYSADDR,$idbi->{physaddr});
	pcapng_write_opt(\$buf,IDB_UUID,$idbi->{uuid},16);
	pcapng_write_opt_32(\$buf,IDB_IFINDEX,$idbi->{ifindex});
	$buf .= pack('SS',0,0);
	$buf .= pack('L',$size);
	print $f $buf;
}

sub pcapng_size_epb {
	my $epbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 4; #interface id
	$size += 8; #timestamp
	$size += 4; #capture length
	$size += 4; #packet length
	$size += pcapng_size_datalen($epbi->{datalen});
	$size += pcapng_size_opt_str($epbi->{comment});
	$size += pcapng_size_opt_32($epbi->{flags});
	$size += pcapng_size_opt_var($epbi->{hash});
	$size += pcapng_size_opt_64($epbi->{dropcount});
	$size += pcapng_size_opt_64($epbi->{compcount});
	$size += 4; #end of options
	$size += 4; #trailer
#	print STDERR "EPB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_epb {
	my ($f,$epbi,$data) = @_;
	my $size = pcapng_size_epb($epbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_EPB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('L',$epbi->{interfaceid});
	$buf .= pack('LL',@{$epbi->{ts}});
	$buf .= pack('L',$epbi->{captlen});
	$buf .= pack('L',$epbi->{packlen});
	pcapng_write_datalen(\$buf,$data,$epbi->{datalen});
	pcapng_write_opt_str(\$buf,OPT_COMMENT,$epbi->{comment});
	pcapng_write_opt_32(\$buf,EPB_FLAGS,$epbi->{flags});
	pcapng_write_opt_var(\$buf,EPB_HASH,$epbi->{hash});
	pcapng_write_opt_64(\$buf,EPB_DROPCOUNT,$epbi->{dropcount});
	pcapng_write_opt_64(\$buf,EPB_COMPCOUNT,$epbi->{compcount});
	$buf .= pack('SS',0,0);
	$buf .= pack('L',$size);
	print $f $buf;
}

sub pcapng_size_spb {
	my $spbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 4; #packet length
	$size += pcapng_size_datalen($spbi->{datalen});
	$size += 4; #trailer
#	print STDERR "SPB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_spb {
	my ($f,$spbi,$data) = @_;
	my $size = pcapng_size_spb($spbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_SPB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('L',$spbi->{packlen});
	pcapng_write_datalen(\$buf,$data,$spbi->{datalen});
	$buf .= pack('L',$size);
	print $f $buf;
}

sub pcapng_size_opb {
	my $opbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 2; #interface id
	$size += 2; #drops
	$size += 8; #timestamp
	$size += 4; #capture length
	$size += 4; #packet length
	$size += pcapng_size_datalen($opbi->{datalen});
	$size += pcapng_size_opt_str($opbi->{comment});
	$size += pcapng_size_opt_32($opbi->{flags});
	$size += pcapng_size_opt_var($opbi->{hash});
	$size += 4; #end of options
	$size += 4; #trailer
#	print STDERR "OPB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_opb {
	my ($f,$opbi,$data) = @_;
	my $size = pcapng_size_opb($opbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_OPB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('S',$opbi->{interfaceid});
	$buf .= pack('S',$opbi->{dropcount});
	$buf .= pack('LL',@{$opbi->{ts}});
	$buf .= pack('L',$opbi->{captlen});
	$buf .= pack('L',$opbi->{packlen});
	pcapng_write_datalen(\$buf,$data,$opbi->{datalen});
	pcapng_write_opt_str(\$buf,OPT_COMMENT,$opbi->{comment});
	pcapng_write_opt_32(\$buf,EPB_FLAGS,$opbi->{flags});
	pcapng_write_opt_var(\$buf,EPB_HASH,$opbi->{hash});
	$buf .= pack('SS',0,0);
	$buf .= pack('L',$size);
	print $f $buf;
}

sub pcapng_detect_pb_type {
	my ($epbi,$idbi,$if_count) = @_;
	if ($epbi->{compcount}) {
		return PCAPNG_EPB_TYPE;
	}
	if ($epbi->{dropcount}) {
		if ($epbi->{dropcount} > 0x0ffff) {
			return PCAPNG_EPB_TYPE;
		}
		return PCAPNG_OPB_TYPE;
	}
	if ($if_count > 1) {
		return PCAPNG_OPB_TYPE;
	}
	if ($epbi->{interfaceid}) {
		return PCAPNG_OPB_TYPE;
	}
	if ($epbi->{ts} && ($epbi->{ts}[0] || $epbi->{ts}[1])) {
		return PCAPNG_OPB_TYPE;
	}
	if ($epbi->{captlen} && $epbi->{datalen} < $epbi->{packlen} &&
			$epbi->{captlen} != $idbi->{snaplen}) {
		return PCAPNG_OPB_TYPE;
	}
	if ($epbi->{datalen} && $epbi->{datalen} < $epbi->{packlen} &&
			$epbi->{datalen} != $idbi->{snaplen}) {
		return PCAPNG_OPB_TYPE;
	}
	if ($epbi->{flags}) {
		return PCAPNG_OPB_TYPE;
	}
	return PCAPNG_SPB_TYPE;
}

sub pcapng_size_isb {
	my $isbi = shift;
	my $size = 0;
	$size += 8; #header
	$size += 4; #interface id
	$size += 8; #timestamp
	$size += pcapng_size_opt_str($isbi->{comment});
	$size += pcapng_size_opt_ts($isbi->{starttime});
	$size += pcapng_size_opt_ts($isbi->{endtime});
	$size += pcapng_size_opt_64($isbi->{ifrecv});
	$size += pcapng_size_opt_64($isbi->{ifdrop});
	$size += pcapng_size_opt_64($isbi->{filteraccept});
	$size += pcapng_size_opt_64($isbi->{osdrop});
	$size += pcapng_size_opt_64($isbi->{usrdeliv});
	$size += pcapng_size_opt_64($isbi->{ifcomp});
	$size += 4; #end of options
	$size += 4; #trailer
#	print STDERR "ISB size 88\n" if $size == 88;
	return $size;
}

sub pcapng_write_isb {
	my ($f,$isbi) = @_;
	my $size = pcapng_size_isb($isbi);
	my $buf = '';
	$buf .= pack('L',PCAPNG_ISB_TYPE);
	$buf .= pack('L',$size);
	$buf .= pack('L',$isbi->{interfaceid});
	$buf .= pack('LL',@{$isbi->{ts}});
	pcapng_write_opt_str(\$buf,OPT_COMMENT,$isbi->{comment});
	pcapng_write_opt_ts(\$buf,ISB_STARTTIME,$isbi->{starttime});
	pcapng_write_opt_ts(\$buf,ISB_ENDTIME,$isbi->{endtime});
	pcapng_write_opt_64(\$buf,ISB_IFRECV,$isbi->{ifrecv});
	pcapng_write_opt_64(\$buf,ISB_IFDROP,$isbi->{ifdrop});
	pcapng_write_opt_64(\$buf,ISB_FILTERACCEPT,$isbi->{filteraccept});
	pcapng_write_opt_64(\$buf,ISB_OSDROP,$isbi->{osdrop});
	pcapng_write_opt_64(\$buf,ISB_USRDELIV,$isbi->{usrdeliv});
	pcapng_write_opt_64(\$buf,ISB_IFCOMP,$isbi->{ifcomp});
	$buf .= pack('SS',0,0);
	$buf .= pack('L',$size);
	print $f $buf;
}

1;

__END__
