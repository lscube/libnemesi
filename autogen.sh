#!/bin/bash

WANT_AUTOCONF=2.5
WANT_AUTOMAKE=1.8

export WANT_AUTOCONF WANT_AUTOMAKE


function dots ()
{
	while ps h -p $1 >/dev/null; do
		sleep 1;
		echo -n ".";
	done	
}

echo
echo "Bootstrapping NeMeSI package AutoTools configuration..."

if ! test -d config; then
	mkdir config;
fi

if which libtoolize >/dev/null 2>&1 ; then
	echo -n "Running libtoolize..."
	libtoolize --force --copy --automake &
	dots $! 
	echo " done."
else
	echo "WARNING! NeMeSI SVN needs LibTool!"
	echo "Please, install it."
	echo "Aborting."
fi
if which aclocal >/dev/null 2>&1 ; then
	echo -n "Running aclocal..."
	aclocal -I config &
	dots $! 
	echo " done."
fi

if which autoheader >/dev/null 2>&1 ; then
	echo -n "Running autoheader..."
	autoheader 2>&1 | grep -v AC_TRY_RUN &
	dots $! 
	echo " done."
fi
if which autoconf >/dev/null 2>&1 ; then
	echo -n "Running autoconf..."
	autoconf 2>&1 | grep -v AC_TRY_RUN &
	dots $! 
	echo " done."
fi
if which automake >/dev/null 2>&1 ; then
	echo -n "Running automake..."
	automake --gnu --add-missing --copy --force-missing 2>&1 | grep -v installing &
	dots $! 
	echo " done."
fi

#echo -n "Copying missing files..."
#	cp -f config/lt* libltdl
#	cp -f config/config.* libltdl
#	cp -f config/install-sh libltdl
#	cp -f config/mkinstalldirs .
#	cp -f libltdl/acinclude.m4 .
#echo " done."
cd libnms
./autogen.sh
cd -

echo "All done. Bye."
echo
