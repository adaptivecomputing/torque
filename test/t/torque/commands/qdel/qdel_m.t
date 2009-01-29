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
use Torque::Test::Utils         qw( 
                                    run_and_check_cmd 
                                    list2array        
                                  );

# Test Description
plan('no_plan');
setDesc("qdel -m");

# Variables
my $cmd;
my %qdel;
my $job_id;
my $params;
my $message;

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
$message = "Test Message - " . localtime();
$cmd     = "qdel -m '$message' $job_id";
%qdel    = run_and_check_cmd($cmd);

# Check that job is deleted
ok(! checkForJob($job_id), "Checking that the job was deleted")
  or delJobs($job_id); # Make sure the job gets deleted
