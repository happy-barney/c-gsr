
gsrincludedir 	= $(includedir)/gsr
pkgconfigdir	= @pkgconfigdir@
m4datadir 	= $(datadir)/aclocal

SUBDIRS =				\
	. 				\
	t				\
	$(NULL)

ACLOCAL_AMFLAGS = -I aclocal.d ${ACLOCAL_FLAGS}

AM_CFLAGS =				\
	$(NULL)

AM_LDFLAGS =				\
	$(NULL)

AM_ETAGSFLAGS =                         \
        --members                       \
        $(NULL)

lib_LTLIBRARIES =			\
	libgsr.la			\
	$(NULL)

EXTRA_DIST = 				\
	gsr.pc.in 			\
	gsr.m4    			\
	$(NULL)

pkgconfig_DATA =			\
	gsr.pc 				\
	$(NULL)

m4data_DATA =				\
	gsr.m4				\
	$(NULL)

gsrinclude_HEADERS = 			\
	gsr/gsr.h			\
	gsr/gsr-gint64.h		\
	gsr/gsr-resolve.h		\
	gsr/gsr-symbol.h		\
	gsr/gsr-type.h			\
	gsr/gsr-value.h			\
	$(NULL)

libgsr_la_SOURCES =			\
	gsr/gsr.c			\
	gsr/gsr-gint64.c		\
	gsr/gsr-resolve.c		\
	gsr/gsr-symbol.c		\
	gsr/gsr-type.c			\
	gsr/gsr-value.c			\
	$(NULL)
