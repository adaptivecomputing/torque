#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib";

use CRI::Test;

plan("no_plan");
setDesc("Update the TORQUE source");

my $source_dir = "$FindBin::Bin/../../../";
my $svn_cmd    = "svn up $source_dir";

my %svn = runCommand($svn_cmd);

cmp_ok($svn{ 'EXIT_CODE' }, '==', 0, "Checking exit code of $svn_cmd");
