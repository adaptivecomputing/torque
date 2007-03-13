#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 1;

ok($> == 0, "Running as 'root'") or
  BAIL_OUT("Rerun tests as 'root'");
