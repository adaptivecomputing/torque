#!/usr/bin/perl
####################################################################
# TRQ-233 - Make sure that qsub -J correctly sets the job ID as well
#  as the output/error paths.
####################################################################
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Job::Ctrl qw( delJobs );
use Torque::Util::Qstat qw( qstat_fx );
use Torque::Test::Qmgr::Utils qw( list_server_info );
plan('no_plan');
setDesc('qsub -J -P');

my $host = $props->get_property('Test.Host');
my $user = $props->get_property('User.1');

my %server = list_server_info();
my $start_id = $server{next_job_number};
my %cmd;

########################
# Test with a plain job
########################
%cmd = runCommand("echo env | qsub -J $start_id -P $user", test_success => 1);

unless( $cmd{EXIT_CODE} )
{
  like($cmd{STDOUT}, qr/^$start_id\.$host(?:\.\w+)?$/m, 'Expected Job ID returned');

  my ($id) = ($cmd{STDOUT} =~ /(\S+)/);

  my %jref = qstat_fx({ job_id => $id });

  like($jref{$id}{Output_Path}, qr/\/STDIN\.o$start_id$/, 'STDOUT file named correctly');
  like($jref{$id}{Error_Path}, qr/\/STDIN\.e$start_id$/, 'STDERR file named correctly');
}

########################
# Test with array jobs
########################
my $array_id = $start_id + 1;
my $num_subjobs = 3;
my @subjobs;
foreach( 1 .. $num_subjobs )
{
  %cmd = runCommand("echo env | qsub -J $array_id\[$_\] -P $user", test_success => 1);
  last if $cmd{EXIT_CODE};
  
  like($cmd{STDOUT}, qr/^$array_id\[$_\]\.$host(?:\.\w+)?$/m, 'Expected Job ID returned');
  push @subjobs, ($cmd{STDOUT} =~ /(\S+)/);
}

unless( $cmd{EXIT_CODE} )
{
  for( @subjobs )
  {
    my %jref = qstat_fx({ job_id => $_ });

    my $file_num = $_;
    $file_num =~ s/\[(\d+)\]\..+$/-$1/;

    like($jref{$_}{Output_Path}, qr/\/STDIN\.o$file_num$/, 'STDOUT file named correctly');
    like($jref{$_}{Error_Path}, qr/\/STDIN\.e$file_num$/, 'STDERR file named correctly');
  }
}

#######################
# Basic failure testing
#######################
%cmd = runCommand("echo env | qsub -J", test_fail => 1, msg => '-J flag requires argument');
like($cmd{STDERR}, qr/^qsub: option requires an argument -- 'J'\nusage: qsub /, 'Expected error message and usage returned');

%cmd = runCommand("echo env | qsub -J 1", test_fail => 1, msg => '-J flag requires -P flag');
like($cmd{STDERR}, qr/^The -J option can only be used in conjunction with -P\nusage: qsub /, 'Expected error message and usage returned');

%cmd = runCommand("echo env | qsub -J 1 -P", test_fail => 1, msg => '-P flag requires argument');
like($cmd{STDERR}, qr/^qsub: option requires an argument -- 'P'\n.+\nusage: qsub /, 'Expected error message and usage returned');

%cmd = runCommand("echo env | qsub -J $start_id -P $user", test_fail => 1, msg => '-J flag prevents ID duplication');
like($cmd{STDERR}, qr/^qsub: Job with requested ID already exists MSG=Job with id $start_id\.$host(?:\.\w+)? already exists, cannot set job id\n\n$/, 'Expected error message and usage returned');

delJobs();
