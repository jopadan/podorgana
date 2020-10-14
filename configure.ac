AC_INIT([podorgana], [0.1], [jopadan@ctemplar.com])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_LIBTOOL

AC_SEARCH_LIBS(pow,m)
AC_SEARCH_LIBS(pthread_create,pthread)
AC_SEARCH_LIBS(dlopen,dl)
AC_SEARCH_LIBS(is_pod,libpodfmt)
AC_CONFIG_FILES([libpodfmt/Makefile
		Makefile])
AC_OUTPUT

