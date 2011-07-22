#!/usr/bin/perl

########################################################################
#
# Name: CRI/Util/User.pm
#
# Description: Unix user related subroutines used in CRI tests
#
# Copyright (C) 2010 by Adaptive Computing Enterprises, Inc.
# All Rights Reserved.
#
########################################################################

package CRI::Util::User;

use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;

use Carp;

use base 'Exporter';

our @EXPORT_OK = qw(
  switchUser
);

#------------------------------------------------------------------------------
# switchUser({
#              'user'      => 'user',
#              'effective' => 1,
#              'real'      => 0
#            });
#------------------------------------------------------------------------------
sub switchUser    #($)
{

  my ($params) = @_;

  my $user      = $params->{'user'} || confess "Please provide a user";
  my $effective = $params->{'effective'};
  my $real      = $params->{'real'};

  $effective = 1 if !defined $effective;
  $real      = 1 if !defined $real;

  my (undef, undef, $uid, $gid) = getpwnam($user);

  $> = $uid if $real;         # Real uid
  $< = $uid if $effective;    # Effective uid
  $) = $gid if $real;         # Real gid
  $( = $gid if $effective;    # Effective gid

  # Switch current directory to the users home directory
  my $home_directory = "/home/$user";
  $home_directory = '/root'
    if $user eq 'root';

  chdir($home_directory)
    or confess "Unable to chdir to $home_directory: $!";

  return;

}    # END sub switchUser #($)

1;

__END__

=head1 NAME

CRI::Util::User - Unix user module for CRI::Test scripts

=head1 SYNOPSIS

 use CRI::Util::User qw(
                         switchUser
                       );

 switchUser({ 
             user => 'testuser1',
             effective => 1,
             real      => 0 
           });                       

=head1 DESCRIPTION

A module for subroutines related to unix users.  To be used with CRI::Test. The following methods are available:

=over 4

=item switchUser({ 'user' => $user, 'effective' => $zero_or_one, 'real' => $zero_or_one });

B<switchUser()> Switches the currently running user to $user.  If $effective is set to 1 then the effective uid and gid are changed are set to match the $user's uid and gid.  $effective defaults to 1. If $real is set to 1 then the real uid and gid are set to match the $user's uid and gid. $real defaults to 0.

=back

=head1 EXAMPLES

 # Set both the effective and real uid and gid to match the passed in users uid and gid
 switchUser({
             'user' => 'testuser1'
           });
 
 # Set the effective uid and gid to match the passed in users uid and gid
 switchUser({
             'user' => 'testuser1',
             'real' => 0
           });
 
 # Set the real uid and gid to match the passed in users uid and gid
 switchUser({
             'user'      => 'testuser1',
             'effective' => 0
           });

=head1 REQUIRES

TestLibFinder, CRI::Test, Carp, Exporter

=head1 AUTHOR

Adaptive Computing Enterprises, Inc. E<lt>http://www.adaptivecomputing.com/E<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2010 by Adaptive Computing Enterprises, Inc. All Rights Reserved.

=cut
