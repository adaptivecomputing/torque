#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

plan('no_plan');
setDesc('Check the user environment');


# Variables
my $path                 = $ENV{ 'PATH' }            || '';
#my $manpath              = $ENV{ 'MANPATH' }         || '';
my $ld_library_path      = $ENV{ 'LD_LIBRARY_PATH' } || '';

my $prefix               = $props->get_property( 'blcr.home.dir' );

my $blcr_path            = $prefix . "bin";
#my $blcr_manpath         = $prefix . "man";
my $blcr_ld_library_path = $prefix . "lib";

# Check for the variables.  Unfortunately we can only look for the variables and not set them
ok($path            =~ /${blcr_path}/,            "Checking for '$blcr_path' in the PATH environment variable"                      )
  or diag("PATH: $path");
#ok($manpath         =~ /${blcr_manpath}/,         "Checking for '$blcr_manpath' in the MANPATH environment variable"                )
#  or diag("MANPATH: $manpath");
ok($ld_library_path =~ /${blcr_ld_library_path}/, "Checking for '$blcr_ld_library_path' in the LD_LIBRARY_PATH environment variable")
  or diag("LD_LIBRARY_PATH: $ld_library_path");
