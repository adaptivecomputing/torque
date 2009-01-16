#!/usr/bin/perl

use strict;
use warnings;

# Test Modules
use CRI::Test;

use Torque::Test::Regexp       qw(
                                   QSTAT_QF1_REGEXP
                                 );
use Torque::Test::Utils        qw(
                                   run_and_check_cmd
                                 );
use Torque::Test::Qstat::Utils qw(
                                   parse_qstat_Q_f_1
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -Q -f -1 [destination]");

# Variables
my $cmd;
my %qstat;
my %queue_info;

my $queue1 = $props->get_property( 'torque.queue.one' );
my $queue2 = $props->get_property( 'torque.queue.two' );

my @queues = ($queue1, $queue2);

my @attributes = qw(
                     queue_type
                     total_jobs
                     state_count
                     resources_default.nodes
                     resources_default.walltime
                     mtime
                     enabled
                     started
                   );

# Check for the correct output
foreach my $queue (@queues)
  {

  my $msg = "Checking queue '$queue'";
  diag($msg);

  # Run the command
  $cmd   = "qstat -Q -f -1 $queue";
  %qstat = run_and_check_cmd($cmd);

  # Parse the output
  %queue_info = parse_qstat_Q_f_1($qstat{ 'STDOUT' });

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_QF1_REGEXP->{ $attribute };
    ok($queue_info{ $queue }{ $attribute } =~ /${reg_exp}/, "Checking '$queue' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $queue (@queues)
