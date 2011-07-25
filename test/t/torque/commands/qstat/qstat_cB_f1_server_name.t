#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Util::Regexp       qw(
                                   QSTAT_BF1_REGEXP
                                 );
use Torque::Util        qw(
                                   run_and_check_cmd
                                 );
use Torque::Util::Qstat qw(
                                   parse_qstat_B_f_1
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -B -f -1 [server_name]");

# Variables
my $cmd;
my %qstat;
my %server_info;
my @servers;

# We are creating an array of servers because we may want to test more than
# just one server in the future.
my $server1 = $props->get_property('Test.Host');

push(@servers, $server1);

# Server attributes
my @attributes = qw(
                     server
                     server_state
                     scheduling
                     total_jobs
                     state_count
                     acl_hosts
                     default_queue
                     log_events
                     mail_from
                     scheduler_iteration
                     node_check_rate
                     tcp_timeout
                     mom_job_sync
                     pbs_version
                     keep_completed
                     next_job_number
                     net_counter
                   );

# Check for the correct output
foreach my $server (@servers)
  {

  my $msg = "Checking server '$server'";
  diag($msg);

  # Run the command
  $cmd   = "qstat -B -f -1 $server";
  %qstat = run_and_check_cmd($cmd);

  # Parse the output
  %server_info = parse_qstat_B_f_1($qstat{ 'STDOUT' });

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_BF1_REGEXP->{ $attribute };
    like($server_info{ $server }{ $attribute }, qr/$reg_exp/, "Checking '$server' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $server (@servers)
