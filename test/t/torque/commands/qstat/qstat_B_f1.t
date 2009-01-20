#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;

use Torque::Test::Regexp       qw(
                                   QSTAT_BF1_REGEXP
                                 );
use Torque::Test::Utils        qw(
                                   run_and_check_cmd
                                 );
use Torque::Test::Qstat::Utils qw(
                                   parse_qstat_B_f_1
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -B -f -1");

# Variables
my $cmd;
my %qstat;
my %server_info;
my @servers;

# We are creating an array of servers because we may want to test more than
# just one server in the future.
my $server1 = $props->get_property('MoabHost');

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

# Run the command
$cmd   = "qstat -B -f -1";
%qstat = run_and_check_cmd($cmd);

# Parse the output
%server_info = parse_qstat_B_f_1($qstat{ 'STDOUT' });

# Check for the correct output
foreach my $server (@servers)
  {

  my $msg = "Checking server '$server'";
  diag($msg);

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_BF1_REGEXP->{ $attribute };
    ok($server_info{ $server }{ $attribute } =~ /${reg_exp}/, "Checking '$server' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $server (@servers)
