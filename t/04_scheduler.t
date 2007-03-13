#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 2;

use constant MAUI => 'maui';
use constant MOAB => 'moab';
use constant FIFO => 'fifo';

my %schedulers;

# Installed Schedulers
{
  # Check Maui
  $schedulers{MAUI} = `which maui` || undef;
  delete $schedulers{MAUI}
    unless defined $schedulers{MAUI};

  # Check Moab
  $schedulers{MOAB} = `which moab` || undef;
  delete $schedulers{MOAB}
    unless defined $schedulers{MOAB};

  # Check FIFO
  $schedulers{FIFO} = `which pbs_sched` || undef;
  delete $schedulers{FIFO}
    unless defined $schedulers{FIFO};
}
ok(scalar (keys %schedulers), 'Installed Scheduler') or
  diag("No recognized scheduler is installed - install Moab, Maui, or pbs_sched and rerun test - see Section XXX");

# Active Schedulers
{
  # Check Maui
  if (exists $schedulers{MAUI})
    {
    $schedulers{MAUI} = `ps aux | grep maui | grep -v grep` || undef;
    delete $schedulers{MAUI}
      unless defined $schedulers{MAUI};
    }

  # Check Moab
  if (exists $schedulers{MOAB})
    {
    $schedulers{MOAB} = `ps aux | grep moab | grep -v grep` || undef;
    delete $schedulers{MOAB}
      unless defined $schedulers{MOAB};
    }

  # Check FIFO
  if (exists $schedulers{FIFO})
    {
    $schedulers{FIFO} = `ps aux | grep pbs_sched | grep -v grep` || undef;
    delete $schedulers{FIFO}
      unless defined $schedulers{FIFO};
    }
}
ok(scalar (keys %schedulers), 'Active Scheduler') or
  diag("No recognized scheduler is running - jobs will not start, start Moab, Maui, or pbs_sched and rerun test - see Section XXX");

