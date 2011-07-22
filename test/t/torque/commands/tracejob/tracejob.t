#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Util qw(
                            run_and_check_cmd
                          );
use Torque::Job::Ctrl   qw(
                            submitSleepJob
                            runJobs
                            delJobs
                          );
plan('no_plan');
setDesc('tracejob');

my $queue = $props->get_property( 'torque.queue.one' );
my $user  = $props->get_property( 'User.1'  );
my $host  = $props->get_property( 'Test.Host'         );
my $home_dir = $props->get_property('Torque.Home.Dir');
my $date_regex = '\d+/\d+/\d+ \d+:\d+:\d+';

# Submit a job
my $job_params = {
                'user'       => $props->get_property( 'User.1' ),
                'torque_bin' => "$home_dir/bin",
                'sleep_time' => 60
              };

my $job_id = submitSleepJob($job_params);

my @exp_stderr = (
    qr/^$home_dir\/mom_logs\/\d+: No matching job records located$/,
    qr/^$home_dir\/sched_logs\/\d+: No such file or directory$/,
    '',
);
my @exp_lines = (
    '',
    "Job: $job_id",
    '',
    qr/^$date_regex  S    enqueuing into $queue, state 1 hop 1$/,
    qr/^$date_regex  S    Job Queued at request of $user\@$host\.ac, owner = $user\@$host\.ac, job name = STDIN, queue = $queue$/,
    qr/^$date_regex  A    queue=$queue$/,
    '',
);

my %cmd = runCommand("tracejob $job_id", 'test_success_die' => 1);

# Check the output of tracejob
my @lines  = split(/\n/, $cmd{STDOUT});
my @stderr = split(/\n/, $cmd{STDERR});

for( my $i=0; $i < scalar @lines; $i++ )
{
    dynamic_cmp(
	'rept' => $lines[$i],
	'expt' => $exp_lines[$i],
	'msg'  => 'Checking STDOUT Line '.($i+1),
    );
}

for( my $i=0; $i < scalar @stderr; $i++ )
{
    dynamic_cmp(
	'rept' => $stderr[$i],
	'expt' => $exp_stderr[$i],
	'msg'  => 'Checking STDERR Line '.($i+1),
    );
}
