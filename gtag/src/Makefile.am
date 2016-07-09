## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gtag

gtag_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	gtag.h \
	tap.c \
	dlc5.h dlc5.c \
	wiggler.h wiggler.c \
	ppdev.h ppdev.c	

gtag_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

