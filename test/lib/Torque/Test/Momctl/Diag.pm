package Torque::Test::Momctl::Diag;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;
use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
                   test_level_3 
                   test_level_2
                   test_level_1
                   test_level_0
                   test_mult_diag
                   );

# Mom Variables
my $mom_host               = $props->get_property('Test.Host');
my $mom_version            = $props->get_property('mom.version');
my $mom_home_dir           = $props->get_property('mom.home.dir');
my $mom_comm_model         = $props->get_property('mom.communication.model');
my $mom_poll_time          = $props->get_property('mom.config.check_poll_time');
my $mom_update_interval    = $props->get_property('mom.config.status_update_time');
my $mom_mem_locked         = $props->get_property('mom.mem.locked');
my $mom_tcp_timeout        = $props->get_property('mom.tcp_timeout');
my $mom_prolog             = $props->get_property('mom.prolog');
my $mom_alarm_time         = $props->get_property('mom.alarm.time');
my $mom_client_list_regexp = '\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}[,\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}]*';
my $mom_copy_command       = $props->get_property('mom.copy.command');
my $mom_varattrs           = $props->get_property('mom.config.varattr');

# Torque Variables
my $torque_spool_dir = $props->get_property('Torque.Home.Dir') . "/spool/";
$torque_spool_dir    =~ s/\/\//\//g; # Replace any // with /

###############################################################################
# test_mult_diag
###############################################################################
sub test_mult_diag ($$$)
  {
 
  # Input
  my ($level, $out, $host_list) = @_;

  # Variables
  my @outs;
  my %test_input;

  # Split up multiple outputs
  @outs = split(/diagnostics complete\n\n/, $out);

  # Create a hash matching the output to the host  
  foreach my $x (0..(length @$host_list - 1))
    {

    $test_input{ $host_list->[$x] } = $outs[$x];

    } # END foreach my $x (0..(length @$host_list))

  # Perform the tests
  while (my ($host, $output) = each (%test_input))
    {

    if ($host eq 'localhost')
      {
      $host = $mom_host;
      }

    if ($level == 0)
      {
      test_level_0($output, $host);
      }

    elsif ($level == 1)
      {
      test_level_1($output, $host);
      }

    elsif ($level == 2)
      {
      test_level_2($output, $host);
      }

    elsif ($level == 3)
      {
      test_level_3($output, $host);
      }

    else 
      {
      croak("Invalid log level specified (0-3)");
      }

    } # END while (my ($host, $output) = each (%test_input))

  } #END sub test_mult_diag ($$$)

###############################################################################
# test_level_3
###############################################################################
sub test_level_3 ($)
  {

  my $out  = $_[0];
  my $host = $_[1] || $mom_host;

  my %results = _parse_momctl($out);

  # Test the log level 3 results
  cmp_ok($results{ 'host'               }, 'eq', "$host/$mom_host", "Checking the host"               );
  cmp_ok($results{ 'copycommand'        }, 'eq', $mom_copy_command, "Checking the mom copy commmand"  );
  cmp_ok($results{ 'homedirectory'      }, 'eq', $mom_home_dir,     "Checking the homedirectory"      );
  cmp_ok($results{ 'communicationmodel' }, 'eq', $mom_comm_model,   "Checking the communication model");

  like($results{ 'version'                     }, qr/^${mom_version}/,                                    "Checking the version"                 );
  like($results{ 'pid'                         }, qr/\d*/,                                                "Checking the pid"                     );
  like($results{ 'stdout/stderrspooldirectory' }, qr/\'${torque_spool_dir}\'\s+\(\d+ blocks available\)/, "Checking the 'stdout/stderr spool'"   );
  like($results{ 'momactive'                   }, qr/\d+ seconds/,                                        "Checking 'mom active'"                );
  like($results{ 'checkpolltime'               }, qr/${mom_poll_time}\sseconds/,                          "Checking the 'check poll time'"       );
  like($results{ 'serverupdateinterval'        }, qr/${mom_update_interval}\sseconds/,                    "Checking the 'server update interval'");
  like($results{ 'loglevel'                    }, qr/^\d{1,2}/,                                           "Checking 'log level'"                 );
  like($results{ 'memlocked'                   }, qr/${mom_mem_locked}\s+\(mlock\)/,                      "Checking the mem locked"              );
  like($results{ 'tcptimeout'                  }, qr/${mom_tcp_timeout}\s+seconds/,                       "Checking the tcp timeout"             );
  like($results{ 'prolog'                      }, qr/${mom_prolog}\s+\((disabled|enabled)\)/,             "Checking the prolog"                  );
  like($results{ 'alarmtime'                   }, qr/\d+\sof\s${mom_alarm_time}\sseconds/,                "Checking the alarm time"              );
  like($results{ 'trustedclientlist'           }, qr/${mom_client_list_regexp}/,                          "Checking the trusted client list"     );
  like($results{ 'server[0]' }{ 'host'         }, qr/${mom_host}/,                                        "Checking the server"                  );

  TODO:
    {

    local $TODO = "Need to implement a check for varattrs"
       if (! defined $results{ 'varattrs'  }{ 'name' } );

#    ok($results{ 'varattrs'  }{ 'name' } eq $mom_varattrs,                            "Checking varattrs"  );

    }; # END TODO:

  ok(defined $results{ 'note' }, "Checking note"); 

  # We cannont know for sure if these attributes will be there
  SKIP :
    {

      skip("'Init Msgs Set' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgssent' });

      ok(defined $results{ 'server[0]' }{ 'initmsgssent' }, "Checking the 'init msgs sent'"); 

    }
  SKIP :
    {

      skip("'Init Msgs Recieved' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgsreceived' });

      ok(defined $results{ 'server[0]' }{ 'initmsgsreceived' }, "Checking the 'init msgs received'"); 

    }
  SKIP :
    {

      skip("'Last Msg From Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgfromserver' });

      ok(defined $results{ 'server[0]' }{ 'lastmsgfromserver' }, "Checking the 'last msg from server'"); 

    }
  SKIP :
    {

      skip("'Last Msg To Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgtoserver' });
      ok(defined $results{ 'server[0]' }{ 'lastmsgtoserver' }, "Checking the 'last msg to server'"); 

    }

  } # END sub test_level_3 ($)

###############################################################################
# test_level_2
###############################################################################
sub test_level_2 ($)
  {

  my $out  = $_[0];
  my $host = $_[1] || $mom_host;

  my %results = _parse_momctl($out);

  cmp_ok($results{ 'host'               }, 'eq', "$host/$mom_host", "Checking the host"               );
  cmp_ok($results{ 'homedirectory'      }, 'eq', $mom_home_dir,     "Checking the homedirectory"      );
  cmp_ok($results{ 'communicationmodel' }, 'eq', $mom_comm_model,   "Checking the communication model");
  cmp_ok($results{ 'copycommand'        }, 'eq', $mom_copy_command, "Checking the mom copy commmand"  );

  like($results{ 'version'                     }, qr/^${mom_version}/,                                    "Checking the version"                 );
  like($results{ 'pid'                         }, qr/\d*/,                                                "Checking the pid"                     );
  like($results{ 'stdout/stderrspooldirectory' }, qr/\'${torque_spool_dir}\'\s+\(\d+ blocks available\)/, "Checking the 'stdout/stderr spool'"   );
  like($results{ 'momactive'                   }, qr/\d+ seconds/,                                        "Checking 'mom active'"                );
  like($results{ 'checkpolltime'               }, qr/${mom_poll_time}\sseconds/,                          "Checking the 'check poll time'"       );
  like($results{ 'serverupdateinterval'        }, qr/${mom_update_interval}\sseconds/,                    "Checking the 'server update interval'");
  like($results{ 'loglevel'                    }, qr/^\d{1,2}/,                                           "Checking 'log level'"                 );
  like($results{ 'memlocked'                   }, qr/${mom_mem_locked}\s+\(mlock\)/,                      "Checking the mem locked"              );
  like($results{ 'tcptimeout'                  }, qr/${mom_tcp_timeout}\s+seconds/,                       "Checking the tcp timeout"             );
  like($results{ 'prolog'                      }, qr/${mom_prolog}\s+\((disabled|enabled)\)/,             "Checking the prolog"                  );
  like($results{ 'alarmtime'                   }, qr/\d+\sof\s${mom_alarm_time}\sseconds/,                "Checking the alarm time"              );
  like($results{ 'trustedclientlist'           }, qr/${mom_client_list_regexp}/,                          "Checking the trusted client list"     );
  like($results{ 'server[0]' }{ 'host'         }, qr/${mom_host}/,                                        "Checking the server"                  );
  TODO:
    {

    local $TODO = "Need to implement a check for varattrs"
       if (! defined $results{ 'varattrs'  }{ 'name' } );

#    ok($results{ 'varattrs'  }{ 'name' } eq $mom_varattrs,                            "Checking varattrs"  );

    }; # END TODO:
 
  ok(defined $results{ 'note' }, "Checking note");

  # We cannont know for sure if these attributes will be there
  SKIP :
    {

      skip("'Init Msgs Set' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgssent' });
      ok(defined $results{ 'server[0]' }{ 'initmsgssent' }, "Checking the 'init msgs sent'"); 

    }
  SKIP :
    {

      skip("'Init Msgs Recieved' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgsreceived' });

      ok(defined $results{ 'server[0]' }{ 'initmsgsreceived' }, "Checking the 'init msgs received'"); 

    }
  SKIP :
    {

      skip("'Last Msg From Server' not found", 1) 
        if (! exists $results{ 'server[0]' }{ 'lastmsgfromserver' });

      ok(defined $results{ 'server[0]' }{ 'lastmsgfromserver' }, "Checking the 'last msg from server'"); 

    }
  SKIP :
    {

      skip("'Last Msg To Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgtoserver' });

      ok(defined $results{ 'server[0]' }{ 'lastmsgtoserver' }, "Checking the 'last msg to server'"); 

    }

  } # END test_level_2 ($)

###############################################################################
# test_level_1
###############################################################################
sub test_level_1 ($)
  {

  my $out  = $_[0];
  my $host = $_[1] || $mom_host;

  my %results = _parse_momctl($out);

  # Test the log level 1 results
  cmp_ok($results{ 'host'               }, 'eq', "$host/$mom_host", "Checking the host"               );
  cmp_ok($results{ 'homedirectory'      }, 'eq', $mom_home_dir,     "Checking the homedirectory"      );
  cmp_ok($results{ 'communicationmodel' }, 'eq', $mom_comm_model,   "Checking the communication model");
  cmp_ok($results{ 'copycommand'        }, 'eq', $mom_copy_command, "Checking the mom copy commmand"  );

  like($results{ 'version'                     }, qr/^${mom_version}/,                                    "Checking the version");
  like($results{ 'pid'                         }, qr/\d*/,                                                "Checking the pid");
  like($results{ 'stdout/stderrspooldirectory' }, qr/\'${torque_spool_dir}\'\s+\(\d+ blocks available\)/, "Checking the 'stdout/stderr spool'");
  like($results{ 'momactive'                   }, qr/\d+ seconds/,                                        "Checking 'mom active'");
  like($results{ 'checkpolltime'               }, qr/${mom_poll_time}\sseconds/,                          "Checking the 'check poll time'");
  like($results{ 'serverupdateinterval'        }, qr/${mom_update_interval}\sseconds/,                    "Checking the 'server update interval'");
  like($results{ 'loglevel'                    }, qr/^\d{1,2}/,                                           "Checking 'log level'");
  like($results{ 'memlocked'                   }, qr/${mom_mem_locked}\s+\(mlock\)/,                      "Checking the mem locked");
  like($results{ 'tcptimeout'                  }, qr/${mom_tcp_timeout}\s+seconds/,                       "Checking the tcp timeout");
  like($results{ 'trustedclientlist'           }, qr/${mom_client_list_regexp}/,                          "Checking the trusted client list");
  like($results{ 'server[0]' }{ 'host'         }, qr/${mom_host}/,                                        "Checking the server");

  TODO:
    {

    local $TODO = "Need to implement a check for varattrs"
       if (! defined $results{ 'varattrs'  }{ 'name' } );

#    ok($results{ 'varattrs'  }{ 'name' } eq $mom_varattrs,                            "Checking varattrs"  );

    }; # END TODO:

  ok(defined $results{ 'note' }, "Checking note");                    

  # We cannont know for sure if these attributes will be there
  SKIP :
    {

      skip("'Init Msgs Set' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgssent' });

      ok(defined $results{ 'server[0]' }{ 'initmsgssent' }, "Checking the 'init msgs sent'"); 

    }
  SKIP :
    {

      skip ("'Init Msgs Recieved' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgsreceived' });

      ok(defined $results{ 'server[0]' }{ 'initmsgsreceived' }, "Checking the 'init msgs received'"); 

    }
  SKIP :
    {

      skip("'Last Msg From Server' not found", 1) 
        if (! exists $results{ 'server[0]' }{ 'lastmsgfromserver' });

      ok(defined $results{ 'server[0]' }{ 'lastmsgfromserver' }, "Checking the 'last msg from server'"); 

    }
  SKIP :
    {

      skip("'Last Msg To Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgtoserver' });
      ok(defined $results{ 'server[0]' }{ 'lastmsgtoserver' }, "Checking the 'last msg to server'"); 

    }

  # Shouldn't be in the list
  ok(! defined $results{ 'prolog'    }, "Checking that the 'prolog' is not printed");
  ok(! defined $results{ 'alarmtime' }, "Checking that the 'alarm time' is not printed");


  } # END sub test_level_1 ($)

###############################################################################
# test_level_0
###############################################################################
sub test_level_0 ($)
  {

  my $out  = $_[0];
  my $host = $_[1] || $mom_host;

  my %results = _parse_momctl($out);

  cmp_ok($results{ 'host'          }, 'eq', "$host/$mom_host", "Checking the host"         );
  cmp_ok($results{ 'homedirectory' }, 'eq', $mom_home_dir,     "Checking the homedirectory");

  like($results{ 'version'             }, qr/^${mom_version}/, "Checking the version" );
  like($results{ 'pid'                 }, qr/\d*/,             "Checking the pid"     );
  like($results{ 'momactive'           }, qr/\d+ seconds/,     "Checking 'mom active'");
  like($results{ 'loglevel'            }, qr/^\d{1,2}/,        "Checking 'log level'" );
  like($results{ 'server[0]' }{ 'host' }, qr/${mom_host}/,     "Checking the server"  );

  TODO:
    {

    local $TODO = "Need to implement a check for varattrs"
       if (! defined $results{ 'varattrs'  }{ 'name' } );

#    ok($results{ 'varattrs'  }{ 'name' } eq $mom_varattrs,                            "Checking varattrs"  );

    }; # END TODO:

  ok(defined $results{ 'note' }, "Checking note");                    

  # We cannont know for sure if these attributes will be there
  SKIP :
    {

      skip("'Init Msgs Set' not found", 1)
        if ! exists $results{ 'server[0]' }{ 'initmsgssent' };

      ok(defined $results{ 'server[0]' }{ 'initmsgssent' }, "Checking the 'init msgs sent'"); 

    }
  SKIP :
    {

      skip("'Init Msgs Recieved' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'initmsgsreceived' });

      ok(defined $results{ 'server[0]' }{ 'initmsgsreceived' }, "Checking the 'init msgs received'"); 

    }
  SKIP :
    {

      skip("'Last Msg From Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgfromserver' });

      ok(defined $results{ 'server[0]' }{ 'lastmsgfromserver' }, "Checking the 'last msg from server'"); 

    }
  SKIP :
    {

      skip("'Last Msg To Server' not found", 1)
        if (! exists $results{ 'server[0]' }{ 'lastmsgtoserver' });
      ok(defined $results{ 'server[0]' }{ 'lastmsgtoserver' }, "Checking the 'last msg to server'"); 

    }

  # Shouldn't be in the list
  ok(! defined $results{ 'checkpolltime'               }, "Checking that the 'check poll time' is not printed");
  ok(! defined $results{ 'serverupdateinterval'        }, "Checking that the 'server update interval' is not printed");
  ok(! defined $results{ 'stdout/stderrspooldirectory' }, "Checking that the 'stdout/stderr spool directory' is not printed");
  ok(! defined $results{ 'communicationmodel'          }, "Checking that the 'communication model' is not printed" );
  ok(! defined $results{ 'memlocked'                   }, "Checking that the 'mem locked' is not printed");
  ok(! defined $results{ 'tcptimeout'                  }, "Checking that the 'tcp timeout' is not printed");
  ok(! defined $results{ 'trustedclientlist'           }, "Checking that the 'trusted client list' is not printed");
  ok(! defined $results{ 'prolog'                      }, "Checking that 'prolog' is not printed");
  ok(! defined $results{ 'alarmtime'                   }, "Checking that the 'alarm time' is not printed");
  ok(! defined $results{ 'copycommand'                 }, "Checking that the 'mom copy commmand' is not printed");

  } # END sub test_level_0 ($)

###############################################################################
# _parse_momctl - Parse the output of momctl
###############################################################################
sub _parse_momctl #($) 
  {
  
  my $stdout = shift;

  # Other Variables
  my %results;
  my @lines;
  my $line;
  my @attributes;
  my $attribute;

  # Parse momctl -d output
  @lines = split("\n", $stdout);

  # Parse the first line seperately
  shift @lines; # Discard the the first empty line
  $line = shift @lines;

  $line =~ s/:\s+/:/g; # Prepare the line

  @attributes = split(/\s+/, $line);

  foreach $attribute (@attributes)
    {

    my ($key, $value) = _gen_key_value($attribute);
    $results{ $key }  =  $value;

    } # END foreach my $attribute (@attributes)


  # Parse the remaining lines
  my $current_server;
  foreach my $x (0..(scalar @lines - 1))
    {

    my $line = $lines[$x];

    next unless length($line) > 0; # Only parse lines that contain something
    if ($line !~ /:/ or $line =~ /=/)
      {
      next;
      } # END if ($line !~ /:/ or $line =~ /=/)

    if ($line =~ /(server\[\d+\]):(.+)/i)
      {

      # We use the regularexpression because we can't depend on spliting on 
      # a ':', which is what _gen_key_value does.
      my $key   = lc $1;
      my $value = lc $2;

      $results{ $key }  = { 'host' => $value };
      $current_server   = $key;
      next;

      } # END if ($line =~ /server\[\d+\]/i)

    if ($line =~ /init msgs/i or $line =~ /last msg/i)
      {
      
      my ($key, $value) = _gen_key_value($line);
      $results{ $current_server }{ $key } = $value;
      next;
  
      } # END if ($line =~ /init msgs/i or $line =~ /last msg/i)

    if ($line =~ /varattrs/i)
      {

      my $line1 = $lines[$x + 1];
      my $line2 = $lines[$x + 2];

      $line1 =~ /name=(\w+)\s+ttl=(\d+)\s+last=(.+)/;

      my $name = $1;
      my $ttl  = $2;
      my $last = $3;

      $line2 =~ /cmd=(.*)value=(.*)/;
      my $cmd   = $1;
      my $value = $2;

      $results{ 'varattrs' } = {
                                 'name'  => $name,
                                 'ttl'   => $ttl,
                                 'last'  => $last,
                                 'cmd'   => $cmd,
                                 'value' => $value
                               };
      next;
      
      } # END if ($line =~ /varattrs/i)

    # Prepare for parsing
    $line =~ s/:\s+/:/g;
    $line =~ s/^\s+//;
     
    my ($key, $value) = _gen_key_value($line);
    $results{ $key }  =  $value;

    } # END foreach my $line (@lines)

    return %results;

  } # END sub _parse_momctl #($)

###############################################################################
# _gen_key_value
###############################################################################
sub _gen_key_value #($)
  {

  my $line = shift;
  $line    = lc $line; # It is easier if everything is lowercased
  my $key;
  my $value;


  ($key, $value) =  split(/:/, $line); 
  $key           =~ s/\s+//g;

  return ($key, $value);

  } # END sub _gen_key_value #($)

1;

=head1 NAME

Torque::Test::Momctl::Diag - A module that provides methods to test the output of momctl diagnostics

=head1 SYNOPSIS

use CRI::Test;
use Torque::Test::Momctl::Diag qw(
                                  test_level_0
                                  test_level_1
                                  test_level_2
                                  test_level_3
                                  test_mult_diag
                                 );

my %momctl;
%momctl = runCommand('momctl -d 0');
test_level_0($momctl{ 'STDOUT' });

%momctl = runCommand('momctl -d 1');
test_level_1($momctl{ 'STDOUT' });

%momctl = runCommand('momctl -d 2');
test_level_2($momctl{ 'STDOUT' });

%momctl = runCommand('momctl -d 3);
test_level_3($momctl{ 'STDOUT' });

%momctl = runCommand('momctl -f /tmp/hostlist -d 3);
test_mult_diag(0, $momctl{ 'STDOUT' }, '/tmp/hostlist');

=head1 DESCRIPTION

Contains tests for diagnostics output from the following commands:

=over 4

=item momctl -d <LOGLEVEL>

=item momctl -h <HOST> -d <LOGLEVEL>

=item momctl -p <PORT> -h <HOST> -d <LOGLEVEL>

=item momctl -f <HOSTLISTFILE> -d <LOGLEVEL>

=item momctl -p <PORT> -f <HOSTLISTFILE> -d <LOGLEVEL>

=item momctl -q diag<LOGLEVEL>

=item momctl -h <HOST> -q diag<LOGLEVEL>

=item momctl -p <PORT> -h <HOST> -q diag<LOGLEVEL>

=item momctl -f <HOSTLIST> -q diag<LOGLEVEL>

=item momctl -p <PORT> -f <HOSTLIST> -q diag<LOGLEVEL>

=item momctl -p <PORT> -q diag<LOGLEVEL>

=back

=head1 DEPENDENDCIES

Moab::Test

=head1 AUTHOR(S)

Jeff Dayley <jdayley at clusterresources dot com>

=head1 COPYRIGHT

Copyright (c) 2008 Cluster Resources Inc.

=cut

__END__
