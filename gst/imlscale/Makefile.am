plugin_LTLIBRARIES = libgstimlscalebad.la

#ORC_SOURCE=gstvideofiltersbadorc
#include $(top_srcdir)/common/orc.mak

libgstvideofiltersbad_la_SOURCES = \
	gstimlscale.c
#nodist_libgstvideofiltersbad_la_SOURCES = $(ORC_NODIST_SOURCES)
libgstimlscalebad_la_CFLAGS = \
	$(GST_PLUGINS_BASE_CFLAGS) \
	$(GST_CFLAGS) \
	$(ORC_CFLAGS)
libgstimlscalebad_la_LIBADD = \
	$(GST_PLUGINS_BASE_LIBS) -lgstvideo-$(GST_API_VERSION) \
	$(GST_BASE_LIBS) \
	$(GST_LIBS) \
	$(ORC_LIBS) \
	$(LIBM)
libgstimlscalebad_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

noinst_HEADERS = \
	gstimlscale.h
