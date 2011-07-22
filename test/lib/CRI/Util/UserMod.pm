#!/usr/bin/perl

########################################################################
#
# Name: CRI/Util/UserMod.pm
#
# Description: Unix user related subroutines used in CRI tests
#
# Copyright (C) 2010 by Adaptive Computing Enterprises, Inc.
# All Rights Reserved.
#
########################################################################

package CRI::Util::UserMod;

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
  usermod_a
  deluser
  check
);

#------------------------------------------------------------------------------
# usermod_a({
#              'user'      => 'user',
#              'group'     => 'group',
#            });
#------------------------------------------------------------------------------
sub usermod_a    #($)
{

  my ($params) = @_;

  my $root      = "root";
  my $run_cmd_href = $params->{ 'run_cmd_href' } || { 'test_success' => 1 };
  my $user      = $params->{'user'} || confess "Please provide a user";
  my $group     = $params->{'group'} || confess "Please provide a group";

  my $cmd = 'usermod -a -G' . " $group $user";

  my %results = runCommandAs($root, $cmd, %$run_cmd_href);

  return \%results;

}    # END sub usermod_a #($)

#------------------------------------------------------------------------------
# deluser({
#              'user'      => 'user',
#              'group'     => 'group',
#            });
#------------------------------------------------------------------------------
sub deluser    #($)
{

  my ($params) = @_;

  my $root      = "root";
  my $run_cmd_href = $params->{ 'run_cmd_href' } || { 'test_success' => 1 };
  my $user      = $params->{'user'} || confess "Please provide a user";
  my $group     = $params->{'group'} || confess "Please provide a group";

  my $cmd = 'deluser' . " $user $group";

  my %results = runCommandAs($root, $cmd, %$run_cmd_href);

  return \%results;

}    # END sub deluser #($)

#------------------------------------------------------------------------------
# check({
#              'user'      => 'user',
#              'group'     => 'group',
#            });
#------------------------------------------------------------------------------
sub check    #($)
{

  my ($params) = @_;

  my $root      = "root";
  my $run_cmd_href = $params->{ 'run_cmd_href' } || { 'test_success' => 1 };
  my $group     = $params->{'group'} || confess "Please provide a group";

  my $cmd = 'cat /etc/group |grep' . " $group";

  my %results = runCommandAs($root, $cmd, %$run_cmd_href);

  return \%results;

}    # END sub check #($)

1;

__END__
