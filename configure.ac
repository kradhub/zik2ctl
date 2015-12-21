AC_PREREQ([2.69])
AC_INIT([zik2ctl], [0.1])

AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_SRCDIR([src/zik2ctl.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

dnl check glib
PKG_CHECK_MODULES(GLIB, [glib-2.0], [],
                  [AC_MSG_ERROR([This package requires glib to compile.])])
PKG_CHECK_MODULES(GIO, [gio-2.0], [],
                  [AC_MSG_ERROR([This package requires gio to compile.])])
PKG_CHECK_MODULES(GIO_UNIX, [gio-unix-2.0], [],
                  [AC_MSG_ERROR([This package requires gio-unix to compile.])])

dnl set gdbus-codegen
GDBUS_CODEGEN="gdbus-codegen"
AC_SUBST(GDBUS_CODEGEN)

AC_OUTPUT(Makefile
          src/Makefile)
