package Torque::Util::Regexp;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../..";


use CRI::Test;

use base 'Exporter';

our @EXPORT_OK = qw(
                   HOST_STATES
                   NTYPES
                   CHECKPOINT_FILE_NAME
                   QSTAT_REGEXP
                   QSTAT_A_REGEXP
                   QSTAT_B_REGEXP
                   QSTAT_BF1_REGEXP
                   QSTAT_F_REGEXP
                   QSTAT_q_REGEXP
                   QSTAT_Q_REGEXP
                   QSTAT_QF1_REGEXP
                   );

###############################################################################
# Regular Expressions
###############################################################################
use constant HOST_STATES          => '(free|offline|down|reserve|job-exclusive|job-sharing|busy|state-unknown|time-shared|cluster)';
use constant NTYPES               => '(cluster|time-shared)';
use constant CHECKPOINT_FILE_NAME => '^ckpt\.\d+(\.\S+)+\.\d+';

# QSTAT regular expressions
use constant QSTAT_REGEXP         => {
                                       'name'     => '^[\w\[\]]+$',
                                       'user'     => '^\w+$',
                                       'time_use' => '^(0|\d{2}(:\d{2})+)$',
                                       'state'    => '^(Q|R|W|H|T|C)$',
                                       'queue'    => '^\w+$'
                                     };
use constant QSTAT_A_REGEXP       => {
                                       'job_id'    => '^\d+\.[\w-]+$',
                                       'username'  => '^\w+$',
                                       'queue'     => '^\w+$',
                                       'jobname'   => '^\w+$',
                                       'sessid'    => '^(--|\d+)$',
                                       'nds'       => '^\d+$',
                                       'tsk'       => '^(--|\d+)$',
                                       'req_mem'   => '^(--|\d+(b|w|kb|kw|mb|mw|gb|gw|tb|tw)?)$',
                                       'req_time'  => '^(--|\d{2}(:\d{2})+)$',
                                       'state'     => '^(Q|R|W|H|T|C)$',
                                       'big'       => '^(--|\w+)$',
                                       'fast'      => '^(--|\w+)$',
                                       'pfs'       => '^(--|\w+)$',
                                       'elap_time' => '^(--|\d{2}(:\d{2})+)$',
                                       'node'      => '(--|[\w-]+\/\d+)',
                                       'comment'   => '(--|\w+)'
                                     };
use constant QSTAT_B_REGEXP       => {
                                       'server' => '^[\w-]+$',
                                       'max'    => '^\d+$',
                                       'tot'    => '^\d+$',
                                       'que'    => '^\d+$',
                                       'run'    => '^\d+$',
                                       'hld'    => '^\d+$',
                                       'wat'    => '^\d+$',
                                       'trn'    => '^\d+$',
                                       'ext'    => '^\d+$',
                                       'status' => '^(Hot_Start|Active|Terminating_Delay|Terminating)$'
                                     };
use constant QSTAT_BF1_REGEXP     => {
                                       'server'              => '^[\w-]+$',
                                       'server_state'        => '^(Hot_Start|Active|Terminating_Delay|Terminating)$',
                                       'scheduling'          => '^(True|False)$',
                                       'total_jobs'          => '^\d+$',
                                       'state_count'         => '^Transit:\d+\s+Queued:\d+\s+Held:\d+\s+Waiting:\d+\s+Running:\d+\s+Exiting:\d+$',
                                       'acl_hosts'           => '[\w-]+(,[\w-]./+)*',
                                       'managers'            => '\w+\@\w+(,w+\@\w+)*',
                                       'operators'           => '\w+\@\w+(,w+\@\w+)*',
                                       'default_queue'       => '^\w+$',
                                       'log_events'          => '^\d+$',
                                       'mail_from'           => '^\w+$',
                                       'scheduler_iteration' => '^\d+$',
                                       'node_check_rate'     => '^\d+$',
                                       'tcp_timeout'         => '^\d+$',
                                       'mom_job_sync'        => '^(True|False)$',
                                       'pbs_version'         => $props->get_property('mom.config.version'),
                                       'keep_completed'      => '^\d+$',
                                       'next_job_number'     => '^\d+$',
                                       'net_counter'         => '^\d+\s+\d+\s+\d+$'
                                     };
use constant QSTAT_F_REGEXP       => {
                                       'Job_Name'                => '\w+',
                                       'Job_Owner'               => '\w+\@\w+',
                                       'job_state'               => '^(Q|R|W|H|T|C)$', # May be incomplete list of states
                                       'queue'                   => '^\w+$',
                                       'server'                  => '^[\w-]+$',
                                       'Checkpoint'              => '^\w+$',
                                       'ctime'                   => '^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$',
                                       'Error_Path'              => '\w+\:(\/[\w\.]+)+',
                                       'Hold_Types'              => '^\w$',
                                       'Join_Path'               => '^\w$',
                                       'Keep_Files'              => '^\w$',
                                       'Mail_Points'             => '^\w$',
                                       'mtime'                   => '^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$',
                                       'Output_Path'             => '\w+\:(\/[\w\.]+)+',
                                       'Priority'                => '^\d$',
                                       'qtime'                   => '^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$',
                                       'Rerunable'               => '^(True|False)$',
                                       'Resource_List.neednodes' => '^\d+$',
                                       'Resource_List.nodect'    => '^\d+$',
                                       'Resource_List.nodes'     => '^\d+$',
                                       'Resource_List.walltime'  => '^\d\d:\d\d:\d\d$',
                                       'substate'                => '^\d+$',
                                       'Variable_List'           => '^.+$',         # Need a stronger test
                                       'euser'                   => '^\w+$',
                                       'egroup'                  => '^\w+$',
                                       'queue_rank'              => '^\d+$',
                                       'queue_type'              => '^\w$',
                                       'etime'                   => '^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$'
                                     };
use constant QSTAT_q_REGEXP       => {
                                       'queue'    => '^[\w-]+$',
                                       'memory'   => '^(--|\d+(b|w|kb|kw|mb|mw|gb|gw|tb|tw)?)$',
                                       'cpu_time' => '^(--|\d\d:\d\d:\d\d)$',
                                       'walltime' => '^(--|\d\d:\d\d:\d\d)$',
                                       'node'     => '^(--|\d+)$',
                                       'run'      => '^\d+$',
                                       'que'      => '^\d+$',
                                       'lm'       => '^(--|\d+)$',
                                       'state'    => '^(E|D)\s+(R|S)$'
                                     };
use constant QSTAT_Q_REGEXP       => {
                                       'queue' => '^\w+$',
                                       'max'   => '^\d+$',
                                       'tot'   => '^\d+$',
                                       'ena'   => '^(yes|no)$',
                                       'str'   => '^(yes|no)$',
                                       'que'   => '^\d+$',
                                       'run'   => '^\d+$',
                                       'hld'   => '^\d+$',
                                       'wat'   => '^\d+$',
                                       'trn'   => '^\d+$',
                                       'ext'   => '^\d+$',
                                       't'     => '^(E|R)$'
                                     };
use constant QSTAT_QF1_REGEXP     => {
                                       'queue_type'                 => '^(Execution|Route)$',
                                       'total_jobs'                 => '^\d+$',
                                       'state_count'                => '^Transit:\d+\s+Queued:\d+\s+Held:\d+\s+Waiting:\d+\s+Running:\d+\s+Exiting:\d+$',
                                       'resources_default.nodes'    => '^\d+$',
                                       'resources_default.walltime' => '^\d\d:\d\d:\d\d$',
                                       'mtime'                      => '^\d+$',
                                       'enabled'                    => '^(True|False)$',
                                       'started'                    => '^(True|False)$'
                                     };

1;

=head1 NAME

Torque::Util::Regexp - Some useful Torque regular expressions

=head1 SYNOPSIS

 use Torque::Util::Regexp qw( 
                              HOST_STATES 
                              NTYPES 
                              CHECKPOINT_FILE_NAME
                              QSTAT_REGEXP
                              QSTAT_A_REGEXP
                              QSTAT_B_REGEXP
                              QSTAT_BF1_REGEXP
                              QSTAT_F_REGEXP
                              QSTAT_q_REGEXP
                              QSTAT_Q_REGEXP
                              QSTAT_QF1_REGEXP
                            );

 # HOST_STATES
 my $state = 'free';
 if ($state +~ /${\HOST_STATES}/)
   {
   # Do Something
   }

 # NTYPES
 my $ntype = 'cluster';
 if ($ntype =~ /${\NTYPE}/)
   {
   # Do Something
   } 

 # CHECKPOINT_FILE_NAME
 my $checkpoint_file_name = 'chkpt.0.host.032490233';
 if ($checkpoint_file_name =~ /${\CHECKPOINT_FILE_NAME}/)
   {
   # Do Something
   }

 # QSTAT_REGEXP
 my $name    = 'job_name';
 my $reg_exp = &QSTAT_REGEXP->{ 'name' };
 if ($name =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_A_REGEXP
 my $job_id  = '30.host';
 my $reg_exp = &QSTAT_A_REGEXP->{ 'job_id' };
 if ($job_id =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_B_REGEXP
 my $status  = 'ACTIVE';
 my $reg_exp = &QSTAT_B_REGEXP->{ 'status' };
 if ($status =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_BF1_REGEXP
 my $server_state = 'ACTIVE';
 my $reg_exp      = &QSTAT_BF1_REGEXP->{ 'server_state' };
 if ($server_state =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_F_REGEXP
 my $job_id = '30.host';
 my $reg_exp = &QSTAT_F_REGEXP->{ 'job_id' };
 if ($job_id =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_Q_REGEXP
 my $t          = 'E';
 my $reg_exp    = &QSTAT_Q_REGEXP->{ 't' };
 if ($t =~ /${reg_exp}/)
   {
   # Do Something
   }

 # QSTAT_QF1_REGEXP
 my $queue_type = 'Execution';
 my $reg_exp    = &QSTAT_QF1_REGEXP->{ 'queue_type' };
 if ($queue_type =~ /${reg_exp}/)
   {
   # Do Something
   }


=head1 DESCRIPTION

Some constants containing some useful regular expressions for torque and pbs_mom.

=head1 REGULAR EXPRESIONS

=over 4

=item HOST_STATES

A regular expression to match valide mom host states

=item NTYPES

A regular expression for valid torque node types

=item CHECKPOINT_FILE_NAME

A regular expression for a valid torque checkpoint file name.  These files are usually found in '/{path_to_torque}/checkpoint/'.

=item QSTAT_REGEXP

A hashref of regular expressions for the different values outputed by the qstat family of commands.

=item QSTAT_A_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -a family of commands.

=item QSTAT_B_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -B family of commands.

=item QSTAT_BF1_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -B -f -1 family of commands.

=item QSTAT_F_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -f family of commands.

=item QSTAT_Q_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -q family of commands.

=item QSTAT_Q_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -Q family of commands.

=item QSTAT_QF1_REGEXP

A hashref of regular expressions for the different values outputed by the qstat -Q -f -1 family of commands.

=back

=head1 DEPENDENDCIES

Exporter, Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
