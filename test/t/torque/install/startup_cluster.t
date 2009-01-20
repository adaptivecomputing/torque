#! /usr/bin/perl 
#* This test starts both pbs_server and pbs_mom

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;
use Torque::Ctrl        qw( startTorque );
use Torque::Test::Utils qw( list2array  );


plan('no_plan'); 
setDesc('Startup Torque');

my @remote_nodes = list2array($props->get_property('torque.remote.nodes'));
startTorque({ 'remote_moms' => \@remote_nodes });
