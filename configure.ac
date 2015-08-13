
AC_PREREQ([2.60])
AC_INIT([gsr],[1.0.0],[happy.barney@gmail.com])
AC_CONFIG_SRCDIR([gsr/gsr.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([aclocal.d])
AC_CONFIG_AUX_DIR([acaux.d])

AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

AC_DEFINE([VERSION], [1.0.0], [Library version])

AC_PROG_CC
AC_PROG_CC_C99

AM_PATH_GLIB_2_0([2.40.0], [], [])
AM_PATH_CMX

AC_CHECK_SIZEOF([gpointer], [], [#include <glib.h>])
AC_CHECK_SIZEOF([gint64],   [], [#include <glib.h>])

# Where to install pkg-config data
PKG_INSTALLDIR

AC_CONFIG_HEADERS([
    gsr/gsr-gint64.h
])

AC_CONFIG_FILES([
    Makefile
    t/Makefile
    gsr.pc
])

AC_OUTPUT
