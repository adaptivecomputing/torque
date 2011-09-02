include $(top_srcdir)/buildutils/config.mk

DIST_SUBDIRS = cmds daemon_client lib resmom scheduler.cc server gui \
 scheduler.basl include mom_rcp scheduler.tcl tools pam drmaa momctl


if INCLUDE_SERVER
SERVER_DIRS = server @SCHD_TYPE_SUBDIR@
endif

if INCLUDE_MOM
RESMOM_DIR = resmom momctl
if INCLUDE_MOM_RCP
MOMRCP_DIR = mom_rcp
endif
MOM_DIRS = $(RESMOM_DIR) $(MOMRCP_DIR)
endif

if INCLUDE_CLIENTS
CLIENTS_DIRS = daemon_client cmds tools momctl
endif

if INCLUDE_GUI
GUI_DIRS = gui tools
endif

if INCLUDE_PAM
PAM_DIRS = pam
endif

if INCLUDE_DRMAA
DRMAA_DIRS = drmaa
endif

SUBDIRS = include lib $(SERVER_DIRS) $(MOM_DIRS) $(CLIENTS_DIRS) $(GUI_DIRS) $(PAM_DIRS) $(DRMAA_DIRS)

install_mom:
	for dir in $(MOM_DIRS) lib ;do (cd $$dir && $(MAKE) install);done

install_server:
	for dir in $(SERVER_DIRS) lib ;do (cd $$dir && $(MAKE) install);done

install_clients:
	for dir in $(CLIENTS_DIRS) lib ;do (cd $$dir && $(MAKE) install);done

install_gui:
	for dir in $(GUI_DIRS) ;do (cd $$dir && $(MAKE) install_gui);done

install_devel:
	for dir in include lib ;do (cd $$dir && $(MAKE) install);done

install_lib:
	for dir in lib ;do (cd $$dir && $(MAKE) install);done

install_pam:
	for dir in $(PAM_DIRS) ;do (cd $$dir && $(MAKE) install);done

install_drmaa:
	for dir in $(DRMAA_DIRS) ;do (cd $$dir && $(MAKE) install);done
