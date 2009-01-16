#!/bin/bash


if [ `id -u` != 0 ]; then
    echo "This script should be run as root"
    exit -1
fi

echo ""
echo "This script is somewhat naive. It will attempt each step, but"
echo "doesn't actually check if the step was successfull. Please"
echo "mind the output of the commands it runs, and be sure that"
echo "each part completes without errors"
echo ""
echo "Press any key to continue, or ctrl-c to quit"

read carryon


# Steps to be completed
USERSADDED="no"
PROGRAMSINSTALLED="no"
PERLLIBSINSTALLED="no"
COREDUMPSIZE="no"
COREDUMPDIR="no"
TESTCONFIGED="no"

########################
# Adding testing users
########################

echo "I am going to add two new groups and users"
echo "users: testuser1, testuser2"
echo "groups: testgroup1, testgroup2"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	groupadd testgroup1 
	groupadd testgroup2 
	useradd -m -s /bin/bash -g testgroup1 -G testgroup1 testuser1 
	useradd -m -s /bin/bash -g testgroup2 -G testgroup2 testuser2 
	echo "Note: No passwords were set for these users. (not needed for testing)."
	USERSADDED="yes"
	;;
    *) 
	echo "Not adding users!"
	;;
esac
echo ""

##########################
# Add paths to .bashrc
##########################

echo "I am going to append /usr/local/perltests/bin to your PERL5LIB in .bashrc for root"
echo "I am going to append /usr/local/perltest/bin plus some over Perltest environment PATHS"
echo "to your PATH in .bashrc for root, testuser1, and testuser2"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	echo "# Added by Moab::Test quick_setup.sh" >> ~/.bashrc
	echo "export PERL5LIB='/usr/local/perltests/bin:$PERL5LIB'" >> ~/.bashrc
	echo "export PATH='/usr/local/perltests/bin:$PATH'" >> ~/.bashrc
	
	echo "# Added by Moab::Test quick_setup.sh" >> /home/testuser1/.bashrc
	echo "export PATH=\"/usr/local/perltests/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/bin/:/root/bin:/usr/local/apitest/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/gold/bin:/usr/test/gold/sbin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:/usr/test/slurm/sbin:/usr/test/slurm/bin:$PATH\"" >> /home/testuser1/.bashrc
	
	echo "# Added by Moab::Test quick_setup.sh" >> /home/testuser2/.bashrc
	echo "export PATH=\"/usr/local/perltests/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/bin/:/root/bin:/usr/local/apitest/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/gold/bin:/usr/test/gold/sbin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:/usr/test/slurm/sbin:/usr/test/slurm/bin:$PATH\"" >> /home/testuser2/.bashrc
	PROGRAMSINSTALLED="yes"
	;;
    *)
	echo "Not modifying your PERL5LIB or PATH!"
	;;
esac
echo ""

#########################
# Enable Unlimited Coredumping
#########################

echo "I am going to set the coredump file size to unlimited by adding 'ulimit -c unlimited' to the .bashrc for root"
echo "Note: Needed for automated Coredump detection in Moab::Test module"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	echo "# Added by Moab::Test quick_setup.sh" >> ~/.bashrc
	echo "ulimit -c unlimited" >> ~/.bashrc
	COREDUMPSIZE="yes"
	;;
    *)
	echo "Not changing default coredump file size!"
	;;
esac
echo ""

####################################
# Set Coredump Pattern for Detection
####################################

echo "I am going to make it so all coredump files are stored in the format /tmp/<executable>-core.<pid>"
echo "by adding the line 'echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern' to root's .bashrc file"
echo "Note: Needed for automated Coredump detection in Moab::Test module"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	echo "echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern" >> ~/.bashrc
	COREDUMPDIR="yes"
	;;
    *)
	echo "Not changing default coredump template!"
	;;
esac
echo ""

################################
# Install packages required by tests
################################

programs="subversion build-essential psmisc autoconf libxml2-dev zlib1g zlib1g-dev bzip2 openssl libssl-dev nfs-common ia32-libs" 
echo "I am going to install needed programs with apt-get."
echo "I will be installing $programs"
echo ""
echo "If you are not on Ubuntu or Debian, say no here, and install these programs"
echo "(or their equivalents) yourself."
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	apt-get update 
	apt-get install $programs 
	PROGRAMSINSTALLED="yes"
	;;
    *)	
	echo "Not installing $programs!"
	;;
esac
echo ""

#############################
# Install Perl Modules required by tests
#############################

libraries="IPC::Run3 Test::More Data::Properties TAP::Harness XML::LibXML XML::Simple Proc::Daemon Expect"
echo "I am going to use CPAN to install needed Perl libraries"
echo "Libraries are $libraries"
echo ""
echo "You may need to configure CPAN before this will work (it will prompt you if needed"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	cpan -i $libraries 
	PERLLIBSINSTALLED="yes"
	;;
    *)	
	echo "Not installing $libraries!"
	;;
esac
echo ""

###############################
# Basic Dir-Structure configuration
###############################

echo "I am going to set up the default QA-tests configuration" 
echo "/usr/test/src will be created, source code will be checked out to this location"
echo "clustertest.conf will be copied to /etc. Be sure to configure it for your setup"
echo "moab5.3.0.props will be copied to /tmp/clustertest.props so you can run"
echo "the release_dev.bat test immediately after this configuration script is done"
echo ""
echo "Is this alright? [y/N]"

continue="n"
read continue

case "$continue" in
    [Yy]*) 
	mkdir -p /usr/test/src
	cp /usr/local/perltests/lib/clustertest.conf /etc
	sed -i -r s/MoabHost=.*/MoabHost=`hostname`/g /etc/clustertest.conf
	cp /usr/local/perltests/etc/props/moab5.3.0.props /tmp/clustertest.props
	TESTCONFIGED="yes"
	;;
    *)	
    echo "Did not create default configuration!"
	;;
esac
echo ""

echo "Summary: "
echo "-------------------"
echo -n "Users added: "
echo $USERSADDED
echo -n "Needed programs installed: " 
echo $PROGRAMSINSTALLED
echo -n "Perl libraries installed: " 
echo $PERLLIBSINSTALLED
echo -n "Coredump filesize changed:"
echo $COREDUMPSIZE
echo -n "Coredump template changed:"
echo $COREDUMPDIR
echo -n "Tests configured: " 
echo $TESTCONFIGED
