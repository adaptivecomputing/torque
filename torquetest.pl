#!/usr/bin/perl -w

use strict;
use warnings;
use Test::Harness;
use Getopt::Std;

print '-' x 21, "\n",
      "- TORQUE Test Suite -\n",
      '-' x 21, "\n";

our $opt_u;
getopt('u');
$ENV{'TORQUE_TEST_USER'} = $opt_u
  if defined $opt_u;

my $dir = 't/';
opendir DIR, $dir or die "Cannot open test directory: $!";
my @tests = sort
            grep { /^$dir\d\d_\w+\.t$/   }
            map  { s/[\r\n]//g; "$dir$_" }
            readdir DIR;
closedir DIR;
runtests(@tests);
