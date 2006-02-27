include $(top_srcdir)/buildutils/config.mk

DIST_SUBDIRS =   cmds  iff      lib      resmom          scheduler.cc   server \
 gui scheduler.basl   include  mom_rcp    scheduler.tcl  tools


if INCLUDE_SERVER
SERVER_DIRS = server @SCHD_TYPE_SUBDIR@
endif

if INCLUDE_MOM
RESMOM_DIR = resmom
if INCLUDE_MOM_RCP
MOMRCP_DIR = mom_rcp
endif
MOM_DIRS = $(RESMOM_DIR) $(MOMRCP_DIR)
endif

if INCLUDE_CLIENTS
CLIENTS_DIRS = iff cmds tools
endif

if INCLUDE_GUI
GUI_DIRS = gui
endif

SUBDIRS = include lib $(SERVER_DIRS) $(MOM_DIRS) $(CLIENTS_DIRS) $(GUI_DIRS)

install_mom:
	for dir in $(MOM_DIRS) ;do (cd $$dir && $(MAKE) install);done

install_server:
	for dir in $(SERVER_DIRS) ;do (cd $$dir && $(MAKE) install);done

install_clients:
	for dir in $(CLIENTS_DIRS) ;do (cd $$dir && $(MAKE) install);done

install_gui:
	for dir in $(GUI_DIRS) ;do (cd $$dir && $(MAKE) install);done

install_devel:
	for dir in include lib ;do (cd $$dir && $(MAKE) install);done
