package CRI::Test::Reader;
$VERSION = 0.03;

# Copyright 2007, Cluster Resources
# By Michael Moore, mmoore@clusterresources.com
# Module to parse test result logs and make them easy to navigate programmatically

=head1 CRI::Test::Reader

Copyright 2007, Cluster Resources
By Michael Moore mmoore@clusterresources.com

use CRI::Test::Reader;

A module which parses and navigates a test results set from the CRI::Test module. 

This module isn't useful by itself, but should be used by other scripts to provide useful functions. Some possible uses include an log browser, an html results creator, a results emailer dameon, a results to XML converter, and more! 
=cut

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";

use Fcntl ':flock';

# Set up tab and return string
my $tab = "\t";
$tab = $ENV{'treetab'} if ($ENV{'treetab'});


# Data structure of the log tree
#
# hash of hash pointers. TID is key 
#  @tree = (1234=>\%hashy,2342=>\%ahsdy);
#
#  Each hash pointed to contains one line of info from the summary.log
#  As we're parsing the file, add a new hash entry to each line's hash
#  The new entry will have the key of CHILDREN and will be a reference
#  to an array of child processes
#
#  Return a reference to this tree, and provide methods to walk the tree
#
#  Have a pointer which always references the top of the tree
#  Have a pointer which always references the current node

use base 'Exporter';
our @ISA = qw(Exporter);

our @EXPORT = qw(loadLog new viewNode nodeSummary);
#our @EXPORT_OK = qw();

=head2 new
Usage: 
my $testResult = CRI::Test::Reader->new;

Creates a new CRI::Test::Reader object
=cut
sub new {
    my $headNode;
    my $curNode;
    my %logHash;
    my $logFile;

    my $self = 
    {
	SHOWSIBLINGS => 0,
	PRINTVERBOSE => 0,
	HEAD         => $headNode,
	CUR          => $curNode,
	HASH         => \%logHash,
	FILE         => $logFile,
	PASS         => 0,
	FAIL         => 0,
	RUNNING      => 0,
	TOTAL        => 0,
	STATUS       => 'FAIL',
    };

    bless $self;
    return $self;
}

=head2 printTree
$testResult->printTree();

Print a tree of the test results summary starting with the head node. The output depends on which nodes are flagged for printing and other options.
=cut
sub printTree
{
    my $self = shift;


    # Reset any EXPANDED variables needed

    foreach my $tid (sort keys %{$self->{'HASH'}})
    {
	$self->_setAncestors($tid,1) if ($self->{'HASH'}->{$tid}->{'EXPANDED'} == 1); 
    }


    my $retString .= $self->_printTree(0,$self->{'HEAD'},$self->_numberStack());
    return $retString;
}

# _printTree is the recursive method, called by printTree. I suppose you could use it to print only a sub-tree if you wanted to
sub _printTree
{
    my $self = shift;
    my ($indent,$Node,%stack) = @_;
    my $retString = ""; 

    my $curnode = $self->{'HASH'}->{$Node};

    # Append spaces and current line, if appropriate
    if($curnode->{'EXPANDED'} || $stack{$Node}) # Always include the head node
    {
	$retString .= $tab x $indent;
	$retString .= $self->_printLine($Node);
	$retString .= $self->_printVerbose($indent) if ($Node == $self->{'CUR'} && $self->{'PRINTVERBOSE'}); # Extra info if needed
    }
    $indent++; 

    foreach my $kid (@{$curnode->{'CHILDREN'}})
    {
	$retString .= $self->_printTree($indent,$kid,%stack); 
    }
    return $retString;
}

# _printLine adds the _printTree's current line into the _printTree output. It includes terminal control sequences to make nice colored output.
# This function will likely change in the near future to allow for color or normal output.
sub _printLine 
{
    my $self = shift;
    my $start = $_[0];
    my $retString = '';
    # see http://www.termsys.demon.co.uk/vtansi.htm for colors
    $retString .= "[01;37;40m" if ($start == $self->{'CUR'}); # set colors
    $retString .= "$start : "; 
    $retString .= "[0m" if ($start == $self->{'CUR'}); # set colors

    $retString .= $self->{'HASH'}->{$start}->{'TEST_RESULT'};

    $retString .= "[01;37;40m" if ($start == $self->{'CUR'}); # set colors
    $retString .= " : " . $self->{'HASH'}->{$start}->{'SCRIPT'};
    $retString .= "[0m" if ($start == $self->{'CUR'}); # set colors
    $retString .=  "\n"; 
    return $retString;
}

# Adds extra info to the _printTree output. 
sub _printVerbose
{
    my $self = shift;
    my ($indent) = @_;
    my $nodeRef = $self->viewNode();   

    my $retString = "\n";
    $retString .= $tab x $indent;
    $retString .= "\tDESC = " . $nodeRef->{'DESC'} . "\n";
    $retString .= $tab x $indent;
    $retString .= "\tLOGFILE = " . $nodeRef->{'LOGFILE'} . "\n";
    $retString .= $tab x $indent;
    $retString .= "\tPTID = " . $nodeRef->{'PTID'} . "\n";
    $retString .= $tab x $indent;
    $retString .= "\tSCRIPT = " . $nodeRef->{'SCRIPT'} . "\n";
    $retString .= $tab x $indent;
    $retString .= "\tTEST_RESULT = " . $nodeRef->{'TEST_RESULT'} . "\n";
    $retString .= $tab x $indent;
    if(defined $nodeRef->{COREDUMP})
    {
	$retString .= "\tCORE_DUMP = " . $nodeRef->{'COREDUMP'} . "\n";
	$retString .= $tab x $indent;
	$retString .= "\tCORE_DUMP_LOG = " . $nodeRef->{'COREDUMPLOG'} . "\n";
	$retString .= $tab x $indent;
    }
    $retString .= "\tCHILDREN = " . join(', ',@{$nodeRef->{'CHILDREN'}}) . "\n\n"; 
    return $retString;
}

=head2 loadLog('Summary.log');
$testResult->loadLog('Summary.log');

Tell the testResult which Summary.log to use. If a log is currently loaded, it is discarded and the given log loaded in its place. Can also be used to reload the current log. 
=cut
sub loadLog {
    my $self = shift;
    $self->{'FILE'} = $_[0];

    unless (defined $self->{'FILE'} && -e $self->{'FILE'}){
	die("The log file " . $self->{'FILE'} . "given doesn't exist!"); 
    }

    $self->_readSpool($_[0]);
    $self->_findRootNode;
    $self->{'CUR'} = $self->{'HEAD'} unless(defined $self->{'CUR'});
    $self->_buildChildLinks;
    $self->_makeStats();
}

#  Loads a log file (the value of $self->{'FILE'}) creates the appropriate data structure and shoves it in to $self->{'HASH'}
sub _readSpool {
    my $self = shift;

    # Load a log file
    require 'shellwords.pl';

    my $spoolFile = $self->{'FILE'};

    my %data = ();

    if (open (SPOOL, "<$spoolFile"))
    {
	foreach my $line (<SPOOL>)
	{
	flock(SPOOL,LOCK_EX) or die('Bad stuff happened locking Spool file'); 
	    my @words;
	    eval
	    "
	    no warnings;
	    \@words = shellwords(\$line)
	    ";
	    next if $@;
	    next unless scalar @words;
	    my $rowId = shift @words;
	    $data{$rowId} = ();
	    foreach my $word (@words)
	    {
		my ($key, $value) = split /=/, $word;
		$data{$rowId}->{$key} = $value;
	    }
	    my @kids = ();
	    $data{$rowId}->{'CHILDREN'} = \@kids; 
	    $data{$rowId}->{'EXPANDED'} = 0;
	}
	close SPOOL;
    }
    else
    {
	die "Couldn't open summary file"; 
    }

    $self->{'HASH'} = \%data;
}

# build arrays of child processes
# Iterate over every node and append self to parent node's child array
# The Summary.log info only points to parent node
sub _buildChildLinks 
{
    my $self = shift;

    foreach my $tid (sort keys %{$self->{'HASH'}})
    {
	next if($tid == $self->{'HEAD'}); # top level doesn't have a parent...

	# Find the parent test line of the current test
	my $curhash = $self->{'HASH'}->{$tid};
	my $ptid = $curhash->{'PTID'}; 
	my $parentHash = $self->{'HASH'}->{$ptid}; 

	# In the parent test line, add the child to the CHILDREN array

	push(@{$parentHash->{'CHILDREN'}},$tid);
    }  
    $self->_sortChildLinks();
}

# Find the top level of the tree
# The Summary.log file may not be in the right order
sub _findRootNode
{
    my $self = shift; 

    foreach my $node (sort keys %{$self->{'HASH'}})
    {
	my $curhash = $self->{'HASH'}->{$node}; 
	# If a node's PTID doesn't exist as a separate
	# line entry, it is the top level process
	if (not exists $self->{'HASH'}->{$curhash->{'PTID'}})
	{
	    $self->{'HEAD'} = $node; 
	    last;
	}
    }
}

# Sort child arrays by time run so they will display in order
# Except that when we go changing the time around, it messes them up!
sub _sortChildLinks
{
    my $self = shift;
    foreach my $tid (keys %{$self->{'HASH'}})
    {
	my $kids = $self->{'HASH'}->{$tid}->{'CHILDREN'};
	my @sortedKids = sort {
	    $self->{'HASH'}->{$a}->{'TIME_RUN'} 
	    <=> 
	    $self->{'HASH'}->{$b}->{'TIME_RUN'}
	} @$kids; 
	$self->{'HASH'}->{$tid}->{'CHILDREN'} = \@sortedKids;
    }
}

=head2 viewNode(); or viewNode('node id number');
$testResult->viewNode();
$testResult->viewNode('5234');

Returns a reference to a hash of the current node's information (or the given hash number's information, if given). 
The hash has all the key value pairs that are found in the Summary.log file 
=cut
sub viewNode 
{
    my $self = shift;
    my $viewMe = $self->{'CUR'};
    $viewMe = $_[0] if (defined $_[0] && $_[0] ne '');

    return $self->{'HASH'}->{$viewMe};
}

# Tree navigation  methods
=head2 toggleSummary(), toggleSummary(0|1)
Toggle's if extended node info is shown for the current node.

If a value is given, that value is used instead
=cut
sub toggleSummary
{
    my $self = shift;
    $self->{'PRINTVERBOSE'} = !$self->{'PRINTVERBOSE'}; 
    $self->{'PRINTVERBOSE'} = $_[0] if(defined($_[0]));
}

=head2 expandBranch()
Sets the current node and all sub nodes to visible in tree view mode.
=cut
sub expandBranch
{
    my $self = shift;
    my $curnode = $self->{'CUR'};
    $self->_setDecendants($curnode,1);
}

=head2 collapseBranch()
Sets the current node and all sub nodes to hidden in tree view mode.
=cut
sub collapseBranch 
{
    my $self = shift;
    my $curnode = $self->{'CUR'};
    $self->_setDecendants($curnode,0);
}

=head2
Set all nodes to visible in tree view mode
=cut
sub expandTree
{
    my $self = shift;
    my $curnode = $self->{'HEAD'};
    $self->_setDecendants($curnode,1);
}

=head2
Set all nodes to hidden in tree view mode
=cut
sub collapseTree
{
    my $self = shift;
    my $curnode = $self->{'HEAD'};
    $self->_setDecendants($curnode,0);
}

=head2 showFails()
Slightly misnamed, showFails() collapses the entire tree, and then expands the tree to reveal only the nodes which do not have a PASS value 

Currently the only nodes which should be shown are RUNNING and FAIL nodes
=cut
sub showFails
{
    my $self = shift;
    $self->collapseTree();
    foreach my $tid (sort keys %{$self->{'HASH'}})
    {
	$self->_setAncestors($tid,1) if ($self->{'HASH'}->{$tid}->{'TEST_RESULT'} ne 'PASS'); 
    }
}

=head2 keepOpen(), keepOpen('node number');
Sets the current node to always stay open until explicetly closed

Can also take a node number as an argument
=cut
sub keepOpen
{
    my $self = shift;
    my $node = $self->{'CUR'};
    $node = $_[0] if(defined($_[0]));
    $self->_setAncestors($node,1);
}

=head2 keepClosed(), keepClosed('node number');
Sets the current node to always stay closed until explicetly opened 

Can also take a node number as an argument
=cut
sub keepClosed
{
    my $self = shift;
    my $node = $self->{'CUR'};
    $node = $_[0] if(defined($_[0]));
    $self->_setAncestors($node,0);
}

# Set the current node and all children nodes to a given value 
sub _setDecendants
{
    my $self = shift;
    my ($curnode,$value) = @_;

    $self->{'HASH'}->{$curnode}->{'EXPANDED'} = $value;
    foreach (@{$self->{'HASH'}->{$curnode}->{'CHILDREN'}})
    {
	$self->_setDecendants($_,$value);
    }
}

# Set the current node and all parent nodes to a given value 
sub _setAncestors
{
    my $self = shift;
    my ($curnode, $value) = @_;

    while($curnode != $self->{'HEAD'})
    {
	$self->{'HASH'}->{$curnode}->{'EXPANDED'} = $value;
	$curnode = $self->{'HASH'}->{$curnode}->{'PTID'}; 
    }
}

sub getOpened
{
    my $self = shift;
    my @opened = ();
    push(@opened,$self->_getOpened($self->{'HEAD'})); 
    return @opened;
}

sub _getOpened
{
    my ($self,$curnode) = @_;
    my @opened = ();
    if( $self->{'HASH'}->{$curnode}->{'EXPANDED'} == 1){
	push(@opened,$curnode);
    }
    foreach (@{$self->{'HASH'}->{$curnode}->{'CHILDREN'}})
    {
	push(@opened,$self->_getOpened($_)); 
    }
    return @opened;
}



=head2 leftTree()
Go left one level in the tree. The tree is visualized as starting with the top level test on the left and expanding to the right, like a tournament bracket. 
This will navigate to the current test's parent test.
=cut
sub leftTree 
{
    my $self = shift;
    unless ($self->{'CUR'} == $self->{'HEAD'}){
	$self->{'CUR'} = $self->{'HASH'}->{$self->{'CUR'}}->{'PTID'}; 
    }
    return $self->{'CUR'};
}

=head2 downTree()
Go down one level in the tree. This will navigate to the current test's next run sibling test.
=cut
sub downTree
{
    my $self = shift;

    return if ($self->{'CUR'} == $self->{'HEAD'}); # Top level test can't have siblings

    # get the children array of the parent node
    my $parentKids = $self->{'HASH'}->{$self->{'HASH'}->{$self->{'CUR'}}->{'PTID'}}->{'CHILDREN'};

    my $curTID = $self->{'CUR'}; 

    for(my $kid = 1;$kid < @$parentKids;$kid++)
    {
	if($$parentKids[$kid - 1] == $curTID)
	{
	    $self->setCurrent($$parentKids[$kid]);
	    last;
	}
    }

    return $self->{'CUR'};
}

=head2 upTree()
Go up one level in the tree. This will navigate to the current test's previously run sibling test.
=cut
sub upTree
{
    my $self = shift;

    return if ($self->{'CUR'} == $self->{'HEAD'}); # Top level test can't have siblings

    # get the children array of the parent node
    my $parentKids = $self->{'HASH'}->{$self->{'HASH'}->{$self->{'CUR'}}->{'PTID'}}->{'CHILDREN'};

    my $curTID = $self->{'CUR'}; 


    for(my $kid = 1; $kid < @$parentKids;$kid++) 
    {
	if($$parentKids[$kid] == $curTID)
	{
	    $self->setCurrent($$parentKids[$kid - 1]);
	    last;
	}
    }

    return $self->{'CUR'};
}

=head2 rightTree()
Go right one level in the tree. This will navigate to the current test's first run child test 
=cut
sub rightTree 
{
    my $self = shift;
    my $nodeRef = $self->viewNode();
    my $kids = $nodeRef->{'CHILDREN'};

    my $kidCount = @{$nodeRef->{'CHILDREN'}};

    if($kidCount != 0)
    {
	my $firstKid = @{$nodeRef->{'CHILDREN'}}[0];
	$self->setCurrent($firstKid);
    }
    return $self->{'CUR'};
} 

=head2
Toggles weather sibling tests are shown when in tree view mode
=cut
sub toggleSiblings 
{
    my $self = shift;
    $self->{'SHOWSIBLINGS'} = !$self->{'SHOWSIBLINGS'}; 
}

# makes a stack from cur to head
# We can then check the stack to see each of the current test's ancestors
sub _numberStack
{
    my $self = shift;
    my @stack = ();

    # Add all current siblings to the stack if SHOWSIBLINGS is set to 1 
    push(@stack,@{$self->{'HASH'}->{$self->{'HASH'}->{$self->{'CUR'}}->{'PTID'}}->{'CHILDREN'}}) if($self->{'SHOWSIBLINGS'} && $self->{'CUR'} != $self->{'HEAD'});

    my $oldcur = $self->{'CUR'};

    while($self->{'CUR'} != $self->{'HEAD'})
    {
	my $curhash = $self->{'HASH'}->{$self->{'CUR'}};
	push(@stack,$self->{'CUR'});
	$self->leftTree();
    }
    push(@stack,$self->{'HEAD'});

    $self->{'CUR'} = $oldcur;
    @stack = reverse(@stack);

    my %niceHash = ();

    for (my $i = 0; $i < @stack;$i++)
    {
	$niceHash{$stack[$i]} = 1;
    }
    return %niceHash; 
}

=head2 nodeSummary(), nodeSummary('node number')
Prints out a one liner about the current node, or a given node.
Format is : TID [STATUS] 'DESC'
=cut
sub nodeSummary 
{
    my $self = shift;
    my $node = $self->{'CUR'};
    $node = $_[0] if(defined($_[0]));
    my $curNode = $self->{'HASH'}->{$node};
    return $node . " " . $curNode->{'TEST_RESULT'} . " '" . $curNode->{'DESC'} . "'";
}

=head2 setCurrent(), setCurrent('node number')
Go to the head node, or the given node number.
=cut
sub setCurrent
{
    my $self = shift;

    my $node = $self->{'HEAD'};
    $node = $_[0] if(defined($_[0]));

    $self->{'CUR'} = $node if(exists $self->{'HASH'}->{$node});

    return $self->{'CUR'};
}


sub _makeStats
{
    my $self    = shift;

    $self->setCurrent();
    $self->{'STATUS'} = $self->{'HASH'}->{$self->{'CUR'}}->{'TEST_RESULT'};
    $self->_statsSub($self->{'CUR'});
}

sub _statsSub
{
    my $self = shift;
    my $status = $self->{'HASH'}->{$_[0]}->{'TEST_RESULT'}; 
    $self->{'PASS'}++ if($status eq 'PASS');
    $self->{'FAIL'}++ if($status eq 'FAIL');
    $self->{'RUNNING'}++ if($status eq 'RUNNING');
    $self->{'TOTAL'}++;

    my $kids = $self->{'HASH'}->{$_[0]}->{'CHILDREN'};
    foreach (@$kids)
    {
	$self->_statsSub($_);
    }
}

sub getAttr
{
    my ($self,$attr,$node) = @_;
    my $nodeRef;
    if(defined $node){ 
	$nodeRef = $self->viewNode($node);
    }else{
	$nodeRef = $self->viewNode();
    }
    return $nodeRef->{$attr};
}

sub getCur
{
    my ($self) =  @_;
    return $self->{'CUR'};
}

1;
__END__;
