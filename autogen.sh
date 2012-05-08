#!/bin/sh

export WANT_AUTOCONF_2_5="1"
export WANT_AUTOMAKE_1_7="1"

aclocal
libtoolize --automake --force --copy
automake -a -c
autoconf

