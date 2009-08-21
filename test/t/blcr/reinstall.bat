#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("Reinstall 'Berkley Lab Checkpoint/Restart (BLCR) for Linux'");

my $testbase = $FindBin::Bin;

# Check to see if blcr is alread installed
my $blcr_bin = $props->get_property('blcr.home.dir') . "/bin";
my $cr_run   = "$blcr_bin/cr_run";

if (-e $cr_run)
  {

  my %result = runCommand($cr_run);

  if ($result{ 'EXIT_CODE' } == 0)
    {

    pass("BLCR is already installed.  Skipping the install"); 
    exit 0;

    } # END if ($result{ 'EXIT_CODE' }, '!=', 0)

  } # END if (-e $cr_run)

execute_tests(
              "$testbase/uninstall/uninstall.bat",
              "$testbase/install/install.bat",
             );
