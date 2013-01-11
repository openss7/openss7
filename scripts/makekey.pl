#!/usr/bin/perl

eval 'exec perl -S $0 ${1+"@"}'
    if $running_under_some_shell;

use strict;
use warnings;

use Digest::HMAC_MD5;
use Digest::MD5;

my ($cmd,$mac,$hmac,$key,$data,$digest);
$cmd='sh -c \'eth=$(for f in /sys/class/net/eth*; do if [ ! -L $f ]; then continue ; fi ; echo $(readlink $f)" "$f ; done | sort | head -1 | cut -f2 "-d ") ; [ -r $eth/address ] && cat $eth/address | sed "s,:,,g"\'';
$mac = `$cmd`; chomp($mac);
print STDERR "mac=$mac\n";
$key = pack('H*','0000'.$mac);
print STDERR "key=", unpack('H*',$key), "\n";
$data = pack('CCn',0xff,0x00,time()>>16).$key;
print STDERR "data=", unpack('H*',$data), "\n";
$hmac = Digest::HMAC_MD5->new($key);
my $mykey = $key;
$hmac->add($data);
my $mydat = $data;
my @bytes = unpack('C*',substr($data,0,4));
print STDERR "prefix=", unpack('H*',pack('C*',@bytes)), "\n";
for (my $i=0;$i<4;$i++) { $bytes[$i] ^= 0x5C; }
print STDERR "prefix=", unpack('H*',pack('C*',@bytes)), "\n";
$digest = $hmac->digest;
my $mydig = $digest;
print STDERR "digest=", unpack('H*',$digest), "\n";
$digest = pack('C*',@bytes).$digest.pack('H*',"b4eecdda");
print unpack('H*',$digest), "\n";
my @i_pad = ();
my @o_pad = ();
for (my $i=0;$i<64;$i++) { push @i_pad, 0; push @o_pad, 0 }
my $i_pad = pack('C*',@i_pad);
my $o_pad = pack('C*',@o_pad);
print STDERR "i_pad=", unpack('H*',$i_pad), "\n";
print STDERR "o_pad=", unpack('H*',$i_pad), "\n";
substr($i_pad,0,length($mykey),$mykey);
substr($o_pad,0,length($mykey),$mykey);
print STDERR "i_pad=", unpack('H*',$i_pad), "\n";
print STDERR "o_pad=", unpack('H*',$i_pad), "\n";
@i_pad = unpack('C*',$i_pad);
@o_pad = unpack('C*',$o_pad);
for (my $i=0;$i<64;$i++) {
	$i_pad[$i] ^= 0x36;
	$o_pad[$i] ^= 0x5c;
}
$i_pad = pack('C*',@i_pad);
$o_pad = pack('C*',@o_pad);
print STDERR "i_pad=", unpack('H*',$i_pad), "\n";
print STDERR "o_pad=", unpack('H*',$i_pad), "\n";
my $inner = Digest::MD5->new;
$inner->add($i_pad.$mydat);
my $ires = $inner->digest;
my $outer = Digest::MD5->new;
$outer->add($o_pad.$ires);
$digest = $outer->digest;
print STDERR "digest=", unpack('H*',$digest), "\n";
$digest = pack('C*',@bytes).$digest.pack('H*',"b4eecdda");
print unpack('H*',$digest), "\n";

