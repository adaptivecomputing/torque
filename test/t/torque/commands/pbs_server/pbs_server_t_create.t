#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;

my $skip_reason =<<REASON;
This test is being skipped because if run it would trash the testing environment. It is also tested implicitly when installing torque.
REASON

plan(skip_all => $skip_reason);
setDesc('pbs_server');

