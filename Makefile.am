## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po 

arpagedocdir = ${prefix}/share/doc/arpage
arpagedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(arpagedoc_DATA) \
	$(INTLTOOL_FILES)\
	$(patches_DATA) \
	$(patches_DATA)

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(arpagedocdir)
