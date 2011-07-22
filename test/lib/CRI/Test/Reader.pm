package CRI::Test::Reader;

########################################################################
# 
# File   :  CRI/Test/Reader.pm
# History:  14-sep-2009 (jdayley) Added this header and made comment
#                       changes.
#
########################################################################
#
# DESCRIPTION:
# A module which parses and navigates a test results set from the 
# CRI::Test module. 
#
# This module isn't useful by itself, but should be used by other 
# scripts to provide useful functions. Some possible uses include an 
# log browser, an html results creator, a results emailer dameon, a 
# results to XML converter, and more! 
#
# DATA STRUCTURE OF THE LOG TREE:
# hash of hash pointers. TID is key
# @tree = (1234=>\%hashy,2342=>\%ahsdy);
#
# Each hash pointed to contains one line of info from the summary.log
# As we're parsing the file, add a new hash entry to each line's hash
# The new entry will have the key of CHILDREN and will be a reference
# to an array of child processes
#
# Return a reference to this tree, and provide methods to walk the tree
#
# Have a pointer which always references the top of the tree
# Have a pointer which always references the current node
#
########################################################################

use strict;
use warnings;

use Fcntl ':flock';

# Set up tab and return string
my $tab = "\t";

$tab = $ENV{ 'treetab' } 
  if ($ENV{ 'treetab' });

# Export
use base 'Exporter';
our @ISA = qw(Exporter);

our @EXPORT = qw(loadLog new viewNode nodeSummary);

#------------------------------------------------------------------------------
# my $testResult = CRI::Test::Reader->new();
#------------------------------------------------------------------------------
#
#Creates a new CRI::Test::Reader object
#
#------------------------------------------------------------------------------
sub new #()
  {

  my $headNode;
  my $curNode;
  my %logHash;
  my $logFile;

  my $self = {
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

  } # END sub new #()

#------------------------------------------------------------------------------
# $testResult->printTree();
#------------------------------------------------------------------------------
#
# Print a tree of the test results summary starting with the head node. The 
# output depends on which nodes are flagged for printing and other options.
#
#------------------------------------------------------------------------------
sub printTree #($)
  {

  my $self = shift;

  # Reset any EXPANDED variables needed
  foreach my $tid (sort keys %{ $self->{ 'HASH' } })
    {

    $self->_setAncestors($tid, 1) 
      if ($self->{ 'HASH' }->{ $tid }->{ 'EXPANDED' } == 1);

    } # END foreach my $tid (sort keys %{ $self->{ 'HASH' } })

  my $retString .= $self->_printTree(0, $self->{ 'HEAD' }, $self->_numberStack());

  return $retString;

  } # END sub printTree #($);

#------------------------------------------------------------------------------
# my $retString 
#   = $self->_printTree(0, $self->{ 'HEAD' }, $self->_numberStack());
#------------------------------------------------------------------------------
#
# DESCRIPTION: 
# _printTree is the recursive method, called by printTree. I suppose you could 
# use it to print only a sub-tree if you wanted to.
#
# PARAMTERS (IN ORDER):
# indent  - How much to indent the current line
# node_id - The node id in the tree where to start printing from.
# stack   - A hash of node_ids that are in the stack of the nodes ancestors.
#
#------------------------------------------------------------------------------
sub _printTree #($$%)
  {

  my $self                    = shift;
  my ($indent, $Node, %stack) = @_;
  my $retString               = "";

  my $curnode = $self->{ 'HASH' }->{ $Node };

  # Append spaces and current line, if appropriate
  if ($curnode->{ 'EXPANDED' } || $stack{ $Node }) # Always include the head node
    {

    $retString .= $tab x $indent;
    $retString .= $self->_printLine($Node);
    $retString .= $self->_printVerbose($indent) 
      if ($Node == $self->{ 'CUR' } && $self->{ 'PRINTVERBOSE' }); # Extra info if needed

    } # if ($curnode->{ 'EXPANDED' } || $stack{ $Node }) # Always include the head node

  $indent++;

  foreach my $kid (@{ $curnode->{ 'CHILDREN' } })
    {

    $retString .= $self->_printTree($indent, $kid, %stack);

    } # END foreach my $kid (@{ $curnode->{ 'CHILDREN' } })

  return $retString;

  } # END sub _printTree #($$%)

#------------------------------------------------------------------------------
# $self->_printLine(1112)
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# _printLine adds the _printTree's current line into the _printTree output. It 
# includes terminal control sequences to make nice colored output.
#
# PARAMETERS (IN ORDER):
# node_id - The node id for which we want to print the line.
#
#------------------------------------------------------------------------------
sub _printLine #($$)
  {

  my $self      = shift;
  my $start     = $_[0];
  my $retString = '';

  # see http://www.termsys.demon.co.uk/vtansi.htm for colors
  $retString .= "[01;37;40m" if ($start == $self->{ 'CUR' });      # set colors
  $retString .= "$start : ";
  $retString .= "[0m"        if ($start == $self->{ 'CUR' });      # set colors

  $retString .= $self->{ 'HASH' }->{ $start }->{ 'TEST_RESULT' };

  $retString .= "[01;37;40m" if ($start == $self->{ 'CUR' });      # set colors
  $retString .= " : " . $self->{ 'HASH' }->{ $start }->{ 'SCRIPT' };
  $retString .= "[0m" if ($start == $self->{ 'CUR' });             # set colors
  $retString .= "\n";

  return $retString;

  } # END sub _printLine #($$)

#------------------------------------------------------------------------------
# $self->_printVerbose($indent);
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Adds extra info to the _printTree output.
#
# PARAMETERS (IN ORDER):
# indent - The indentation for the line
#
#------------------------------------------------------------------------------
sub _printVerbose #($$)
  {
  my $self     = shift;
  my ($indent) = @_;
  my $nodeRef  = $self->viewNode();

  my $retString = "\n";

  $retString .= $tab x $indent;
  $retString .= "\tDESC = " . $nodeRef->{ 'DESC' } . "\n";
  $retString .= $tab x $indent;
  $retString .= "\tLOGFILE = " . $nodeRef->{ 'LOGFILE' } . "\n";
  $retString .= $tab x $indent;
  $retString .= "\tPTID = " . $nodeRef->{ 'PTID' } . "\n";
  $retString .= $tab x $indent;
  $retString .= "\tSCRIPT = " . $nodeRef->{ 'SCRIPT' } . "\n";
  $retString .= $tab x $indent;
  $retString .= "\tTEST_RESULT = " . $nodeRef->{ 'TEST_RESULT' } . "\n";
  $retString .= $tab x $indent;

  if (defined $nodeRef->{ COREDUMP })
    {

    $retString .= "\tCORE_DUMP = " . $nodeRef->{ 'COREDUMP' } . "\n";
    $retString .= $tab x $indent;
    $retString .= "\tCORE_DUMP_LOG = " . $nodeRef->{ 'COREDUMPLOG' } . "\n";
    $retString .= $tab x $indent;

    } # END if (defined $nodeRef->{ COREDUMP })

  $retString .= "\tCHILDREN = " . join(', ', @{ $nodeRef->{ 'CHILDREN' } }) . "\n\n";

  return $retString;

  } # END sub _printVerbose #($$)

#------------------------------------------------------------------------------
# $testResult->loadLog('Summary.log');
#------------------------------------------------------------------------------
#
# Tell the testResult which Summary.log to use. If a log is currently loaded, 
# it is discarded and the given log loaded in its place. Can also be used to 
# reload the current log. 
#
#------------------------------------------------------------------------------
sub loadLog #($$)
  {

  my $self          = shift;
  $self->{ 'FILE' } = $_[0];

  unless (defined $self->{ 'FILE' } && -e $self->{ 'FILE' })
    {

    die("The log file " . $self->{ 'FILE' } . "given doesn't exist!");

    } # END unless (defined $self->{ 'FILE' } && -e $self->{ 'FILE' })

  $self->_readSpool($_[0]);
  $self->_findRootNode;
  $self->{ 'CUR' } = $self->{ 'HEAD' } 
    unless (defined $self->{ 'CUR' });
  $self->_buildChildLinks;
  $self->_makeStats();

  } # END sub loadLog #($$)

#------------------------------------------------------------------------------
# $self->_readSpool('Summary.log');
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Loads a log file (the value of $self->{'FILE'}) creates the appropriate data 
# structure and shoves it in to $self->{'HASH'}.
#
# PARAMETERS (IN ORDER)
# summary_loc - The location of the log summary
#
#------------------------------------------------------------------------------
sub _readSpool #($$)
  {

  my $self = shift;

  # Load a log file
  require 'shellwords.pl';

  my $spoolFile = $self->{ 'FILE' };

  my %data = ();

  if (open(SPOOL, "<$spoolFile"))
    {

    foreach my $line (<SPOOL>)
      {

      flock(SPOOL, LOCK_EX) 
        or die('Bad stuff happened locking Spool file');

      my @words;
      eval "
	    no warnings;
	    \@words = shellwords(\$line)
	    ";
      next if $@;
      next unless scalar @words;
      my $rowId = shift @words;
      $data{ $rowId } = ();

      foreach my $word (@words)
        {

        my ($key, $value) = split /=/, $word;
        $data{ $rowId }->{ $key } = $value;

        } # END foreach my $word (@words)

      my @kids = ();
      $data{ $rowId }->{ 'CHILDREN' } = \@kids;
      $data{ $rowId }->{ 'EXPANDED' } = 0;

      } # END foreach my $line (<SPOOL>)

    close SPOOL;

    } # END if (open(SPOOL, "<$spoolFile"))
  else
    {

    die "Couldn't open summary file";

    } # END else

  $self->{ 'HASH' } = \%data;

  } # END sub _readSpool #($$)

#------------------------------------------------------------------------------
# $self->_buildChildLinks()
#------------------------------------------------------------------------------
#
# Build arrays of child processes. Iterate over every node and append self to 
# parent node's child array.  The Summary.log info only points to parent node
#
#------------------------------------------------------------------------------
sub _buildChildLinks #($)
  {

  my $self = shift;

  foreach my $tid (sort keys %{ $self->{ 'HASH' } })
    {

    next if ($tid == $self->{ 'HEAD' });    # top level doesn't have a parent...

    # Find the parent test line of the current test
    my $curhash    = $self->{ 'HASH' }->{ $tid };
    my $ptid       = $curhash->{ 'PTID' };
    my $parentHash = $self->{ 'HASH' }->{ $ptid };

    # In the parent test line, add the child to the CHILDREN array

    push(@{ $parentHash->{ 'CHILDREN' } }, $tid);

    } # END foreach my $tid (sort keys %{ $self->{ 'HASH' } })

  $self->_sortChildLinks();

  } # END sub _buildChildLinks #($)

#------------------------------------------------------------------------------
# $self->_findRootNode()
#------------------------------------------------------------------------------
#
# Find the top level of the tree. The Summary.log file may not be in the right 
# order.
#
#------------------------------------------------------------------------------
sub _findRootNode #($)
  {

  my $self = shift;

  foreach my $node (sort keys %{ $self->{ 'HASH' } })
    {

    my $curhash = $self->{ 'HASH' }->{ $node };

    # If a node's PTID doesn't exist as a separate
    # line entry, it is the top level process
    if (not exists $self->{ 'HASH' }->{ $curhash->{ 'PTID' } })
      {

      $self->{ 'HEAD' } = $node;
      last;

      } # END if (not exists $self->{ 'HASH' }->{ $curhash->{ 'PTID' } })

    } # END foreach my $node (sort keys %{ $self->{ 'HASH' } })

  } # END sub _findRootNode #($)

#------------------------------------------------------------------------------
# $self->_sortChildLinks()
#------------------------------------------------------------------------------
#
# Sort child arrays by time run so they will display in order. Except that when 
# we go changing the time around, it messes them up!
#
#------------------------------------------------------------------------------
sub _sortChildLinks #($)
  {

  my $self = shift;

  foreach my $tid (keys %{ $self->{ 'HASH' } })
    {

    my $kids = $self->{ 'HASH' }->{ $tid }->{ 'CHILDREN' };
    my @sortedKids 
      = sort { $self->{ 'HASH' }->{ $a }->{ 'TIME_RUN' } <=> $self->{ 'HASH' }->{ $b }->{ 'TIME_RUN' } } @$kids;
    $self->{ 'HASH' }->{ $tid }->{ 'CHILDREN' } = \@sortedKids;

    } # END  foreach my $tid (keys %{ $self->{ 'HASH' } })

  } # END sub _sortChildLinks #($)

#------------------------------------------------------------------------------
# $testResult->viewNode();
# $testResult->viewNode('5234');
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Returns a reference to a hash of the current node's information (or the given 
# hash number's information, if given). 
#
# The hash has all the key value pairs that are found in the Summary.log file 
#
# PARAMETERS:
# node_id - The id of the node (Optional)
#
#------------------------------------------------------------------------------
sub viewNode #($;$)
  {

  my $self   = shift;
  my $viewMe = $self->{ 'CUR' };

  $viewMe = $_[0] 
    if (defined $_[0] && $_[0] ne '');

  return $self->{ 'HASH' }->{ $viewMe };

  } # END sub viewNode #($;$)

#
# Tree navigation  methods
# 

#------------------------------------------------------------------------------
# $self->toggleSummary();
# $self->toggleSummary(0);
# $self->toggleSummary(1);
#------------------------------------------------------------------------------
#  
# Toggle's if extended node info is shown for the current node.
#
# If a value is given, that value is used instead
#
#------------------------------------------------------------------------------
sub toggleSummary #($;$)
  {

  my $self = shift;

  $self->{ 'PRINTVERBOSE' } = !$self->{ 'PRINTVERBOSE' };
  $self->{ 'PRINTVERBOSE' } = $_[0] 
    if (defined($_[0]));

  } # END sub toggleSummary #($;$)

#------------------------------------------------------------------------------
# $self->expandBranch();
#------------------------------------------------------------------------------
#
# Sets the current node and all sub nodes to visible in tree view mode.
#
#------------------------------------------------------------------------------
sub expandBranch #($)
  {

  my $self    = shift;
  my $curnode = $self->{ 'CUR' };

  $self->_setDecendants($curnode, 1);

  } # END sub expandBranch #($)

#------------------------------------------------------------------------------
# $self->collapseBranch()
#------------------------------------------------------------------------------
#
# Sets the current node and all sub nodes to hidden in tree view mode.
#
#------------------------------------------------------------------------------
sub collapseBranch #($)
  {

  my $self    = shift;
  my $curnode = $self->{ 'CUR' };

  $self->_setDecendants($curnode, 0);

  } # END sub collapseBranch #($)

#------------------------------------------------------------------------------
# $self->expandTree()
#------------------------------------------------------------------------------
#
# Set all nodes to visible in tree view mode
#
#------------------------------------------------------------------------------
sub expandTree #($)
  {

  my $self    = shift;
  my $curnode = $self->{ 'HEAD' };

  $self->_setDecendants($curnode, 1);

  } # END sub expandTree #($)

#------------------------------------------------------------------------------
# $self->collapseTree()
#------------------------------------------------------------------------------
#
# Set all nodes to hidden in tree view mode
#
#------------------------------------------------------------------------------
sub collapseTree #($)
  {

  my $self    = shift;
  my $curnode = $self->{ 'HEAD' };

  $self->_setDecendants($curnode, 0);

  } # END sub collapseTree #($)

#------------------------------------------------------------------------------
# $self->showFails()
#------------------------------------------------------------------------------
#
# Slightly misnamed, showFails() collapses the entire tree, and then expands 
# the tree to reveal only the nodes which do not have a PASS value. 
#
# Currently the only nodes which should be shown are RUNNING and FAIL nodes.
#
#------------------------------------------------------------------------------
sub showFails #($)
  {

  my $self = shift;

  $self->collapseTree();

  foreach my $tid (sort keys %{ $self->{ 'HASH' } })
    {

    $self->_setAncestors($tid, 1) if ($self->{ 'HASH' }->{ $tid }->{ 'TEST_RESULT' } ne 'PASS');

    } # END foreach my $tid (sort keys %{ $self->{ 'HASH' } })

  } # END sub showFails #($)

#------------------------------------------------------------------------------
# $self->keepOpen();
# $self->keepOpen(1121);
#------------------------------------------------------------------------------
#
# Sets the current node to always stay open until explicetly closed
# 
# Can also take a node number as an argument
#
#------------------------------------------------------------------------------
sub keepOpen #($;$)
  {

  my $self = shift;
  my $node = $self->{ 'CUR' };

  $node = $_[0] 
    if (defined($_[0]));

  $self->_setAncestors($node, 1);

  } # END sub keepOpen #($;$)

#------------------------------------------------------------------------------
# $self->keepClosed();
# $self->keepClosed(1121);
#------------------------------------------------------------------------------
#
# Sets the current node to always stay closed until explicetly opened 
# 
# Can also take a node number as an argument
#
#------------------------------------------------------------------------------
sub keepClosed #($;$)
  {

  my $self = shift;
  my $node = $self->{ 'CUR' };

  $node = $_[0] 
    if (defined($_[0]));

  $self->_setAncestors($node, 0);

  } # END sub keepClosed #($;$)

#------------------------------------------------------------------------------
# $self->_setAncestors(1112, 1);
#------------------------------------------------------------------------------
#
# Set the current node and all children nodes to a given value
#
#------------------------------------------------------------------------------
sub _setDecendants #($$$)
  {

  my $self = shift;
  my ($curnode, $value) = @_;

  $self->{ 'HASH' }->{ $curnode }->{ 'EXPANDED' } = $value;

  foreach (@{ $self->{ 'HASH' }->{ $curnode }->{ 'CHILDREN' } })
    {

    $self->_setDecendants($_, $value);

    } # END foreach (@{ $self->{ 'HASH' }->{ $curnode }->{ 'CHILDREN' } })
    
  } # END sub _setDecendants #($$$)

#------------------------------------------------------------------------------
# $self->_setAncestors(1112, 1);
#------------------------------------------------------------------------------
#
# Set the current node and all parent nodes to a given value
#
#------------------------------------------------------------------------------
sub _setAncestors #($$$)
  {

  my $self = shift;
  my ($curnode, $value) = @_;

  while ($curnode != $self->{ 'HEAD' })
    {

    $self->{ 'HASH' }->{ $curnode }->{ 'EXPANDED' } = $value;

    $curnode = $self->{ 'HASH' }->{ $curnode }->{ 'PTID' };

    } # END while ($curnode != $self->{ 'HEAD' })

  } # END sub _setAncestors #($$$)

#------------------------------------------------------------------------------
# $self->getOpened()
#------------------------------------------------------------------------------
#
# Returns an array of node_ids that are opened (ie EXPANDED).
#
#
#------------------------------------------------------------------------------
sub getOpened #($)
  {

  my $self   = shift;
  my @opened = ();

  push(@opened, $self->_getOpened($self->{ 'HEAD' }));

  return @opened;

  } # END sub getOpened #($)

#------------------------------------------------------------------------------
# $self->_getOpened(1112)
#------------------------------------------------------------------------------
#
# Recursive subroutine that returns all the child node_ids of the given node_id
# that are opened (ie EXPANDED).
#
#------------------------------------------------------------------------------
sub _getOpened #($$)
  {

  my ($self, $curnode) = @_;
  my @opened = ();

  if ($self->{ 'HASH' }->{ $curnode }->{ 'EXPANDED' } == 1)
    {

    push(@opened, $curnode);

    } # END if ($self->{ 'HASH' }->{ $curnode }->{ 'EXPANDED' } == 1)

  foreach (@{ $self->{ 'HASH' }->{ $curnode }->{ 'CHILDREN' } })
    {

    push(@opened, $self->_getOpened($_));

    } # END foreach (@{ $self->{ 'HASH' }->{ $curnode }->{ 'CHILDREN' } })

  return @opened;

  } # END sub _getOpened #($$) 

#------------------------------------------------------------------------------
# $parent_node_id = $self->leftTree()
#------------------------------------------------------------------------------
#
# Go left one level in the tree. The tree is visualized as starting with the 
# top level test on the left and expanding to the right, like a tournament 
# bracket. 
#
# This will navigate to the current test's parent test.
#
#------------------------------------------------------------------------------
sub leftTree #($)
  {

  my $self = shift;

  unless ($self->{ 'CUR' } == $self->{ 'HEAD' })
    {

    $self->{ 'CUR' } = $self->{ 'HASH' }->{ $self->{ 'CUR' } }->{ 'PTID' };

    } # END  unless ($self->{ 'CUR' } == $self->{ 'HEAD' })

  return $self->{ 'CUR' };

  } # END sub leftTree #($)

#------------------------------------------------------------------------------
# $next_run_node_id = $self->downTree();
#------------------------------------------------------------------------------
#
# Go down one level in the tree. This will navigate to the current test's next 
# run sibling test.
#
#------------------------------------------------------------------------------
sub downTree #($)
  {

  my $self = shift;

  return 
    if ($self->{ 'CUR' } == $self->{ 'HEAD' });    # Top level test can't have siblings

  # get the children array of the parent node
  my $parentKids 
    = $self->{ 'HASH' }->{ $self->{ 'HASH' }->{ $self->{ 'CUR' } }->{ 'PTID' } }->{ 'CHILDREN' };

  my $curTID = $self->{ 'CUR' };

  for (my $kid = 1 ; $kid < @$parentKids ; $kid++)
    {

    if ($$parentKids[$kid - 1] == $curTID)
      {

      $self->setCurrent($$parentKids[$kid]);
      last;

      } # END if ($$parentKids[$kid - 1] == $curTID)

    } # END for (my $kid = 1 ; $kid < @$parentKids ; $kid++)

  return $self->{ 'CUR' };

  } # END sub downTree #($)

#------------------------------------------------------------------------------
# $prev_run_node_id = $self->upTree();
#------------------------------------------------------------------------------
#
# Go up one level in the tree. This will navigate to the current test's 
# previously run sibling test.
#
#------------------------------------------------------------------------------
sub upTree #($)
  {

  my $self = shift;

  return 
    if ($self->{ 'CUR' } == $self->{ 'HEAD' });    # Top level test can't have siblings

  # get the children array of the parent node
  my $parentKids 
    = $self->{ 'HASH' }->{ $self->{ 'HASH' }->{ $self->{ 'CUR' } }->{ 'PTID' } }->{ 'CHILDREN' };

  my $curTID = $self->{ 'CUR' };

  for (my $kid = 1 ; $kid < @$parentKids ; $kid++)
    {

    if ($$parentKids[$kid] == $curTID)
      {

      $self->setCurrent($$parentKids[$kid - 1]);
      last;

      } # END if ($$parentKids[$kid] == $curTID)

    } # END for (my $kid = 1 ; $kid < @$parentKids ; $kid++)

  return $self->{ 'CUR' };

  } # END sub upTree #($)

#------------------------------------------------------------------------------
# $first_child_node_id = $self->rightTree()
#------------------------------------------------------------------------------
# 
# Go right one level in the tree. This will navigate to the current test's 
# first run child test. 
#
#------------------------------------------------------------------------------
sub rightTree #($)
  {

  my $self    = shift;
  my $nodeRef = $self->viewNode();
  my $kids    = $nodeRef->{ 'CHILDREN' };

  my $kidCount = @{ $nodeRef->{ 'CHILDREN' } };

  if ($kidCount != 0)
    {

    my $firstKid = @{ $nodeRef->{ 'CHILDREN' } }[0];
    $self->setCurrent($firstKid);

    } # END if ($kidCount != 0)

  return $self->{ 'CUR' };

  } # END sub rightTree #($)

#------------------------------------------------------------------------------
# $self->toggleSiblings()
#------------------------------------------------------------------------------
#
# Toggles wether sibling tests are shown when in tree view mode
#
#------------------------------------------------------------------------------
sub toggleSiblings #($)
  {

  my $self = shift;
  $self->{ 'SHOWSIBLINGS' } = !$self->{ 'SHOWSIBLINGS' };

  } # END sub toggleSiblings #($)

#------------------------------------------------------------------------------
# $self->_numberStack()
#------------------------------------------------------------------------------
#
# Makes a stack from cur to head. We can then check the stack to see each of 
# the current test's ancestors.
#
#------------------------------------------------------------------------------
sub _numberStack
  {

  my $self  = shift;
  my @stack = ();

  # Add all current siblings to the stack if SHOWSIBLINGS is set to 1
  push(@stack, @{ $self->{ 'HASH' }->{ $self->{ 'HASH' }->{ $self->{ 'CUR' } }->{ 'PTID' } }->{ 'CHILDREN' } }) 
    if ($self->{ 'SHOWSIBLINGS' } && $self->{ 'CUR' } != $self->{ 'HEAD' });

  my $oldcur = $self->{ 'CUR' };

  while ($self->{ 'CUR' } != $self->{ 'HEAD' })
    {

    my $curhash = $self->{ 'HASH' }->{ $self->{ 'CUR' } };
    push(@stack, $self->{ 'CUR' });
    $self->leftTree();

    } # END  while ($self->{ 'CUR' } != $self->{ 'HEAD' })

  push(@stack, $self->{ 'HEAD' });

  $self->{ 'CUR' } = $oldcur;
  @stack = reverse(@stack);

  my %niceHash = ();

  for (my $i = 0 ; $i < @stack ; $i++)
    {

    $niceHash{ $stack[$i] } = 1;

    } # END for (my $i = 0 ; $i < @stack ; $i++)

  return %niceHash;

  } # END sub _numberStack

#------------------------------------------------------------------------------
# $summary_str = $self->nodeSummary();
# $summary_str = $self->nodeSummary(1112);
#------------------------------------------------------------------------------
#
# DESCRIPTION:
# Prints out a one liner about the current node, or a given node.
#
# Format is : TID [STATUS] 'DESC'
#
# PARAMETERS (IN ORDER):
# node_id - The id of the node to print a summary for.
#
#------------------------------------------------------------------------------
sub nodeSummary #($;$)
  {

  my $self = shift;
  my $node = $self->{ 'CUR' };

  $node = $_[0] 
    if (defined($_[0]));

  my $curNode = $self->{ 'HASH' }->{ $node };

  return $node                       . 
         " "                         . 
         $curNode->{ 'TEST_RESULT' } . 
         " '"                        . 
         $curNode->{ 'DESC' }        .  
         "'";

  } # END sub nodeSummary #($;$)

#------------------------------------------------------------------------------
# $node_id = $self->setCurrent();
# $node_id = $self->setCurrent(1112);
#------------------------------------------------------------------------------
#
# Go to the head node, or the given node number.
#
# PARAMETERS (IN ORDER):
# node_id - The id of the node to set to the current node.
#
#------------------------------------------------------------------------------
sub setCurrent #($;$)
  {

  my $self = shift;
  my $node = $self->{ 'HEAD' };

  $node = $_[0] 
    if (defined($_[0]));

  $self->{ 'CUR' } = $node 
    if (exists $self->{ 'HASH' }->{ $node });

  return $self->{ 'CUR' };

  } # END sub setCurrent #($;$)

#------------------------------------------------------------------------------
# $self->_makeStats();
#------------------------------------------------------------------------------
#
# Calculate statisitics for the whole tree
#
#------------------------------------------------------------------------------
sub _makeStats #($)
  {

  my $self = shift;

  $self->setCurrent();
  $self->{ 'STATUS' } = $self->{ 'HASH' }->{ $self->{ 'CUR' } }->{ 'TEST_RESULT' };
  $self->_statsSub($self->{ 'CUR' });

  } # END sub _makeStats #($)

#------------------------------------------------------------------------------
# $self->_statsSub(1112)
#------------------------------------------------------------------------------
#
# Calculate statistics for the given node and its children
#
#------------------------------------------------------------------------------
sub _statsSub #($)
  {

  my $self   = shift;
  my $status = $self->{ 'HASH' }->{ $_[0] }->{ 'TEST_RESULT' };

  $self->{ 'PASS' }++    if ($status eq 'PASS');
  $self->{ 'FAIL' }++    if ($status eq 'FAIL');
  $self->{ 'RUNNING' }++ if ($status eq 'RUNNING');
  $self->{ 'TOTAL' }++;

  my $kids = $self->{ 'HASH' }->{ $_[0] }->{ 'CHILDREN' };

  foreach (@$kids)
    {

    $self->_statsSub($_);

    } # END foreach (@$kids)

  } # END sub _statsSub #($)

#------------------------------------------------------------------------------
# $attribute = $self->getAttr('DESC')
# $attribute = $self->getAttr('DESC', 1112)
#------------------------------------------------------------------------------
#
# Returns the given attributes value for the current node or the given node id.
#
#------------------------------------------------------------------------------
sub getAttr #($$;$)
  {

  my ($self, $attr, $node) = @_;
  my $nodeRef;

  if (defined $node)
    {

    $nodeRef = $self->viewNode($node);

    } # END if (defined $node)
  else
    {

    $nodeRef = $self->viewNode();

    } # END else

  return $nodeRef->{ $attr };

  } # END sub getAttr #($$;$)

#------------------------------------------------------------------------------
# $cur_node_id = $self->getCur()
#------------------------------------------------------------------------------
#
# Returns the current node id.
#
#------------------------------------------------------------------------------
sub getCur #($)
  {

  my ($self) = @_;
  return $self->{ 'CUR' };

  } # END sub getCur #($)

1;

__END__;
