#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -S');

my @ports = (
  22,
  3334,
);

foreach my $port (@ports)
{
  stopPbsserver();
  startPbsserver({ 'args' => "-S $port" });
}
