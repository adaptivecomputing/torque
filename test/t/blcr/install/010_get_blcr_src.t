#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;
use CRI::Install::Utils qw(
                            get_hg
                            get_svn
                            get_url
                            get_dir
                            get_tarball
                          );

plan('no_plan'); 
setDesc('Get Latest Torque Source');

my $get_funcs = {
                  'hg'      => \&get_hg,
                  'svn'     => \&get_svn,
                  'url'     => \&get_url,
                  'dir'     => \&get_dir,
                  'tarball' => \&get_tarball
                };

my ($method, $src) = split(/\|/, $props->get_property('blcr.get.src'));
my $build_dir      = $props->get_property('blcr.build.dir');

# Run the approriate method
$method    = lc $method;
my $params = {
               'src'  => $src,
               'dest' => $build_dir
             };
ok(defined $get_funcs->{ $method }, "Checking for the method: $method")
  or die ("Invalid method: $method");
&{ $get_funcs->{ $method } }($params);
