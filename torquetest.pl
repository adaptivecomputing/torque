#!/usr/bin/perl -w

use strict;
use warnings;
use Test::Harness;

print '-' x 21, "\n",
      "- TORQUE Test Suite -\n",
      '-' x 21, "\n";

my $dir = 't/';
opendir DIR, $dir or die "Cannot open test directory: $!";
my @tests = sort
            grep { /^$dir\d\d_\w+\.t$/   }
            map  { s/[\r\n]//g; "$dir$_" }
            readdir DIR;
closedir DIR;
runtests(@tests);
