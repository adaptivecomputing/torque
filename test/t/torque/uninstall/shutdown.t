#! /usr/bin/perl 
#  This test stops both pbs_server and pbs_mom

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

use Torque::Ctrl qw( stopTorque );

plan('no_plan');
setDesc('Shutdown Torque');

stopTorque();
