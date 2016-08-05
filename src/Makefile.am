SUBDIRS =

AM_CFLAGS = \
	$(WARN_CFLAGS) \
	-I./ \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	@LIBTBM_CFLAGS@

libtbm_la_LTLIBRARIES = libtbm.la
libtbm_ladir = $(libdir)
libtbm_la_LDFLAGS = -version-number 1:0:0 -no-undefined
libtbm_la_LIBADD = @LIBTBM_LIBS@ @CLOCK_LIB@ -ldl

libtbm_la_SOURCES = \
	tbm_surface_internal.c \
	tbm_surface.c \
	tbm_surface_queue.c \
	tbm_bufmgr_backend.c \
	tbm_bufmgr.c \
	tbm_drm_helper_server.c \
	tbm_drm_helper_client.c \
	tbm_sync.c

nodist_libtbm_la_SOURCES =		\
	wayland-tbm-drm-auth-server-protocol.h	\
	wayland-tbm-drm-auth-client-protocol.h	\
	wayland-tbm-drm-auth-protocol.c

%-protocol.c : $(top_srcdir)/protocol/%.xml
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(wayland_scanner) code < $< > $@

%-server-protocol.h : $(top_srcdir)/protocol/%.xml
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(wayland_scanner) server-header < $< > $@

%-client-protocol.h : $(top_srcdir)/protocol/%.xml
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(wayland_scanner) client-header < $< > $@

BUILT_SOURCES = $(nodist_libtbm_la_SOURCES)

libtbmincludedir=$(includedir)
libtbminclude_HEADERS = tbm_bufmgr.h tbm_surface.h tbm_bufmgr_backend.h tbm_type.h tbm_surface_internal.h tbm_surface_queue.h tbm_drm_helper.h tbm_sync.h

CLEANFILES = $(BUILT_SOURCES)
