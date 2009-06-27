#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw( 
                                    submitSleepJob
                                    delJobs
                                  );
use Torque::Job::Utils          qw( 
                                    checkForJob
                                  );
use Torque::Util::Qstat  qw(
                                    qstat_fx
                                  );
use Torque::Util         qw( 
                                    run_and_check_cmd 
                                    list2array        
                                  );

# Test Description
plan('no_plan');
setDesc("qdel -W");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;
my $delay;

# Submit a job
$params = {
            'user'       => $props->get_property('torque.user.one'),
            'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/',
            'sleep_time' => 200
          };
$job_id = submitSleepJob($params);

# Check for an error
if ($job_id eq '-1')
  {
  die 'Unable to submit a sleep job';
  }

# Delete the job
$delay = 2;
$cmd   = "qdel -W $delay $job_id";
%qdel  = run_and_check_cmd($cmd);

sleep 2 * $delay;

# Check that job is deleted
my %job_info = qstat_fx();

cmp_ok($job_info{ $job_id }{ 'job_state' }, 'eq', 'C', "Checking job state of job:$job_id");
delJobs($job_id); # Make sure the job gets deleted
