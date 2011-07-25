#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Util::Regexp       qw(
                                   QSTAT_q_REGEXP
                                 );
use Torque::Util        qw(
                                   run_and_check_cmd
                                 );
use Torque::Util::Qstat qw(
                                   parse_qstat_q
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -q");

# Variables
my $cmd;
my %qstat;
my %queue_info;

my $queue1 = $props->get_property( 'torque.queue.one' );
my $queue2 = $props->get_property( 'torque.queue.two' );

my @queues = ($queue1, $queue2);

my @attributes = qw(
                     queue
                     memory
                     cpu_time
                     walltime
                     node
                     run
                     que
                     lm
                     state
                   );

# Run the command
$cmd   = "qstat -q";
%qstat = run_and_check_cmd($cmd);

# Parse the output
%queue_info = parse_qstat_q($qstat{ 'STDOUT' });

# Check for the correct output
foreach my $queue (@queues)
  {

  my $msg = "Checking queue '$queue'";
  diag($msg);

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_q_REGEXP->{ $attribute };
    ok($queue_info{ $queue }{ $attribute } =~ /${reg_exp}/, "Checking '$queue' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $queue (@queues)
