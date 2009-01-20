#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Expect;

use Torque::Ctrl        qw(
                            startPbssched
                            stopPbssched
                          );
use Torque::Job::Ctrl   qw(
                            runJobs
                            delJobs 
                          );
use Torque::Test::Utils qw( 
                           job_info
                          );                        

plan('no_plan');
setDesc('qsub -I');

my $user1 = $props->get_property('moab.user.one');
my %command;

my $cmd = "su - $user1 -m -c 'qsub -I'";
my $mPattern = 'qsub: job (\d+\S+) ready';
my $timeout = 10;

# Delete all jobs
diag("Delete all jobs");
delJobs( 'all' );

# create an Expect object by spawning another process
my $exp = new Expect;
$exp->raw_pty(1);
$exp->log_stdout(0);
$exp->log_file(\&logIt);

diag("Test qsub -I");
$exp->spawn($cmd)
  or die "Cannot spawn $cmd: $!\n";   

# Give the job time to submit
sleep 1;

# Run the job - This assumes there is only one job in the queue
my %job_info = job_info();
my ($job_id) = keys %job_info;
runJobs($job_id);

unless (ok(defined $exp->expect($timeout,'-re',$mPattern),"Checking that Interactive Job started"))
{
   $exp->hard_close;
   die "Didn't match pattern '$mPattern'";
}

my $pbsJobId = $1 if $exp->exp_match =~ /(\d+\S+)/;

# Run qstat -f on the submitted job and look for interactive and rerunable
%command = runCommand("qstat -f $pbsJobId");

my $interactive = $1 if $command{STDOUT} =~ /interactive = (\w+)/;
my $rerunable = $1 if $command{STDOUT} =~ /Rerunable = (\w+)/;

# Clean up the child process and the job script
$exp->soft_close;

cmp_ok($interactive,'eq','True',"Checking that Interactive is Set for Job $pbsJobId in 'qstat'");
cmp_ok($rerunable,'eq','False',"Checking that Rerunable is NOT Set for Job $pbsJobId in 'qstat'");

# Delete all jobs
diag("Delete all jobs");
delJobs( 'all' );

###############################################################################
# logIt
###############################################################################
sub logIt 
  {

    foreach my $logline (@_)
      {

   	  logMsg("Expect log: $logline");

      } # END foreach my $logline (@_)

  } # END foreach my $logline (@_)
