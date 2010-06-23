#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc('RELEASE qsub Compatibility Tests');

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die('Could not setup qsub tests');

execute_tests(
              "$testbase/qsub_a.t",
              "$testbase/qsub_cA.t",
              "$testbase/qsub_c.t",
              "$testbase/qsub_cC.t",
              "$testbase/qsub_d.t",
              "$testbase/qsub_cD.t",
              "$testbase/qsub_e.t",
              "$testbase/qsub_h.t",
              "$testbase/qsub_cI.t",
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
              "$testbase/qsub_cM.t",
              "$testbase/qsub_cN.t",
              "$testbase/qsub_o.t",
              "$testbase/qsub_p.t",
              "$testbase/qsub_q.t",
              "$testbase/qsub_r.t",
              "$testbase/qsub_cS.t",
              "$testbase/qsub_stdin.t",
              "$testbase/qsub_t.t",
              "$testbase/qsub_t_slotlimit.t",
              "$testbase/qsub_t_cW_depend_afterstartarray.t",
              "$testbase/qsub_u.t",
              "$testbase/qsub_v.t",
              "$testbase/qsub_cV.t",
              "$testbase/qsub_cW_depend.t",
              "$testbase/qsub_cW_group_list.t",
              "$testbase/qsub_z.t",
              "$testbase/qsub_b.t",
             ); 

execute_tests("$testbase/cleanup.t"); 

