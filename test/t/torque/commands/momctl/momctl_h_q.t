#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Test::Regexp       qw(
                                 HOST_STATES
                                 );
use Torque::Test::Momctl::Diag qw(
                                 test_level_0
                                 test_level_1
                                 test_level_2
                                 test_level_3
                                 );

# Test Description
plan('no_plan');
setDesc('Momctl -h <host> -q <query>');

# Variables
my $momctl;
my $attr;
my $reg_exp;
my $query;

my $host = $props->get_property( 'MoabHost' );

# Perform the tests on the attributes shown in the documentation
# (see: http://www.clusterresources.com/torquedocs21/commands/momctl.shtml)
my %momctl_doc_tests = (
                        'arch'      => '\w+',
                        'availmem'  => '\d+(kb|mb|gb|tb)',
                        'loadave'   => '\d+\.\d{2}',
                        'ncpus'     => '\d+',
                        'netload'   => '\d+',
                        'nsessions' => '\d+',
                        'nusers'    => '\d+',
                        'physmem'   => '\d+(kb|mb|gb|tb)',
                        'sessions'  => '\d+[\s\d+]*',
                        'totmem'    => '\d+(kb|mb|gb|tb)',
                        'opsys'     => '\w+',
                        'uname'     => '.*',
                        'idletime'  => '\d+',
                        'state'     => HOST_STATES,
                       );

while (($attr, $reg_exp) = each (%momctl_doc_tests)) 
  {

  $momctl  = run_query($host, $attr);
  check_query_results($momctl, $host, $attr, $reg_exp);

  } # END while (($attr, $regexp) = each (%momctl_doc_tests)) 

# Use the query to set and reset variables
my @set_unset_tests = qw(
                        status_update_time
                        check_poll_time
                        jobstartblocktime
                        enablemomrestart
                        loglevel
                        down_on_error
                        rcpcmd
                        );

foreach my $set_unset_test (@set_unset_tests)
  {

  $attr    = $set_unset_test;
  $reg_exp = $props->{ '_props' }{ "tmp.mom.config.$attr" };
  $query   = "$attr='$reg_exp'";
  $momctl  = run_query($host, $query);
  check_query_results($momctl, $host, $attr, $reg_exp);
  $reg_exp = $props->{ '_props' }{ "mom.config.$attr" };
  $query   = "$attr='$reg_exp'";
  $momctl  = run_query($host, $query);
  check_query_results($momctl, $host, $attr, $reg_exp);

  } # END foreach my $set_unset_test (@set_unset_tests)

# Perform tests based on the 'Initialization Value' section of 'man pbs_mom' 
my @init_value_tests = qw(
                         configversion
                         version
                         );

foreach my $init_value_test (@init_value_tests)
  {

  $attr    = $init_value_test;
  $reg_exp = $props->get_property('mom.config.' . $attr);
  $momctl  = run_query($host, $attr);
  check_query_results($momctl, $host, $attr, $reg_exp);

  } # END foreach my $init_value_test (@init_value_tests)

# Use the query to cycle moab
$momctl = run_query($host, 'cycle');
ok($momctl->{ 'EXIT_CODE' } == 0,                                               "Checking that 'momctl -q cycle' ran");
ok($momctl->{ 'STDOUT'    } =~ /mom ${host} successfully cycled cycle forced/i, "Checking output of 'momctl -q cycle'");

# Use the query command to run the diagnositics
$momctl = run_query($host, 'diag0');
test_level_0($momctl->{ 'STDOUT' });

$momctl = run_query($host, 'diag1');
test_level_1($momctl->{ 'STDOUT' });

$momctl = run_query($host, 'diag2');
test_level_2($momctl->{ 'STDOUT' });

$momctl = run_query($host, 'diag3');
test_level_3($momctl->{ 'STDOUT' });


###############################################################################
# run_query - Wrapper method for runCommand that checks the exit code
###############################################################################
sub run_query #($$)
 {

  my $host = $_[0];
  my $attr = $_[1];
  my $cmd  = "momctl -h $host -q $attr";

  my %result = runCommand($cmd);
  ok($result{ 'EXIT_CODE' } == 0,"Checking that '$cmd' ran");

  return \%result;

  } #END sub run_query #($$)

###############################################################################
# check_query_results ($$$$)
###############################################################################
sub check_query_results #($$$$)
  {

  my $momctl  = $_[0];
  my $host    = $_[1];
  my $attr    = $_[2];
  my $reg_exp = $_[3];

  # STDOUT
  my $stdout  = $momctl->{ 'STDOUT' };

  # Return value
  my $result = ($stdout =~ /${host}:\s+${attr}\s=\s\'${attr}=${reg_exp}\'/);

  # Perform the test
  ok($result, "Checking 'momctl -h $host -q $attr' output");

  return $result;

  } # END sub check_query_results #($$$$)
