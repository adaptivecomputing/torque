package CRI::Test;
$VERSION = 1.2;
# Copyright 2007-2009, Cluster Resources Inc.

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../lib/";

use IPC::Run3;
use Cwd qw(realpath); 
use Data::Properties;
use Test::More;
use TAP::Harness; 
use Scalar::Util qw(tainted);
use Time::HiRes qw(gettimeofday usleep);
use Fcntl ':flock';
use sigtrap 'handler', \&CRI::Test::_timed_out,'normal-signals'; # Catch kill signals and cleanup 
use Proc::Daemon;
use POSIX ":sys_wait_h";

# Variables visible throughout module 
our $props;
our $batch;
$ENV{PrintLogInfo} = 1;
my $curbin;
my $scriptName;
my $_LOGFILE;
my $testStatus;
my $logName;
my $commandAlarm = 0;
my @forks = ();
our $zombies = 0;
$SIG{ALRM} = sub { CRI::Test::_timed_out()}; 
$SIG{TERM} = sub { CRI::Test::_cleanExit()}; # I don't think this one works 
$SIG{CHLD} = sub { $zombies++ }; 
my $imafork = 0;
my @kidPids = ();

my $full_props = '/tmp/.test.internal.props';
my $set_props = '/tmp/.test.internal.set.props';
my $fatal_errors = '';
my $coreDumpDir = '/tmp';

my %badCoredumpExes = (
    'cr_checkpoint' 	=> 1,
    'cr_restart' 	=> 1,
    'cr_run' 		=> 1,
    'moab' 		=> 1,
    'pbs_mom' 		=> 1,
    'pbs_sched' 	=> 1,
    'pbs_server' 	=> 1,
    'slurmctld' 	=> 1,
    'slurmd' 		=> 1,
);

# Catches and logs compilation errors
$SIG{__DIE__} = sub {
   
    my $message = join("\n",@_);

    if($message =~ /compilation error/)
    {
	_writeLog(COMPILATION_ERROR => $message);
	$testStatus = 'ERROR';
    }
};

require Exporter;
our @ISA    = qw(Exporter Test::More);
our @EXPORT = (
    @Test::More::EXPORT,
    'runtests',       # Now an alias of execute_tests, deprecated 
    'execute_tests',  # Run a batch of tests. 
    'setTimeout',     # Set how long of an alarm will be used for runCommand calls 
    'runCommand',     # Run a system command, with logging
    'runCommandAs',   # Run a system command as a different user, with logging
    'runCommandSsh',  # Run a system command on a remote host via SSH, with logging (a work-around for a common SSH hangup)
    'forkCommand',    # Fork a command
    'forkCommandAs',  # Fork a command
    'setProps',       # Set properties which will carry over to subsequent tests (in the current test run)
    '$props',         # The properties object (Data::properties)
    'die',            # Die, with logging 
    'setDesc',        # Set the test description
    'logMsg', 	      # Print a message to the test log
    'logEnv',         # Log all the environment variables
    'testTimeout',    # Set a time limit for the test itself
); 


# set initial values for variables used module wide. Add logging data to Summary.log and open handle for individual test log
sub _init 
{
    # Set some initial variables
    my $startTime = Time::HiRes::gettimeofday();
    $ENV{START_TIME} = $startTime;
    $ENV{PrintLogInfo} = 0;    # only print log info on first load

    # set some default props for critical values
    $props = Data::Properties->new();
    $props->set_property('data.props.loc','/tmp/clustertest.props');
    $props->set_property('Outdir','/tmp/');

    # Load system wide config and test-specific props, probably overriding the defaults above  
    if (not exists $ENV{MoabTestOut} or (defined $ENV{MoabTestOut} and $ENV{MoabTestOut} eq '')){
	_topLevelInit(); 
    }

    # Loads internal props file for use in each individual test
    open(FH, "<$full_props") or die "$full_props read error: $!\n";
    flock(FH,LOCK_EX) or die("Bad stuff happened with the file lock on $full_props"); 
    $props->load(\*FH);
    close(FH);

    if( -e $set_props )
    {
	# Loads internal set.props file for use in each individual test
	open(FH, "<$set_props") or die "$set_props read error: $!\n";
	flock(FH,LOCK_EX) or die("Bad stuff happened with the file lock on $set_props"); 
	$props->load(\*FH);
	close(FH);
    }

    # Put all properties in the environment
    my @varNames = $props->property_names();
    foreach (@varNames) { $ENV{$_} = $props->get_property($_); }

    # get script name and full path to script
    $curbin     = realpath($0);
    $scriptName =  $curbin;
    $scriptName =~ s/.*\///;

    $logName = "$ENV{MoabTestOut}/${$}_$scriptName.log";
    open(_LOGFILE, ">>$logName") or die("Couldn't open log file $logName :  $!");
    flock(_LOGFILE,LOCK_EX) or die('Bad stuff happened with the file lock on ' . $logName . "  " . $!); 
    chmod(0777,$logName);
    close(_LOGFILE) or die("Couldn't close logfile : $!");


    # Set up Tap::Harness  
    my %tapHarnessArgs = (
      # verbosity => 1,
	timer     => 1,
	merge     => 1,
	errors    => 1,
    );
    
    $batch = TAP::Harness->new(\%tapHarnessArgs);


    # Set up test stat variables
    my @date = localtime;
    my $timestamp = sprintf
    (
	'%d%02d%02d%02d%02d%02d',
	$date[5] + 1900,
	$date[4] + 1,
	$date[3],
	$date[2],
	$date[1],
	$date[0]
    );

    $startTime = Time::HiRes::gettimeofday();
    my %initialHash =
    (
	PTID        => getppid(),
	SCRIPT      => $curbin,
	DESC        => $scriptName, 
	TEST_RESULT => "RUNNING",
	LOGFILE     => $logName,
	TIME_RUN    => $timestamp, 
	START_TIME  => $startTime, 
    );
    
    # Assume Fail by default
    $testStatus = "FAIL";

    _writeSpool($$,%initialHash);

    _writeLog("SCRIPTNAME",$curbin);
    _writeLog("SCRIPT_PID",$$);
    _writeLog("PATH",$ENV{PATH});
    _writeLog("START_TIME",$startTime);
    
    #$ENV{'PATH'} = $props->get_property('test.base') . "/../bin/tools/:" . $ENV{'PATH'};
} # END sub _init




# Note: This init only should get called at the start of the first script!
# It reads the system wide config and test-specific .props file into a combined
#   internal props file $full_props. Also, creates test log directory and opens
#   the handle for the System.log
sub _topLevelInit 
{
    $ENV{PrintLogInfo} = 1; # This is the top level test, so print the logging info when this test exits 

    # if /etc/clustertest.conf exists, read it in
    if(-e "/etc/clustertest.conf") 
    {
	open(FH, "/etc/clustertest.conf") or print STDERR "Couln't read /etc/clustertest.conf: $!\n";
	flock(FH,LOCK_EX) or die('Bad stuff happened with the file lock on clustertest.conf'); 
	$props->load(\*FH) or print STDERR "Couldn't load Data::Properties $!\n";
	close(FH);
    }
    else{
	print "My /etc/clustertest.conf didn't exist!\n";
    }

    # create a time-stampped test results dir 
    my @date = localtime;
    my $timestamp = sprintf
    (
	'%d-%02d-%02d_%02d.%02d.%02d',
	$date[5] + 1900,
	$date[4] + 1,
	$date[3],
	$date[2],
	$date[1],
	$date[0]
    );

    # Recursively make the outdir or complain if we can't
    my $outdir = $props->get_property("Outdir");
    $outdir =~ s/\/$//; # remove trailing slash if needed
    $outdir .=  '/' . $timestamp;
    chomp($outdir);
    unless (-d $outdir)
    {
	my @path = split('/',$outdir);
	my $makeMe = "/";
	for (my $counter = 1;$counter < @path;$counter++){
	    $makeMe .= $path[$counter] . '/'; 
	    unless (-d $makeMe)
	    {
		mkdir($makeMe,0777) or die "Could not create $outdir. Do you have permission and does "
		. $props->get_property("Outdir")
		. "exist?\nERROR: $!\n";
	    }
	}
    }

    # Set Log outdir across tests
    $ENV{MoabTestOut} = $outdir; 

    # Read in .props file and $set_props, if it exists
    my $dataProps = $props->get_property('data.props.loc');
    if( defined $dataProps && -e $dataProps )
    {
	open(FH, "<$dataProps") or die "$dataProps read error: $!\n";
	flock(FH,LOCK_EX) or die("Bad stuff happened with the file lock on $dataProps"); 
	$props->load(\*FH);
	close(FH);
    }
    else
    {
	print "WARNING: No .props file specified for this test run!!\n";
    }

    if( -f $set_props )
    {
	open(FH, "<$set_props") or die "$set_props read error: $!\n";
	flock(FH,LOCK_EX) or die("Bad stuff happened with the file lock on $set_props"); 
	$props->load(\*FH);
	close(FH);
    }
   
    # Get all keys in $props hash-ref
    my @keys = $props->property_names();

    # Create test.base variables (if nonexistent) for relevant Products
    foreach ('moab', 'torque')
    {
	unless( $props->get_property("$_.test.base") ne '' )
	{
	    if( $props->get_property("$_.branch") ne '' )
	    {
		my $new_str = $props->get_property("$_.test.prefix") . '/' . $props->get_property("$_.branch") . '/t';
		$props->set_property( "$_.test.base" => $new_str );
	    }
	}
    }
    
    # Get an array of Products to use in PATH
    my @prods = grep { /get\.src/ } @keys;

    # Create PATH evironment variable for tests
    foreach (@prods)
    {
	my $prod = $1 if $_ =~ /(\w+)\.get/;
	my $homeDir = $props->get_property("$prod.home.dir");

	foreach my $dir ('bin', 'sbin')
	{
	    $ENV{PATH} = "$homeDir/$dir/:".$ENV{PATH};
	}
    }

    $props->set_property('PATH' => $ENV{PATH} );
    $ENV{MOABHOMEDIR} = $props->get_property('moab.home.dir');

    # Create combined internal props file
    open(FH, ">",$full_props) or die("can't open $full_props $!\n");
    flock(FH,LOCK_EX) or die "Bad stuff happened with the file lock on $full_props"; 
    $props->store(\*FH,"Last written to by $0 during test run with results in $ENV{'MoabTestOut'}") or die("Couldn't save properties to file");
    close(FH);
    
    # Enable Coredump Detection
    qx{echo `ulimit -c unlimited`};
    my $cmd = "echo '$coreDumpDir/\%e-core.\%p' > /proc/sys/kernel/core_pattern";
    `$cmd`;

    # Lines starting with # are ignored by TAP.
    print "#\n#######   Logging results to $ENV{MoabTestOut}\n#\n";
} # END sub _topLevelInit

# appends key/value pairs to a test log (not summary log!)
sub _writeLog
{
    my ($logKey,$logValue) = @_;

    open(_LOGFILE, ">>$logName") or die("Couldn't open $logName : $!");
    flock(_LOGFILE,LOCK_EX) or die('Bad stuff happened with the file lock on ' . $logName . ': ' . $!); 
    chmod(0777,$logName);

    $logValue  = "" if not defined $logValue;
    my $retval = print _LOGFILE "$logKey=$logValue\n"; # ascii values 19,20,15,16
    close(_LOGFILE) or die("Couldn't close logfile : $!");

    return $retval;
} # END sub _writeLog

sub testTimeout {
    my $parentPID = $$;
    my $pid;
    if ($pid = fork()) {
	return 1; 
    } elsif(defined $pid) {

	$imafork = 1;
	die("testTimeout value wasn't a number, it was $_[0]") unless($_[0] =~ m/^[0-9]*$/); 
	sleep($_[0]);
	for my $kill ('TERM','INT','HUP','KILL'){ 
	    logMsg("Sending $kill signal to parent process : $parentPID");
	    last if kill($kill,$parentPID);
	    sleep(2);
	}
	exit(0);
    }

    logMsg("We didn't fork, but should have");
}

sub setTimeout {
    my ($commandAlarm) = @_;
    $commandAlarm = 0 unless($commandAlarm=~ m/^[0-9]*$/);
    $commandAlarm = 0;
} # END setTimeout

sub forkCommandAs 
{

    my ($username,$command) = @_;
    logMsg("Forking command '$command' as user '$username'");

    # Tainted variables are hard to debug...
    if(tainted($command))
    {
	logMsg("The given command given in forkCommandAs was tainted and likely won't work"); 
    }
  
  my ($uid, $gid) = getUserIds($username);
    {  

	local $( =  $gid; 
	local $) = $gid;
	local $< = $uid;
	local $> = $uid; 

	return forkCommand($command);
    }
} # END sub forkCommandAs

# returns ($uid, $gid)
sub getUserIds {
  my ($name,$passwd,$uid,$gid, $quota,$comment,$gcos,$dir,$shell,$expire) = getpwnam(shift);
  return ($uid, $gid);
}
sub reaper {
    my $zombie;
    our %Kid_Status;
    $zombies = 0;
    while (($zombie = waitpid(-1,WNOHANG)) != -1){$Kid_Status{$zombie} = $?;}
}

sub forkCommand {
    logMsg("Forking command '$_[0]'");

    my $pid;

    if($pid = fork()){

	push(@kidPids,$pid) if (@kidPids);
	return 1; 

    } elsif (defined $pid) {


	$ENV{PrintLogInfo} = 0;
	undef(@kidPids);
	$imafork = 1;
	Proc::Daemon::Init;
	runCommand($_[0]);
	exit();
    }

    logMsg("forkCommand didn't work correctly");
    return 0;
} # END forkCommand

# kills all children processes which haven't exited yet
sub _cleanExit {
    logMsg("Test asked to exit cleanly (possibly by a timer). Attempting to terminate children and exit as requested");

    reaper() if $zombies;
    foreach(@kidPids){
	if(kill(0,$_)){ # Sending kill 0 should return true if the process is running
	    for my $kill ('TERM','INT','HUP','KILL'){ 
		logMsg("Sending $kill signal to child process $_");
		last if kill($kill,$_);
		sleep 1;
	    }
	}
    }
    # Hopefully all children are dead now. Kill ourselves too.
    $testStatus = "FAIL";
    _finalizeReport();
    logMsg("Recieved Signal to Terminate");
    `kill -9 $$`;
} # End _cleanExit

sub runCommand
{
    haveCoreDump();
    
    my $command = shift;

    my ($thePid, $stdout, $stderr, $theExitCode);

    # Catch generic errors
    $thePid = $stdout = $stderr = "Run3 gave an error. This probably means the command was not found.";
    $theExitCode = 999;

    # Tainted variables are hard to debug...
    if(tainted($command))
    {
	$thePid = $stdout = $stderr = "The given command was tainted and probably won't work ($command)";
    }

    my @date = localtime;

    my $startTime = gettimeofday(); 
    eval
    {
	local $SIG{ALRM} = sub { die("Command timed out after $commandAlarm seconds. Command was : $command"); }; 
	alarm($commandAlarm);
	$thePid = run3($command,undef,\$stdout,\$stderr);
	alarm(0);
	$theExitCode = $? >> 8;
    };
    alarm(0);
    my $endTime = gettimeofday(); 

    my $timestamp = sprintf
    (
	'%d%02d%02d%02d%02d%02d',
	$date[5] + 1900,
	$date[4] + 1,
	$date[3],
	$date[2],
	$date[1],
	$date[0]
    );

    _writeLog("PROGRAM_RUN",$command);
    _writeLog("CWD",Cwd::getcwd());
    _writeLog("RUN_AS_USER",scalar getpwuid($<));
    _writeLog("RUN_AS_GROUP",scalar getgrgid($());
    _writeLog("TIME_RUN",$timestamp);
    _writeLog("BENCHMARK",$endTime - $startTime);
    _writeLog("PTID",getppid());
    _writeLog("TID",$thePid);
    _writeLog("STDOUT",$stdout);
    _writeLog("STDERR",$stderr);
    _writeLog("EXIT_CODE",$theExitCode);

    if($stderr =~ /cannot load packet size|connection refused/)
    {
	logMsg('Searching for possible Coredump');
	sleep 10;
	haveCoreDump();
    }
    
    if (wantarray)
    {
	return
	(
	    PROGRAM_RUN  => $command,
	    PTID         => getppid(),
	    EXIT_CODE    => $theExitCode,
	    RUN_AS_USER  => scalar getpwuid($<),
	    RUN_AS_GROUP => scalar getpwuid($<),
	    CWD          => Cwd::getcwd(),
	    STDOUT       => $stdout,
	    STDERR       => $stderr,
	    TIME_RUN     => $timestamp,
	    SECONDS_TAKEN=> $endTime - $startTime,
	);
    }

    return $theExitCode;
} # END sub runCommand

sub runCommandSsh
{
    my ($host,$command) = @_;
    
    haveCoreDump({'ssh' => $host});

    my ($thePid, $stdout, $stderr, $theExitCode);

    # Catch generic errors
    $thePid = $stdout = $stderr = "Run3 gave an error. This probably means the command was not found.";
    $theExitCode = 999;

    # Tainted variables are hard to debug...
    if(tainted($command))
    {
	$thePid = $stdout = $stderr = "The given command was tainted and probably won't work ($command)";
    }

    my @date = localtime;

    $command = "ssh root\@$host 'sleep 0.5; $command'";

    my $startTime = gettimeofday();
    eval
    {
	local $SIG{ALRM} = sub { die("Command timed out after $commandAlarm seconds. Command was : $command"); }; 
	alarm($commandAlarm);
	$thePid = run3($command,undef,\$stdout,\$stderr);
	alarm(0);
	$theExitCode = $? >> 8;
    };
    alarm(0);
    my $endTime = gettimeofday(); 

    my $timestamp = sprintf
    (
	'%d%02d%02d%02d%02d%02d',
	$date[5] + 1900,
	$date[4] + 1,
	$date[3],
	$date[2],
	$date[1],
	$date[0]
    );

    _writeLog("PROGRAM_RUN",$command);
    _writeLog("CWD",Cwd::getcwd());
    _writeLog("RUN_AS_USER",scalar getpwuid($<));
    _writeLog("RUN_AS_GROUP",scalar getgrgid($());
    _writeLog("TIME_RUN",$timestamp);
    _writeLog("BENCHMARK",$endTime - $startTime);
    _writeLog("PTID",getppid());
    _writeLog("TID",$thePid);
    _writeLog("STDOUT",$stdout);
    _writeLog("STDERR",$stderr);
    _writeLog("EXIT_CODE",$theExitCode);

    if($stderr =~ /cannot load packet size/)
    {
	logMsg("Searching for possible Coredump on remote host $host");
	sleep 10;
    	haveCoreDump({'ssh' => $host});
    }
    
    if (wantarray)
    {
	return
	(
	    PROGRAM_RUN  => $command,
	    PTID         => getppid(),
	    EXIT_CODE    => $theExitCode,
	    RUN_AS_USER  => scalar getpwuid($<),
	    RUN_AS_GROUP => scalar getpwuid($<),
	    CWD          => Cwd::getcwd(),
	    STDOUT       => $stdout,
	    STDERR       => $stderr,
	    TIME_RUN     => $timestamp,
	    SECONDS_TAKEN=> $endTime - $startTime,
	);
    }

    return $theExitCode;
} # END sub runCommandSsh

sub runCommandAs 
{
    my ($username,$command) = @_;

    haveCoreDump();

    # Tainted variables are hard to debug...
    if(tainted($command))
    {
	logMsg("The given command given in runCommandAs was tainted and likely won't work"); 
    }

    {  
	my ($uid, $gid) = getUserIds($username);

	local $( = $gid; 
	local $) = $gid;
	local $< = $uid;
	local $> = $uid; 

	my $old_cwd = $ENV{'PWD'};
	my $new_cwd = "/home/$username";
	chdir $new_cwd;

	if (wantarray) {
	    my %results = runCommand($command);
	    return %results;
	} 
	else 
	{
	    my $results = runCommand($command);
	    return $results;
	}
	chdir $old_cwd;
    }
} # END sub runCommandAs

# Override functions provided by Test::More so we can log the results 
# Previous versions rehooked the symbol table to override these functions, but it seems to work fine without doing that now.  

no warnings qw(redefine);

sub ok ($;$)
{
    my $result = Test::More::ok($_[0],$_[1]);

    _writeLog("OK[$result]",join(' , ',@_)) or die("Printing threw an error in sub ok() $!\n"); 

    return $result;
} # END ok 

# It seems that cmp_ok makes bad stuff happen in debugging :-/ 
sub cmp_ok ($$$;$)
{
    my ($one,$two,$three,$four) = @_;

    my $result = Test::More::cmp_ok($one,$two,$three,$four);

    _writeLog("CMP_OK[$result]",join(' , ',@_)) or die("Printing threw an error in sub cmp_ok()");

    return $result;
} # END cmp_ok

# Override isnt so we can log these tests too
sub isnt ($$;$)
{
    my ($one,$two) = @_;
    my $isntResult = Test::More::isnt($one,$two);

    _writeLog("ISNT[$isntResult]", join(' , ',@_)) or die("Printing threw an error in sub isnt() $!\n"); 

    return $isntResult;
} # END isnt 

sub like ($$;$)
{
    my ($one, $two, $three) = @_;
    my $likeResults = Test::More::like($one, $two, $three);

    _writeLog("LIKE[$likeResults]" , join(' , ',@_)) or die("Printing threw an error in sub like() $!\n");

    return $likeResults;
} #END like

sub unlike ($$;$)
{
    my ($one, $two,$three) = @_;
    my $unlikeResults = Test::More::unlike($one,$two,$three);

    _writeLog("UNLIKE[$unlikeResults]" , join(' , ',@_)) or die("Printing threw an error in sub unlike() $!\n"); 

    return $unlikeResults;
} # END unlike 

sub is ($$;$)
{
    my ($one,$two,$three) = @_;
    my $isResult = Test::More::is($one,$two,$three);

    _writeLog("IS[$isResult]", join(',',@_)) or die("Printing threw an error in sub is() $!\n"); 

    return $isResult;
} # END is 

sub pass (;$)
{
    _writeLog("PASS",$_[0]) or die("Printing threw an error in sub pass() $!\n"); 

    return Test::More::pass($_[0]);
} # END pass 

sub fail (;$)
{
    _writeLog("FAIL",$_[0]) or die("Printing threw an error in sub fail() $!\n"); 

    return Test::More::fail($_[0]);
} # END fail

sub error (;$)
{
    _writeLog("FATAL_ERROR",$_[0]) or die("Printing threw an error in sub error() $!\n"); 

    print "FATAL ERROR CAUGHT in $_[0]\n";
    return fail("FATAL ERROR in Child Test $_[0]");
} # END error

sub is_deeply
{
    my $isDeep = Test::More::is_deeply(@_);

    my($ref1, $ref2, $desc) = @_;
    my $log_string;
    my $header = "Index#\tExpected    <=>    Got\n";

    my $test = "$ref1";

    if( $test =~ /ARRAY/ )
    {
	$log_string .= $header;
	foreach (0 .. scalar @$ref2 - 1)
	{
	    $log_string .= "$_:\t$ref2->[$_]    <=>\t$ref1->[$_]\n" if $ref2->[$_] ne $ref1->[$_];
	}
    }
    elsif( $test =~ /HASH/ )
    {
	$log_string .= "Key\tExpected-Value    <=>    Got-Value\n";
	foreach (keys %$ref2)
	{
	    $log_string .= "$_:\t$ref2->{$_}    <=>\t$ref1->{$_}\n" if $ref2->[$_] ne $ref1->[$_];;
	}
    }
    else
    {
	$log_string = "Did not recieve an array or hash reference!";
    }

    $log_string = "Data Structures are Equal!" if $log_string eq $header;

    _writeLog("IS_DEEPLY[$isDeep]","$desc\n$log_string") or die('Printing threw an error in sub is_deeply()');

    return $isDeep;
} # END is_deeply

sub diag
{
    my ($one) = @_;
    _writeLog('DIAG',$one);

    Test::More::diag($one);
} # End diag

sub skip
{
    my $skipped = 'unknown';
    $skipped = $_[1] if defined $_[1];

    _writeLog("SKIPPING[$skipped]",$_[0]);

    return Test::More::skip(@_);
} # END skip 

# Catch and log bailout
sub BAIL_OUT 
{
    _writeLog("BAIL_OUT",$_[0]);
    logMsg("WARNING: Running BAIL_OUT can cause parent tests to erroniously report a PASS status"); 
    return Test::More::BAIL_OUT($_[0]);
} # END BAIL_OUT 

sub plan  {
    _writeLog("PLAN",join(", ",@_));
    Test::More::plan(@_); 
} # END PLAN 

use warnings;

use FindBin;
use lib "$FindBin::Bin../../../lib/";


sub runtests {
    return execute_tests(@_);
} # END runtests

sub execute_tests {
    my $aggregate = $batch->runtests(@_); 

    _writeLog('RT_TESTSRUN',join(", ", @_));
    _writeLog('RT_FAILED',join(" ",$aggregate->failed()));
    _writeLog('RT_PARSE_ERROR',join(" ",$aggregate->parse_errors()));
    _writeLog('RT_PASSED',join(" ",$aggregate->passed()));
    _writeLog('RT_SKIPPED',join(" ",$aggregate->skipped()));
    _writeLog('RT_TODO',join(" ",$aggregate->todo()));
    _writeLog('RT_TODO_PASSED',join(" ",$aggregate->todo_passed()));
    _writeLog('RT_WAIT',join(" ",$aggregate->wait()));
    _writeLog('RT_EXIT',join(" ",$aggregate->exit()));
    _writeLog('RT_BENCHMARK',$aggregate->elapsed_timestr());

    foreach($aggregate->passed())
    {
	pass($_ . " (in execute_test)");
    }

    foreach($aggregate->failed())
    {
	fail($_ . " (in execute_test)");
    }

    foreach($aggregate->parse_errors())
    {
	error($_ . " (in execute_test)");
    }

    if(wantarray)
    {
	return $aggregate;
    }
    else
    {
	my @results = $aggregate->failed();
	my $res = scalar @results;
	$res = !$res;
	return $res; 
    }
} # END Execute_tests


sub logMsg
{
    # Tainted variables are hard to debug...
    if(tainted($_[0]))
    {
	logMsg("logMsg was passed a tainted message and may have trouble logging it"); 
    }

    return _writeLog("MSG",$_[0]);
} # END sub logMsg

# This writes to a tmp file used to carry test-set attributes
#   between test runs
sub setProps
{
    my %pairs = @_;
    my $tmp_props = Data::Properties->new();
    
    # Don't want the whole props to be written to $set_props, 
    #    just test-set attributes, so need to ensure this happens

    if( -e $set_props )
    {
	open(FH, "<$set_props") or die "can't open $set_props: $!\n";
	$tmp_props->load(\*FH);
	close FH;
    }

    for (keys %pairs)
    {
	if(tainted($_)){logMsg('You passed setProps a tainted key, this could cause problems');}
	if(tainted($pairs{$_})){logMsg('You passed setProps a tainted value, this could cause problems');}

	$props->set_property($_,$pairs{$_});
	$tmp_props->set_property($_,$pairs{$_});
    }

    open(FH, ">$set_props") or die "can't open $set_props: $!\n";
    flock(FH,LOCK_EX) or die("Bad stuff happened with the file lock on $set_props"); 
    $tmp_props->store(\*FH,"Last written to by $scriptName during test run with results in $ENV{'MoabTestOut'}");
    close(FH);
} # END sub setProps

sub readSpool
{
    require 'shellwords.pl';
    my $spoolFile = "$ENV{MoabTestOut}/Summary.log"; 
    my %data = ();

    unless (-e $spoolFile)
    {
	# Automatically create spool file to avoid error
	open(SPOOL, ">>$spoolFile");
	flock(SPOOL,LOCK_EX) or die('Bad stuff happened with the file lock on ' . $spoolFile); 
	close(SPOOL);
    }

    if (open(SPOOL, "<$spoolFile"))
    {
	flock(SPOOL,LOCK_EX) or die('Bad stuff happened with the file lock on ' . $spoolFile); 

	foreach my $line (<SPOOL>)
	{
	    my @words;
	    eval
	    "
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
	    }
	}
	close(SPOOL);
    }
    else
    {
	die "Couldn't open spool file"; 
    }

    return %data;
} # END sub readSpool

# Taken from old Moab tests. Updates the Summary.log file or adds new values to it as needed
sub _writeSpool
{
    my ($tid,%logInfo) = @_;
    my %currentLog;

    my %wholeLog = readSpool();

    foreach my $key (sort keys %logInfo)
    {
	$wholeLog{$tid}->{$key} = $logInfo{$key}; 
    }

    open(SPOOL, ">$ENV{'MoabTestOut'}/Summary.log") or die "Couldn't open Summary.log!\n";
    flock(SPOOL,LOCK_EX) or die('Bad stuff happened with the file lock on Summary.log'); 

    foreach my $rowID (sort keys %wholeLog)
    {
	print SPOOL $rowID;
	foreach my $key (sort keys %{$wholeLog{$rowID}})
	{
	    print SPOOL qq| $key="$wholeLog{$rowID}->{$key}"|;
	}
	print SPOOL "\n";
    }
    close(SPOOL) or die("Couldn't close spools file");
} # END _writeSpool 

sub setDesc {
    return 0 if ($_[0] =~ /[\n\"]/);

    if(tainted($_[0]))  
    {
	logMsg('The description you tried to set was tainted and may not work');
    }

    _writeSpool($$,DESC=>"$_[0]");
    return 1;
} # END setDesc

# Set a new timeout value. Default is to not timeout 
sub alarm { 
    die("alarm value wasn't a number, it was $_[0]") unless($_[0] =~ m/^[0-9]*$/); 
    alarm(0); # Catch any pending 

    my ($newtime) = @_;
    logMsg("Setting alarm time to $newtime"); 
    alarm($newtime);
} # END alarm

sub _timed_out {
    logMsg("This test timed out and will now be killed");
    $testStatus = "FAIL";
    _finalizeReport();
    CRI::Test::die("Killing test now");
} # END _timed_out

# YAMOTPL: Yet Another Method Overridden To Provide Logging
sub die {
    if(tainted($_[0]))
    {
	logMsg('You gave die a tainted message. This could cause problems');
    }
    $_[0] = "" unless defined($_[0]);

    fail("Died: " .  $_[0]);
    _writeLog("DIED",$_[0]);
    unless($props->get_property('no.die'))
    {
	die($_[0]);
    } else {
	logMsg("Would have died, except that the no.die property was set");
    }
} # END die

sub _finalizeReport
{
    my %finalHash = (TEST_RESULT=>$testStatus);
    _writeSpool($$,%finalHash);

    my $endTime = Time::HiRes::gettimeofday();
    _writeLog("END_TIME",$endTime);
    my $totalTime = $endTime - $ENV{START_TIME};
    _writeLog("TOTAL_TIME",$totalTime);
    if($ENV{PrintLogInfo})
    {
	print "#\n#######   Logged results to $ENV{MoabTestOut}\n#\n";
    }
} # END _finalizeReport

sub logEnv {
    my $envstring = "";
    foreach (keys %ENV){
	$envstring .= $_ . " => " . $ENV{$_} . "\n"; 
    }
    logMsg($envstring);
}

# Check for Seg-Fault and kill script if coredump present
sub haveCoreDump 
{
    my $host = $_[0]->{ssh} if defined $_[0];
    
    my (@exe_names, @coreList, @coreLogs, @coredumps);
    
    if(defined $host)
    {
	my $output = `ssh $host 'sleep 0.5; ls $coreDumpDir'`;

	@coredumps = grep { m/\S+-core\.\d+/ } split(/\n/,$output);
    }
    else
    {
	opendir(DIR, "$coreDumpDir");
	@coredumps = grep { m/\S+-core\.\d+/ } readdir(DIR);
	closedir DIR;
    }

    if( scalar @coredumps )
    {
	foreach my $core (@coredumps)
	{
	    # Remove any taint from variables
	    $core =~ /((\S+)-core\.\d+)/;
	    my $filename = $1;
	    my $process_name = $2;
	    push @exe_names, $2;
	    
	    my $outdir = $1 if $ENV{MoabTestOut} =~ /(.+)/;
	    
	    my $core_fullpath = "$outdir/$filename";
	    
	    my $cmd = "$coreDumpDir/$filename $outdir/.";
	    $cmd = defined $host ? "scp $host:$cmd; ssh $host 'sleep 0.5; rm $coreDumpDir/$filename'" : "mv $cmd";
	    `$cmd`;
	    
	    push @coreList, $core_fullpath;

	    open(GDB, ">/tmp/cmds.gdb");
	    print GDB "bt\nq\n";
	    close GDB;

	    $cmd = "gdb $process_name $core_fullpath -x /tmp/cmds.gdb -q";
	    my ($output, $errors);
	    run3($cmd,undef,\$output,\$errors);

	    open(COREDUMPLOG, ">$core_fullpath-gdb-out.log");
	    print COREDUMPLOG "----STDOUT----\n$output\n----STDERR----\n$errors";
	    close COREDUMPLOG;

	    push @coreLogs, "$core_fullpath-gdb-out.log";
	}

	foreach my $exe (@exe_names)
	{
	    if(exists $badCoredumpExes{$exe})
	    {
		logMsg("---------FOUND SIGNIFICANT COREDUMP!!-----------");
		_writeSpool($$,'COREDUMP'=>join(',',@coreList));
		_writeSpool($$,'COREDUMPLOG'=>join(',',@coreLogs));
		print "# SIGNIFICANT COREDUMP FILE FOUND!\n";

		$testStatus = "COREDUMP";
		_finalizeReport();

		my $cmd = "kill " . getppid();
		`$cmd`;
		CRI::Test::die("Significant COREDUMP was Found, so Killing Test!");
	    }
	    else
	    {
		logMsg("----Found Coredump, but Insignificant (maybe)----");
		_writeSpool($$,'COREDUMP'=>join(',',@coreList));
		_writeSpool($$,'COREDUMPLOG'=>join(',',@coreLogs));
		print "# Possibly Insignificant Coredump Found!\n";
	    }
	}
    }
}

END
{
    # Log results to test log and summary log

    # I'm pretty sure that checking our pass/fail state this way 
    # will always give correct results. If we were defining the 
    # number of tests we planned on running, we could have a 
    # higher degree of surety.
    #
    #  Note: Discovered won't give correct results if it dies or is killed unexpectedly

    exit() if ($imafork);

    my $testObject = Test::More->builder; 
    my @testResults = sort($testObject->summary);

    $testStatus = "PASS" if (@testResults and $testResults[0] == 1 );

    _finalizeReport();
} # END END block


_init(); # Kick it off!
1;


__END__

=head1 NAME

CRI::Test -- A wrapper around Test::More that provides logging and summary reports

=head1 SYNOPSIS

CRI::Test inherits all methods from Test::More and provides 'runCommand' and other  
functions which aid in testing Moab and related programs.

Requires IPC::Run3


=head1 /etc/clustertest.conf

CRI::Test also helps the Moab tester by setting environmental variables based 
on the contents of /etc/clustertest.conf. /etc/clustertest.conf is in the 
Data::Properties format (key=value).  



=head1 Copyright 

  Copyright 2007-2009, Cluster Resources Inc.
  http://www.clusterresources.com/



=head1 FUNCTIONS


=head2 runCommand($command)

runCommand always runs the command given and logs the results.

When the result is assigned to a hash, a hash is returned with the 
following keys: 

      PROGRAM_RUN     -- What command was passed to runCommand (actual executable 
			 used may depend on your $PATH variable!)
      PTID            -- What was the process ID?
      EXIT_CODE       -- What was the exit code? 
      RUN_AS_USER     -- Which user ran the command?
      RUN_AS_GROUP    -- Which group ran the command? 
      CWD             -- What was the working directory when the command was run?  
      STDOUT          -- The STDOUT output 
      STDERR          -- The STDERR output
      TIME_RUN        -- What date and time was the command run
      SECONDS_TAKEN   -- Time taken to run (calculated from the difference of two 
			 gettimeofday commands from Time::HiRes)  

When the result is assigned to a scalar, the exit code of the 
process is returned.

It is up to the user to analyze the returned values and determine 
if the results were successfull. 

If the command completely fails (eg. the command was not found) 
runCommand will have an exit code of 999.

=head2 runCommandSsh($serverName,$command)

Acts like runCommand() but runs the command via SSH on the remote server specified
in the variable $serverName

=head2 runCommandAs($userName,$command)

Does "runCommand($command)" as the specified user. Returns the 
same hash that runCommand() does. 

=head2 forkCommand($command)

Run the given command in a forked process. As with runCommand, results will be 
logged when the process is finished, however they are not returned to the 
calling script. A positive value is returned if the fork was successfull, a zero 
is returned if the fork failed.

=head2 forkCommandAs($user,$command)

Same as forkCommand, but the process is run with the permissions of the specified user.

=head2 setTimeout()

Set the maximum time that a runCommand, runCommandAs, forkCommand or forkCommandAs 
function will run. After the timeout occurs, the command will be killed  


=head2 pass($comment), fail($comment), ok($test,$test_name), is($test,$condition,$test_name), isnt($test,$condition,$test_name), 

pass(), fail(), ok(), is() and isnt() behave exactly as the ok(), is()and isnt() 
in Test::More, except that they logs the results. Returns a 1 if the test passed, 0 if it did not. 

=head2 execute_tests(@testfiles)

Runs the TAP::Harness runtests and returns a TAP::Parser::Aggregator 
object when called in an array context, or a boolean if called in 
a scalar context.  

=head2 logMsg($message)

Appends message to the logfile for this test script.

Returns true on success, false on failure

=head2 setProps(%key_value_pairs), setProps(foo=>"bar")

Adds key-value pairs to the Data::Properties object and writes the modified 
properties to the $full_props file.

Use this method if you want to communicate between tests. Add a property, 
and then access it later in $props->get_property('key')

=head2 setDesc($string)

Set a short description for the current test. Default value is the script name 
(without the path).

The description must not have 1) A newline (\n) or 2) A double quote ("). 

Returns 0 on failure and 1 on success. 

=head2 testTimeout($seconds)

Set a maximum length of time that the test is allowed to run for. The argument 
must be the number of seconds after which the current test and it's children 
will be killed. If testTimeout is not called, the test will run until it exits 
or dies on it's own. This method should only be called once during a test. 

=head2 logEnv()

Log all environment variables to the test's log file. 

=cut
