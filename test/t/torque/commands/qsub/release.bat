#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

setDesc('RELEASE qsub Compatibility Tests');

my $testbase = $FindBin::Bin;


execute_tests("$testbase/setup.t") 
  or die('Could not setup qsub tests');

execute_tests(
              "$testbase/qsub_a.t",
              "$testbase/qsub_A.t",
              "$testbase/qsub_b.t",
              "$testbase/qsub_c.t",
#              "$testbase/qsub_C.t",
              "$testbase/qsub_d.t",
              "$testbase/qsub_D.t",
              "$testbase/qsub_e.t",
              "$testbase/qsub_h.t",
              "$testbase/qsub_I.t",
              "$testbase/qsub_j.t",
              "$testbase/qsub_k.t",
              "$testbase/qsub_l_arch.t",
              "$testbase/qsub_l_cput.t",
              "$testbase/qsub_l_file.t",
              "$testbase/qsub_l_host.t",
              "$testbase/qsub_l_mem.t",
              "$testbase/qsub_l_ncpus.t",
              "$testbase/qsub_l_nice.t",
              "$testbase/qsub_l_nodes.t",
              "$testbase/qsub_l_pcput.t",
              "$testbase/qsub_l_pmem.t",
              "$testbase/qsub_l_pvmem.t",
              "$testbase/qsub_l_stagein.t",
              "$testbase/qsub_l_vmem.t",
              "$testbase/qsub_l_walltime.t",
              "$testbase/qsub_m.t",
              "$testbase/qsub_M.t",
              "$testbase/qsub_N.t",
              "$testbase/qsub_o.t",
              "$testbase/qsub_p.t",
              "$testbase/qsub_q.t",
              "$testbase/qsub_r.t",
              "$testbase/qsub_S.t",
              "$testbase/qsub_stdin.t",
              "$testbase/qsub_u.t",
              "$testbase/qsub_v.t",
              "$testbase/qsub_V.t",
              "$testbase/qsub_W_depend.t",
              "$testbase/qsub_W_group_list.t",
              "$testbase/qsub_z.t",
             ); 

execute_tests("$testbase/cleanup.t"); 

