#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc('Run the ./configure script for BLCR');

# Variables
my $blrc_prefix        = $props->get_property( 'blcr.home.dir'        );
my $blcr_extracted_dir = $props->get_property( 'blcr.build.dir' );
my $configure_script   = "$blcr_extracted_dir/configure";

# Make sure the extracted directory exists
ok(-e $configure_script, "Checking for the BLCR configure script at '$configure_script'")
  or die "BLCR configure command not found";

# cd to the blcr directory
ok(chdir $blcr_extracted_dir, "Changing directory to '$blcr_extracted_dir'")
  or die "Unable to switch to directory '$blcr_extracted_dir'";

# Extract the file
logMsg("Running the configure command ...\n");
my $configure_cmd = "$configure_script --prefix $blrc_prefix";
my %configure     = runCommand($configure_cmd);

# Check the configuration results
ok($configure{ 'EXIT_CODE' } == 0,                                "Checking the exit code for '$configure_cmd'"   );
ok(-e "$blcr_extracted_dir/Makefile",                             "Check for the existance of the 'Makefile'"     );
ok(`grep "prefix='$blrc_prefix'" $blcr_extracted_dir/config.log`, "Check the prefix variable in the 'config.log'" );
