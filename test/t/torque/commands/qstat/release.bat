#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qstat Compatibilty Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t")
  or die("Could not set up qstat tests");

execute_tests(
              "$testbase/qstat.t",
              "$testbase/qstat_jobid.t",
              "$testbase/qstat_dest.t",
              "$testbase/qstat_f.t",
              "$testbase/qstat_f_jobid.t",
              "$testbase/qstat_f_dest.t",
              "$testbase/qstat_f1.t",
              "$testbase/qstat_f1_jobid.t",
              "$testbase/qstat_f1_dest.t",
              "$testbase/qstat_a.t",
              "$testbase/qstat_a_n.t",
              "$testbase/qstat_a_n1.t",
              "$testbase/qstat_a_n1_s_cG_cR.t",
              "$testbase/qstat_a_n1_s_cG_cR_jobid.t",
              "$testbase/qstat_a_n1_s_cG_cR_dest.t",
              "$testbase/qstat_a_n1_s_cM_cR.t",
              "$testbase/qstat_a_n1_s_cM_cR_jobid.t",
              "$testbase/qstat_a_n1_s_cM_cR_dest.t",
              "$testbase/qstat_e_n1_s_cG_cR.t",
              "$testbase/qstat_e_n1_s_cG_cR_jobid.t",
              "$testbase/qstat_e_n1_s_cG_cR_dest.t",
              "$testbase/qstat_e_n1_s_cM_cR.t",
              "$testbase/qstat_e_n1_s_cM_cR_jobid.t",
              "$testbase/qstat_e_n1_s_cM_cR_dest.t",
              "$testbase/qstat_i_n1_s_cG_cR.t",
              "$testbase/qstat_i_n1_s_cG_cR_jobid.t",
              "$testbase/qstat_i_n1_s_cG_cR_dest.t",
              "$testbase/qstat_i_n1_s_cM_cR.t",
              "$testbase/qstat_i_n1_s_cM_cR_jobid.t",
              "$testbase/qstat_i_n1_s_cM_cR_dest.t",
              "$testbase/qstat_r_n1_s_cG_cR.t",
              "$testbase/qstat_r_n1_s_cG_cR_jobid.t",
              "$testbase/qstat_r_n1_s_cG_cR_dest.t",
              "$testbase/qstat_r_n1_s_cM_cR.t",
              "$testbase/qstat_r_n1_s_cM_cR_jobid.t",
              "$testbase/qstat_r_n1_s_cM_cR_dest.t",
              "$testbase/qstat_t.t",
              "$testbase/qstat_q.t",              
              "$testbase/qstat_q_cG.t",              
              "$testbase/qstat_q_cM.t",              
              "$testbase/qstat_q_dest.t",              
              "$testbase/qstat_q_cG_dest.t",              
              "$testbase/qstat_q_cM_dest.t",
              "$testbase/qstat_cQ.t",
              "$testbase/qstat_cQ_dest.t",
              "$testbase/qstat_cQ_f1.t",
              "$testbase/qstat_cQ_f1_dest.t",
              "$testbase/qstat_cB.t",
              "$testbase/qstat_cB_server_name.t",
              "$testbase/qstat_cB_f1.t",
              "$testbase/qstat_cB_f1_server_name.t",
);

execute_tests("$testbase/cleanup.t");
