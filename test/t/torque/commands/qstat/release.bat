#!/usr/bin/perl

use CRI::Test;
plan('no_plan');

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


my $testbase = $FindBin::Bin;

setDesc("RELEASE qstat Compatibilty Tests");

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
              "$testbase/qstat_a_n1_s_G_R.t",
              "$testbase/qstat_a_n1_s_G_R_jobid.t",
              "$testbase/qstat_a_n1_s_G_R_dest.t",
              "$testbase/qstat_a_n1_s_M_R.t",
              "$testbase/qstat_a_n1_s_M_R_jobid.t",
              "$testbase/qstat_a_n1_s_M_R_dest.t",
              "$testbase/qstat_e_n1_s_G_R.t",
              "$testbase/qstat_e_n1_s_G_R_jobid.t",
              "$testbase/qstat_e_n1_s_G_R_dest.t",
              "$testbase/qstat_e_n1_s_M_R.t",
              "$testbase/qstat_e_n1_s_M_R_jobid.t",
              "$testbase/qstat_e_n1_s_M_R_dest.t",
              "$testbase/qstat_i_n1_s_G_R.t",
              "$testbase/qstat_i_n1_s_G_R_jobid.t",
              "$testbase/qstat_i_n1_s_G_R_dest.t",
              "$testbase/qstat_i_n1_s_M_R.t",
              "$testbase/qstat_i_n1_s_M_R_jobid.t",
              "$testbase/qstat_i_n1_s_M_R_dest.t",
              "$testbase/qstat_r_n1_s_G_R.t",
              "$testbase/qstat_r_n1_s_G_R_jobid.t",
              "$testbase/qstat_r_n1_s_G_R_dest.t",
              "$testbase/qstat_r_n1_s_M_R.t",
              "$testbase/qstat_r_n1_s_M_R_jobid.t",
              "$testbase/qstat_r_n1_s_M_R_dest.t",
              "$testbase/qstat_q.t",              
              "$testbase/qstat_q_G.t",              
              "$testbase/qstat_q_M.t",              
              "$testbase/qstat_q_dest.t",              
              "$testbase/qstat_q_G_dest.t",              
              "$testbase/qstat_q_M_dest.t",
              "$testbase/qstat_Q.t",
              "$testbase/qstat_Q_dest.t",
              "$testbase/qstat_Q_f1.t",
              "$testbase/qstat_Q_f1_dest.t",
              "$testbase/qstat_B.t",
              "$testbase/qstat_B_server_name.t",
              "$testbase/qstat_B_f1.t",
              "$testbase/qstat_B_f1_server_name.t",
);

execute_tests("$testbase/cleanup.t");
