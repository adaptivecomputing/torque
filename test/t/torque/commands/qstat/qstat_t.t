#!/usr//bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;


use Torque::Job::Ctrl    qw(
                             qsub
                             runJobs
                             delJobs 
                           );
use Torque::Util::Regexp qw(
                             QSTAT_REGEXP
                           );
use Torque::Util         qw(
                             run_and_check_cmd
                             job_info
                           );
use Torque::Util::Qstat  qw(
                             parse_qstat
                           );
use Torque::Job::Utils   qw(  
                             generateArrayIds         
                           );

# Test Description
plan('no_plan');
setDesc("qstat -t");

# Variables
my $cmd;
my %qstat;
my $jid;
my @job_ids;
my %job_info;
my $job_params;
my $id_exp = '0-1';

my @attributes = qw(
                     name
                     user
                     time_use
                     state
                     queue
                   );

# Submit a job
$job_params = {
                'flags' => "-t $id_exp"
              };

$jid = qsub($job_params);
@job_ids = generateArrayIds({
                              'job_id' => $jid,
                              'id_exp' => $id_exp
                           });

# Test qstat
sleep_diag(1, "Allow job to be queued");
$cmd   = "qstat -t";
%qstat = run_and_check_cmd($cmd);

%job_info = parse_qstat( $qstat{ 'STDOUT' } );

foreach my $job_id (@job_ids)
  {

  my $msg = "Checking job '$job_id'";
  diag($msg);
  logMsg($msg);

  foreach my $attribute (@attributes)
    {

    my $reg_exp = &QSTAT_REGEXP->{ $attribute };
    like($job_info{ $job_id }{ $attribute }, qr/${reg_exp}/, "Checking the '$job_id' $attribute attribute"); 

    } # END foreach my $attribute (@attributes)

  } # END foreach my $job_id (@job_ids)

# Delete the job
delJobs(@job_ids);
