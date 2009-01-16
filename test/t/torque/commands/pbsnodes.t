#!/usr/bin/perl

###########################################################
# this is a simple test that check the mem and ncpu
# from pbsnodes -a
#
# author: smcquay
#
# circa jan/feb '08
###########################################################

use CRI::Test;
plan('no_plan');
setDesc("this test checks pbsnodes output for correct values");
use strict;
use warnings;

my %out = runCommand('pbsnodes -a');
if(!($out{"EXIT_CODE"} == 0))
{
	die("pbsnodes didn't work; is torque up?");
}
else
{
	my $words = $out{"STDOUT"};
	my($a, $b) = $words =~ /np\ *=\ *(\d+).*ncpus=(\d+)/s;
	ok(($a) && ($b) && ($1 == $2), 'does np == ncpus?');
	ok($words =~/totmem=\d+/ && $words =~/availmem=\d+/, 'mem check test (do totmem and availmem have numeric vals?)');
}
