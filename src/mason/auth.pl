package OpenSS7::Auth;

use Fcntl qw(:DEFAULT :flock);
use DB_File;
use MIME::Base64;
use Storable qw(nfreeze thaw);
use Apache::Constants qw(:common);

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

    my $user = $r->connection->user;

    if ( $r->filename =~ m/\/openss7-repo.*\.rpm$/ ) {
	#$r->warn("Anybody ($user) can download the repo defintion rpm ".$r->uri);
	return OK;
    }
    if ( $r->filename =~ m,/repo/rpms/repodata/[^/]+\.xml(\.gz)?$, ) {
	#$r->warn("Anybody ($user) can download the empty repository ".$r->uri);
	return OK;
    }
    if ( $r->filename !~ m/(\.xml(\.gz)?|\.rpm)$/ ) {
	$r->log_reason("not an rpm or xml file", $r->filename);
	return FORBIDDEN;
    }

    unless ($user and defined $sent_pw) {
	$r->note_basic_auth_failure;
	$r->log_reason("username and password required", $r->uri);
	return AUTH_REQUIRED;
    }

    my $grps = check_host($r,$user);
    return $grps if ($grps == FORBIDDEN or $grps == AUTH_REQUIRED);

    if ( $r->filename !~ m/\/repo\/rpms\/(([^\/].*)\/)?(repodata|RPMS|SRPMS)\/(.*)$/ ) {
	$r->log_reason("file is out of place", $r->filename);
	return FORBIDDEN;
    }
    my $path = $2;
    my $type = $3;
    my $also = $4;

    unless ($type and $also) {
	$r->log_reason("incorrect data in path", $r->filename);
	return NOT_FOUND;
    }

    if ( $type eq 'SRPMS' ) {
	$r->log_reason("SRPM access forbidden by rule", $r->filename);
	return NOT_FOUND;
    }
    if ( $r->filename =~ m/\.src\.rpm$/ ) {
	$r->log_reason("SRPM access forbidden by rule", $r->filename);
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
	    if ($fields[3] =~ m/^(main|debug|devel|source)$/) {
		$fields[4] = $fields[3];
		$fields[3] = 'base';
	    } else {
		$fields[4] = 'full';
	    }
	}
	$fields[3] = 'base' unless ($fields[3]);
	$fields[4] = 'full' unless ($fields[4]);

	if ($fields[4] !~ m/^(main|debug|devel|source|full)$/) {
	    $r->log_reason("incorrect data in filename", $r->filename);
	    return NOT_FOUND;
	}

	($distro,$relver,$osarch,$branch,$subrep) = @fields;
    }

    my $retval = OK;

    if ($retval == OK) { $retval = check_distarch($r,$user,$grps,$distro,$osarch); }
    if ($retval == OK) { $retval = check_bransubr($r,$user,$grps,$branch,$subrep); }

    if ($retval == FORBIDDEN) {
	return NOT_FOUND unless ( $type eq 'repodata' );
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
    my ($r,$user,$grps,$distro,$osarch) = @_;

    my $manage = ( $distro =~ m/^(sle|sles|sled|suse|opensuse)$/ ) ? 'zypp' : 'yum';
    my $enterp = ( $distro =~ m/^(sle|sles|sled|rhel|centos)$/ ) ? 'enterprise' : 'community';
    my $server = ( $osarch !~ m/^i[3456]86$/ ) ? 'server' : 'desktop';

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
    my ($r,$user,$grps,$branch,$subrep) = @_;

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

