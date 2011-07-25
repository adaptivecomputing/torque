#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("Reinstall 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase    = $FindBin::Bin;
my $blcr_exists = 0;

foreach my $blcr_module ('blcr_imports', 'blcr')
  {

  my %ls_mod = runCommand("lsmod | grep $blcr_module");

  if ($ls_mod{ 'STDOUT' } =~ /${blcr_module}/)
    {

    $blcr_exists = 1;
    last;

    } # END if ($ls_mod{ 'STDOUT' } =~ /${blcr_module}/)

  } # END foreach my $blcr_module ('blcr_imports', 'blcr')

if ($blcr_exists)
  {

  pass("BLCR is already installed.  Skipping the install"); 
  exit 0;

  } # END if ($blcr_exists)

execute_tests(
              "$testbase/uninstall/uninstall.bat",
              "$testbase/install/install.bat",
             );
