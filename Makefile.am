
ACLOCAL_AMFLAGS = -I aclocal.d ${ACLOCAL_FLAGS}

AM_CFLAGS =				\
	$(NULL)

AM_LDFLAGS =				\
	$(NULL)

AM_ETAGSFLAGS =                         \
        --members                       \
        $(NULL)

SUBDIRS =				\
	. 				\
	t				\
	$(NULL)

lib_LTLIBRARIES =			\
	libgsr.la			\
	$(NULL)

EXTRA_DIST = 				\
	gsr.pc.in 			\
	gsr.m4    			\
	$(NULL)

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA =			\
	gsr.pc
	$(NULL)

m4datadir = $(datadir)/aclocal
m4data_DATA =				\
	gsr.m4				\
	$(NULL)

gsrincludedir = $(includedir)/gsr
gsrinclude_HEADERS = 			\
	gsr/gsr.h			\
	gsr/gsr-coerce.h		\
	gsr/gsr-subtype.h		\
	gsr/gsr-type.h			\
	$(NULL)

libgsr_la_SOURCES =			\
	gsr/gsr.c			\
	gsr/gsr-coerce.c		\
	gsr/gsr-type.c			\
	$(NULL)
