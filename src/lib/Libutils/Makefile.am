include $(top_srcdir)/buildutils/config.mk

if HAVE_CHECK
SUBDIRS = test
endif

noinst_LIBRARIES = libutils.a

libutils_a_SOURCES = u_groups.c u_tree.c u_mu.c u_MXML.c u_xml.c \
										 u_threadpool.c u_resizable_array.c u_hash_table.c \
                     u_mom_hierarchy.c u_dynamic_string.c \
                     u_hash_map_structs.c u_memmgr.c


