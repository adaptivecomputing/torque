#! /usr/bin/perl 
#* This test starts both pbs_server and pbs_mom
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


use CRI::Test;
use Torque::Ctrl;

plan('no_plan'); 
setDesc('Startup Torque');

startTorque();
