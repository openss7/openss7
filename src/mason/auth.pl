package OpenSS7::Auth;

use Fcntl qw(:DEFAULT :flock);
use DB_File;
use MIME::Base64;
use Storable qw(nfreeze thaw);
BEGIN {
    if (eval { require Apache::Constants; }) {
	import Apache::Constants qw(:common);
    } else {
	require Apache2::Const;
	import  Apache2::Const -compile=>qw(:common);
	require Apache2::Access;
    }
}

sub rpmaccesshandler {
    my $r = shift;
    return OK;
}

sub rpmauthenhandler {
    my $r = shift;

    # a previous module might have already already forbidden
    # the user and password
    my ($res,$sent_pw) = $r->get_basic_auth_pw;
    return $res if ( $res != OK );

    my $user;
    if ($r->connection->can('user')) {
	    $user = $r->connection->user;
    } else {
	    $user = $r->user;
    }

    if ( $r->filename =~ m,/openss7-repo.*\.(rpm|deb|pkg\.tar\.xz)$, ) {
	#$r->warn("Any host ($user) can download the repo defintion rpm, deb or pkg ".$r->uri);
	return OK;
    }
    if ( $r->filename =~ m,/repo/rpms/repodata/[^/]+\.xml(\.gz)?$, ) {
	#$r->warn("Any host ($user) can download the empty repository ".$r->uri);
	return OK;
    }
    if ( $r->filename =~ m,/repo/debs/((Packages|Sources|Contents|Release)(\.(gz|bz2|gpg|key))?|md5sum.txt)$, ) {
	#$r->warn("Any host ($user) can download the empty repository ".$r->uri);
	return OK;
    }
    if ( $r->filename =~ m,/repo/pkgs/openss7.*\.db*(\.tar\.(gz|bz2|xz))?(\.sig)?$, ) {
	#$r->warn("Any host ($user) can download the empty repository ".$r->uri);
	return OK;
    }
    if ( $r->filename !~ m,(\.xml(\.gz)?|\.rpm|\.deb|\.pkg.tar.xz)$, and
	 $r->filename !~ m,((Packages|Sources|Contents|Release)(\.(gz|bz2|gpg|key))?|md5sum.txt)$, )
    {
	$r->log_reason("not an rpm, deb, xml, pkg or apt file", $r->filename);
	return FORBIDDEN;
    }

    unless ($user and defined $sent_pw) {
	$r->note_basic_auth_failure;
	$r->log_reason("username and password required", $r->uri);
	return AUTH_REQUIRED;
    }

    my $grps = check_host($r,$user);
    return $grps if ($grps == FORBIDDEN or $grps == AUTH_REQUIRED);

    if ( $r->filename =~ m,/repo/repoindex\.xml$, ) {
	if ( ",$grps," !~ ',zypp-hosts,' ) {
	    $r->log_reason("Host $user not in group 'zypp-hosts'",$r->filename);
	    return FORBIDDEN;
	}
	return OK;
    }
    if ( $r->filename =~ m,/repo/openss7\.list$, ) {
	if ( ",$grps," != ',apt-hosts,' ) {
	    $r->log_reason("Host $user not in group 'apt-hosts'", $r->filename);
	    return FORBIDDEN;
	}
	return OK;
    }
    if ( $r->filename !~ m,/repo/rpms/(([^/].*)/)?(repodata|RPMS|SRPMS)/(.*)$, and
	 $r->filename !~ m,/repo/debs/(([^/].*)/)?()(.*)$, )
    {
	$r->log_reason("file is out of place", $r->filename);
	return FORBIDDEN;
    }
    my $path = $2;
    my $type = $3;
    my $also = $4;

    unless ($type or $also) {
	$r->log_reason("incorrect data in path", $r->filename);
	return NOT_FOUND;
    }

    if ( $type eq 'SRPMS' ) {
	$r->log_reason("SRPM access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }
    if ( $r->filename =~ m,\.src\.rpm$, ) {
	$r->log_reason("SRPM access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }
    if ( $r->filename =~ m,\.dsc(\.asc)?$, ) {
	$r->log_reason("DSC access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }
    if ( $r->filename =~ m,\.tar(\.(gz|bz2|lzma|xz))?$, ) {
	$r->log_reason("DSC access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }
    if ( $r->filename =~ m,\.src\.tar\.xz$, ) {
	$r->log_reason("SRC access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }

    # top level repodata is always ok
    return OK unless ($path);

    my ($distro,$relver,$osarch,$branch,$subrep);
    {
	my @fields = split(/\//, $path);
	if ($#fields < 2 or $#fields > 4) {
	    $r->log_reason("incorrect data in path", $r->filename);
	    return NOT_FOUND;
	}
	if ($#fields == 2) {
	    $fields[3] = 'base';
	    $fields[4] = 'full';
	}
	if ($#fields == 3) {
	    if ($fields[3] =~ m,^(main|debug|devel|source)$,) {
		$fields[4] = $fields[3];
		$fields[3] = 'base';
	    } else {
		$fields[4] = 'full';
	    }
	}
	$fields[3] = 'base' unless ($fields[3]);
	$fields[4] = 'full' unless ($fields[4]);

	if ($fields[4] !~ m,^(main|debug|devel|source|full)$,) {
	    $r->log_reason("incorrect data in filename", $r->filename);
	    return NOT_FOUND;
	}

	($distro,$relver,$osarch,$branch,$subrep) = @fields;
    }

    my $retval = OK;

    if ($retval == OK) { $retval = check_distarch($r,$user,$grps,$distro,$relver,$osarch); }
    if ($retval == OK) { $retval = check_bransubr($r,$user,$grps,$branch,$subrep,$osarch); }

    if ($retval == FORBIDDEN) {
	return NOT_FOUND unless ( $type eq 'repodata' or $type eq '' );
	my $subr = $r->lookup_uri("/repo/rpms/$type/$also");
	my $file = $subr->filename;
	return NOT_FOUND unless ($file and -e $file);
	## just remap the filename
	$r->filename($file);
	return OK;
    }

    return $retval;
}

sub check_host {
    my ($r,$user) = @_;
    my $entry;
    {
	open(my $fd, ">", '/var/www/mason/repousers.LOCK') or die $!;
	flock($fd,LOCK_SH);
	tie my %users, 'DB_File', '/var/www/mason/repousers', O_CREAT|O_RDWR, 0640 or die $!;
	$entry = $users{$user};
	untie %users;
	flock($fd,LOCK_UN);
	close($fd);
    }
    # this should have been checked by apache already
    unless (defined $entry) {
	$r->note_basic_auth_failure;
	$r->log_reason("user $user not in database", $r->uri);
	return AUTH_REQUIRED;
    }
    # this should have been checked by apache already
    my ($dummy1,$grps,$dummy3) = split(/:/,$entry,3);
    if ( ",$grps," !~ ',hosts,' ) {
	$r->log_reason("user $user not in group 'hosts'", $r->uri);
	return FORBIDDEN;
    }
    return $grps;
}

sub check_distarch {
    my ($r,$user,$grps,$distro,$relver,$osarch) = @_;

    my ($manage,$enterp,$server) = ( 'yum', 'community', 'desktop' );

    $manage = 'yum';
    $manage = 'zypp'	if ( $distro =~ m,^(sle|sles|sled|suse|openSUSE)$, );
    $manage = 'apt'	if ( $distro =~ m,^(debian|ubuntu|mint|mepis|knoppix|pclinux)$, );
    $manage = 'pacman'	if ( $distro =~ m,^(arch)$, );

    $enterp = 'community'; 
    $enterp = 'enterprise' if ( $distro =~ m,^(sle|sles|sled|rhel|centos|lts)$, );
    # FIXME: need to make ubuntu LTS enterprise

    $server = 'desktop';
    $server = 'server' if ( $osarch !~ m,^i[3456]86$, );

    my $access = ",$grps,";

    # must be in distro and osarch assigned to host
    if ($access !~ /,$distro-$osarch-host,/) {
	$r->log_reason("user $user not in group '$distro-$osarch-host'", $r->uri);
	return FORBIDDEN;
    }

    # must be in zypp-hosts or yum-hosts as appropriate
    if ($access !~ /,$manage-hosts,/) {
	$r->log_reason("user $user not in group '$manage-hosts'", $r->uri);
	return FORBIDDEN;
    }

    # is the specific distribution and architecture permitted?
    return OK if ($access =~ /,$distro-$osarch-hosts,/);
    return OK if ($access =~ /,$distro-hosts,/ and $access =~ /,$osarch-hosts,/);
    return OK if ($access =~ /,$distro-$server-hosts,/);
    return OK if ($access =~ /,$distro-hosts,/ and $access =~ /,$server-hosts,/);
    return OK if ($access =~ /,$enterp-$osarch-hosts,/);
    return OK if ($access =~ /,$enterp-hosts,/ and $access =~ /,$osarch-hosts,/);
    return OK if ($access =~ /,$enterp-$server-hosts,/);
    return OK if ($access =~ /,$enterp-hosts,/ and $access =~ /,$server-hosts,/);

    $r->log_reason("user $user not in group '$distro-$osarch-hosts'", $r->uri);
    return FORBIDDEN;
}

sub check_bransubr {
    my ($r,$user,$grps,$branch,$subrep,$osarch) = @_;

    my $access = ",$grps,";

    return OK if ($access =~ /,$branch-$subrep-hosts,/);
    return OK if ($access =~ /,$branch-hosts,/ and $access =~ /,$subrep-hosts,/);
    $r->log_reason("user $user not in group '$branch-$subrep-hosts'", $r->uri);
    return FORBIDDEN;
}

sub rpmauthzhandler {
    my $r = shift;
    return OK;
}

1;
