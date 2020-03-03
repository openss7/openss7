#!/bin/bash

# need --enable-maintainer-mode to be able to run in place
#      must be disabled to build an installable package

# *FLAGS are what Arch Linux makepkg uses with the exception
#      that -Wall -Werror is added

set -x

case "`uname -m`" in
	i686)
		CPPFLAGS="-D_FORTIFY_SOURCE=2"
		CFLAGS="-march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong -fno-plt"
		CXXFLAGS="-march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong -fno-plt"
		GCJFLAGS="-march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong -fno-plt"
		LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now"
		DEBUG_CFLAGS="-g -ggdb -fvar-tracking-assignments"
		DEBUG_CXXFLAGS="-g -ggdb -fvar-tracking-assignments"
		DEBUG_GCJFLAGS="-g -ggdb -fvar-tracking-assignments"
	;;
	x86_64)
		CPPFLAGS="-D_FORTIFY_SOURCE=2"
		CFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fno-plt"
		CXXFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fno-plt"
		GCJFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fno-plt"
		LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now"
		DEBUG_CFLAGS="-g -ggdb -fvar-tracking-assignments"
		DEBUG_CXXFLAGS="-g -ggdb -fvar-tracking-assignments"
		DEBUG_GCJFLAGS="-g -ggdb -fvar-tracking-assignments"
	;;
esac

#_kvr="$(pacman -Qi linux-lts316|awk '/^Version/{print$3}')-lts316"
#KCC="gcc"
#_kvr="$(pacman -Qi linux-lts44|awk '/^Version/{print$3}')-lts44"
#KCC="gcc"
#_kvr="$(pacman -Qi linux-lts49|awk '/^Version/{print$3}')-lts49"
#KCC="gcc"
#_kvr="$(pacman -Qi linux-lts414|awk '/^Version/{print$3}')-lts414"
#KCC="gcc"
#_kvr="$(pacman -Qi linux-lts419|awk '/^Version/{print$3}')-lts419"
#KCC="gcc"
#_kvr="$(pacman -Qi linux-lts|awk '/^Version/{print$3}')-lts"
#KCC="gcc"
_kvr="$(pacman -Qi linux|awk '/^Version/{print$3}'|sed 's,\.arch,-arch,')"
KCC="gcc"

#_opt="size"
#_opt="speed"
_opt="normal"
#_opt="quick"
#CPPFLAGS=""
#_opt="auto"

./configure \
	KCC="$KCC" \
	GCJ="gcj" \
	CXX="g++-6" \
	CPPFLAGS="$CPPFLAGS" \
	CFLAGS="$DEBUG_CFLAGS -Wall -Werror -Wno-missing-braces $CFLAGS" \
	CXXFLAGS="$DEBUG_CXXFLAGS -Wall -Werror -Wno-missing-braces $CXXFLAGS" \
	GCJFLAGS="$DEBUG_GCJFLAGS -Wall -Wno-suppress -Wno-unused-function $GCJFLAGS" \
	LDFLAGS="$LDFLAGS" \
	DEBUG_CFLAGS="$DEBUG_CFLAGS" \
	DEBUG_CXXFLAGS="$DEBUG_CXXFLAGS" \
	DEBUG_GCJFLAGS="$DEBUG_GCJFLAGS" \
	MODPOST_DEBUG=5 \
	MODPOST_VERBOSE=5 \
	syslibdir=/usr/lib \
	sysbindir=/usr/bin \
	syssbindir=/usr/bin \
	--prefix=/usr \
	--sysconfdir=/etc \
	--libdir=/usr/lib \
	--bindir=/usr/bin \
	--sbindir=/usr/bin \
	--libexecdir=/usr/lib \
	--docdir=/usr/share/doc/openss7 \
	--enable-maintainer-mode \
	--enable-dependency-tracking \
	--disable-static \
	--disable-sysvinit \
	--disable-k-abi-support \
	--disable-k-weak-symbols \
	--enable-k-weak-modules \
	--disable-specfs-lock \
	--with-k-release=$_kvr \
	--with-k-optimize=$_opt \
	--with-optimize=$_opt \
	--with-gnu-ld
#	--disable-docs \
#	--disable-tools \
#	--disable-java

set +x
