#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc('Check the user environment');


# Variables
my $path                       = $ENV{ 'PATH' }            || '';
my $ld_library_path            = $ENV{ 'LD_LIBRARY_PATH' } || '';

my $prefix                     = $props->get_property( 'blcr.home.dir' );

my $blcr_path                  = $prefix . "bin";
my $blcr_ld_library_path       = $prefix . "lib";

my $blcr_path_regex            = $blcr_path;
my $blcr_ld_library_path_regex = $blcr_ld_library_path;

$blcr_path_regex            =~ s/\//\/+/g;
$blcr_ld_library_path_regex =~ s/\//\/+/g;

# Check for the variables.  Unfortunately we can only look for the variables and not set them
ok($path            =~ /${blcr_path_regex}/,            "Checking for '$blcr_path' in the PATH environment variable"                      )
  or diag("PATH: $path");
ok($ld_library_path =~ /${blcr_ld_library_path_regex}/, "Checking for '$blcr_ld_library_path' in the LD_LIBRARY_PATH environment variable")
  or diag("LD_LIBRARY_PATH: $ld_library_path");
