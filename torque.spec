
# comment out snap if building a real release
%define name torque
%define version TVERSION 
#%define snap TSNAP
%define release 1cri


# Hrm, should we default to the name of the buildhost?  That seems only
# slightly better than picking a hostname at random.  This iis exactly the
# kind of compile-time default that doesn't work well with distributable
# packages.  Let's force the issue with the non-sensical "localhost".
#
# Note that "localhost" doesn't actually work.  You must either define
# the correct hostname here, or be sure that $PBS_SERVER_HOME/server_name
# contains the correct hostname.
%define server_name localhost

# change as you wish
%define use_tcl 1
%define use_syslog 1
%define use_scp 1
%define use_rpp 0
%define use_fsync 0

# these are non-defaults, but fit better into most RPM-based systems
%define torquehomedir %{_localstatedir}/lib/%{name}
%define torquelibdir  %{_libdir}/%{name}
%define torquemandir  %{_mandir}



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

%if %use_tcl
BuildRequires: tclx, tcl-devel, tk-devel
%else
Obsoletes: torque-gui
%endif



# don't change these
%define fsyncflags  --disable-filesync
%define syslogflags --disable-syslog
%define tclflags    --disable-gui --without-tclx --without-tcl
%define rppflags    --disable-rpp
%define scpflags    --without-scp

%if %use_fsync
%define fsyncflags  --enable-filesync
%endif
%if %use_syslog
%define syslogflags --enable-syslog
%endif
%if %use_tcl
%define tclflags    --enable-gui --with-tclx --with-tcl
%endif
%if %use_rpp
%define rppflags    --enable-rpp
%endif
%if %use_scp
%define scpflags    --with-scp
%endif

%define shared_description %(echo -e "TORQUE (Tera-scale Open-source Resource and QUEue manager) is a resource \\nmanager providing control over batch jobs and distributed compute nodes.  \\nTorque is based on OpenPBS version 2.3.12 and incorporates scalability, \\nfault tolerance, and feature extension patches provided by USC, NCSA, OSC, \\nthe U.S. Dept of Energy, Sandia, PNNL, U of Buffalo, TeraGrid, and many \\nother leading edge HPC organizations.\\n\\nThis build was configured with:\\n  %{fsyncflags}\\n  %{syslogflags}\\n  %{tclflags}\\n  %{rppflags}\\n  %{scpflags}\\n")


%description
%shared_description
This package holds just a few shared files and directories.

%prep
%setup -n %{name}-%{version}


%build
CFLAGS="-fPIC %optflags -Wall -std=gnu99  -pedantic -D_GNU_SOURCE"
export CFLAGS

# The config.guess in torque *was* ancient, but let's do it anyways
for i in $(find . -name config.guess -o -name config.sub) ; do
    [ -f /usr/lib/rpm/$(basename $i) ] && \
        %{__rm} -f $i && %{__cp} -fv /usr/lib/rpm/$(basename $i) $i
done

# autoconf and friends to work with torque, so we can't use the
# various configure macros
./configure --prefix=%{_prefix} --sbindir=%{_sbindir} --bindir=%{_bindir} \
  --includedir=%{_includedir} --mandir=%{torquemandir} --libdir=%{torquelibdir} \
  --enable-docs --enable-server --enable-mom --enable-clients \
  --set-server-home=%{torquehomedir} --set-default-server=%{server_name} \
  %{fsyncflags} %{syslogflags} %{tclflags} %{rppflags} %{scpflags}



%{__make} clean
%{__make} %{_smp_mflags} depend
%{__make} %{_smp_mflags} all


%install
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf "$RPM_BUILD_ROOT"

%{makeinstall} libdir=$RPM_BUILD_ROOT%{torquelibdir} PBS_SERVER_HOME=$RPM_BUILD_ROOT%{torquehomedir}

# Kind of gross, but it's easier to get maui/mpiexec/etc to build with these
%__ln_s . $RPM_BUILD_ROOT%{torquelibdir}/lib
%__ln_s %{_includedir} $RPM_BUILD_ROOT%{torquelibdir}/include


%{__mkdir_p} $RPM_BUILD_ROOT%{_initrddir}
for initscript in pbs_mom pbs_sched pbs_server; do
  %__sed -e 's|^PBS_PREFIX=.*|PBS_PREFIX=%{_prefix}|' \
      -e 's|^PBS_HOME=.*|PBS_HOME=%{torquehomedir}|' \
      -e 's|^PBS_DAEMON=.*|PBS_DAEMON=%{_sbindir}/'$initscript'|' \
        < contrib/init.d/$initscript > $RPM_BUILD_ROOT%{_initrddir}/$initscript
  %__chmod 755 $RPM_BUILD_ROOT%{_initrddir}/$initscript
done


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
%config(noreplace) %{torquehomedir}/pbs_environment
%config(noreplace) %{torquehomedir}/server_name
%{torquehomedir}/aux
%{torquehomedir}/checkpoint
%{torquehomedir}/undelivered
%{torquehomedir}/spool


%package docs
Group: Documentation
Summary: docs for Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-docs
%description docs
%shared_description
This package holds the documentation files
%files docs
%defattr(-, root, root)
%{torquemandir}/man*/*
%doc doc/admin_guide.ps INSTALL README.torque torque.setup Release_Notes CHANGELOG PBS_License.txt

%package scheduler
Group: System Environment/Daemons
Summary: scheduler part of Torque
Requires: %{name} = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: pbs-scheduler
%description scheduler
%shared_description
This package holds the fifo C scheduler

%files scheduler
%defattr(-, root, root)
%{_sbindir}/pbs_sched
%{_initrddir}/pbs_sched
%{torquehomedir}/sched_priv
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
This package holds the server

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
This package holds the command-line client programs
%files client
%defattr(-, root, root)
%{_bindir}/q*
%{_bindir}/chk_tree
%{_bindir}/hostn
%{_bindir}/nqs2pbs
%{_bindir}/pbsdsh
%{_bindir}/pbsnodes
%{_bindir}/printjob
%{_bindir}/tracejob
%attr(4755 root root) %{_sbindir}/pbs_iff
%{_sbindir}/pbs_demux

%package gui
Group: Applications/System
Summary: graphical client part of Torque
Requires: %{name}-client = %{?epoch:%{epoch}:}%{version}-%{release}
Provides: xpbs xpbsmon
%description gui
%shared_description
This package holds the graphical clients
%if %use_tcl
%files gui
%defattr(-, root, root)
%{_bindir}/pbs_wish
%{_bindir}/pbs_tclsh
%{_bindir}/xpbs
%{_bindir}/xpbsmon
%{torquelibdir}/xpbs/*
%{torquelibdir}/xpbsmon/*
%endif


%package localhost
Group: Applications/System
Summary: installs and configures a minimal localhost-only batch queue system
PreReq: pbs-mom pbs-server pbs-client pbs-scheduler
%description localhost
%shared_description
This package installs and configures a minimal localhost-only batch queue system
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
The %{name}-devel package includes the header files and static libraries
necessary for developing programs which will use the %{name} library.

%files devel
%defattr(-, root, root)
%{torquelibdir}/*.*a
%{torquelibdir}/lib
%{torquelibdir}/include
%{_includedir}/*.h

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && %{__rm} -rf $RPM_BUILD_ROOT



