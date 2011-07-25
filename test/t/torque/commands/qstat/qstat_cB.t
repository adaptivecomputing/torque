#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

use Torque::Util::Regexp       qw(
                                   QSTAT_B_REGEXP
                                 );
use Torque::Util        qw(
                                   run_and_check_cmd
                                 );
use Torque::Util::Qstat qw(
                                   parse_qstat_B
                                 );

# Test Description
plan('no_plan');
setDesc("qstat -B");

# Variables
my $cmd;
my %qstat;
my %server_info;
my @servers;

# We are creating an array of servers because we may want to test more than
# just one server in the future.
my $server1 = $props->get_property('Test.Host').'.ac';

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

# Run the command
$cmd   = "qstat -B";
%qstat = run_and_check_cmd($cmd);

# Parse the output
%server_info = parse_qstat_B($qstat{ 'STDOUT' });

# Check for the correct output
foreach my $server (@servers)
  {

  my $msg = "Checking server '$server'";
  diag($msg);

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_B_REGEXP->{ $attribute };
    ok($server_info{ substr($server,0,16) }{ $attribute } =~ /${reg_exp}/, "Checking '$server' $attribute attribute");

    } # END foreach my $attribue (@attributes)

  } # END foreach my $server (@servers)
