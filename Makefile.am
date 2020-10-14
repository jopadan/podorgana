SUBDIRS = libpodfmt
bin_PROGRAMS = podorgana
ACLOCAL_AMFLAGS = -I m4
podorgana_CFLAGS = -std=gnu17
podorgana_SOURCES = podorgana.c
podorgana_LDADD = $(top_builddir)/libpodfmt/libpodfmt.la
AM_CPPFLAGS = -I$(top_srcdir)/libpodfmt

