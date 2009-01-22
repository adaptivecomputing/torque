#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Job::Ctrl           qw(
                                    submitSleepJob
                                    runJobs
                                    delJobs
                                  );
use Torque::Test::Utils         qw( run_and_check_cmd 
                                    list2array        );
use Torque::Test::Qstat::Utils  qw( parse_qstat_fx    );

# Test Description
plan('no_plan');
setDesc("qalter -M");

# Variables
my $m_cmd;
my $fx_cmd;
my %qstat;
my %qstat_fx;
my %qalter;
my $job_id;
my $mail_users;

# Users and host
my $user1 = $props->get_property('torque.user.one');
my $user2 = $props->get_property('torque.user.two');
my $host  = $props->get_property('Test.Host'       );

# Submit the jobs
my $job_params = {
                   'user'       => $props->get_property('torque.user.one'),
                   'torque_bin' => $props->get_property('Torque.Home.Dir') . '/bin/'
                 };

$job_id = submitSleepJob($job_params);

# Run a job
runJobs($job_id);

# Check qalter -M user1,$user2
$mail_users  = "$user1,$user2";
$m_cmd       = "qalter -M $mail_users $job_id";
%qalter      = run_and_check_cmd($m_cmd);
 
$fx_cmd      = "qstat -f -x $job_id";
%qstat       = run_and_check_cmd($fx_cmd);
%qstat_fx    = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Mail_Users' } eq $mail_users, "Checking if '$m_cmd' was successful");

# Check qalter -M user1@host,user2@host
$mail_users  = "$user1\@$host,$user2\@$host";
$m_cmd       = "qalter -M $mail_users $job_id";
%qalter      = run_and_check_cmd($m_cmd);
 
$fx_cmd      = "qstat -f -x $job_id";
%qstat       = run_and_check_cmd($fx_cmd);
%qstat_fx    = parse_qstat_fx($qstat{ 'STDOUT' });
ok($qstat_fx{ $job_id }{ 'Mail_Users' } eq $mail_users, "Checking if '$m_cmd' was successful");

# Delete the jobs
delJobs($job_id);
