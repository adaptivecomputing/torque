package CRI::Test::HTML;
$VERSION = 0.01;

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use base 'Exporter';
use CRI::Test::Reader qw();
use Data::Properties;
use CGI;
use HTML::Entities; 
our @ISA = qw(Exporter CGI);
our @EXPORT = qw(
new
loadLog
testSummary
runSummary
pageStart
pageEnd
listRuns
nestedTests
_testDiv
);

# Copyright 2007, Cluster Resources
# By Michael Moore, mmoore@clusterresources.com
# Module to produce HTML from test result logs

# What do I want to access? 
# Tests for a specific test run
# Test summary 
# Individual test results
# http://perltestdev/perltest.pl?run=2007-11-19_06.01.40&test=3455_uninstall.bat.log 

sub new 
{
# Load the cluster test config file to find out where the test results are stored
    my $props = Data::Properties->new();
    open(FH, "</etc/clustertest.conf") or die "Couldn't read /etc/clustertest.conf : $!\n";
    $props->load(\*FH);
    close FH;

    my $logfile = "";
    my $cgi = new CGI;
    print $cgi->header;
    my $run = "";    
    my $readerObj = CRI::Test::Reader->new;

    my $self = 
    {
	FILE   => $logfile,
	RUN    => $run,
	CGI    => $cgi,
	READER => $readerObj,
	PROPS  => $props,

    };
    bless $self;
    return ($self,$cgi);
}

sub loadLog
{
    my $self = shift;
    $self->{'FILE'} = $self->{'PROPS'}->get_property('Outdir') . $_[0] . "/Summary.log";
    unless(-e $self->{'FILE'}){
	print "<html><body><h1>The specified file $self->{'FILE'} did not exist on the server</h1></body></html>";
	exit(-1);
    }
    $self->{'RUN'} = $_[0];
    $self->{'READER'}->loadLog($self->{'FILE'}); 
}


sub pageStart 
{
    my $self = shift; 
    my $retstring = "";
    $retstring .= "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">
    <html>
    <head>
    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" >
    <title>Test results for test run " . $self->{'RUN'} . "</title>
    <link rel='stylesheet' type='text/css' href='/perltest.css'>
    <script type='text/javascript' src='/js.js'></script>
    </head>
    <body>
    ";
    return $retstring;
}

sub runSummary 
{
    my $self = shift;

    my $tot     = $self->{'READER'}->{'TOTAL'};
    my $status  = $self->{'READER'}->{'STATUS'};
    my $bad     = $self->{'READER'}->{'FAIL'};
    my $good    = $self->{'READER'}->{'PASS'};
    $self->{'READER'}->setCurrent();
    my $desc = $self->{'READER'}->getAttr('DESC'); 

    my $retstring = "";
    $retstring .= "<h1>Run: $self->{'RUN'}</h1>";

    $retstring .= "<table>\n<tr><td>Status</td><td colspan='3'>Batch</td><td>Test File</td><td>Description</td></tr>\n";
    $retstring .= "<tr><td class='$status'>$status</td><td class='TOTAL'>$tot</td><td class='PASS'>$good</td><td class='FAIL'>$bad</td><td>$self->{'FILE'}</td><td>$desc</td></tr>\n</table>\n";

    # build an appropriate summary for the test in $run and return it 
    return $retstring;
}

sub testSummary
{
    my $self = shift;
}

sub listRuns
{
    my $self = shift;
    my $dir = $self->{'PROPS'}->get_property('Outdir');
    chdir($dir);
    my @runs = <*>;
    return @runs;
}
    
my @allDivs = ();
my @allImgs = ();

sub nestedTests 
{
    my $self = shift;
    my $retstring = "";
    $self->{'READER'}->setCurrent();
    $retstring .= $self->_subNest($self->{'READER'}->{'CUR'});
    
    # Build javascript for the all/none expanders
    my $jsString = "<script type='text/javascript'>
    <!--//
    var allImgs = new Array('" . join("','",@allImgs) . "');
    var allDivs = new Array('" . join("','",@allDivs) . "');
    //-->
    </script>\n
    <span onclick='allOpen();'>Open all tests</span> | <span onclick='allClose();'>Close all tests</span><br><br>\n\n
    \n\n";

    $retstring = $jsString . $retstring;

    return $retstring;
}

sub _subNest
{
    my ($self,$tid) = @_;

    my $retstring = "";
    my $kids = $self->{'READER'}->{'HASH'}->{$tid}->{'CHILDREN'}; 

    if((scalar @$kids) != 0){
	$retstring .= "\n<div class='testreskids'>\n";
	$retstring .= "<img class='plusminus' id='img$tid' onclick='toggleSubs(\"img$tid\",\"div$tid\");' src='/plus.png' alt='expand or contract this test set' >\n";
	push(@allDivs,"div$tid");
	push(@allImgs,"img$tid");
    }else{
	$retstring .= "\n<div class='testres' id='div$tid'>\n";
    }

    $retstring .= "<div class='passbar ". $self->{'READER'}->getAttr('TEST_RESULT',$tid) . "' onclick='toggleLong(\"long$tid\");'>" . $self->{'READER'}->getAttr('DESC',$tid) . "</div>\n";  
    $retstring .= "<div class='longresults' id='long$tid'>\n<pre>\n";
    $retstring .= _readLog($self->{'READER'}->getAttr('LOGFILE',$tid)); 
    $retstring .= "\n</pre>\n</div>\n";

    $retstring .= "\n<div id='div$tid' class='hiddenKids'>" if((scalar @$kids) != 0);
    foreach(@$kids)
    {
	$retstring .= $self->_subNest($_); 
    }
    
    $retstring .= "</div>\n" if((scalar @$kids) != 0);

    $retstring .= "\n</div>\n";

    return $retstring;
}

sub _readLog
{
    my $retstring = "";
    my @log;
    {
	local $/ =  "\n";

	open (LOG,"<$_[0]");
	@log = <LOG>;
	close(LOG);
	chomp(@log);
    }

    $retstring = join("\n",@log);
   $retstring = HTML::Entities::encode($retstring);
    return $retstring;
}

sub pageEnd
{
    my $self = shift;
    my $retstring = "";
    $retstring .= "</body>\n</html>";
    return $retstring;
}

1;
