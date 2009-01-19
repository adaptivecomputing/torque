#!/usr/bin/perl

use strict;
use warnings;

###############################################################################
# Variables
###############################################################################
my %action_taken      = (
                          '01 USERS ADDED'                 => 'no',
                          '02 PATHS MODIFIED'              => 'no',
                          '03 COREDUMP SIZE'               => 'no',
                          '04 COREDUMP TEMPLATE'           => 'no',
                          '05 PROGRAMS INSTALLED'          => 'no',
                          '06 PERLLIBS INSTALLED'          => 'no',
#                          '07 TEST ENVIRONMENT CONFIGURED' => 'no'
                        );

my $stdin;
my $div               = "\n================================================================================\n\n";
my $yes_regex         = '^(y|yes)$';
my $root              = 'root';
my $group1            = 'testgroup1';
my $group2            = 'testgroup2';
my $user1             = 'testuser1';
my $user2             = 'testuser2';
my $root_bashrc_loc   = '/root/.bashrc';
my $user1_bashrc_loc  = "/home/$user1/.bashrc";
my $user2_bashrc_loc  = "/home/$user2/.bashrc";

###############################################################################
# Check that the user is $root
###############################################################################
my $user = $ENV{ 'USER' };

die("This script should be run as $root")
  unless ($user eq $root);

###############################################################################
# Explain the script
###############################################################################
my $script_explain =<<EXPLAIN;

This script is somewhat naive. It will attempt each step, but doesn't actually 
check if the step was successfull. Please mind the output of the commands it 
runs, and be sure that each part completes without errors

Press any key to continue, or ctrl-c to quit
EXPLAIN

chomp $script_explain;
print $script_explain;
$stdin = <STDIN>;

###############################################################################
# Add the testing users
###############################################################################
my $users_explain =<<USERS_EXPLAIN;
I am going to add two new groups and users

 users: $user1, $user2
groups: $group1, $group2

Is this alright? [y/N] 
USERS_EXPLAIN

print $div;
chomp $users_explain;
print $users_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  my $groupadd1 = `groupadd $group1`;
  my $groupadd2 = `groupadd $group2`;
  my $useradd1  = `useradd -m -s /bin/bash -g $group1 -G testgroup1 $user1`;
  my $useradd2  = `useradd -m -s /bin/bash -g $group2 -G testgroup2 $user2`;

  print "\nNOTE: No passwords were set for these users. (not needed for testing).\n";
  $action_taken{ '01 USERS ADDED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot adding users!\n";

  } # END else

###############################################################################
# Add paths to bashrc
###############################################################################
my $path_explain =<<PATH_EXPLAIN;
I am going to append /usr/local/perltests/bin to your PERL5LIB in .bashrc for $root
I am going to append /usr/local/perltest/bin plus some over Perltest environment PATHS
to your PATH in .bashrc for $root, $user1, and $user2

Is this alright? [y/N] 
PATH_EXPLAIN

print $div;
chomp $path_explain;
print $path_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  # Set the $root .bashrc
  if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "export PERL5LIB='/usr/local/perltests/bin:\$PERL5LIB'\n";
    print ROOT_BASHRC "export PATH='/usr/local/perltests/bin:\$PATH'\n";

    close ROOT_BASHRC;

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else
	
  # Set the $user1 .bashrc
  if (open (USER1_BASHRC, ">>$user1_bashrc_loc"))
    {

    print USER1_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print USER1_BASHRC "export PATH=\"/usr/local/perltests/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/bin/:/root/bin:/usr/local/apitest/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/gold/bin:/usr/test/gold/sbin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:/usr/test/slurm/sbin:/usr/test/slurm/bin:\$PATH\"\n";

    close USER1_BASHRC;

    } # END if (open (USER1_BASHRC, ">>$user1_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$user1_bashrc_loc': $!";

    } # END else
	
  # Set the $user2 .bashrc
  if (open (USER2_BASHRC, ">>$user2_bashrc_loc"))
    {

    print USER2_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print USER2_BASHRC "export PATH=\"/usr/local/perltests/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/bin/:/root/bin:/usr/local/apitest/bin:/usr/test/moab/bin:/usr/test/moab/sbin:/usr/test/torque/bin:/usr/test/torque/sbin:/usr/test/gold/bin:/usr/test/gold/sbin:/usr/kerberos/sbin:/usr/kerberos/bin:/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:/usr/test/slurm/sbin:/usr/test/slurm/bin:\$PATH\"\n";

    close USER2_BASHRC;
 
    }
  else
    {

    warn "WARNING: Unable to open '$user2_bashrc_loc': $!";

    } # END else

  # Update the action taken
  $action_taken{ '02 PATHS MODIFIED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot modifying your PERL5LIB or PATH!\n";

  } # END else

###############################################################################
# Enable Unlimited Coredumping
###############################################################################
my $coreulimit_explain =<<COREULIMIT_EXPLAIN;
I am going to set the coredump file size to unlimited by adding 'ulimit -c unlimited' to the .bashrc for root

NOTE: Needed for automated Coredump detection in CRI::Test module

Is this alright? [y/N] 
COREULIMIT_EXPLAIN

print $div;
chomp $coreulimit_explain;
print $coreulimit_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "ulimit -c unlimited\n";

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else

  $action_taken{ '03 COREDUMP SIZE' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot changing default coredump file size!\n";

  } # END else

###############################################################################
# Set Coredump Pattern for Detection
###############################################################################
my $coreformat_explain =<<COREFORMAT_EXPLAIN;
I am going to make it so all coredump files are stored in the format /tmp/<executable>-core.<pid>
by adding the line 'echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern' to ${root}'s .bashrc file

Note: Needed for automated Coredump detection in CRI::Test module

Is this alright? [y/N]
COREFORMAT_EXPLAIN

print $div;
chomp $coreformat_explain;
print $coreformat_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
    {

    print ROOT_BASHRC "\n# Added by CRI::Test quick_setup.pl\n";
    print ROOT_BASHRC "echo '/tmp/%e-core.%p' > /proc/sys/kernel/core_pattern\n";

    } # END if (open (ROOT_BASHRC, ">>$root_bashrc_loc"))
  else
    {

    warn "WARNING: Unable to open '$root_bashrc_loc': $!";

    } # END else

  $action_taken{ '04 COREDUMP TEMPLATE' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot changing default coredump template!\n";

  } # END else

###############################################################################
# Install packages required by tests
###############################################################################
my @programs = qw( 
                   subversion
                   build-essential
                   psmisc
                   autoconf
                   libxml2-dev
                   zlib1g
                   zlib1g-dev
                   bzip2
                   openssl
                   libssl-dev
                   nfs-common
                   ia32-libs
                 );
my $program_list = join(", ", @programs);

my $program_explain =<<PROGRAM_EXPLAIN;
I am going to install needed programs with apt-get.
I will be installing $program_list.

If you are not on Ubuntu or Debian, say no here, and install these programs (or their equivalents) yourself.

Is this alright? [y/N] 
PROGRAM_EXPLAIN

print $div;
chomp $program_explain;
print $program_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  my $apt_update_cmd = "apt-get update";
  print "\nRunning '$apt_update_cmd'\n";

  my $apt_update_result = `apt-get update`;
  warn "$apt_update_cmd failed: $apt_update_result"
    if $? != 0;

  foreach my $program (@programs)
    {

    my $apt_cmd = "apt-get install --assume-yes $program";
    print "Running '$apt_cmd'\n";
    my $apt_result = `$apt_cmd`;
    warn "$apt_cmd failed: $apt_result"
      if $? != 0;

    } # END foreach my $program (@programs)

  $action_taken{ '05 PROGRAMS INSTALLED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot installing $program_list\n";

  } # END else

###############################################################################
# Install CPAN modules required by tests
###############################################################################
my @libraries = qw(
                    IPC::Run3
                    Test::More
                    Data::Properties
                    TAP::Harness
                    XML::LibXML
                    XML::Simple
                    Proc::Daemon
                    Expect
                  );
my $library_list = join(", ", @libraries);

my $cpan_explain =<<CPAN_EXPLAIN;
I am going to use CPAN to install needed Perl libraries
Libraries are $library_list

You may need to configure CPAN before this will work (it will prompt you if needed)

Is this alright? [y/N ]
CPAN_EXPLAIN

print $div;
chomp $cpan_explain;
print $cpan_explain;
$stdin = <STDIN>;

if ($stdin =~ /${yes_regex}/i)
  {

  foreach my $library (@libraries)
    {
   
    my $cpan_cmd    = "cpan $library";
    print "Running '$cpan_cmd'\n";

    my $cpan_result = `$cpan_cmd`;

    warn "'$cpan_cmd' failed: $cpan_result"
      if $? != 0;

    } # END foreach my $library (@libraries)

  $action_taken{ '06 PERLLIBS INSTALLED' } = 'yes';

  } # END if ($stdin =~ /${yes_regex}/i)
else
  {

  print "\nNot installing $library_list\n";

  } # END else

###############################################################################
# Summary
###############################################################################
print $div;
print "SUMMARY:\n\n";

foreach my $step (sort keys %action_taken)
  {

  print "$step: " . $action_taken{ $step } . "\n"; 

  } # END foreach my $step (sort keys %action_taken)
