## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/arpage/ui
ui_DATA = arpage.ui \
	zonage.ui\
	ledred_on.png\
	ledred_off.png \
	ledgreen_on.png \
	ledgreen_off.png \
	ledblue_on.png \
	ledblue_off.png \
	arpage.png \
	zonage.png \
	arpageps.ui

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(ARPAGE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = arpage \
	zonage cs102_filter

zonage_SOURCES = \
	event_zone.cc\
	midimsg.cc \
	ccrange.h

cs102_filter_SOURCES = \
	cs102_filter.cc\
	midimsg.cc \
	ccrange.h

zonage_LDADD = \
	$(ARPAGE_LIBS)

cs102_filter_LDADD = \
	$(ARPAGE_LIBS)

arpage_SOURCES = \
	main.cc \
	jack_process.cc \
	arpui.cc \
	arprun.cc \
	arppatt.cc \
	scales.cc \
	midimsg.cc \
	arpage.h \
	arppatt.h \
	arprun.h \
	arpui.h \
	jack_process.h \
	midimsg.h \
	pattelem.h \
	scales.h \
	ccrange.h \
	arpstorage.cc \
	arpstorage.h 

arpage_LDFLAGS = 

arpage_LDADD = $(ARPAGE_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/arpage
