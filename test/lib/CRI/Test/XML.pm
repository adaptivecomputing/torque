package CRI::Test::XML;
$VERSION = 0.01;

use strict;
use warnings;

use base 'Exporter';
use CRI::Test::Reader qw();
use XML::Simple qw(:strict);
our @ISA = qw(Exporter);
our @EXPORT = qw(loadLog makeXML);


# Copyright 2007, Cluster Resources
# By Michael Moore, mmoore@clusterresources.com
# Module to write test result logs to XML 

use CRI::Test::XML;

#Parses test result sets into an appropriate data structure for writing out with XML::Simple. 

=cut

=head2 new()
Returns a CRI::Test::XML object
=cut
sub new 
{
    my $logfile;
    my $xmlhash;
    my $readerobj = CRI::Test::Reader->new();
    my $xmlsimple = XML::Simple->new(
	KeyAttr        => [],
	KeepRoot       => 1,
	SuppressEmpty  => 1,
	XMLDecl        => 1,
	# ForceArray      => ['STDERR','STDOUT'],
	# KeyAttr        => {PROGRAM_RUN=>'STDOUT'},
	# AttrIndent     => 1 
	# RootName       => 'TEST_RESULT_SET',
	# NoAttr         => 1,
	# NoIndent       => 1,
	# NormaliseSpace => 1,
    );

    my $self = 
    {
	FILE   => $logfile,
	XML    => $xmlhash,
	READER => $readerobj,
	SIMPLE => $xmlsimple,
    };
    bless $self;
    return $self;
}

=head2 loadLog($logname)
Give this function the Summary.log file produced by running CRI::Test type tests.

It loads the log into the data structure and makes it available to other XML functions
=cut
sub loadLog
{
    my $self = shift;
    $self->{'FILE'} = $_[0];
    $self->{'READER'}->loadLog($self->{'FILE'}); 
    $self->{'XML'} = $self->_makeXMLref(0);
}

=head2 makeXML()
Creates the XML from the data strcture and returns it at a string.
=cut
sub makeXML 
{
    my $self = shift;
    #my $xml = $self->{'SIMPLE'}->XMLout($self->{'READER'},KeyAttr=>[]);
    my $xml = $self->{'SIMPLE'}->XMLout($self->_makeXMLref());
    return $xml;  
}

# Builds the top level hash, and calls the _makeSubHash recursive function
# Starts with whatever node is the HEAD according to CRI::Test::Reader
sub _makeXMLref
{
    my $self = shift;
    my %superhash = ();
    my $reader = $self->{'READER'};

    $superhash{'TEST'} = $self->_makeSubHash($reader->{'HEAD'});
    return \%superhash;
}

# Add info on the current test
# For each child test, call this function recursively
sub _makeSubHash
{
    my $self = shift;
    my %subhash = ();
    my $curnode = $self->{'READER'}->{'HASH'}->{$_[0]};

    $subhash{'TID'} = $_[0];
    $subhash{'DESC'} = $curnode->{'DESC'};
    $subhash{'LOGFILE'} = $curnode->{'LOGFILE'};
    $subhash{'SCRIPT'} = $curnode->{'SCRIPT'};
    $subhash{'TIME_RUN'} = $curnode->{'TIME_RUN'};
    $subhash{'TEST_RESULTS'} = $self->_getResults($curnode->{'LOGFILE'});
    my @childTests = ();
    foreach my $child (@{$curnode->{'CHILDREN'}})
    {
	push(@childTests,$self->_makeSubHash($child)); 
    }
    $subhash{'TEST'} = \@childTests;

    return \%subhash;
}

# Include the individual test log results
# Any test log components which should be 
# considered their own stanzas will need to call
# a helper function (eg. PROGRAM_RUN and it's related output)
sub _getResults
{
    my $self = shift;
    my @log;
    my %testresults;

    {
	local $/ =  "\n";

	open (LOG,"<$_[0]");
	@log = <LOG>;
	close(LOG);
	chomp(@log);
    }

    my @programRun = ();
    my @execTests = ();
    my $line;
    while ($line = shift(@log))
    {
	$line =~ m/(.*?)=(.*)/s;
	my $word = $1;
	my $value = $2;
	$value = undef if ($value eq '');

	if($word eq 'PROGRAM_RUN')
	{
	    push(@{$testresults{'PROGRAM_RUN'}},_addProgram(\@log,$value));

	}
	elsif($word eq 'RT_TESTSRUN')
	{
	    push(@{$testresults{'EXECUTE_TESTS'}},_addTestBatch(\@log,$value));
	}
	elsif (
	    $word =~ /^OK.*$/        || 
	    $word =~ /^CMP_OK.*$/    ||
	    $word =~ /^ISNT.*$/      ||
	    $word =~ /^LIKE.*$/      ||
	    $word =~ /^UNLIKE.*$/    ||
	    $word =~ /^IS.*$/        ||
	    $word eq 'PASS'          ||
	    $word eq 'FAIL'          ||
	    $word eq 'DIAG'          ||
	    $word =~ /^IS_DEEPLY.*$/ ||
	    $word =~ /^SKIPPING.*$/  
	)
	{
	    push(@{$testresults{$word}},$value);
	}
	else
	{
	    $testresults{$word} = $value;
	}
    }
    return \%testresults; 
}

# Parses the info for a single command run with runCommandAs or runCommand 
sub _addProgram
{
    my ($log,$line) = @_;  
    my %programrun= ();

    $programrun{'PROGRAM_RUN'} = $line;

    my $go = 1;
    while($go)
    {
	my $nextline = shift(@$log);
	$nextline =~ m/(.*?)=(.*)/s;
	my $word = $1;
	my $value = $2;

	if($word eq 'STDOUT' || $word eq 'STDERR')
	{
	    #$value =~ s/\n/\\n/g; # Want your stdout and stderr on a single line? 
	    $value = undef if ($value eq '');
	    $programrun{$word} = [$value];
	}
	else
	{
	    $value = undef if ($value eq '');
	    $programrun{$word} = $value;
	}
	$go = 0 if($word eq 'EXIT_CODE' || @$log == 0);
    }
    return \%programrun;
}

sub _addTestBatch
{
    my ($log,$line) = @_;  
    my %programrun= ();

    $programrun{'RT_TESTSRUN'} = [$line];

    my $go = 1;
    while($go)
    {
	my $nextline = shift(@$log);
	$nextline =~ m/(.*?)=(.*)/s;
	my $word = $1;
	my $value = $2;

	if( 
	    $word eq 'RT_TESTSRUN' || 
	    $word eq 'RT_FAILED' ||
	    $word eq 'RT_PARSE_ERROR' ||
	    $word eq 'RT_PASSED' || 
	    $word eq 'RT_SKIPPED' ||
	    $word eq 'RT_TODO' || 
	    $word eq 'RT_TODO_PASSED' || 
	    $word eq 'RT_WAIT' || 
	    $word eq 'RT_EXIT'
	)
	{
	    $value = undef if ($value eq '');
	    $programrun{$word} = [$value];
	}
	else
	{
	    $value = undef if ($value eq '');
	    $programrun{$word} = $value;
	}
	$go = 0 if($word eq 'RT_BENCHMARK' || @$log == 0);
    }
    return \%programrun;
}

1;

__END__;

