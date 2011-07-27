package CRI::Test;
########################################################################
#
# A wrapper around Test::More that provides logging, summary reports,
# run utilities, coredump logging, and other miscellenous subroutines.
#
########################################################################
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use Carp;
use Test::More;
use Test::Deep;
use TAP::Harness;
use Data::Properties;
use Proc::Daemon;
use IPC::Run3;
use XML::Simple;
use Cwd qw(realpath);
use MLDBM qw(DB_File);
use Scalar::Util qw(tainted);
use Time::HiRes qw(gettimeofday usleep);
use Fcntl ':flock';
use sigtrap 'handler',
  \&CRI::Test::_timed_out, 'normal-signals';    # Catch kill signals and cleanup
use POSIX ":sys_wait_h";

################################
# EXPORT SECTION
################################
use base qw( Exporter Test::More );
our @EXPORT = (
  @Test::More::EXPORT,
  @Test::Deep::EXPORT,
  'execute_tests',    # Run a batch of tests.
  'setTimeout',       # Set how long of an alarm will be used
  # for runCommand calls
  'runCommand',       # Run a system command, with logging
  'runCommandAs',     # Run a system command as a different
  # user, with logging
  'runCommandSsh',    # Run a system command on a remote host
  # via SSH, with logging (a work-around for
  # a common SSH hangup)
  'runCommandSshAs',    # Run a system command as a differnt user
  # on a remote host via SSH, with logging
  # (a work-around for a common SSH hangup)
  'forkCommand',        # Fork a command
  'forkCommandAs',      # Fork a command
  'setProps',           # Set properties which will carry over to
  # subsequent tests (in the current test
  # run)
  'commitData',     # Commit changes to $data into the Data-Persistence DB file
  'newEnvData',     # Reset/Clear Data-Persistence DB file
  '$props',         # The properties object (Data::properties)
  '$data',          # The Data-Persistence hash
  'setDesc',        # Set the test description
  'logMsg',         # Print a message to the test log
  'logEnv',         # Log all the environment variables
  'testTimeout',    # Set a time limit for the test itself
  'dynamic_cmp',    # Chooses Test method based on variable
  # type of expected value
  'cmp_tolr',       # Tests Difference between values given
  # set tolerance levels
  'sleep_diag',     # Sleeps for a given amount of time and
  # prints out a diag message
  'haveCoreDump',    # Checks for existence of coredump file
  'useForkEnv',      # Sets up a fork environment to avoid
  # logging errors
  'newTestInstance',
);

################################################
# VARIABLES SECTION
################################################
# Global Variables
our $props;
our $batch;
our $zombies = 0;
our $data;

# Environment Variables
$ENV{PrintLogInfo} = 1;

# local variables
my %db;
my $test_id;
my $parent_test_id;    # Only used when newTestInstance() is in play
my %initialHash;
my $curbin;
my $scriptName;
my $testStatus;
my $_LOGFILE;
my $logName;
my $props_missing     = 0;
my $timed_out_already = 0;
my $commandAlarm      = 0;
my @forks             = ();

my $imafork = 0;
my @kidPids = ();

my $full_props   = '/tmp/.test.internal.props';
my $set_props    = '/tmp/.test.internal.set.props';
my $fatal_errors = '';
my $coreDumpDir  = '/tmp';
my $db_file      = '/tmp/.qatests.db';

# Boolean to help decide if a pass should override a lower state
my $state_initialized = 0;
# codes that related to different test states
my %states = (
  -2 => 'COREDUMP',
  -1 => 'ERROR',
  0  => 'FAIL',
  1  => 'PASS',
);

my %badCoredumpExes = (
  'cr_checkpoint' => 1,
  'cr_restart'    => 1,
  'cr_run'        => 1,
  'moab'          => 1,
  'msub'          => 1,
  'pbs_mom'       => 1,
  'pbs_sched'     => 1,
  'pbs_server'    => 1,
  'perl'          => 1,
  'slurmctld'     => 1,
  'slurmd'        => 1,
);

################################################################
# SIGNAL-TRAPPING DEFINITION SECTION
################################################################
$SIG{ALRM} = sub { CRI::Test::_timed_out() };
$SIG{CHLD} = sub { $zombies++ };

# Catch Perl warnings and set test as ERROR
$SIG{__WARN__} = sub {

  if ($_[0] =~ /\(eval in \w+\)/)
  {
    warn join("\n", @_);
    return;
  }

  unless ($_[0] =~ /Warning:\s+XML::LibXML compiled/i)
  {
    _setTestStatus(-1);
    _writeLog(ERROR => join("\n", @_) . "\n" . Carp::longmess('STACKTRACE'));
    Test::More::fail('');
  }
};

# Catch Perl sigdies and set test as ERROR
$SIG{__DIE__} = sub {

  my $message = join("\n", @_);
  my @loc = caller;

  if ($message =~ /compilation (?:error|aborted)/)
  {
    _setTestStatus(-1);

    _writeLog(COMPILATION_ERROR => $message);
    Test::More::fail('');
    _finalizeReport();
  }
  else
  {
    my $stack = Carp::longmess('STACKTRACE');

    # If this was run in an eval statement, let it handle the problem
    die $message if $stack =~ /eval \{\.\.\.\} called /;

    _setTestStatus(-1);
    _writeLog(FATAL_ERROR => "$message\n$stack");
    Test::More::fail('');
    _finalizeReport();
  }

};

####################################################################################
# SUBROUTINE SECTION
####################################################################################
MAIN:
{
  _init();    # Kick it off!
}

#-------------------
# END BLOCK
#-------------------
END
{
  # Log results to test log and summary log
  exit()
    if ($imafork);

  unless ($props_missing)
  {
    _finalizeReport();

    if ($ENV{PrintLogInfo})
    {
      print "#\n#######   Logged results to $ENV{CRILogDir}\n#\n";
    }
  }
}

#------------------------------------------------------------------------------
# _init()
#------------------------------------------------------------------------------
#
# Set initial values for variables used module wide. Add logging data to
# Summary.log and open handle for individual test log.
#
#------------------------------------------------------------------------------
sub _init    #()
{
  die "Tests MUST be run as root" unless $ENV{USER} eq 'root';

  # Set some initial variables
  my $startTime = Time::HiRes::gettimeofday();

  $ENV{START_TIME}   = $startTime;
  $ENV{PrintLogInfo} = 0;            # only print log info on first load

  $state_initialized = 0;

  # set some default props for critical values
  $props = Data::Properties->new();

  # Setting these without seeing if they already set is probably a bad idea
  #  $props->set_property('Data.Props.Loc', '/tmp/clustertest.props');
  #  $props->set_property('Log.Dir',        '/tmp/'                 );

# Load system wide config and test-specific props, probably overriding the defaults above
  if (!exists $ENV{CRILogDir}
    || (defined $ENV{CRILogDir} && $ENV{CRILogDir} !~ /^\S+$/))
  {

    _topLevelInit();

  }

  # Don't re-init the persistence variable if newTestInstance has been called
  unless (defined $parent_test_id)
  {
    # Initialize data-persisting DB hash
    tie(%db, 'MLDBM', $db_file)
      or die "Cannot open DB file $db_file: $!";
    $data = $db{data};
  }

  # Loads internal props file for use in each individual test
  open(FH, "+<$full_props")
    or die "$full_props read error: $!\n";
  flock(FH, LOCK_EX)
    or die("Bad stuff happened with the file lock on $full_props: $!");
  $props->load(\*FH);
  close(FH);

  if (-e $set_props)
  {

    # Loads internal set.props file for use in each individual test
    open(FH, "+<$set_props")
      or die "$set_props read error: $!\n";
    flock(FH, LOCK_EX)
      or die("Bad stuff happened with the file lock on $set_props: $!");
    $props->load(\*FH);
    close(FH);

  }    # END if (-e $set_props)

  # Convert all User.# and Group.# into arrays
  foreach my $type (qw(User Group))
  {
    my $key = lc($type) . 's';

    my @names = sort grep {/^$type\.\d+$/} $props->property_names();
    $data->{$key} = [map { $props->get_property($_) } @names];
  }

  # get script name
  $curbin     = realpath($0);
  $scriptName = $curbin;
  $scriptName =~ s/.*\///;

  # Determine the Unique Test ID
  $test_id = $props->get_property('next.tid') || 1;

  setProps('next.tid' => $test_id + 1);
  setProps("$$.tid"   => $test_id);

  # get full path to script
  $logName = "$ENV{CRILogDir}/$test_id\_$scriptName.log";
  open(_LOGFILE, ">>$logName")
    or die("Couldn't open log file $logName :  $!");
  flock(_LOGFILE, LOCK_EX)
    or die('Bad stuff happened with the file lock on ' . $logName . "  " . $!);
  chmod(0777, $logName);
  close(_LOGFILE)
    or die("Couldn't close logfile : $!");

  # Set up Tap::Harness
  my %tapHarnessArgs = (
    # verbosity => 1,
    timer  => 1,
    merge  => 1,
    errors => 1,
  );

  $batch = TAP::Harness->new(\%tapHarnessArgs);

  # Set up test stat variables
  my @date      = localtime;
  my $timestamp = sprintf(
    '%d%02d%02d%02d%02d%02d',
    $date[5] + 1900,
    $date[4] + 1,
    $date[3], $date[2], $date[1], $date[0]
  );

  # Determine the number of known regressions
  my $known_regressions = 0;
  if (open(SCRIPT, "<$curbin"))
  {

    my @lines = <SCRIPT>;
    close SCRIPT;

    my @known_reg_lines = grep(/#\s*(JR-\w+-|RT)\d+/, @lines);
    $known_regressions = scalar @known_reg_lines;

  }    # END  if (open(SCRIPT, "<$curbin")
  else
  {

    die "Unable to open '$curbin' for reading: $!";

  }    # END else

  # Write to logs
  $startTime   = Time::HiRes::gettimeofday();
  %initialHash = (
    PTID => $parent_test_id || $props->get_property(getppid() . '.tid') || 0,
    SCRIPT            => $curbin,
    DESC              => "test$test_id",
    TEST_RESULT       => "RUNNING",
    LOGFILE           => $logName,
    TIME_RUN          => $timestamp,
    START_TIME        => $startTime,
    KNOWN_REGRESSIONS => $known_regressions,
  );

  _writeSpool($test_id, %initialHash);

  _writeLog("SCRIPTNAME", $curbin);
  _writeLog("SCRIPT_PID", $$);
  _writeLog("PATH",       $ENV{PATH});
  _writeLog("START_TIME", $startTime);

  # This die is here because we must have a valid $logName value and Summary.log
  #   file before dying. This allows the failure to be logged.
  die "FATAL: No .props file specified for this test run!!" if $props_missing;

}    # END sub _init #()

#------------------------------------------------------------------------------
# _topLevelInit()
#------------------------------------------------------------------------------
#
# Reads the system wide config and test-specific .props file into a combined
# internal props file $full_props. Also, creates test log directory and opens
# the handle for the System.log
#
# NOTE: This init only should get called at the start of the first script!
#
#------------------------------------------------------------------------------
sub _topLevelInit    #()
{

  $ENV{PrintLogInfo} = 1;    # This is the top level test, so print the
  # logging info when this test exits

  # if /etc/clustertest.conf exists, read it in
  if (-e "/etc/clustertest.conf")
  {

    open(FH, "+</etc/clustertest.conf")
      or print STDERR "Couln't read /etc/clustertest.conf: $!\n";

    flock(FH, LOCK_EX)
      or die("Bad stuff happened with the file lock on clustertest.conf: $!");

    $props->load(\*FH)
      or print STDERR "Couldn't load Data::Properties $!\n";

    close(FH);

  }    # END if (-e "/etc/clustertest.conf")
  else
  {

    print "My /etc/clustertest.conf didn't exist!\n";

  }    # END else

  # create a time-stampped test results dir
  my @date      = localtime();
  my $timestamp = sprintf(
    '%d-%02d-%02d_%02d.%02d.%02d',
    $date[5] + 1900,
    $date[4] + 1,
    $date[3], $date[2], $date[1], $date[0]
  );

  # Recursively make the outdir or complain if we can't
  my $outdir = $props->get_property("Log.Dir");

  $outdir =~ s/\/$//;    # remove trailing slash if needed
  $outdir .= '/' . $timestamp;
  chomp($outdir);

  unless (-d $outdir)
  {

    my @path = split('/', $outdir);
    my $makeMe = "/";

    for (my $counter = 1; $counter < @path; $counter++)
    {

      $makeMe .= $path[$counter] . '/';

      unless (-d $makeMe)
      {

        mkdir($makeMe, 0777)
          or die "Could not create $outdir. Do you have permission and does "
          . $props->get_property("Log.Dir")
          . "exist?\nERROR: $!\n";

      }    # END unless (-d $makeMe)

    }    # END for (my $counter = 1 ; $counter < @path ; $counter++)

  }    # END  unless (-d $outdir)

  # Set Log outdir across tests
  $ENV{CRILogDir} = $outdir;
  $props->set_property('Current.Test.Log.Dir' => $ENV{CRILogDir});

  # Read in .props file and $set_props, if it exists
  my $dataProps = $props->get_property('Data.Props.Loc');

  if (defined $dataProps && -e $dataProps)
  {

    open(FH, "+<$dataProps")
      or die "$dataProps read error: $!\n";
    flock(FH, LOCK_EX)
      or die("Bad stuff happened with the file lock on $dataProps: $!");
    $props->load(\*FH);
    close(FH);

  }    # END if (defined $dataProps && -e $dataProps)
  else
  {

    $props_missing = 1;

  }    # END else

  if (-f $set_props)
  {

    open(FH, "+<$set_props")
      or die "$set_props read error: $!\n";
    flock(FH, LOCK_EX)
      or die("Bad stuff happened with the file lock on $set_props: $!");
    $props->load(\*FH);
    close(FH);

  }    # END if (-f $set_props)

  # Get all keys in $props hash-ref
  my @keys = $props->property_names();

  # Get an array of Products to use in PATH
  my @prods = grep {/home\.dir/i} @keys;

  # Create PATH evironment variable for tests
  foreach (@prods)
  {

    my $homeDir = $props->get_property("$_");

    foreach my $dir ('bin', 'sbin')
    {

      $ENV{PATH} = "$homeDir/$dir/:" . $ENV{PATH};

    }    # END foreach my $dir ('bin', 'sbin')

  }    # END foreach (@prods)

  $props->set_property('PATH' => $ENV{PATH});
  $ENV{MOABHOMEDIR} = $props->get_property('Moab.Home.Dir');

  # Create combined internal props file
  open(FH, ">", $full_props)
    or die("can't open $full_props $!\n");
  flock(FH, LOCK_EX)
    or die "Bad stuff happened with the file lock on $full_props: $!";
  $props->store(\*FH,
    "Last written to by $0 during test run with results in $ENV{'CRILogDir'}")
    or die("Couldn't save properties to file");
  close(FH);

  # Lines starting with # are ignored by TAP.
  print "#\n#######   Logging results to $ENV{CRILogDir}\n#\n";

}    # END sub _topLevelInit #()

#------------------------------------------------------------------------------
# _setTestStatus()
#------------------------------------------------------------------------------
# Sets the current state of the current test instance.
#------------------------------------------------------------------------------
sub _setTestStatus
{
  my $code = $_[0];

  if ($state_initialized == 0)
  {
    $testStatus        = $code;
    $state_initialized = 1;
  }
  elsif ($code < $testStatus)
  {
    $testStatus = $code;
  }
  else
  {
    # Do nothing to state
  }

  return;
}

#------------------------------------------------------------------------------
# _writeLog('PID', '24553');
#------------------------------------------------------------------------------
#
# Appends key/value pairs to a test log (not summary log!)
#
#------------------------------------------------------------------------------
sub _writeLog    #($$)
{

  my ($logKey, $logValue) = @_;

  if (uc $logKey =~ /^(?:ERROR|COMPILATION_ERROR)$/)
  {
    Test::More::diag("$logKey: $logValue");
  }

  return 0 unless defined $logName;

  open(_LOGFILE, ">>$logName")
    or die("Couldn't open $logName : $!");
  flock(_LOGFILE, LOCK_EX)
    or die('Bad stuff happened with the file lock on ' . $logName . ': ' . $!);
  chmod(0777, $logName);

  # Setup the line prefix
  my $line_prefix = '';

  $line_prefix .= "CHILD[$ENV{ 'TestChildId' }]_"
    if defined $ENV{'TestChildId'};

  $logValue = "" if not defined $logValue;
  my $retval = print _LOGFILE
    "${line_prefix}$logKey=$logValue\n";    # ascii values 19,20,15,16
  close(_LOGFILE)
    or die("Couldn't close logfile : $!");

  return $retval;

}    # END sub _writeLog #($$)

#------------------------------------------------------------------------------
# setTimeout(300);
#------------------------------------------------------------------------------
#
# Set a maximum length of time that the test is allowed to run for. The
# argument must be the number of seconds after which the current test and it's
# children will be killed. If testTimeout is not called, the test will run
# until it exits or dies on it's own. This method should only be called once
# during a test.
#
#------------------------------------------------------------------------------
sub testTimeout    #($)
{

  my $parentPID = $$;
  my $pid;

  if ($pid = fork())
  {

    return 1;

  }                # END if ($pid = fork())
  elsif (defined $pid)
  {

    $imafork = 1;
    die("testTimeout value wasn't a number, it was $_[0]")
      unless ($_[0] =~ m/^[0-9]*$/);
    sleep($_[0]);

    for my $kill ('TERM', 'INT', 'HUP', 'KILL')
    {

      logMsg("Sending $kill signal to parent process : $parentPID");
      last if kill($kill, $parentPID);
      sleep(2);

    }    # END for my $kill ('TERM', 'INT', 'HUP', 'KILL')

    exit(0);

  }    # END elsif (defined $pid)

  logMsg("We didn't fork, but should have");

}    # END sub testTimeout #($)

#------------------------------------------------------------------------------
# setTimeout(300);
#------------------------------------------------------------------------------
#
# Set the maximum time that a runCommand, runCommandAs, forkCommand or
# forkCommandAs function will run. After the timeout occurs, the command will
# be killed
#
#------------------------------------------------------------------------------
sub setTimeout
{

  my ($commandAlarm) = @_;
  $commandAlarm = 0 unless ($commandAlarm =~ m/^[0-9]*$/);
  $commandAlarm = 0;

}    # END setTimeout

#------------------------------------------------------------------------------
# ($uid, $gid) = getUserIds('testuser1');
#------------------------------------------------------------------------------
#
# Takes a username and returns the uid and gid.
#
#------------------------------------------------------------------------------
sub getUserIds    #($)
{

  my ($name, $passwd, $uid, $gid, $quota, $comment, $gcos, $dir, $shell,
    $expire) = getpwnam(shift);

  return ($uid, $gid);

}                 # END sub getUserIds #($)

#------------------------------------------------------------------------------
# $success = forkCommand('sleep 300');
#------------------------------------------------------------------------------
#
# Run the given command in a forked process. As with runCommand, results will
# be logged when the process is finished, however they are not returned to the
# calling script. A positive value is returned if the fork was successfull, a
# zero is returned if the fork failed.
#
#------------------------------------------------------------------------------
sub forkCommand    #($)
{

  logMsg("Forking command '$_[0]'");

  my $pid;

  if ($pid = fork())
  {

    push(@kidPids, $pid)
      if (@kidPids);

    return 1;

  }    # END if ($pid = fork())
  elsif (defined $pid)
  {

    useForkEnv();
    undef(@kidPids);
    Proc::Daemon::Init;
    runCommand($_[0], $_[1]);
    exit();

  }    # END elsif (defined $pid)

  logMsg("forkCommand didn't work correctly");
  return 0;

}    # END forkCommand #($)

#------------------------------------------------------------------------------
# $success = forkCommandAs('user1', 'sleep 300');
#------------------------------------------------------------------------------
#
# Same as forkCommand, but the process is run with the permissions of the
# specified user.
#
#------------------------------------------------------------------------------
sub forkCommandAs    #($$)
{

  my ($username, $command) = @_;
  logMsg("Forking command '$command' as user '$username'");

  # Tainted variables are hard to debug...
  if (tainted($command))
  {

    logMsg(
      "The given command given in forkCommandAs was tainted and likely won't work"
    );

  }    # END if (tainted($command))

  my ($uid, $gid) = getUserIds($username);

  {

    local $( = $gid;
    local $) = $gid;
    local $< = $uid;
    local $> = $uid;

    return forkCommand($command);

  }    # END

}    # END sub forkCommandAs #($$)

#------------------------------------------------------------------------------
# $success = forkCommandSsh('host1', 'sleep 300');
#------------------------------------------------------------------------------
#
# Same as forkCommand but forks the command on the given host.
#
#------------------------------------------------------------------------------
sub forkCommandSsh    #($$)
{

  logMsg("Forking ssh command '$_[1]'");

  my $pid;

  if ($pid = fork())
  {

    push(@kidPids, $pid)
      if (@kidPids);
    return 1;

  }    # END if ($pid = fork())
  elsif (defined $pid)
  {

    useForkEnv();
    undef(@kidPids);
    Proc::Daemon::Init;
    runCommandSsh($_[0], $_[1]);
    exit();

  }    # END elsif (defined $pid)

  logMsg("forkCommandSsh didn't work correctly");
  return 0;

}    # END forkCommandSsh #($$)

#------------------------------------------------------------------------------
# %result    = runCommand('sleep 300');
# $exit_code = runCommand('sleep 300');
# %result    = runCommand('sleep 300', (
#                                       'test_success'      => 1,
#                                       'no_coredump_check' => 1,
#                                       'logging_off'       => 1
#                                      ));
# $exit_code = runCommand('sleep 300', (
#                                       'test_success'      => 1,
#                                       'no_coredump_check' => 1,
#                                       'logging_off'       => 1
#                                      ));
#------------------------------------------------------------------------------
#
# runCommand always runs the command given and logs the results.
#
# When the result is assigned to a hash, a hash is returned with the
# following keys:
#
#      PROGRAM_RUN     -- What command was passed to runCommand (actual
#                         executable used may depend on your $PATH variable!)
#      PTID            -- What was the process ID?
#      EXIT_CODE       -- What was the exit code?
#      RUN_AS_USER     -- Which user ran the command?
#      RUN_AS_GROUP    -- Which group ran the command?
#      CWD             -- What was the working directory when the command was
#                      -- run?
#      STDOUT          -- The STDOUT output
#      STDERR          -- The STDERR output
#      TIME_RUN        -- Precise Epoch timestamp of when command was run
#      SECONDS_TAKEN   -- Time taken to run (calculated from the difference of
#                         two gettimeofday commands from Time::HiRes)
#
# When the result is assigned to a scalar, the exit code of the
# process is returned.
#
# It is up to the user to analyze the returned values and determine
# if the results were successfull.

# If the command completely fails (eg. the command was not found)
# runCommand will have an exit code of 999.
#
#------------------------------------------------------------------------------
sub runCommand    #($;%)
{

  my $command = shift || croak "Must pass in a command to run";
  my %flags = @_;

  my $host = $flags{host} || undef;
  my $user = $flags{user} || 'root';

  my $use_valg = 0;
  unless (exists $flags{no_checks})
  {
    $use_valg = scalar grep {/do\.memcheck$/} $props->property_names();
  }

  my $old_dir;
  if ($user ne 'root')
  {
    my $tmp_cmd = $command;
    $tmp_cmd =~ s/(?:\\)?"/\\"/g;

    $command = "su -p -c \"$tmp_cmd\" $user";

 # Some OSes (SUSE) have problems with a user running commands from certain CWDs
    $old_dir = Cwd::getcwd();
    chdir "/tmp/";
  }

  if (defined $host)
  {
    $command =~ s/(?:\\)?"/\\"/g;
    $command = qq{ssh -o BatchMode=yes root\@$host "cd /tmp; $command"};
  }

  my ($uid, $gid) = getUserIds($user);

  my %core_hash = ();
  $core_hash{ssh} = $host if defined $host;

  my $add_msg = '';
  $add_msg = " on Remote Host $host" if defined $host;

  unless ($use_valg)
  {
    haveCoreDump(\%core_hash)
      unless exists $flags{no_coredump_check} || exists $flags{no_checks};
  }

  my ($thePid, $stdout, $stderr, $theExitCode);

  # Catch generic errors
  $thePid = $stdout = $stderr =
    "Run3 gave an error. This probably means the command was not found.";
  $theExitCode = 999;

  # Tainted variables are hard to debug...
  if (tainted($command))
  {

    $thePid = $stdout = $stderr =
      "The given command was tainted and probably won't work ($command)";

  }    # END if (tainted($command))

  my $startTime = gettimeofday();

  eval {
    local $SIG{ALRM} = sub {
      die(
        "Command timed out after $commandAlarm seconds. Command was : $command"
      );
    };

    alarm($commandAlarm);
    $thePid = run3($command, undef, \$stdout, \$stderr);

    alarm(0);
    $theExitCode = $? >> 8;

  };    # end eval

  alarm(0);
  my $endTime = gettimeofday();

  # Remove odd symbols added by SSH
  if (defined $host)
  {
    $stdout =~ s/\cM$//mg;
    $stderr =~ s/\cM$//mg;
    $stderr =~ s/Connection to $host closed\.\n//;
  }

  my %results = (
    "PROGRAM_RUN"  => $command,
    "CWD"          => Cwd::getcwd(),
    "RUN_AS_USER"  => scalar getpwuid($uid),
    "RUN_AS_GROUP" => scalar getgrgid($gid),
    "TIME_RUN"     => $startTime,
    "BENCHMARK"    => $endTime - $startTime,
    "PTID"         => getppid(),
    "TID"          => $thePid,
    "STDOUT"       => $stdout,
    "STDERR"       => $stderr,
    "EXIT_CODE"    => $theExitCode,
  );

  # Change back to previous CWD
  chdir $old_dir if defined $old_dir;

  my @log_order = qw(
    PROGRAM_RUN
    CWD
    RUN_AS_USER
    RUN_AS_GROUP
    TIME_RUN
    BENCHMARK
    PTID
    TID
    STDOUT
    STDERR
    EXIT_CODE
  );

  unless (exists $flags{logging_off})
  {
    _writeLog($_, $results{$_}) foreach @log_order;
  }

  unless (exists $flags{no_checks})
  {
    if (
         $use_valg == 0
      && !exists $flags{no_coredump_check}
      && ($stderr =~
        /cannot load packet size|connection refused|core dumped|Aborted/
        || ($stderr eq '' && $stdout eq '' && $theExitCode != 0))
      )
    {
      logMsg("Searching for possible Coredump$add_msg...");
      haveCoreDump(\%core_hash);
    }

    checkValgrindXML({host => $host}) if $use_valg;
  }

  if (exists $flags{test_success} || exists $flags{test_success_die})
  {

    my $default_msg = "Command Ran Successfully";
    my $msg = $flags{msg} || $default_msg;

    my $success = cmp_ok($theExitCode, '==', 0, $msg . " (CMD: '$command')")
      or diag "STDERR:$stderr";

    die("Command '$command' failed")
      if (exists $flags{test_success_die} and !$success);

  } # END if (exists $flags{ test_success } || exists $flags{ test_success_die })
  elsif (exists $flags{test_fail} || exists $flags{test_fail_die})
  {

    my $default_msg = "Command Failed as Expected";
    my $msg = $flags{msg} || $default_msg;

    my $success = cmp_ok($theExitCode, '!=', 0, $msg . " (CMD: '$command')")
      or diag("STDERR:$stderr");

    die("Command '$command' did not fail as expected")
      if (exists $flags{test_fail_die} and !$success);

  }   # END elsif (exists $flags{ test_fail } || exists $flags{ test_fail_die })

  return %results if wantarray;

  return $theExitCode;

}    # END sub runCommand #($$)

#------------------------------------------------------------------------------
# %result    = runCommandSsh('host1', 'sleep 300');
# $exit_code = runCommandSsh('host1', 'sleep 300');
#------------------------------------------------------------------------------
#
# Acts like runCommand() but runs the command via SSH on the remote server.
#
#------------------------------------------------------------------------------
sub runCommandSsh    #($$)
{

  my $host    = shift @_ || croak 'Must Specify a Host!';
  my $command = shift @_ || croak 'Must Specify a Command!';

  my @params = ($command, 'host' => $host, @_);

  return (wantarray ? (runCommand(@params)) : runCommand(@params));

}                    # END sub runCommandSsh #($$)

#------------------------------------------------------------------------------
# %result    = runCommandSsh('host1', 'user1', 'sleep 300');
# $exit_code = runCommandSsh('host1', 'user1', 'sleep 300');
#------------------------------------------------------------------------------
#
# Acts like runCommand() but runs the command via SSH on the remote server as
# the given user.
#
#------------------------------------------------------------------------------
sub runCommandSshAs
{

  my $host    = shift @_ || croak 'Must Specify a Host!';
  my $user    = shift @_ || croak 'Must Specify a User!';
  my $command = shift @_ || croak 'Must Specify a Command!';

  my @params = ($command, 'host' => $host, 'user' => $user, @_);

  return (wantarray ? (runCommand(@params)) : runCommand(@params));

}    # END sub runCommandSshAs #($$$)

#------------------------------------------------------------------------------
# %result    = runCommand('user1', 'sleep 300');
# $exit_code = runCommand('user1', 'sleep 300');
# %result    = runCommand('user1', 'sleep 300', (
#                                                'test_success'      => 1,
#                                                'no_coredump_check' => 1
#                                               ));
# $exit_code = runCommand('user1', 'sleep 300', (
#                                                'test_success'      => 1,
#                                                'no_coredump_check' => 1
#                                               ));
#------------------------------------------------------------------------------
#
# Does "runCommand($command)" as the specified user. Returns the same hash that
# runCommand() does.
#
#------------------------------------------------------------------------------
sub runCommandAs    #($$;%)
{

  my $user    = shift @_ || croak 'Must Specify a User!';
  my $command = shift @_ || croak 'Must Specify a Command!';

  my @params = ($command, 'user' => $user, @_);

  return (wantarray ? (runCommand(@params)) : runCommand(@params));

}                   # END sub runCommandAs

# Override functions provided by Test::More so we can log the results.  Previous
# versions rehooked the symbol table to override these functions, but it seems
# to work fine without doing that now.
no warnings qw(redefine);

#------------------------------------------------------------------------------
# $result = ok(1, "Test Passed");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::ok()
#
#------------------------------------------------------------------------------
sub ok ($;$)
{

  my $result = Test::More::ok($_[0], $_[1]);
  _setTestStatus $result;

  _writeLog("OK[$result]", join(' , ', replaceUndef(@_)))
    or die("Printing threw an error in sub ok() $!\n");

  return $result;

}    # END ok ($;$)

#------------------------------------------------------------------------------
# $string = cmp_ok('string1', 'eq', 'string2', "Comparing strings");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::cmp_ok().
#
# NOTE: It seems that cmp_ok makes bad stuff happen in debugging :-/
#
#------------------------------------------------------------------------------
sub cmp_ok ($$$;$)
{

  my ($one, $two, $three, $four) = @_;

  my $result = Test::More::cmp_ok($one, $two, $three, $four);
  _setTestStatus $result;

  _writeLog("CMP_OK[$result]", join(' , ', replaceUndef(@_)))
    or die("Printing threw an error in sub cmp_ok()");

  return $result;

}    # END cmp_ok ($$$;$)

#------------------------------------------------------------------------------
# $result = cmp_tolr(95, '<=', 100, '5%', "Test that value is within tolerance");
# $result = cmp_tolr(95, '<=', 100, 5,    "Test that value is within tolerance");
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Tests that two values are within an expected tolerance.
#
# PARAMETERS (IN ORDER):
#   reported value -- The value that was reported
#   operator       -- The operator to use to compare the difference and the
#                     allowed difference (NOTE: In the synopsis section the
#                     operator '<=' is used.  This will check that the percent
#                     difference between 95 and 100 is less than or equal to
#                     5%. In this case the test would pass)
#   expected value -- The value that is expected
#   tolerance      -- The allowed tolerance.  If a % symbol is used at the end
#                     of the string the tolerance is checked in terms of
#                     percent.  Otherwise it checked to ensure that the
#                     expected and reported values don't differ by the tolerance
#                     integer given.
#   message        -- The comparision message.
#
#------------------------------------------------------------------------------
sub cmp_tolr ($$$$;$)
{

  my ($rep, $opr, $exp, $tolr, $msg) = @_;

  # Determine Precision of Tolerance
  my $precision = 0;
  $precision = length($1) if $tolr =~ /\d+\.(\d+)/;

  my $mk_precise = sub {
    my $format = $precision ? '%.' . $precision . 'f' : '%d';
    return sprintf($format, @_);
  };

  my $log_msg = "Reported $rep and Expected $exp";
  my $exp_tolr;

  my $value;
  if ($tolr =~ /^([\-\+]?[\d\.]+)%$/)
  {
    # Need to prevent divide-by-zero errors
    if ($exp == 0)
    {
      $exp += 5;
      $rep += 5;
#fail "ALERT: INFINITE% (Divide by Zero). Perhaps it would be more appropriate to use a non-percentange value in this case.\n";
#return 0;
    }

    $value = ($rep - $exp) / $exp * 100;

    $exp_tolr = &$mk_precise($1);
    $msg .= " ($exp_tolr% Tolerance)";
    $log_msg .= ' have ' . &$mk_precise($value) . '% Difference.';

    $value = abs($value) if $value < 0 && $tolr !~ /^[\-\+]/;

  }    # END if ($tolr =~ /^([\-\+]?[\d\.]+)%$/)
  else
  {

    $exp_tolr = &$mk_precise($tolr);
    $value    = $rep - $exp;
    $msg .= " (Tolerance Used)";
    $log_msg .= ' Differ by ' . &$mk_precise($value) . '.';

    $value = abs($value) if $value < 0 && $tolr !~ /^[\-\+]/;

  }    # END else

  $log_msg .= " Tolerance Set at $tolr.";

  $value = &$mk_precise($value);
  my $result = Test::More::cmp_ok($value, $opr, $exp_tolr, $msg);
  _setTestStatus $result;

  _writeLog("CMP_TOLR[$result]", $msg)
    or die("Printing threw an error in sub cmp_tolr()");
  _writeLog('CMP_TOLR_MSG', $log_msg)
    if $result;

  diag $log_msg
    unless $result;

  return $result;

}    # END cmp_tolr ($$$$;$)

#------------------------------------------------------------------------------
# $result = is('string1', 'string2', "Comparing Strings");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::is().
#
#------------------------------------------------------------------------------
sub is ($$;$)
{

  my ($one, $two, $three) = @_;
  my $isResult = Test::More::is($one, $two, $three) || 0;
  _setTestStatus $isResult;

  _writeLog("IS[$isResult]", join(',', replaceUndef(@_)))
    or die("Printing threw an error in sub is() $!\n");

  return $isResult;

}    # END is ($$;$)

#------------------------------------------------------------------------------
# $result = isnt('string1', 'string2', "Comparing Strings");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::isnt().
#
#------------------------------------------------------------------------------
sub isnt ($$;$)
{

  my ($one, $two, $three) = @_;
  my $isntResult = Test::More::isnt($one, $two, $three);
  _setTestStatus $isntResult;

  _writeLog("ISNT[$isntResult]", join(' , ', replaceUndef(@_)))
    or die("Printing threw an error in sub isnt() $!\n");

  return $isntResult;

}    # END isnt ($$;$)

#------------------------------------------------------------------------------
# $result = like('string', qr/ing/, "Checking string with a regexp");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::like()
#
#------------------------------------------------------------------------------
sub like ($$;$)
{

  my ($one, $two, $three) = @_;
  my $likeResults = Test::More::like($one, $two, $three);
  _setTestStatus $likeResults;

  _writeLog("LIKE[$likeResults]", join(' , ', replaceUndef(@_)))
    or die("Printing threw an error in sub like() $!\n");

  return $likeResults;

}    #END like ($$;$)

#------------------------------------------------------------------------------
# $result = unlike('string', qr/ing/, "Checking string with a regexp");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::unlike()
#
#------------------------------------------------------------------------------
sub unlike ($$;$)
{
  my ($one, $two, $three) = @_;
  my $unlikeResults = Test::More::unlike($one, $two, $three);
  _setTestStatus $unlikeResults;

  _writeLog("UNLIKE[$unlikeResults]", join(' , ', replaceUndef(@_)))
    or die("Printing threw an error in sub unlike() $!\n");

  return $unlikeResults;

}    # END unlike ($$;$)

#------------------------------------------------------------------------------
# my $result = dynamic_cmp(
# 	                   'rept' => 10,
#                          'expt' => 12,
#                          'msg'  => 'Some Message',
# 	                   'tolr' => '15.5%',
#                          'opr'  => '>=',
#                          negate => 1,
#                         );
#------------------------------------------------------------------------------
#
# Dynamically determines testing method based on variable type of expected
# value. If the 'opr' flag exists, the default flag will be replaced.
#
#------------------------------------------------------------------------------
sub dynamic_cmp    #(%)
{

  my %case = @_;

  my $negate = $case{negate} || 0;

  # Set undefined values to '<undef>', this prevents errors
  $case{expt} = '<undef>'
    if !defined $case{expt};
  $case{rept} = '<undef>'
    if !defined $case{rept};

  if ($case{expt} =~ /^-?\d+(?:\.\d+)?$/)
  {
    if (defined $case{tolr})
    {
      my $opr = $case{opr} || '<=';
      return cmp_tolr($case{rept}, $opr, $case{expt}, $case{tolr}, $case{msg});
    }
    else
    {
      my $opr = $case{opr};
      $opr ||= '!=' if $negate;
      $opr ||= '==';

      return cmp_ok($case{rept}, $opr, $case{expt}, $case{msg});
    }
  }    # END elsif ($case{ expt } =~ /^-?\d+(?:\.\d+)?$/)
  elsif (ref $case{expt} eq 'Regexp')
  {

    if ($negate)
    {
      return unlike($case{rept}, $case{expt}, $case{msg});
    }
    else
    {
      return like($case{rept}, $case{expt}, $case{msg});
    }

  }    # END elsif (ref $case{ expt } eq 'Regexp')
  elsif (ref $case{expt} eq '')
  {
    my $opr = $case{opr} || '';
    if ($opr eq 'ne' || $negate)
    {
      return isnt($case{rept}, $case{expt}, $case{msg});
    }
    else
    {
      return is($case{rept}, $case{expt}, $case{msg});
    }
  }
  else
  {
    return cmp_deeply($case{rept}, $case{expt}, $case{msg});
  }

}    # END sub dynamic_cmp #(%)

#------------------------------------------------------------------------------
# $result = pass();
# $result = pass('Test Passed');
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::Pass()
#
#------------------------------------------------------------------------------
sub pass (;$)
{
  _setTestStatus 1;
  _writeLog("PASS", $_[0])
    or die("Printing threw an error in sub pass() $!\n");

  return Test::More::pass($_[0]);

}    # END pass (;$)

#------------------------------------------------------------------------------
# $result = fail();
# $result = fail('Test Failed');
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::Fail()
#
#------------------------------------------------------------------------------
sub fail (;$)
{
  _setTestStatus 0;

  _writeLog("FAIL", $_[0])
    or die("Printing threw an error in sub fail() $!\n");

  return Test::More::fail($_[0]);

}    # END fail #(;$)

#------------------------------------------------------------------------------
# $result = error();
# $result = error('An error occured');
#------------------------------------------------------------------------------
#
# Writes an error message to the test log.
#
#------------------------------------------------------------------------------
sub error (;$)
{
  _setTestStatus -1;

  diag "ERROR: $_[0]";
  _writeLog("FATAL_ERROR", $_[0])
    or die("Printing threw an error in sub error() $!\n");

  return 1;

}    # END error (;$)

#------------------------------------------------------------------------------
# $result = is_deeply($reported_ref, $expected_ref, "Is deeply test");
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::is_deeply();
#
#------------------------------------------------------------------------------
sub is_deeply    #($$;$)
{
  my $isDeep = Test::More::is_deeply(@_);
  _setTestStatus $isDeep;

  my ($ref1, $ref2, $desc) = @_;
  my $log_string = "Data_Struct: Reported   <=>   Expected\n";

  my $cmp_string = _log_is_deeply($ref1, $ref2, '');
  $log_string .= $cmp_string if defined $cmp_string;

  # Catch errors
  if (defined $cmp_string && $cmp_string =~ /^ERROR:/m)
  {
    $log_string = $cmp_string;
  }

  my @lines = split(/\n/, $log_string);
  $log_string = "Data Structures are Equal!" if scalar @lines < 2 && $isDeep;

  _writeLog("IS_DEEPLY[$isDeep]", "$desc\n$log_string")
    or die('Printing threw an error in sub is_deeply()');

  return $isDeep;

  sub _log_is_deeply
  {
    my ($ref1, $ref2, $curr_place) = @_;
    my $cmp_string;

    my $ref1_type = ref $ref1;
    my $ref2_type = ref $ref2;

    my $format = sub { return sprintf("%s: '%s' <=> '%s'\n", @_); };

    if ($ref1_type ne $ref2_type)
    {
      $cmp_string .= &$format($curr_place, $ref1_type, $ref2_type);
    }
    elsif ($ref2_type eq 'ARRAY')
    {
      my $num = [reverse sort (scalar @$ref1 - 1, scalar @$ref2 - 1)]->[0];

      foreach (0 .. $num)
      {
        my $tmp1 = defined $ref1->[$_] ? $ref1->[$_] : 'Does Not Exist';
        my $tmp2 = defined $ref2->[$_] ? $ref2->[$_] : 'Does Not Exist';

        if (ref $ref2->[$_] eq 'ARRAY' || ref $ref2->[$_] eq 'HASH')
        {
          my $tmp =
            _log_is_deeply($ref1->[$_], $ref2->[$_], "$curr_place\[$_]");
          $cmp_string .= $tmp if defined $tmp;
        }
        else
        {
          $cmp_string .= &$format("$curr_place\[$_]", $tmp1, $tmp2)
            if $tmp2 ne $tmp1;
        }
      }
    }
    elsif ($ref2_type eq 'HASH')
    {

      my %use;
      $use{$_} = 1 foreach keys %$ref1;
      $use{$_} = 1 foreach keys %$ref2;

      foreach (sort keys %use)
      {
        my $tmp1 = defined $ref1->{$_} ? $ref1->{$_} : 'Does Not Exist';
        my $tmp2 = defined $ref2->{$_} ? $ref2->{$_} : 'Does Not Exist';

        if (ref $ref2->{$_} eq 'ARRAY' || ref $ref2->{$_} eq 'HASH')
        {
          my $tmp =
            _log_is_deeply($ref1->{$_}, $ref2->{$_}, "$curr_place\{$_}");
          $cmp_string .= $tmp if defined $tmp;
        }
        else
        {
          $cmp_string .= &$format("$curr_place\{$_}", $tmp1, $tmp2)
            if $tmp2 ne $tmp1;
        }
      }
    }
    else
    {
      $cmp_string = "ERROR: Did not recieve an array or hash reference!";
    }

    return $cmp_string;
  }
}    # END is_deeply

#------------------------------------------------------------------------------
# Wrapper method: Adds logging.
#
# NOTE: To get the details of a failure, I had to use cmp_details and deep_diag,
#  which does not hit the Test::Builder object (meaning it won't show in the
#  command-line output). That is okay if it passes. However, when it fails,
#  I had to have that displayed in the TB object via the fail() method.
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
sub cmp_deeply
{
  my @args = @_;

  die 'Too many arguments supplied!' unless @args == 2 || @args == 3;

  my ($pass, $stack) = Test::Deep::cmp_details(@args);
  _setTestStatus $pass;

  if ($pass)
  {
    _writeLog("CMP_DEEPLY[$pass]", 'Data Structures are equal')
      or die 'Unable to write log for cmp_deeply()';

    print 'cmp_deeply ok - ' . ($args[2] || '<no message passed in>') . "\n";
  }
  else
  {
    _writeLog("CMP_DEEPLY[$pass]", 'Data Structures Differ!')
      or die 'Unable to write log for cmp_deeply()';

    # This is just so Test::Builder picks up the failure
    my $reason = Test::Deep::deep_diag($stack);
    fail(($args[2] || 'cmp_deeply() failure stack:') . "\n$reason");
  }

  return $pass;
}

#------------------------------------------------------------------------------
# diag("Diagnostics message")
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::diag().
#
#------------------------------------------------------------------------------
sub diag
{

  my ($one) = @_;
  _writeLog('DIAG', $one);

  Test::More::diag($one);

}    # END sub diag

#------------------------------------------------------------------------------
# sleep_diag 15;
#------------------------------------------------------------------------------
#
# Sleeps for a given amount of time and prints out a diag message.
#
#------------------------------------------------------------------------------
sub sleep_diag    #($)
{

  my ($sleep_time, $msg) = @_;

  # Check the passed in sleep time
  die "No sleep time given"
    unless defined $sleep_time;
  die "Sleep time needs to be an integer"
    unless $sleep_time =~ /^\d+$/;

  # Print out the message and sleep
  diag($msg) if $msg;
  diag("Sleeping for $sleep_time second(s) ...");
  sleep $sleep_time;

}    # END sleep_diag #($)

#------------------------------------------------------------------------------
# SKIP:
#   {
#
#   skip $why, $how_many if $condition;
#
#   ...normal testing code goes here...
#
#   }
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::skip().
#
#------------------------------------------------------------------------------
sub skip
{

  my $skipped = 'unknown';
  $skipped = $_[1]
    if defined $_[1];

  _writeLog("SKIPPING[$skipped]", $_[0]);

  return Test::More::skip(@_);

}    # END sub skip

#------------------------------------------------------------------------------
# BAIL_OUT("This is too crazy.  I'm outta here!");
#------------------------------------------------------------------------------
#
# Add logging Test::More::BAIL_OUT().
#
#------------------------------------------------------------------------------
sub BAIL_OUT
{

  _writeLog("BAIL_OUT", $_[0]);
  logMsg(
    "WARNING: Running BAIL_OUT can cause parent tests to erroniously report a PASS status"
  );
  return Test::More::BAIL_OUT($_[0]);

}    # END sub BAIL_OUT

#------------------------------------------------------------------------------
# plan('no_plan');
#------------------------------------------------------------------------------
#
# Adds logging to Test::More::plan
#
#------------------------------------------------------------------------------
sub plan
{

  _writeLog("PLAN", join(", ", @_));
  Test::More::plan(@_);

}    # END sub plan

#------------------------------------------------------------------------------
# @results = execute_tests('test1', 'test2', ...);
# $result  = execute_tests('test1', 'test2', ...);
#------------------------------------------------------------------------------
#
# Runs the TAP::Harness runtests and returns a TAP::Parser::Aggregator object
# when called in an array context, or a boolean if called in a scalar context.
#
#------------------------------------------------------------------------------
sub execute_tests    #(@)
{

  my @not_found = grep { !-e $_ } @_;
  my $total_fail = scalar @not_found == scalar @_;

  my $aggregate;
  unless ($total_fail)
  {
    $aggregate = $batch->runtests(grep { -e $_ } @_);

    _writeLog('RT_TESTSRUN_ARRAY', join(", ", @_));
    _writeLog('RT_BENCHMARK', $aggregate->elapsed_timestr());

    my %filter = map { $_ => 1 } $aggregate->failed();
    my @passed = grep { !exists $filter{$_} } $aggregate->passed();

    pass("$_ (in execute_test)") foreach @passed;

    fail("$_ (in execute_test)") foreach $aggregate->failed();
  }

  error("Test $_ does not exist (in execute_test)") foreach @not_found;

  if (wantarray)
  {

    return $aggregate;

  }    # END if (wantarray)
  else
  {
    my @fails = @not_found;
    push @fails, $aggregate->failed() unless $total_fail;

    return scalar @fails == 0;
  }
}    # END sub execute_tests #(@)

#------------------------------------------------------------------------------
# $success = logMsg("Message");
#------------------------------------------------------------------------------
#
# Appends message to the logfile for this test script.
#
# Returns true on success, false on failure
#
#------------------------------------------------------------------------------
sub logMsg
{

  # Tainted variables are hard to debug...
  if (tainted($_[0]))
  {

    logMsg(
      "logMsg was passed a tainted message and may have trouble logging it");

  }    # END if (tainted($_[0]))

  return _writeLog("MSG", $_[0]);

}    # END sub logMsg

#------------------------------------------------------------------------------
# Commits any changes to the Data-Persistent hashref ($data) to the DB file.
#  This is needed because of a limitation in Perl's TIEHASH, which does not
#  support multi-dimensional data structure assignments
#  (see 'Bugs' section of perldocs on MLDBM for more detailed info).
#------------------------------------------------------------------------------
# No PARAMETERS needed, for now
#------------------------------------------------------------------------------
sub commitData
{
  $db{data} = $data;
}

#------------------------------------------------------------------------------
# Resets the Data-persistent hashref in preparation for a new environment.
#------------------------------------------------------------------------------
# No PARAMETERS needed, for now
#------------------------------------------------------------------------------
sub newEnvData
{
  $data = {};

  # Convert all User.# and Group.# into arrays
  foreach my $type (qw(User Group))
  {
    my $key = lc($type) . 's';

    my @names = sort grep {/^$type\.\d+$/} $props->property_names();
    $data->{$key} = [map { $props->get_property($_) } @names];
  }

  commitData();
}

#------------------------------------------------------------------------------
# setProps( 'key1' => 'value1' )
#------------------------------------------------------------------------------
#
# Adds key-value pairs to the Data::Properties object and writes the modified
# properties to the $full_props file.
#
# Use this method if you want to communicate between tests. Add a property,
# and then access it later in $props->get_property('key')
#
#------------------------------------------------------------------------------
sub setProps    #(%)
{

  my %pairs     = @_;
  my $tmp_props = Data::Properties->new();

  # Don't want the whole props to be written to $set_props,
  # just test-set attributes, so need to ensure this happens

  if (-e $set_props)
  {

    open(FH, "+<$set_props")
      or die "can't open $set_props: $!\n";
    $tmp_props->load(\*FH);
    close FH;

  }    # END if (-e $set_props)

  for (keys %pairs)
  {

    if (tainted($_))
    {
      logMsg('You passed setProps a tainted key, this could cause problems');
    }
    if (tainted($pairs{$_}))
    {
      logMsg('You passed setProps a tainted value, this could cause problems');
    }

    $props->set_property($_, $pairs{$_});
    $tmp_props->set_property($_, $pairs{$_});

  }    # END for (keys %pairs)

  open(FH, ">$set_props")
    or die "can't open $set_props: $!\n";
  flock(FH, LOCK_EX)
    or die("Bad stuff happened with the file lock on $set_props: $!");
  $tmp_props->store(\*FH,
    "Last written to by $scriptName during test run with results in $ENV{'CRILogDir'}"
  );
  close(FH);

}    # END sub setProps #(%)

#------------------------------------------------------------------------------
# %data = readSpool()
#------------------------------------------------------------------------------
#
# Reads in the data found in the Summary.log.  Return a hash of the data.
#
#------------------------------------------------------------------------------
sub readSpool    #()
{

  require 'shellwords.pl';

  my $spoolFile = "$ENV{CRILogDir}/Summary.log";
  my %data      = ();

  unless (-e $spoolFile)
  {

    # Automatically create spool file to avoid error
    open(SPOOL, ">>$spoolFile");
    flock(SPOOL, LOCK_EX)
      or die("Bad stuff happened with the file lock on '$spoolFile': $!");
    close(SPOOL);

  }    # END unless (-e $spoolFile)

  if (open(SPOOL, "+<$spoolFile"))
  {

    flock(SPOOL, LOCK_EX)
      or die("Bad stuff happened with the file lock on '$spoolFile': $!");

    foreach my $line (<SPOOL>)
    {

      my @words;
      eval "
	    no warnings;
	    \@words = shellwords(\$line)
	    ";

      next if $@;
      next unless scalar @words;

      my $rowId = shift @words;
      $data{$rowId} = ();

      foreach my $word (@words)
      {

        my ($key, $value) = split(/=/, $word, 2);
        $data{$rowId}->{$key} = $value;

      }    # END foreach my $word (@words)

    }    # END foreach my $line (<SPOOL>)

    close(SPOOL);

  }    # END if (open(SPOOL, "+<$spoolFile"))
  else
  {

    die "Couldn't open spool file";

  }    # END else

  return %data;

}    # END sub readSpool #(%)

#------------------------------------------------------------------------------
# _writeSpool($tid, $logInfo);
#------------------------------------------------------------------------------
#
# Updates the Summary.log file or adds new values to it as needed.
#
#------------------------------------------------------------------------------
sub _writeSpool    #($$)
{

  my ($tid, %logInfo) = @_;
  my %currentLog;

  my %wholeLog = readSpool();

  foreach my $key (sort keys %logInfo)
  {

    $wholeLog{$tid}->{$key} = $logInfo{$key};

  }    # END foreach my $key (sort keys %logInfo)

  open(SPOOL, ">$ENV{'CRILogDir'}/Summary.log")
    or die "Couldn't open Summary.log!\n";
  flock(SPOOL, LOCK_EX)
    or die("Bad stuff happened with the file lock on Summary.log: $!");

  foreach my $rowID (sort keys %wholeLog)
  {

    print SPOOL $rowID;

    foreach my $key (sort keys %{$wholeLog{$rowID}})
    {

      print SPOOL qq| $key="$wholeLog{$rowID}->{$key}"|;

    }    # END foreach my $key (sort keys %{ $wholeLog{ $rowID } })

    print SPOOL "\n";

  }    # END foreach my $rowID (sort keys %wholeLog)

  close(SPOOL)
    or die("Couldn't close spools file");

}    # END _writeSpool #($$)

#------------------------------------------------------------------------------
# $result = setDesc('Test Description');
#------------------------------------------------------------------------------
#
# Set a short description for the current test. Default value is the script
# name (without the path).
#
# The description must not have 1) A newline (\n) or 2) A double quote (").
#
# Returns 0 on failure and 1 on success
#
#------------------------------------------------------------------------------
sub setDesc    #($)
{

  return 0
    if ($_[0] =~ /[\n\"]/);

  if (tainted($_[0]))
  {

    logMsg('The description you tried to set was tainted and may not work');

  }            # END if (tainted($_[0]))

  _writeSpool($test_id, DESC => "$_[0]");

  _setTestStatus(1);
  return 1;

}    # END setDesc #($)

#------------------------------------------------------------------------------
# alarm(5)
#------------------------------------------------------------------------------
#
# Set a new timeout value. Default is to not timeout
#
#------------------------------------------------------------------------------
sub alarm    #($)
{

  die("alarm value wasn't a number, it was $_[0]")
    unless ($_[0] =~ m/^[0-9]*$/);
  alarm(0);    # Catch any pending

  my ($newtime) = @_;
  logMsg("Setting alarm time to $newtime");
  alarm($newtime);

}    # END sub alarm #($)

#------------------------------------------------------------------------------
# _timed_out()
#------------------------------------------------------------------------------
#
# Called when the test times out.
#
#------------------------------------------------------------------------------
sub _timed_out    #()
{
  unless ($timed_out_already)
  {
    $timed_out_already = 1 if $timed_out_already == 0;
    logMsg("This test timed out and will now be killed");

    _setTestStatus 0;

    die("Killing test now");
  }
}                 # END _timed_out #()

#------------------------------------------------------------------------------
# _finalizeReport()
#------------------------------------------------------------------------------
#
# Finalizes the log for the test.
#
#------------------------------------------------------------------------------
sub _finalizeReport    #()
{
  my $endTime = Time::HiRes::gettimeofday();
  _writeLog("END_TIME", $endTime);
  my $totalTime = $endTime - $ENV{START_TIME};
  _writeLog("TOTAL_TIME", $totalTime);

  _setTestStatus(-1) unless defined $testStatus;

  my %finalHash = (
    PTID => $parent_test_id || $props->get_property(getppid() . '.tid') || 0,
    SCRIPT      => $curbin,
    TEST_RESULT => $states{$testStatus},
    LOGFILE     => $logName,
    START_TIME  => $ENV{START_TIME},
    TOTAL_TIME  => $totalTime,
  );

  _writeSpool($test_id, %finalHash);

  # Only applicable when newTestInstance() is in play
  # Set the state on the parent if set and is non-passing
  if (defined $parent_test_id && $testStatus < 1)
  {
    my %logs         = readSpool();
    my $parent_state = $logs{$parent_test_id}{TEST_RESULT};

    my $curr_status_id =
      (grep { $states{$_} eq $parent_state } keys %states)[0];

    _writeSpool($parent_test_id, TEST_RESULT => $states{$testStatus})
      if $curr_status_id > $testStatus;
  }
}

#------------------------------------------------------------------------------
# logEnv()
#------------------------------------------------------------------------------
#
# Log all environment variables to the test's log file.
#
#------------------------------------------------------------------------------
sub logEnv    #()
{

  my $envstring = "";

  foreach (keys %ENV)
  {

    $envstring .= $_ . " => " . $ENV{$_} . "\n";

  }           # END foreach (keys %ENV)

  logMsg($envstring);

}    # END sub logEnv #()

#------------------------------------------------------------------------------
# haveCoreDump();
#------------------------------------------------------------------------------
#
# Check for Seg-Fault and kill script if coredump present
#
#------------------------------------------------------------------------------
sub haveCoreDump    #($)
{
  my ($params) = @_;

  my %runcmd_flags;
  $runcmd_flags{host}              = $params->{ssh} if exists $params->{ssh};
  $runcmd_flags{no_coredump_check} = 1;
  $runcmd_flags{logging_off}       = 1;

  my %non_host_flags;
  $non_host_flags{$_} = $runcmd_flags{$_}
    foreach grep { $_ ne 'host' } keys %runcmd_flags;

  my %execs;
  #  my (%execs, %recover_subs);
  my (@exe_names, @coreList, @coreLogs);

  my %cmd = runCommand("ls $coreDumpDir", %runcmd_flags);

  my @coredumps = grep {m/^\S+-core\.\d+$/} split(/\n/, $cmd{STDOUT});

  # for parsing the filesize of given file
  my $getSize = sub {
    my $i = 4;
    %cmd = runCommand("ls -l $_[0]", %runcmd_flags);
    my @parts = split /\s+/, $cmd{STDOUT};
    if (defined $_[0] && defined $parts[$i] && $parts[$i] =~ /^\d+$/)
    {
      return $parts[$i];
    }
    else
    {
      return -1;
    }
  };

  if (scalar @coredumps)
  {
    foreach my $core (@coredumps)
    {
      # make sure coredump is finished writing out
      # waits for about 2 seconds, at least
      my $abs_core_path = "$coreDumpDir/$core";
      my $init_size     = &$getSize($abs_core_path);
      my $curr_size     = 0;
      my $unchanged     = 0;
      do
      {
        sleep 1;
        $curr_size = &$getSize($abs_core_path);

        $unchanged++ if $curr_size == $init_size;

        if ($curr_size > $init_size)
        {
          $unchanged = 0;
          $init_size = $curr_size;
        }
      } while ($unchanged < 2);

      # Remove any taint from variables
      $core =~ /((\S+)-core\.\d+)/;
      my $filename     = $1;
      my $process_name = $2;
      push @exe_names, $2;

      my $outdir = $ENV{CRILogDir};

      my $core_fullpath = "$outdir/$filename";

      $execs{$process_name}{coredump} = $core_fullpath;

      my $cmd = "$coreDumpDir/$filename $outdir/.";
      $cmd =
        defined $runcmd_flags{host}
        ? "scp -o BatchMode=yes $runcmd_flags{host}:$cmd"
        : "mv $cmd";

      runCommand($cmd, no_coredump_check => 1);
      runCommand("rm $coreDumpDir/$filename", %runcmd_flags)
        if defined $runcmd_flags{host};

      my $gdb_cmd_file = '/tmp/cmds.gdb';

      open(GDB, ">$gdb_cmd_file");
      print GDB "bt\nq\n";
      close GDB;

      %cmd = runCommand("gdb $process_name $core_fullpath -x $gdb_cmd_file -q",
        %non_host_flags);

      open COREDUMPLOG, ">$core_fullpath-gdb-out.log";
      print COREDUMPLOG
        "----STDOUT----\n$cmd{STDOUT}\n----STDERR----\n$cmd{STDERR}";
      close COREDUMPLOG;

      $execs{$process_name}{gdb_log} = "$core_fullpath-gdb-out.log";

#      $recover_subs{$process_name} = sub{ runCommand($process_name, host => $runcmd_flags{host}); };

      # Hard-coding in archiving algorithm for each relevant executable
      my @files;
      if ($process_name eq 'moab')
      {
        my $home = $props->get_property('Moab.Home.Dir');

        my @moab_files = (
          "$home/sbin/moab", "$home/log/moab.log",
          "$home/moab.cfg",  '/tmp/tmp_include.cfg',
          '/tmp/regression.txt'
        );

        foreach (@moab_files)
        {
          if (defined $runcmd_flags{host})
          {
            runCommand(
              "scp -B -r $runcmd_flags{host}:$_ $outdir/. ",
              %runcmd_flags,
              logging_off => 1,
              host        => undef
            );
          }
          else
          {
            runCommand("cp -r $_ $outdir/. ", %runcmd_flags, logging_off => 1);
          }

          my @path = split '/', $_;
          push @files, $path[-1];
        }

        push @files, $filename;
      }
      if ($process_name =~ /^pbs_(\w+)$/)
      {
        my $pbs_type = $1;
        my $home = $props->get_property('Torque.Home.Dir');

        my %torque_files = (
          mom => [
            "$home/sbin/pbs_mom",
            "$home/mom_logs",
            "$home/mom_priv/config",
          ],
          server => [
            "$home/sbin/pbs_server",
            "$home/server_logs",
            "$home/server_priv",
          ],
        );

        foreach(@{$torque_files{$pbs_type}})
        {
          if (defined $runcmd_flags{host})
          {
            runCommand(
              "scp -B -r $runcmd_flags{host}:$_ $outdir/. ",
              %runcmd_flags,
              logging_off => 1,
              host        => undef
            );
          }
          else
          {
            runCommand("cp -r $_ $outdir/. ", %runcmd_flags, logging_off => 1);
          }

          my @path = split '/', $_;
          push @files, $path[-1];
        }

        push @files, $filename;
      }

      runCommand(
        "tar --remove-files -C $outdir -cvzf $core_fullpath.tar.gz "
          . join(' ', @files),
        %non_host_flags,
        logging_off => 1,
      ) if scalar @files;
    }

    foreach my $exe (keys %execs)
    {
      if (exists $badCoredumpExes{$exe})
      {
        diag("---------FOUND SIGNIFICANT COREDUMP!!-----------");
        _writeSpool($test_id, 'COREDUMP'    => $execs{$exe}{coredump});
        _writeSpool($test_id, 'COREDUMPLOG' => $execs{$exe}{gdb_log});

        _setTestStatus -2;

        # Attempt to recover from bad coredump
        my @path         = split '/', realpath($0);
        my $current_test = pop @path;
        my $setup_file   = join('/', @path) . '/setup.t';
        if (-e $setup_file && $current_test ne 'setup.t')
        {
          diag "=== Attempting to Recover... ===";
          execute_tests($setup_file);
        }

        die("Significant COREDUMP was Found, so Killing Test!");
      }
      else
      {
        diag("----Found Coredump, but Insignificant (maybe)----");
        _writeSpool($test_id, 'COREDUMP'    => $execs{$exe}{coredump});
        _writeSpool($test_id, 'COREDUMPLOG' => $execs{$exe}{gdb_log});

        _setTestStatus -2;
      }
    }
  }
}

#------------------------------------------------------------------------------
# Checks the Valgrind XML output for possible memory leaks. Is capable of
#   catching other problems as well.
#------------------------------------------------------------------------------
# PARMAETERS: provided in hashref
# o host = string of hostname of machine to run check on
#------------------------------------------------------------------------------
sub checkValgrindXML
{
  my ($params) = @_;
  my %cmd;

  my $host = $params->{host} || undef;

  my $add_msg = '';
  $add_msg .= " on Remote Host $host" if $host;

  # runCommand options for this subroutine
  my %runcmd_flags = (
    host        => $host,
    logging_off => 1,
    no_checks   => 1,
  );

  # Filters applied to <error></error> XML
  my @filters = (
    qr"<stack>\s+(?:<frame>.+?<fn>malloc</fn>.+?</frame>\s+)*?<frame>.+?<obj>(?:/usr)?/lib/.*?/?(?:ld-|lib(?:c-|sqlite|mysql|odbc)).+?</obj>.+?</frame>"s,
    qr"<kind>UninitCondition</kind>", qr"<frame>\s+</frame>"s,
  );

  # for parsing the filesize of given file
  my $getSize = sub {
    my $i = 4;
    %cmd = runCommand("ls -l $_[0]", %runcmd_flags);
    my @parts = split /\s+/, $cmd{STDOUT};
    if (defined $_[0] && defined $parts[$i] && $parts[$i] =~ /^\d+$/)
    {
      return $parts[$i];
    }
    else
    {
      return -1;
    }
  };

  my @exe_checks =
    map  { (/(\w+)\.do\.memcheck$/)[0] }
    grep {/\w+\.do\.memcheck$/} $props->property_names();

  foreach my $prod (@exe_checks)
  {
    #diag "Checking Valgrind XML for $prod".$add_msg;
    my $outfile = $props->get_property("$prod.memcheck.log")
      || "/tmp/valgrind_$prod.out";

    # wait for filesize to be unchaged to several seconds
    #   (means file is not being written to)
    my $timeout   = time() + 30;
    my $init_size = &$getSize($outfile);
    my $curr_size = -1;
    my $unchanged = -1;

    while ($unchanged < 3 && time() <= $timeout)
    {
      sleep 1 unless $unchanged < 0;

      $curr_size = &$getSize($outfile);

      return if $curr_size <= 1;

      if ($curr_size == $init_size)
      {
        $unchanged++;
      }
      else
      {
        $unchanged = -1;
        $init_size = $curr_size;
      }
    }

    %cmd = runCommand("cat $outfile; echo '' > $outfile", %runcmd_flags);

    if ($cmd{EXIT_CODE})
    {
      CRI::Test::fail("File $outfile not found");
      return;
    }

    my @errors = ($cmd{STDOUT} =~ /(<error>.+?<\/error>)/gs);

    @errors = grep
    {
      sub {
        eval { XMLin($_); };
        return $@ ? 0 : 1;
      };
    } @errors;

    foreach my $filter (@filters)
    {
      @errors = grep { $_ !~ $filter } @errors;
    }

    foreach (@errors)
    {
      s/^\s+<ip>.+?<\/ip>\n//gm;
      s/^\s+<unique>.+?<\/unique>\n//m;
      s/record \d+ of \d+/record X of Y/;
      s/Address 0x\w+ is/Address #location# is/;
    }

    # Remove duplicate errors, if present
    my %tmp_errors;
    $tmp_errors{$_}++ foreach @errors;

    @errors = keys %tmp_errors;

    if (scalar @errors > 0)
    {
      diag( 'Valgrind Found '
          . (scalar @errors)
          . ' Possible Problem(s) in '
          . ucfirst($prod)
          . "${add_msg}! (see test log)");

      for (my $i = 0; $i < scalar @errors; $i++)
      {
        logMsg "++++++VALGRIND PROBLEM #${\($i+1)}++++++\n$errors[$i]";
      }

      return;
    }
  }
}

#------------------------------------------------------------------------------
# replaceUndef(@values)
#------------------------------------------------------------------------------
#
# Used to replace undef values with empty strings for log printing and prevent
# Perl from throwing  concatenation errors.
#------------------------------------------------------------------------------
sub replaceUndef    #(@)
{

  my @params;

  foreach (@_)
  {

    my $element = $_;

    if (!defined $element)
    {

      $element = '';

    }    # END if (!defined $element)

    push @params, $element;

  }    # END foreach (@_)

  return @params;

}    # END sub replaceUndef #(@)

#------------------------------------------------------------------------------
# useForkEnv()
#------------------------------------------------------------------------------
#
# Causes the test to be run with certain fork variables set.  This prevents
# topInit from being called and log info from being printed.
#
#------------------------------------------------------------------------------
sub useForkEnv    #($)
{

  my ($params) = @_;

  # Gather parameters
  my $child_id = $params->{'child_id'};

  # Set the defaults
  $child_id = $$
    unless defined $child_id;

  # Set the environment
  $ENV{TestChildId}  = $child_id;
  $ENV{PrintLogInfo} = 0;
  $imafork           = 1;

}    # END sub useForkEnv #()

#------------------------------------------------------------------------------
# newTestInstance('Description of new test');
#------------------------------------------------------------------------------
# This function will create a new test instance with its own unique id and logs.
# This is useful for scripts that run through multiple permutations in a single
# script.
# PARAMETERS:
# Takes a string that will be used in the setDesc() function.
#------------------------------------------------------------------------------
sub newTestInstance
{
  my $test_desc = $_[0]
    || die 'Must pass in a description for new test instance';

  _finalizeReport();

  $parent_test_id = $test_id unless defined $parent_test_id;

  _init();

  setDesc($test_desc);

  return 1;
}

1;
