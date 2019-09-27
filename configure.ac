AC_INIT([screenshoter],
        [0.1],
        [https://github.com/linnea-s/screenshoter/issues],
        [screenshoter],
        [https://github.com/linnea-s/screenshoter])
AC_PREREQ([2.64])
AC_CONFIG_SRCDIR([screenshoter.c])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-xz tar-ustar check-news])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG

GTK3_REQUIRED=3.6

dnl pkg-config dependency checks

PKG_CHECK_MODULES(GTK3, gtk+-3.0 >= $GTK3_REQUIRED)
PKG_CHECK_MODULES(POPT, popt)

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT
