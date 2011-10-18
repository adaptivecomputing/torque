package Torque::Test::Regexp;

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


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
use constant HOST_STATES          => qr/(?:free|offline|down|reserve|job-exclusive|job-sharing|busy|state-unknown|time-shared|cluster)/;
use constant NTYPES               => qr/(?:cluster|time-shared)/;
use constant CHECKPOINT_FILE_NAME => qr/^ckpt\.\d+(\.\S+)+\.\d+/;

# QSTAT regular expressions
use constant QSTAT_REGEXP         => {
                                       'name'     => qr/^[\w\-]+$/,
                                       'user'     => qr/^\w+$/,
                                       'time_use' => qr/^(0|\d{2}(:\d{2})+)$/,
                                       'state'    => qr/^(?:Q|R|W|H|T|C)$/,
                                       'queue'    => qr/^\w+$/,
                                     };
use constant QSTAT_A_REGEXP       => {
                                       'job_id'    => qr/^\d+\.[\w\-]+(?:\.[\w\-]+)?$/,
                                       'username'  => qr/^\w+$/,
                                       'queue'     => qr/^\w+$/,
                                       'jobname'   => qr/^\w+$/,
                                       'sessid'    => qr/^(--|\d+)$/,
                                       'nds'       => qr/^\d+$/,
                                       'tsk'       => qr/^(--|\d+)$/,
                                       'req_mem'   => qr/^(--|\d+(b|w|kb|kw|mb|mw|gb|gw|tb|tw)?)$/,
                                       'req_time'  => qr/^(--|\d{2}(:\d{2})+)$/,
                                       'state'     => qr/^(Q|R|W|H|T|C)$/,
                                       'big'       => qr/^(--|\w+)$/,
                                       'fast'      => qr/^(--|\w+)$/,
                                       'pfs'       => qr/^(--|\w+)$/,
                                       'elap_time' => qr/^(--|\d{2}(:\d{2})+)$/,
                                       'node'      => qr/(--|[\w\-]+\/\d+)/,
                                       'comment'   => qr/(--|\w+)/,
                                     };
use constant QSTAT_B_REGEXP       => {
                                       'server' => qr/^\S+(?:\.\S+)?$/,
                                       'max'    => qr/^\d+$/,
                                       'tot'    => qr/^\d+$/,
                                       'que'    => qr/^\d+$/,
                                       'run'    => qr/^\d+$/,
                                       'hld'    => qr/^\d+$/,
                                       'wat'    => qr/^\d+$/,
                                       'trn'    => qr/^\d+$/,
                                       'ext'    => qr/^\d+$/,
                                       'com'    => qr/^\d+$/,
                                       'status' => qr/^(Hot_Start|Active|Terminating_Delay|Terminating)$/,
                                     };
use constant QSTAT_BF1_REGEXP     => {
                                       'server'              => qr/^\S+(?:\.\S+)?$/,
                                       'server_state'        => qr/^(Hot_Start|Active|Terminating_Delay|Terminating)$/,
                                       'scheduling'          => qr/^(True|False)$/,
                                       'total_jobs'          => qr/^\d+$/,
                                       'state_count'         => qr/^Transit:\d+\sQueued:\d+\sHeld:\d+\sWaiting:\d+\sRunning:\d+\sExiting:\d+\sComplete:\d+$/,
                                       'acl_hosts'           => qr/[\w\-]+(,[\w\-].\/+)*/,
                                       'managers'            => qr/\w+\@\w+(,w+\@\w+)*/,
                                       'operators'           => qr/\w+\@\w+(,w+\@\w+)*/,
                                       'default_queue'       => qr/^\w+$/,
                                       'log_events'          => qr/^\d+$/,
                                       'mail_from'           => qr/^\w+$/,
                                       'scheduler_iteration' => qr/^\d+$/,
                                       'node_check_rate'     => qr/^\d+$/,
                                       'tcp_timeout'         => qr/^\d+$/,
                                       'mom_job_sync'        => qr/^(True|False)$/,
                                       'pbs_version'         => qr/^\d+(\.\d+)*$/,
                                       'keep_completed'      => qr/^\d+$/,
                                       'next_job_number'     => qr/^\d+$/,
                                       'net_counter'         => qr/^\d+\s+\d+\s+\d+$/,
                                     };
use constant QSTAT_F_REGEXP       => {
                                       'job_name'                => qr/\w+/,
                                       'job_owner'               => qr/\w+\@\w+/,
                                       'job_state'               => qr/^(Q|R|W|H|T|C)$/, # May be incomplete list of states
                                       'queue'                   => qr/^\w+$/,
                                       'server'                  => qr/^\S+(?:\.\S+)?$/,
                                       'checkpoint'              => qr/^\w+$/,
                                       'ctime'                   => qr/^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$/,
                                       'error_path'              => qr/\w+\:(\/[\w\.]+)+/,
                                       'hold_types'              => qr/^\w$/,
                                       'join_path'               => qr/^\w$/,
                                       'keep_files'              => qr/^\w$/,
                                       'mail_points'             => qr/^\w$/,
                                       'mtime'                   => qr/^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$/,
                                       'output_path'             => qr/\w+\:(\/[\w\.]+)+/,
                                       'priority'                => qr/^\d$/,
                                       'qtime'                   => qr/^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$/,
                                       'rerunable'               => qr/^(True|False)$/,
                                       'resource_list.neednodes' => qr/^\d+$/,
                                       'resource_list.nodect'    => qr/^\d+$/,
                                       'resource_list.nodes'     => qr/^\d+$/,
                                       'resource_list.walltime'  => qr/^\d\d:\d\d:\d\d$/,
                                       'substate'                => qr/^\d+$/,
                                       'variable_list'           => qr/^.+$/,         # Need a stronger test
                                       'euser'                   => qr/^\w+$/,
                                       'egroup'                  => qr/^\w+$/,
                                       'queue_rank'              => qr/^\d+$/,
                                       'queue_type'              => qr/^\w$/,
                                       'etime'                   => qr/^\w\w\w\s+\w\w\w\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+\d\d\d\d$/,
                                     };
use constant QSTAT_q_REGEXP       => {
                                       'queue'    => qr/^[\w\-]+$/,
                                       'memory'   => qr/^(--|\d+(b|w|kb|kw|mb|mw|gb|gw|tb|tw)?)$/,
                                       'cpu_time' => qr/^(--|\d\d:\d\d:\d\d)$/,
                                       'walltime' => qr/^(--|\d\d:\d\d:\d\d)$/,
                                       'node'     => qr/^(--|\d+)$/,
                                       'run'      => qr/^\d+$/,
                                       'que'      => qr/^\d+$/,
                                       'lm'       => qr/^(--|\d+)$/,
                                       'state'    => qr/^(E|D)\s+(R|S)$/,
                                     };
use constant QSTAT_Q_REGEXP       => {
                                       'queue' => qr/^\w+$/,
                                       'max'   => qr/^\d+$/,
                                       'tot'   => qr/^\d+$/,
                                       'ena'   => qr/^(yes|no)$/,
                                       'str'   => qr/^(yes|no)$/,
                                       'que'   => qr/^\d+$/,
                                       'run'   => qr/^\d+$/,
                                       'hld'   => qr/^\d+$/,
                                       'wat'   => qr/^\d+$/,
                                       'trn'   => qr/^\d+$/,
                                       'ext'   => qr/^\d+$/,
                                       't'     => qr/^(E|R)$/,
                                     };
use constant QSTAT_QF1_REGEXP     => {
                                       'queue_type'                 => qr/^(Execution|Route)$/,
                                       'total_jobs'                 => qr/^\d+$/,
                                       'state_count'                => qr/^Transit:\d+\sQueued:\d+\sHeld:\d+\sWaiting:\d+\sRunning:\d+\sExiting:\d+\sComplete:\d+$/,
                                       'resources_default.nodes'    => qr/^\d+$/,
                                       'resources_default.walltime' => qr/^\d\d:\d\d:\d\d$/,
                                       'mtime'                      => qr/^\d+$/,
                                       'enabled'                    => qr/^(True|False)$/,
                                       'started'                    => qr/^(True|False)$/,
                                     };

1;

=head1 NAME

Torque::Test::Regexp - Some useful Torque regular expressions

=head1 SYNOPSIS

 use Torque::Test::Regexp qw( 
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
