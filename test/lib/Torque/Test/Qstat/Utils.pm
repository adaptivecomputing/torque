package Torque::Test::Qstat::Utils;

use strict;
use warnings;

use CRI::Test;

use XML::Simple;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                   parse_qstat
                   parse_qstat_f
                   parse_qstat_f1
                   parse_qstat_a
                   parse_qstat_a_n
                   parse_qstat_a_n_1
                   parse_qstat_a_e_i_r
                   parse_qstat_q
                   parse_qstat_Q
                   parse_qstat_Q_f_1
                   parse_qstat_B
                   parse_qstat_B_f_1
                   list_queue_info
                   );

###############################################################################
# parse_qstat - parse the output of qstat  
###############################################################################
sub parse_qstat #($)
  {

  my ($output) = @_;

  # Return value
  my %rtn_jobs = ();
  
  # Parse the output
  my @lines = split(/\n/, $output);

  # Remove the first couple lines
  shift @lines;
  shift @lines;

  foreach my $line (@lines)
    {

    my ($job_id, $name, $user, $time_use, $state, $queue) = split(/\s+/, $line);

    $job_id = _auto_complete_job_id($job_id);

    $rtn_jobs{ $job_id }{ 'name'     } = $name;
    $rtn_jobs{ $job_id }{ 'user'     } = $user;
    $rtn_jobs{ $job_id }{ 'time_use' } = $time_use;
    $rtn_jobs{ $job_id }{ 'state'    } = $state;
    $rtn_jobs{ $job_id }{ 'queue'    } = $queue;

    } # END foreach my $line (@lines)

  return %rtn_jobs;

  } # END sub parse_qstat #($)


###############################################################################
# parse_qstat_f - parse the output of qstat -f 
###############################################################################
sub parse_qstat_f #($)
  {

  my ($output) = @_;

  # Return Jobs
  my %rtn_jobs = ();

  # Parse the output
  my @jobs = split(/\n\n/, $output);

  foreach my $job (@jobs)
    {

    my @lines = split(/\n/, $job);

    # Get the job_id
    my $line = shift @lines;
    my (undef, $job_id) = split(/:\s+/, $line);

    foreach $line (@lines)
      {

      next if ($line !~ /\s=\s/); # We want to skip the line if is additional 
                                  # lines for attributes like 'Variable_List'

      # Remove trailing and leading spaces
      $line =~ s/^\s+//;
      $line =~ s/\s+$//;

      my ($att_key, $att_value) = split(/\s=\s/, $line);

      # Handle the multi-line Variable_list, the variable list has multiple lines
      # that are not consistent.  We are just going to state that found it.
      if ($att_key eq 'Variable_List')
        {

        $rtn_jobs{ $job_id }{ $att_key } = 1;  

        next;  # We are done with parsing for this loop
  
        } # END if ($key eq 'Variable_List')

      $rtn_jobs{ $job_id }{ $att_key } = $att_value;

      } # END foreach $line (@lines)

    } # END foreach my $job (@jobs)

  return %rtn_jobs;

  } # END sub parse_qstat_f

###############################################################################
# parse_qstat_f1 - parse the output of qstat -f -1
###############################################################################
sub parse_qstat_f1 #($)
  {

  my ($output) = @_;

  # Return Jobs
  my %rtn_jobs = ();

  # Parse the output
  my @jobs = split(/\n\n/, $output);

  foreach my $job (@jobs)
    {

    my @lines = split(/\n/, $job);

    # Get the job_id
    my $line = shift @lines;
    my (undef, $job_id) = split(/:\s+/, $line);

    foreach $line (@lines)
      {

      # Remove trailing and leading spaces
      $line =~ s/^\s+//;
      $line =~ s/\s+$//;

      my ($att_key, $att_value) = split(/\s=\s/, $line);

      if ($att_key eq 'Variable_List')
        {

        my @pbs_attributes = split(/,/, $att_value);

        foreach my $pbs_attribute (@pbs_attributes)
          {

          my ($pbs_key, $pbs_value) = split(/=/, $pbs_attribute);
          $rtn_jobs{ $job_id }{ $att_key }{ $pbs_key } = $pbs_value;  

          } # END foreach my $pbs_attribute (@pbs_attributes)


        next;  # We are done with parsing for this loop
  
        } # END if ($key eq 'Variable_List')

      $rtn_jobs{ $job_id }{ $att_key } = $att_value;

      } # END foreach $line (@lines)

    } # END foreach my $job (@jobs)

  return %rtn_jobs;

  } # END sub parse_qstat_f1

###############################################################################
# parse_qstat_a - parse the output of qstat -a 
###############################################################################
sub parse_qstat_a #($)
  {

  my ($output) = @_;

  # Return value
  my %rtn_jobs = ();
  
  # Parse the output
  my @lines = split(/\n/, $output);

  # Remove the first few lines
  shift @lines;
  shift @lines;
  shift @lines;
  shift @lines;
  shift @lines;

  foreach my $line (@lines)
    {

    my ($job_id, 
        $username, 
        $queue, 
        $jobname, 
        $sessid, 
        $nds, 
        $tsk, 
        $req_mem, 
        $req_time, 
        $state, 
        $elap_time) = split(/\s+/, $line);

    $job_id = _auto_complete_job_id($job_id);

    $rtn_jobs{ $job_id }{ 'job_id'    } = $job_id;
    $rtn_jobs{ $job_id }{ 'username'  } = $username;
    $rtn_jobs{ $job_id }{ 'queue'     } = $queue;
    $rtn_jobs{ $job_id }{ 'jobname'   } = $jobname;
    $rtn_jobs{ $job_id }{ 'sessid'    } = $sessid;
    $rtn_jobs{ $job_id }{ 'nds'       } = $nds;
    $rtn_jobs{ $job_id }{ 'tsk'       } = $tsk;
    $rtn_jobs{ $job_id }{ 'req_mem'   } = $req_mem;
    $rtn_jobs{ $job_id }{ 'req_time'  } = $req_time;
    $rtn_jobs{ $job_id }{ 'state'     } = $state;
    $rtn_jobs{ $job_id }{ 'elap_time' } = $elap_time;

    } # END foreach my $line (@lines)

  return %rtn_jobs;

  } # END sub parse_qstat_a #($)

###############################################################################
# parse_qstat_a_n - parse the output of qstat -a -n
###############################################################################
sub parse_qstat_a_n #($)
  {

  my ($output) = @_;

  # Return value
  my %rtn_jobs = ();
  
  # Parse the output
  my @lines = split(/\n/, $output);

  # Remove the first few lines
  shift @lines;
  shift @lines;
  shift @lines;


  for (my $x = 0; $x < scalar @lines; $x += 2)
    {

    my $line = $lines[$x];

    my ($job_id, 
        $username, 
        $queue, 
        $jobname, 
        $sessid, 
        $nds, 
        $tsk, 
        $req_mem, 
        $req_time, 
        $state, 
        $elap_time) = split(/\s+/, $line);

    my $node = $lines[$x + 1];

    $job_id = _auto_complete_job_id($job_id);
   
    $rtn_jobs{ $job_id }{ 'job_id'    } = $job_id;
    $rtn_jobs{ $job_id }{ 'username'  } = $username;
    $rtn_jobs{ $job_id }{ 'queue'     } = $queue;
    $rtn_jobs{ $job_id }{ 'jobname'   } = $jobname;
    $rtn_jobs{ $job_id }{ 'sessid'    } = $sessid;
    $rtn_jobs{ $job_id }{ 'nds'       } = $nds;
    $rtn_jobs{ $job_id }{ 'tsk'       } = $tsk;
    $rtn_jobs{ $job_id }{ 'req_mem'   } = $req_mem;
    $rtn_jobs{ $job_id }{ 'req_time'  } = $req_time;
    $rtn_jobs{ $job_id }{ 'state'     } = $state;
    $rtn_jobs{ $job_id }{ 'elap_time' } = $elap_time;
    $rtn_jobs{ $job_id }{ 'node'      } = $node;

    } # END for (my $x = 0; $x < scalar @lines; $x += 2)

  return %rtn_jobs;

  } # END sub parse_qstat_a_n #($)

###############################################################################
# parse_qstat_a_n_1 - parse the output of qstat -a -n -1
###############################################################################
sub parse_qstat_a_n_1 #($)
  {

  my ($output) = @_;

  # Return value
  my %rtn_jobs = ();
  
  # Parse the output
  my @lines = split(/\n/, $output);

  # Remove the first few lines
  shift @lines;
  shift @lines;
  shift @lines;

  foreach my $line (@lines)
    {

    my ($job_id, 
        $username, 
        $queue, 
        $jobname, 
        $sessid, 
        $nds, 
        $tsk, 
        $req_mem, 
        $req_time, 
        $state, 
        $elap_time,
        $node) = split(/\s+/, $line);

    $job_id = _auto_complete_job_id($job_id);
   
    $rtn_jobs{ $job_id }{ 'job_id'    } = $job_id;
    $rtn_jobs{ $job_id }{ 'username'  } = $username;
    $rtn_jobs{ $job_id }{ 'queue'     } = $queue;
    $rtn_jobs{ $job_id }{ 'jobname'   } = $jobname;
    $rtn_jobs{ $job_id }{ 'sessid'    } = $sessid;
    $rtn_jobs{ $job_id }{ 'nds'       } = $nds;
    $rtn_jobs{ $job_id }{ 'tsk'       } = $tsk;
    $rtn_jobs{ $job_id }{ 'req_mem'   } = $req_mem;
    $rtn_jobs{ $job_id }{ 'req_time'  } = $req_time;
    $rtn_jobs{ $job_id }{ 'state'     } = $state;
    $rtn_jobs{ $job_id }{ 'elap_time' } = $elap_time;
    $rtn_jobs{ $job_id }{ 'node'      } = $node;

    } # END foreach my $line (@lines)

  return %rtn_jobs;

  } # END sub parse_qstat_a_n_1 #($)

###############################################################################
# parse_qstat_a_e_i_r
###############################################################################
sub parse_qstat_a_e_i_r #($)
  {

  my ($output) = @_;

  # Return value
  my %rtn_jobs = ();
  
  # Parse the output
  my @lines = split(/\n/, $output);

  # Remove the first few lines
  shift @lines;
  shift @lines;
  shift @lines;

  for (my $x = 0; $x < scalar @lines; $x += 2)
    {

    my $line = $lines[$x];

    my ($job_id, 
        $username, 
        $queue, 
        $nds,
        $tsk,
        $req_mem, 
        $req_time, 
        $state, 
        $elap_time,
        $big,
        $fast,
        $pfs,
        $node) = split(/\s+/, $line);

    $job_id = _auto_complete_job_id($job_id);
   
    my $comment = $lines[$x + 1];

    $rtn_jobs{ $job_id }{ 'job_id'    } = $job_id;
    $rtn_jobs{ $job_id }{ 'username'  } = $username;
    $rtn_jobs{ $job_id }{ 'nds'       } = $nds;
    $rtn_jobs{ $job_id }{ 'queue'     } = $queue;
    $rtn_jobs{ $job_id }{ 'tsk'       } = $tsk;
    $rtn_jobs{ $job_id }{ 'req_mem'   } = $req_mem;
    $rtn_jobs{ $job_id }{ 'req_time'  } = $req_time;
    $rtn_jobs{ $job_id }{ 'state'     } = $state;
    $rtn_jobs{ $job_id }{ 'elap_time' } = $elap_time;
    $rtn_jobs{ $job_id }{ 'big'       } = $big;
    $rtn_jobs{ $job_id }{ 'fast'      } = $fast;
    $rtn_jobs{ $job_id }{ 'pfs'       } = $pfs;
    $rtn_jobs{ $job_id }{ 'node'      } = $node;
    $rtn_jobs{ $job_id }{ 'comment'   } = $comment;

    } # END for (my $x = 0; $x < scalar @lines; $x += 2)

  return %rtn_jobs;

  } # END parse_qstat_a_e_i_r 

###############################################################################
# parse_qstat_q
###############################################################################
sub parse_qstat_q #($)
  {

  my ($output)  = @_;

  my %rtn_value = ();

  # parse the lines
  my @lines = split(/\n/, $output);


  # Remove the first three lines
  shift @lines;
  shift @lines;
  shift @lines;
  shift @lines;
  shift @lines;

  # Remove the last two lines
  pop @lines;
  pop @lines;

  # Parse out the information
  foreach my $line (@lines)
    {

    my (
         $queue,
         $memory,
         $cpu_time,
         $walltime,
         $node,
         $run,
         $que,
         $lm,
         $state1,
         $state2
       ) = split(/\s+/, $line);

     
      $rtn_value{ $queue }{ 'queue'    } = $queue;
      $rtn_value{ $queue }{ 'memory'   } = $memory;
      $rtn_value{ $queue }{ 'cpu_time' } = $cpu_time;
      $rtn_value{ $queue }{ 'walltime' } = $walltime;
      $rtn_value{ $queue }{ 'node'     } = $node;
      $rtn_value{ $queue }{ 'run'      } = $run;
      $rtn_value{ $queue }{ 'que'      } = $que;
      $rtn_value{ $queue }{ 'lm'       } = $lm;
      $rtn_value{ $queue }{ 'state'    } = "$state1 $state2";


    } # END foreach my $line (@lines)

  return %rtn_value;

  } # END sub parse_qstat_q #($)

###############################################################################
# parse_qstat_Q
###############################################################################
sub parse_qstat_Q #($)
  {

  my ($output)  = @_;

  my %rtn_value = ();

  # parse the lines
  my @lines = split(/\n/, $output);

  # Remove the first two lines
  shift @lines;
  shift @lines;

  # Parse out the information
  foreach my $line (@lines)
    {

    my (
         $queue,
         $max,
         $tot,
         $ena,
         $str,
         $que,
         $run,
         $hld,
         $wat,
         $trn,
         $ext,
         $t
       ) = split(/\s+/, $line);

     
      $rtn_value{ $queue }{ 'queue' } = $queue;
      $rtn_value{ $queue }{ 'max'   } = $max;
      $rtn_value{ $queue }{ 'tot'   } = $tot;
      $rtn_value{ $queue }{ 'ena'   } = $ena;
      $rtn_value{ $queue }{ 'str'   } = $str;
      $rtn_value{ $queue }{ 'que'   } = $que;
      $rtn_value{ $queue }{ 'run'   } = $run;
      $rtn_value{ $queue }{ 'hld'   } = $hld;
      $rtn_value{ $queue }{ 'wat'   } = $wat;
      $rtn_value{ $queue }{ 'trn'   } = $trn;
      $rtn_value{ $queue }{ 'ext'   } = $ext;
      $rtn_value{ $queue }{ 't'     } = $t;


    } # END foreach my $line (@lines)

  return %rtn_value;

  } # END sub parse_qstat_Q #($)

###############################################################################
# parse_qstat_Q_f_1
###############################################################################
sub parse_qstat_Q_f_1 #($)
  {

  my ($output) = @_;

  # Variables
  my @queues;
  my $queue;
  my @lines;
  my $line;
  my $qname;
  my %queue_info = ();

  # Parse the qstat -Q -f -1 output
  @queues = split(/\n\n/, $output);

  foreach $queue (@queues)
    {

    @lines = split(/\n/, $queue);

    # Get the name of the queue
    $line  =  shift @lines;
    $line  =~ /^Queue\:\s(\w+)/i;
    $qname =  $1;

    $queue_info{ $qname } = {};

    foreach $line (@lines)
      {

      # Remove leading and trailing whitespaces
      $line =~ s/^\s+//;
      $line =~ s/\s+$//;

      my ($key, $value) = split(/\s=\s/, $line);
      $queue_info{ $qname }{ $key } = $value;

      } # END foreach $line (@lines)

    } # END foreach $q (@queues)

  return %queue_info;

  } # END sub parse_qstat_Q_f_1 #($)

###############################################################################
# parse_qstat_B
###############################################################################
sub parse_qstat_B #($)
  {

  my ($output) = @_;

  my %rtn_value = ();

  # parse the lines
  my @lines = split(/\n/, $output);

  # Remove the first two lines
  shift @lines;
  shift @lines;

  # Parse out the information
  foreach my $line (@lines)
    {

    my (
        $server,
        $max,
        $tot,
        $que,
        $run,
        $hld,
        $wat,
        $trn,
        $ext,
        $status
       ) = split(/\s+/, $line);

     
      $rtn_value{ $server }{ 'server' } = $server;
      $rtn_value{ $server }{ 'max'    } = $max;
      $rtn_value{ $server }{ 'tot'    } = $tot;
      $rtn_value{ $server }{ 'que'    } = $que;
      $rtn_value{ $server }{ 'run'    } = $run;
      $rtn_value{ $server }{ 'hld'    } = $hld;
      $rtn_value{ $server }{ 'wat'    } = $wat;
      $rtn_value{ $server }{ 'trn'    } = $trn;
      $rtn_value{ $server }{ 'ext'    } = $ext;
      $rtn_value{ $server }{ 'status' } = $status;

    } # END foreach my $line (@lines)

  return %rtn_value;

  } # END parse_qstat_B #($)

###############################################################################
# parse_qstat_B_f_1
###############################################################################
sub parse_qstat_B_f_1 #($)
  {

  my ($output) = @_;

  # Variables
  my @servers;
  my $server;
  my @lines;
  my $line;
  my $server_name;
  my %server_info = ();

  # Parse the qstat B- -f -1 output
  @servers = split(/\n\n/, $output);

  foreach $server (@servers)
    {

    @lines = split(/\n/, $server);

    # Get the name of the server
    $line  =  shift @lines;
    $line  =~ /^Server\:\s([\w-]+)/i;
    $server_name =  $1;

    $server_info{ $server_name } = { 'server' => $server_name };

    foreach $line (@lines)
      {

      # Remove leading and trailing whitespaces
      $line =~ s/^\s+//;
      $line =~ s/\s+$//;

      my ($key, $value) = split(/\s=\s/, $line);
      $server_info{ $server_name }{ $key } = $value;

      } # END foreach $line (@lines)

    } # END foreach $server (@servers)

  return %server_info;

  } # END sub parse_qstat_B_f_1 #($)

###############################################################################
# list_queue_info
###############################################################################
sub list_queue_info #($)
  {

  my ($queue) = @_;

  $queue = ''
    unless defined $queue;

  # Variables
  my $cmd;
  my %qstat;
  my $output;
  my %queue_info = ();
  
  # Get the information
  $cmd   = "qstat -Q -f -1 $queue";
  %qstat = runCommand($cmd);
  ok($qstat{ 'EXIT_CODE' } == 0, "Checking exit code of '$cmd'")
    or return %queue_info;

  # Parse out the information
  %queue_info = parse_qstat_Q_f1($output);

  return %queue_info;

  } # END sub list_queue_info #($)

###############################################################################
#
#
# PRIVATE METHODS
#
#
##############################################################################

##############################################################################
# _auto_complete_job_id - Completes the full job_id
##############################################################################
sub _auto_complete_job_id #($)
  {

  my ($job_id) = @_;

  # Make sure that we know what the server name is
  my $server_name = $props->get_property( 'Test.Host' )
     || return;

  # Make sure that we do need to complete the job_id
  if ($job_id !~ /\.${server_name}$/)
    {
    
    $job_id =~ s/\..*$/\.${server_name}/;

    } # END 

  return $job_id;

  } # END sub _auto_complete_job_id #($)

1;

=head1 NAME

Torque::Test::Qstat::Utils - Some useful Torque test utilities for the qstat command

=head1 SYNOPSIS

 use Torque::Test::Qalter::Utils qw( 
                                     parse_qstat
                                     parse_qstat_f
                                     parse_qstat_f1
                                     parse_qstat_a
                                     parse_qstat_a_n
                                     parse_qstat_a_n_1
                                     parse_qstat_a_e_i_r
                                     parse_qstat_Q
                                     parse_qstat_Q_f_1
                                     parse_qstat_B
                                     parse_qstat_B_f_1
                                     list_queue_info
                                   );

 # parse_qstat
 my %qstat    = runCommand('qstat');
 my %job_info = parse_qstat($qstat{ 'STDOUT' });

 # parse_qstat_f
 my %qstat    = runCommand('qstat -f');
 my %job_info = parse_qstat($qstat{ 'STDOUT' });

 # parse_qstat_f1
 my %qstat    = runCommand('qstat -f -1');
 my %job_info = parse_qstat($qstat{ 'STDOUT' });

 # parse_qstat_a
 my %qstat    = runCommand('qstat -a');
 my %job_info = parse_qstat_a($qstat{ 'STDOUT' });

 # parse_qstat_a_n
 my %qstat    = runCommand('qstat -a -n');
 my %job_info = parse_qstat_a_n($qstat{ 'STDOUT' });

 # parse_qstat_a_n_1
 my %qstat     = runCommand('qstat -a -n -1');
 my %job_info  = parse_qstat_a_n_1($qstat{ 'STDOUT' });

 # parse_qstat_a_e_i_r
 my %qstat     = runCommand('qstat -a -n -1 -s -G -R');
 my %job_info  = parse_qstat_a_e_i_r($qstat{ 'STDOUT' });

 # parse_qstat_q
 my %qstat      = runCommand('qstat -q');
 my %queue_info = parse_qstat_q($qstat{ 'STDOUT' });

 # parse_qstat_Q
 my %qstat      = runCommand('qstat -Q');
 my %queue_info = parse_qstat_Q1($qstat{ 'STDOUT' });

 # parse_qstat_Q_f_1
 my %qstat      = runCommand('qstat -Q -f -1');
 my %queue_info = parse_qstat_Q_f_1($qstat{ 'STDOUT' });

 # parse_qstat_B
 my %qstat      = runCommand('qstat -B');
 my %queue_info = parse_qstat_B($qstat{ 'STDOUT' });

 # parse_qstat_B_f_1
 my %qstat      = runCommand('qstat -B -f -1');
 my %queue_info = parse_qstat_B_f_1($qstat{ 'STDOUT' });

 # list_queue_info
 my $queue      = 'batch';
 my %queue_info = list_queue_info();
 my %queue_info = list_queue_info($queue);

=head1 DESCRIPTION

Some useful methods to use when running the qstat command.

=head1 SUBROUTINES/METHODS

=over 4

=item parse_qstat

Takes the qstat output and returns a hash of the jobs and their attributes.

=item parse_qstat_f

Takes the qstat -f output and returns a hash of the jobs and their attributes.

=item parse_qstat_f1

Takes the qstat -f -1 output and returns a hash of the jobs and their attributes.

=item parse_qstat_a

Takes the qstat -a output and returns a hash of the jobs and their attributes.

=item parse_qstat_a_n

Takes the qstat -a -n output and returns a hash of the jobs and their attributes.

=item parse_qstat_a_n_1

Takes the qstat -a -n -1 output and returns a hash of the jobs and their attributes.

=item  parse_qstat_a_e_i_r

Takes the qstat -[a|e|i|r] -n -1 -s -G -R output and returns a hash of the jobs and their attributes.

=item  parse_qstat_q

Takes the qstat -q output and returns a hash of the queues and their attributes.

=item  parse_qstat_Q

Takes the qstat -Q output and returns a hash of the queues and their attributes.

=item  parse_qstat_Q_f_1

Takes the qstat -Q -f -1 output and returns a hash of the queues and their attributes.

=item  parse_qstat_B

Takes the qstat -B output and returns a hash of the queues and their attributes.

=item  parse_qstat_B_f_1

Takes the qstat -B -f -1 output and returns a hash of the queues and their attributes.

=item list_queue_info([$queue])

Returns the results of 'qstat -Q -f -1' in a hash.  Takes an option queue name.

=back

=head1 DEPENDENDCIES

Moab::Test, CRI::Util, XML::Simple

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
