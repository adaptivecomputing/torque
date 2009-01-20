#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc('Uninstall (make uninstall) BLCR');

# Variables
my $blcr_build_dir = $props->get_property( 'blcr.build.dir' );
my $makefile       = "$blcr_build_dir/Makefile";

SKIP:
  {

  # Make sure the extracted directory exists
  skip("$makefile not found. Must not be installed", 1) 
    if (! -e $makefile);

  # Extract the file
  logMsg("Running the make uninstall command ...\n");
  my $make_cmd = "cd $blcr_build_dir; make uninstall";
  my %make     = runCommand($make_cmd);

  # Check the make uninstall results
  cmp_ok($make{ 'EXIT_CODE' }, '==', 0, "Checking the exit code for '$make_cmd'"  );

  }; # END SKIP:

my $rm_cmd    = "rm -rf $blcr_build_dir";
my %rm_result = runCommand($rm_cmd);

cmp_ok($rm_result{ 'EXIT_CODE' }, '==', 0, "Checking the exit code for '$rm_cmd'")
  or diag("STDERR: $rm_result{ 'STDERR' }");
