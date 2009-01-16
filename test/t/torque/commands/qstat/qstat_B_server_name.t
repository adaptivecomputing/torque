#!/usr/bin/perl

use strict;
use warnings;

# Test Modules
use CRI::Test;

use Torque::Test::Regexp       qw(
                                   QSTAT_B_REGEXP
                                 );
use Torque::Test::Utils        qw(
                                   run_and_check_cmd
                                 );
use Torque::Test::Qstat::Utils qw(
                                   parse_qstat_B
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -B [server_name]");

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
                     max
                     tot
                     que
                     run
                     hld
                     wat
                     trn
                     ext
                     status
                   );

# Check for the correct output
foreach my $server (@servers)
  {

  my $msg = "Checking server '$server'";
  diag($msg);

  # Run the command
  $cmd   = "qstat -B $server";
  %qstat = run_and_check_cmd($cmd);

  # Parse the output
  %server_info = parse_qstat_B($qstat{ 'STDOUT' });

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_B_REGEXP->{ $attribute };
    ok($server_info{ $server }{ $attribute } =~ /${reg_exp}/, "Checking '$server' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $server (@servers)
