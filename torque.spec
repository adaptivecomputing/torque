
# comment out snap if building a real release
%define name torque
%define version 2.1.0p0
%define snap 200602181912
%define release 1cri

# The following options are supported:
#   --with server_name=hostname
#   --with homedir=directory
#   --with libdir=directory
#   --with includedir=directory
#   --with prefix=directory
#   --with[out] scp
#   --with[out] syslog
#   --with[out] rpp
#   --with[out] filesync
#   --with[out] wordexp
#   --with[out] gui
#   --with[out] tcl
# Note that prefix overrides homedir, libdir, and includedir


# Hrm, should we default to the name of the buildhost?  That seems only
# slightly better than picking a hostname at random.  This is exactly the kind
# of compile-time default that doesn't work well with distributable packages.
# Let's force the issue with the non-sensical "localhost".
#
# Note that "localhost" doesn't actually work.  You must either define the
# correct hostname here, pass '--with server_name=foo' to rpmbuild, or be sure
# that $PBS_SERVER_HOME/server_name contains the correct hostname.
%define server_name localhost

# change as you wish
%define use_syslog 1
%define use_scp 1
%define use_rpp 1
%define use_fsync 0
%define use_tcl 1
%define use_wordexp 1
%define build_gui 1

# these are non-defaults, but fit better into most RPM-based systems
%define torquehomedir %{_localstatedir}/lib/%{name}
%define _libdir       %{_prefix}/%{_lib}/%{name}/lib
%define _includedir   %{_prefix}/%{_lib}/%{name}/include




# --with/--without processing
# first, error if conflicting options are used
%{?_with_filesync: %{?_without_filesync: %{error: both _with_filesync and _without_filesync}}}
%{?_with_syslog: %{?_without_syslog: %{error: both _with_syslog and _without_syslog}}}
%{?_with_rpp: %{?_without_rpp: %{error: both _with_rpp and _without_rpp}}}
%{?_with_scp: %{?_without_scp: %{error: both _with_scp and _without_scp}}}
%{?_with_tcl: %{?_without_tcl: %{error: both _with_tcl and _without_tcl}}}
%{?_with_wordexp: %{?_without_wordexp: %{error: both _with_wordexp and _without_wordexp}}}
%{?_with_gui: %{?_without_gui: %{error: both _with_gui and _without_gui}}}

# did we find any --with options?
%{?_with_filesync: %define use_fsync 1}
%{?_with_syslog: %define use_syslog 1}
%{?_with_rpp: %define use_rpp 1}
%{?_with_scp: %define use_scp 1}
%{?_with_tcl: %define use_tcl 1}
%{?_with_wordexp: %define use_wordexp 1}
%{?_with_gui: %define build_gui 1}

%{?_with_server_name:%define server_name %(set -- %{_with_server_name}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_homedir:%define torquehomedir %(set -- %{_with_homedir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_libdir:%define _libdir %(set -- %{_with_libdir}; echo $1 | grep -v with | sed 's/=//')}
%{?_with_includedir:%define _includedir %(set -- %{_with_includedir}; echo $1 | grep -v with | sed 's/=//')}

%if %{?_with_prefix:1}%{!?_with_prefix:0}
%define _prefix %(set -- %{_with_prefix}; echo $1 | grep -v with | sed 's/=//')
%define torquehomedir %{_prefix}/spool
%define _libdir %{_prefix}/lib
%define _includedir %{_prefix}/include
%define _mandir %{_prefix}/man
%define _bindir %{_prefix}/bin
%define _sbindir %{_prefix}/sbin
%define _defaultdocdir %{_prefix}/doc
%endif

# did we find any --without options?
%{?_without_filesync: %define use_fsync 0}
%{?_without_syslog: %define use_syslog 0}
%{?_without_rpp: %define use_rpp 0}
%{?_without_scp: %define use_scp 0}
%{?_without_tcl: %define use_tcl 0}
%{?_without_wordexp: %define use_wordexp 0}
%{?_without_gui: %define build_gui 0}

# Set up all options as disabled
%define fsyncflags  --disable-filesync
%define syslogflags --disable-syslog
%define rppflags    --disable-rpp
%define scpflags    %{nil}
%define tclflags    --without-tcl --without-tclx
%define wordexpflags  --disable-wordexp
%define guiflags    --disable-gui

# Enable options that we want
%if %use_fsync
%define fsyncflags  --enable-filesync
%endif
%if %use_syslog
%define syslogflags --enable-syslog
%endif
%if %use_rpp
%define rppflags    --enable-rpp
%endif
%if %use_scp
%define scpflags    --with-scp
%endif
%if %use_wordexp
%define wordexpflags  --enable-wordexp
%endif

# dealing with tcl and gui is way too complicated
%if %build_gui
%define guiflags   --enable-gui
%define use_tcl 1
%endif

%if %use_tcl
%if %build_gui
%define tclflags    --with-tcl --with-tclx
%else
%define tclflags    --with-tcl --without-tclx
%endif
%endif

# finish up the configs...
%define server_nameflags --set-default-server=%{server_name}


%define shared_description %(echo -e "TORQUE (Tera-scale Open-source Resource and QUEue manager) is a resource \\nmanager providing control over batch jobs and distributed compute nodes.  \\nTorque is based on OpenPBS version 2.3.12 and incorporates scalability, \\nfault tolerance, and feature extension patches provided by USC, NCSA, OSC, \\nthe U.S. Dept of Energy, Sandia, PNNL, U of Buffalo, TeraGrid, and many \\nother leading edge HPC organizations.\\n\\nThis build was configured with:\\n  %{fsyncflags}\\n  %{syslogflags}\\n  %{tclflags}\\n  %{rppflags}\\n  %{server_nameflags}\\n  %{guiflags}\\n  %{wordexpflags}\\n  %{scpflags}\\n")


Summary: Tera-scale Open-source Resource and QUEue manager
Name: %{name}
Version: %{version}
Release: %{?snap:snap.%snap.}%{release}
Source: %{name}-%{version}%{?snap:-snap.%snap}.tar.gz
License: distributable
Group: System Environment/Daemons
URL: http://www.clusterresources.com/products/torque/
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Provides: pbs
BuildRequires: ed
Conflicts: pbspro, openpbs, openpbs-oscar
Obsoletes: scatorque

%if ! %build_gui
Obsoletes: torque-gui
%endif

%description
%shared_description
This package holds just a few shared files and directories.

%prep
%setup -n %{name}-%{version}
echo '#define PBS_VERSION "%{version}-%{release}"' > src/include/pbs_version.h


%build
CFLAGS="-fPIC %optflags -Wall -Wno-unused -std=gnu99 -pedantic -D_GNU_SOURCE"
export CFLAGS

for i in $(find . -name config.guess -o -name config.sub) ; do
   if [ -f /usr/lib/rpm/%{_host_vendor}/$(basename $i) ] ; then
       %{__rm} -f $i && %{__cp} -fv /usr/lib/rpm/%{_host_vendor}/$(basename $i) $i
   elif [ -f /usr/lib/rpm/$(basename $i) ] ; then
        %{__rm} -f $i && %{__cp} -fv /usr/lib/rpm/$(basename $i) $i
   fi
done

# autoconf and friends don't work with torque, so we can't use the
# various configure macros
./configure --prefix=%{_prefix} --sbindir=%{_sbindir} --bindir=%{_bindir} \
 --includedir=%{_includedir} --mandir=%{_mandir} --libdir=%{_libdir} \
 --enable-server --enable-clients --enable-mom --enable-docs %{guiflags} \
 --set-server-home=%{torquehomedir} %{server_nameflags} --set-cflags="$CFLAGS" \
 %{fsyncflags} %{syslogflags} %{tclflags} %{rppflags} %{scpflags} %{wordexpflags}



%{__make} clean
%{__make} depend
%{__make} %{_smp_mflags} all
 


%install
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf "$RPM_BUILD_ROOT"

# we don't use makeinstall because we don't support DESTDIR (as happens on Suse)
%{__make} \
    sbindir=$RPM_BUILD_ROOT%{_sbindir} \
    bindir=$RPM_BUILD_ROOT%{_bindir} \
    includedir=$RPM_BUILD_ROOT%{_includedir} \
    mandir=$RPM_BUILD_ROOT%{_mandir} \
    libdir=$RPM_BUILD_ROOT%{_libdir} \
    PBS_SERVER_HOME=$RPM_BUILD_ROOT%{torquehomedir} \
  install

# Correct the tclIndex files that are broken when installing into a fakeroot
%if %build_gui
pat=$(echo $RPM_BUILD_ROOT | sed 's/\// /g')
for indexfile in $RPM_BUILD_ROOT%{_libdir}/*/tclIndex; do
  %__sed -e "s/$pat//" < $indexfile > tcltmp
  %__cat < tcltmp > $indexfile
done
%__rm -f tcltmp
%endif

# install initscripts
%{__mkdir_p} $RPM_BUILD_ROOT%{_initrddir}
for initscript in pbs_mom pbs_sched pbs_server; do
  %__sed -e 's|^PBS_HOME=.*|PBS_HOME=%{torquehomedir}|' \
         -e 's|^PBS_DAEMON=.*|PBS_DAEMON=%{_sbindir}/'$initscript'|' \
        < contrib/init.d/$initscript > $RPM_BUILD_ROOT%{_initrddir}/$initscript
  %__chmod 755 $RPM_BUILD_ROOT%{_initrddir}/$initscript
done

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT


%post
if %__grep -q "PBS services" /etc/services;then
   : PBS services already installed
else
   cat<<-__EOF__>>/etc/services
	# Standard PBS services
	pbs           15001/tcp           # pbs server (pbs_server)
	pbs           15001/udp           # pbs server (pbs_server)
	pbs_mom       15002/tcp           # mom to/from server
	pbs_mom       15002/udp           # mom to/from server
	pbs_resmom    15003/tcp           # mom resource management requests
	pbs_resmom    15003/udp           # mom resource management requests
	pbs_sched     15004/tcp           # scheduler
	pbs_sched     15004/udp           # scheduler
	__EOF__
fi


%files
%defattr(-, root, root)
%doc INSTALL README.torque torque.setup Release_Notes CHANGELOG PBS_License.txt
%config(noreplace) %{torquehomedir}/pbs_environment
%config(noreplace) %{torquehomedir}/server_name
%{torquehomedir}/aux
%{torquehomedir}/spool


%package docs
Group: Documentation
Summary: docs for Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-docs
%description docs
%shared_description
This package holds the documentation files.

%files docs
%defattr(-, root, root)
%doc doc/admin_guide.ps
%{_mandir}/man*/*

%package scheduler
Group: System Environment/Daemons
Summary: scheduler part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-scheduler
%description scheduler
%shared_description
This package holds the fifo C scheduler.

%files scheduler
%defattr(-, root, root)
%{_sbindir}/pbs_sched
%{_initrddir}/pbs_sched
%dir %{torquehomedir}/sched_priv
%config(noreplace) %{torquehomedir}/sched_priv/*
%{torquehomedir}/sched_logs

%post scheduler
/sbin/chkconfig --add pbs_sched

%preun scheduler
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_sched


%package server
Group: System Environment/Daemons
Summary: server part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-server
%description server
%shared_description
This package holds the server.

%files server
%defattr(-, root, root)
%{_sbindir}/pbs_server
%{_sbindir}/momctl
%{_initrddir}/pbs_server
%{torquehomedir}/server_logs
%{torquehomedir}/server_priv

%post server
/sbin/chkconfig --add pbs_server

%preun server
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_server


%package mom
Group: System Environment/Daemons
Summary: execution part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-mom
%description mom
%shared_description
This package holds the execute daemon required on every node.

%files mom
%defattr(-, root, root)
%{_sbindir}/pbs_mom
%{_initrddir}/pbs_mom
%if ! %{use_scp}
%attr(4755 root root) %{_sbindir}/pbs_rcp
%endif
%{torquehomedir}/mom_priv/*
%{torquehomedir}/mom_logs
%{torquehomedir}/checkpoint
%{torquehomedir}/undelivered

%post mom
/sbin/chkconfig --add pbs_mom

%preun mom
[ $1 = 0 ] || exit 0
/sbin/chkconfig --del pbs_mom


%package client
Group: Applications/System
Summary: client part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-client
%description client
%shared_description
This package holds the command-line client programs.

%files client
%defattr(-, root, root)
%{_bindir}/q*
%{_bindir}/chk_tree
%{_bindir}/hostn
%{_bindir}/nqs2pbs
%{_bindir}/pbsdsh
%{_bindir}/pbsnodes
%{_bindir}/printjob
%{_bindir}/printtracking
%{_bindir}/tracejob
%attr(4755 root root) %{_sbindir}/pbs_iff
%{_sbindir}/pbs_demux
%if %use_tcl
%{_bindir}/pbs_tclsh
%endif

%package gui
Group: Applications/System
Summary: graphical client part of Torque
Requires: %{name}-client = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: xpbs xpbsmon
%description gui
%shared_description
This package holds the graphical clients.

%if %{build_gui}
%files gui
%defattr(-, root, root)
%{_bindir}/pbs_wish
%{_bindir}/xpbs
%{_bindir}/xpbsmon
%{_libdir}/xpbs
%{_libdir}/xpbsmon
%endif


%package localhost
Group: Applications/System
Summary: installs and configures a minimal localhost-only batch queue system
PreReq: pbs-mom pbs-server pbs-client pbs-scheduler

%description localhost
%shared_description
This package installs and configures a minimal localhost-only batch queue system.

%files localhost
%defattr(-, root, root)
%post localhost
/sbin/chkconfig pbs_mom on
/sbin/chkconfig pbs_server on
/sbin/chkconfig pbs_sched on
/bin/hostname --long > %{torquehomedir}/server_priv/nodes
/bin/hostname --long > %{torquehomedir}/server_name
/bin/hostname --long > %{torquehomedir}/mom_priv/config
pbs_server -t create
qmgr -c "s s scheduling=true"
qmgr -c "c q batch queue_type=execution"
qmgr -c "s q batch started=true"
qmgr -c "s q batch enabled=true"
qmgr -c "s q batch resources_default.nodes=1"
qmgr -c "s q batch resources_default.walltime=3600"
qmgr -c "s s default_queue=batch"
%{_initrddir}/pbs_mom restart
%{_initrddir}/pbs_sched restart
%{_initrddir}/pbs_server restart
qmgr -c "s n `/bin/hostname --long` state=free" -e

%package devel
Summary: Development tools for programs which will use the %{name} library.
Group: Development/Libraries
Provides: lib%{name}-devel
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}

%description devel
%shared_description
This package includes the header files and static libraries
necessary for developing programs which will use %{name}.

%files devel
%defattr(-, root, root)
%{_libdir}/*.*a
%{_includedir}/*.h

%pre devel
# previous versions of this spec file installed these as symlinks
test -L %{_libdir} && rm -f %{torquelibdir}
test -L %{_includedir} && rm -f %{_includedir}
exit 0



