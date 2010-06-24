#! /usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

use Torque::Util::Qstat qw( qstat_tfx                        );
use Torque::Job::Ctrl   qw( qsub             delJobs runJobs );
use Torque::Job::Utils  qw( generateArrayIds                 );

plan('no_plan');
setDesc('Qsub -t -W (afterstartarray)');

# Variables
my %qhash       = ();
my $qref        = {};
my $id_exp      = '0-1';
my $jid1        = undef;
my $jid1_mod    = undef;
my $jid2        = undef;
my $jid3        = undef;
my @jaids       = ();
my $test_host   = `hostname -f`;
chomp $test_host;

# Perform the test
diag("Test dependency");
$qref = {
          'cmd'   => "sleep 5",
          'flags' => "-t $id_exp",
        };
$jid1 = qsub($qref);
sleep_diag(1, "Allow time for the job to queue");

@jaids = generateArrayIds({
                           'job_id' => $jid1,
                           'id_exp' => $id_exp,
                         });


$qref = {
          'flags' => "-W depend=afterstartarray:$jid1",
        };
$jid2 = qsub($qref);


sleep_diag(1, "Allow time for the job to queue");


%qhash = qstat_tfx();

foreach my $jaid (@jaids)
  {

  ok(defined $qhash{ $jaid }, "Checking job:$jid1 for subjob:$jaid");
  cmp_ok($qhash{ $jaid }{ 'job_array_request' }, 'eq', $id_exp, "Verifying 'job_array_request' for subjob:$jaid");

  } # END foreach my $jaid (@jaids)

cmp_ok($qhash{ $jid2 }{ 'job_state'  }, 'eq', "H",                                 "Verifying the dependent job:$jid2 'job_state'" );
cmp_ok($qhash{ $jid2 }{ 'Hold_Types' }, 'eq', "s",                                 "Verifying the dependent job:$jid2 'Hold_Types'"); 
cmp_ok($qhash{ $jid2 }{ 'depend'     }, 'eq', "afterstartarray:$jid1\@$test_host", "Verifying the dependent job:$jid2 'depend'"    );

# Test specifying number of jobs
diag("Test specifying number of jobs");
$jid1_mod = $jid1;
$jid1_mod =~ s/\[\]/\[\]\[1\]/;
$qref = {
          'flags' => "-W depend=afterstartarray:$jid1_mod",
        };
$jid3 = qsub($qref);

%qhash = qstat_tfx();
cmp_ok($qhash{ $jid3 }{ 'job_state'  }, 'eq', "H",                                 "Verifying the dependent job:$jid3 'job_state'" );
cmp_ok($qhash{ $jid3 }{ 'Hold_Types' }, 'eq', "s",                                 "Verifying the dependent job:$jid3 'Hold_Types'"); 
cmp_ok($qhash{ $jid3 }{ 'depend'     }, 'eq', "afterstartarray:$jid1_mod\@$test_host", "Verifying the dependent job:$jid3 'depend'"    );

diag("Run first subjob:$jaids[0]");
runJobs($jaids[0]);
sleep_diag(1);

%qhash = qstat_tfx();
cmp_ok($qhash{ $jid2 }{ 'job_state'  }, 'eq', "H",                                 "Verifying the dependent job:$jid2 'job_state' doesnt exist after a subjob:$jaids[0] is run" );
cmp_ok($qhash{ $jid2 }{ 'Hold_Types' }, 'eq', "s",                                 "Verifying the dependent job:$jid2 'Hold_Types' doesnt exist after a subjob:$jaids[0] is run"); 
cmp_ok($qhash{ $jid2 }{ 'depend'     }, 'eq', "afterstartarray:$jid1\@$test_host", "Verifying the dependent job:$jid2 'depend' doesnt exist after a subjob:$jaids[0] is run"    );

cmp_ok($qhash{ $jid3 }{ 'job_state'  }, 'eq', "Q",                                 "Verifying the dependent job:$jid3 'job_state' doesnt exist after a subjob:$jaids[0] is run" );
cmp_ok($qhash{ $jid3 }{ 'Hold_Types' }, 'eq', "n",                                 "Verifying the dependent job:$jid3 'Hold_Types' doesnt exist after a subjob:$jaids[0] is run"); 
cmp_ok($qhash{ $jid3 }{ 'depend'     }, 'eq', "afterstartarray:$jid1_mod\@$test_host", "Verifying the dependent job:$jid3 'depend'"    );

diag("Run second subjob:$jaids[1]");
runJobs($jaids[1]);
sleep_diag(1);

%qhash = qstat_tfx();
cmp_ok($qhash{ $jid2 }{ 'job_state'  }, 'eq', "Q", "Verifying the dependent job:$jid2 'job_state' doesnt exist after a subjob:$jaids[1] is run" );
cmp_ok($qhash{ $jid2 }{ 'Hold_Types' }, 'eq', "n", "Verifying the dependent job:$jid2 'Hold_Types' doesnt exist after a subjob:$jaids[1] is run"); 
cmp_ok($qhash{ $jid2 }{ 'depend'     }, 'eq', "afterstartarray:$jid1\@$test_host", "Verifying the dependent job:$jid2 'depend' doesnt exist after a subjob:$jaids[0] is run"    );

# Cleanup
delJobs();
