#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  on the rights to use, copy, modify, merge, publish, distribute, sub
#  license, and/or sell copies of the Software, and to permit persons to whom
#  the Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice (including the next
#  paragraph) shall be included in all copies or substantial portions of the
#  Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
#  ADAM JACKSON BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

AC_PREREQ(2.60)
AC_INIT(libtbm, 2.0.0)
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([dist-bzip2])

AM_CONFIG_HEADER([config.h])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX

AC_HEADER_STDC
AC_SYS_LARGEFILE
AC_FUNC_ALLOCA

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl Make sure the pkg-config macros are defined
m4_ifndef([PKG_PROG_PKG_CONFIG],
    [m4_fatal([Could not locate the pkg-config autoconf macros.
  These are usually located in /usr/share/aclocal/pkg.m4. If your macros
  are in a different location, try setting the environment variable
  ACLOCAL="aclocal -I/other/macro/dir" before running autoreconf.])])
PKG_PROG_PKG_CONFIG()

# set the dir for the tbm module
DEFAULT_BUFMGR_MODULE_PATH="${libdir}/bufmgr"
AC_ARG_WITH(bufmgr-module-path, AS_HELP_STRING([--with-bufmgr-module-path=PATH], [tbm bufmgr module dir]),
				[ BUFMGR_MODULE_PATH="$withval" ],
				[ BUFMGR_MODULE_PATH="${DEFAULT_BUFMGR_MODULE_PATH}" ])

AC_ARG_WITH(utest, AS_HELP_STRING([--with-utest=yes/no], [whether build/run unit tests or not]),
				[ utest="$withval" ],
				[ utest="no" ])

AM_CONDITIONAL(HAVE_UTEST, test "x$utest" = "xyes")

#AC_DEFINE(BUFMGR_MODULE_DIR, "${BUFMGR_MODULE_PATH}", [Directory for the modules of tbm_bufmgr])
AC_DEFINE_UNQUOTED(BUFMGR_MODULE_DIR, "${BUFMGR_MODULE_PATH}", [Directory for the modules of tbm_bufmgr])

AC_CHECK_FUNCS([clock_gettime], [CLOCK_LIB=],
               [AC_CHECK_LIB([rt], [clock_gettime], [CLOCK_LIB=-lrt],
                             [AC_MSG_ERROR([Couldn't find clock_gettime])])])
AC_SUBST([CLOCK_LIB])

AC_PATH_PROG([wayland_scanner], [wayland-scanner])
if test x$wayland_scanner = x; then
      AC_MSG_ERROR([wayland-scanner is needed to compile wayland-tbm])
fi

PKG_CHECK_MODULES(CAPI, [capi-base-common >= 0.1.1], [capi_0_1_1=yes], [capi_0_1_1=no])
if test x"$capi_0_1_1" = xyes; then
	AC_DEFINE(HAVE_CAPI_0_1_1,1,[The version of capi-base-common is over 0.1.1])
fi

PKG_CHECK_MODULES(LIBDRM, libdrm)
PKG_CHECK_MODULES(WL_CLIENT, wayland-client)
PKG_CHECK_MODULES(WL_SERVER, wayland-server)
PKG_CHECK_MODULES(WL_SCANNER, wayland-scanner)
PKG_CHECK_MODULES(LIBPNG, libpng)

LIBTBM_CFLAGS+="$LIBTBM_CFALGS $LIBDRM_CFLAGS $CAPI_CFLAGS $WL_CLIENT_CFLAGS $WL_SERVER_CFLAGS $LIBPNG_CFLAGS "
LIBTBM_LIBS+="$LIBTBM_LIBS $LIBDRM_LIBS $CAPI_LIBS $WL_CLIENT_LIBS $WL_SERVER_LIBS $LIBPNG_LIBS "

PKG_CHECK_EXISTS([dlog], [have_dlog="yes"], [have_dlog="no"])
AC_MSG_CHECKING([Have dlog logger])
AC_MSG_RESULT([${have_dlog}])
if test "x${have_dlog}" = "xyes"; then
    AC_DEFINE([HAVE_DLOG], [1], [Define to 1 if you have dlog])

    PKG_CHECK_MODULES(DLOG, dlog)

    LIBTBM_CFLAGS+="$DLOG_CFLAGS "
    LIBTBM_LIBS+="$DLOG_LIBS "
fi

AC_SUBST(LIBTBM_CFLAGS)
AC_SUBST(LIBTBM_LIBS)

HOST_CPU_X86_64=no
case $host_cpu in
  x86_64*|amd64*)
    HOST_CPU_X86_64=yes
esac

AM_CONDITIONAL(HOST_CPU_X86_64, test "x$HOST_CPU_X86_64" = "xyes")

AC_OUTPUT([
   src/Makefile
	Makefile
	libtbm.pc
	ut/Makefile])

echo ""
echo "CFLAGS            : $CFLAGS"
echo "LDFLAGS           : $LDFLAGS"
echo "LIBTBM_CFLAGS     : $LIBTBM_CFLAGS"
echo "LIBTBM_LIBS       : $LIBTBM_LIBS"
echo "BUFMGR_MODULE_DIR : $BUFMGR_MODULE_PATH"
echo ""

