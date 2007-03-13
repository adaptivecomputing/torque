#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 4;

ok($> == 0, "Running as 'root'") or
  BAIL_OUT("Rerun tests as 'root'");

my $message = "A username must be supplied for testing job submission.\n"
            . "Do one of the following:\n"
            . "- Set the username using the -u commandline parameter\n"
            . "  torquetest.pl -u <USERNAME>\n"
            . "- Set the \$TORQUE_TEST_USER environment variable.\n";

ok(exists $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  do
    {
    diag($message);
    BAIL_OUT('Set test username');
    };

ok(defined $ENV{'TORQUE_TEST_USER'}, 'Test User Exists') or
  do
    {
    diag($message);
    BAIL_OUT('Set test username');
    };

ok(length $ENV{'TORQUE_TEST_USER'}, 'Valid Test User') or
  do
    {
    diag($message);
    BAIL_OUT('Set test username');
    };

