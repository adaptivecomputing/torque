#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More qw(no_plan);

# Queue List
my (@xqueues, %queues);
# - Build first-level entries for queue hash table
map
  {
  /^create queue (\w+)/i;
  $queues{$1} = ();
  }
  grep
    {
    /^create queue \w+/i
    }
    `qmgr -c 'p s'`;
# - Populate additional entries for each queue in the hash
foreach my $queue (keys %queues)
  {
  my $data = `qmgr -c 'p queue $queue'`;
  push(@xqueues, $queue)
    if $data =~ /^set queue batch queue_type\s+=\s+Execution/im;
  foreach my $line (split /[\r\n]+/, $data)
    {
    next unless $line =~ /^set queue batch (\S+) = (\S+)/;
    $queues{$queue}{$1} = $2;
    }
  }
# - Verify that queues exist
ok(scalar (keys %queues), 'Configured Queues') or
  BAIL_OUT('No TORQUE queues are configured - run torque.setup or see Sections 1.2.1 & 4.1');
ok(scalar @xqueues, 'Execution Queues') or
  BAIL_OUT('No execution queues are configured - run torque.setup or see Sections 1.2.1 & 4.1');

# Queue Check
my $queuesOK = 1;
foreach my $queue (keys %queues)
  {
  ok($queues{$queue}{'enabled'} =~ /^True$/i, "Queue '$queue' Enabled") or
    do
      {
      diag("TORQUE queue '$queue' has not been enabled - see Section 4.1");
      $queuesOK = 0;
      };
  ok($queues{$queue}{'started'} =~ /^True$/i, "Queue '$queue' Started") or
    do
      {
      diag("TORQUE queue '$queue' has not been started - see Section 4.1");
      $queuesOK = 0;
      };
  }
ok($queuesOK, 'TORQUE Queues Configured') or
  BAIL_OUT("TORQUE queues have not been correctly configured - run torque.setup or see section 4.1");

