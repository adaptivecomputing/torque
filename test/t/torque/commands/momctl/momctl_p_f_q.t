#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Test::Regexp       qw(HOST_STATES);
use Torque::Test::Momctl::Diag qw(test_mult_diag);

# Test Description
plan('no_plan');
setDesc('Momctl -p <PORT> -f <HOSTLISTFILE> -q <QUERY>');

# Variables
my $momctl;
my $stdout;
my $attr;
my $reg_exp;
my $query;

my $host_list_file = $props->get_property('mom.host.list.file');
my $port           = $props->get_property('mom.host.port');
my @hosts          = split(/,|\s+/, `cat $host_list_file`);

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

  $momctl  = run_query($host_list_file, $port, $attr);
  check_query_results($momctl, $host_list_file, \@hosts, $port, $attr, $reg_exp);

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
  $momctl  = run_query($host_list_file, $port, $query);
  check_query_results($momctl, $host_list_file, \@hosts, $port, $attr, $reg_exp);
  $reg_exp = $props->{ '_props' }{ "mom.config.$attr" };
  $query   = "$attr='$reg_exp'";
  $momctl  = run_query($host_list_file, $port, $query);
  check_query_results($momctl, $host_list_file, \@hosts, $port, $attr, $reg_exp);

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
  $momctl  = run_query($host_list_file, $port, $attr);
  check_query_results($momctl, $host_list_file, \@hosts, $port, $attr, $reg_exp);

  } # END foreach my $init_value_test (@init_value_tests)

# Use the query to cycle moab
$momctl = run_query($host_list_file, $port, 'cycle');
ok($momctl->{ 'EXIT_CODE' } == 0,                                               "Checking that 'momctl -q cycle' ran");

foreach my $host (@hosts)
  {
  ok($momctl->{ 'STDOUT'    } =~ /mom ${host} successfully cycled cycle forced/i, "Checking output of 'momctl -q cycle'");
  }

# Use the query command to run the diagnositics
$momctl = run_query($host_list_file, $port, 'diag0');
test_mult_diag(0, $momctl->{ 'STDOUT' }, \@hosts);

$momctl = run_query($host_list_file, $port, 'diag1');
test_mult_diag(1, $momctl->{ 'STDOUT' }, \@hosts);

$momctl = run_query($host_list_file, $port, 'diag2');
test_mult_diag(2, $momctl->{ 'STDOUT' }, \@hosts);

$momctl = run_query($host_list_file, $port, 'diag3');
test_mult_diag(3, $momctl->{ 'STDOUT' }, \@hosts);

###############################################################################
# run_query - Wrapper method for runCommand that checks the exit code
###############################################################################
sub run_query #($$)
 {

  my $host_list_file = $_[0];
  my $port           = $_[1];
  my $attr           = $_[2];
  my $cmd            = "momctl -p $port -f $host_list_file -q $attr";

  my %result = runCommand($cmd);
  ok($result{ 'EXIT_CODE' } == 0,"Checking that '$cmd' ran");

  return \%result;

  } #END sub run_query #($$)

###############################################################################
# check_query_results ($$$$)
###############################################################################
sub check_query_results #($$$$)
  {

  my ($momctl, $host_list_file, $hosts, $port, $attr, $reg_exp) = @_;

  my $rtn_val = 1;

  # STDOUT
  my $stdout  = $momctl->{ 'STDOUT' };

  # Perform the test
  foreach my $host (@$hosts)
    {

    my $result = ($stdout =~ /${host}:\s+${attr}\s=\s\'${attr}=${reg_exp}\'/);
    ok($result, "Checking 'momctl -p $port -f $host_list_file -q $attr' output for $host");

    $rtn_val = 0 
      unless $result;

    } # END foreach my $host (@$hosts)

  return $rtn_val;

  } # END sub check_query_results #($$$$)
