

# Sample Understand PERL API Script
#
# Synopsis: Export all metrics to a csv file, same as Projects|Export Metrics in the GUI
#
# Language: C/C++, Java
#
# Usage:
sub usage ($)
{
    return << "END_USAGE";
${ \(shift @_) }
Usage: cj_metric_export.pl -db database -f name
       -db database      Specify Understand database (required for uperl, inherited from Understand)
       -r|res name      path to directory
       -f|file name      name of the file to export (*.csv)
END_USAGE
}
#
#  For the latest Understand perl API documentation, see
#      http://www.scitools.com/perl.html
#
#  14-Dec-2000 KN
# 03-Aug-2001 DLL - updated for Understand::Gui::db()

use Understand;
use Getopt::Long;
use strict;
use warnings;
use Cwd;
#use Text::CSV;

my $dbPath;
my $help;
my %metrics;
my %metricList;
my $exportFile;
my $res;
my $computerLinesTotal;
my $humanLinesTotal;
GetOptions(
	   "db=s" => \$dbPath,
	   "help" => \$help,
           "f|file=s" => \$exportFile,
          "r|res=s" => \$res,
          );

# help message
die usage("") if ($help);

# export file
die usage("Please specify the file to export metrics to\n") if (! $exportFile);

# open the database
my $db=openDatabase($dbPath);

# check language
if ( $db->language() !~ "C|Java" ) {
    die "This script is designed for C and Java only\n";
}

my $functions = "Function ~unknown ~unresolved";
my $classes = "Class ~unknown ~unresolved ,Struct ~unknown ~unresolved,Union ~unknown ~unresolved";
my $codefiles = "File ~Header ~unknown ~unresolved";
my $headerFiles = "Header File ~unknown ~unresolved";

#my $csv = Text::CSV->new({ sep_char => ',' });

my $file =$res;
my @cls;
my @chg;
my $n;
my $flg;
my $i;

open(my $data, '<', $file) or die "Could not open '$file' $!\n";
while (my $line = <$data>) 
{
  chomp $line;
  my @fields = split "," , $line;
      push(@cls,$fields[0]);
      push(@chg,$fields[1]);
 
}
$n=@chg;

my @ents = $db->ents($functions.",".$classes.",".$codefiles.",".$headerFiles);
my @mat=("CountClassDerived","CountLineCode","MaxInheritanceTree","CountDeclMethodAll","CountDeclMethod","CountClassCoupled","PercentLackOfCohesion","CountDeclMethodPrivate","CountDeclMethodProtected","CountDeclMethodPublic");
    foreach my $ent (@ents)
{
        next if $ent->library();
        foreach my $metric(@mat)
    { 
            $metrics{$ent->id()}{$metric}=$ent->metric($metric);
            $metricList{$metric} = 1;
    }
}
    
printMetrics($exportFile);
print "Metrics exported to $exportFile\n";

closeDatabase($db);


# subroutines
# print the metrics to the specified file
sub printMetrics()
{
    my $filename = shift;
    open (FILEHANDLE, ">".$filename) || die "Can't create $filename: $! \n";
    
    #print metric header print FILEHANDLE "Kind,Name";
    print FILEHANDLE "Name";
    for my $metric(sort keys %metricList) {
        print FILEHANDLE ",$metric";
    }
=pod
    print FILEHANDLE ",Hand Written LOC, Generated LOC\n";    
   # Print Functions
    foreach my $ent (sort {lc($a->longname()) cmp lc($b->longname());} $db->ents($functions)){
        print FILEHANDLE "Function,".$ent->longname();
        for my $metric(sort keys %metricList) {
            my $metValue = $metrics{$ent->id}{$metric};
            print FILEHANDLE $metValue?",".$metValue:",0"
        }
        print FILEHANDLE ",\n";
    }
=cut
    print FILEHANDLE ",Changes\n";
    
    #Print Classes
   foreach my $ent (sort {lc($a->longname()) cmp lc($b->longname());} $db->ents($classes))
   {     
     $flg = 0;
     for($i = 0;$i < $n;$i = $i+1)
     {           
       if( @cls[$i] eq $ent->longname() )
       {
        $flg=1;
        @cls[$i]="heeriye.c";
        last;
       }
     }
     if($flg > 0)
     {
        my $kindname = "Class";
        $kindname = "Struct" if $ent->kind->check("Struct");
        $kindname = "Union" if $ent->kind->check("Union");
        #       print FILEHANDLE "$kindname,".$ent->longname();
        print FILEHANDLE $ent->longname();
        for my $metric(sort keys %metricList) {
            my $metValue = $metrics{$ent->id}{$metric};
            print FILEHANDLE $metValue?",".$metValue:",0"
        }
        if(@chg[$i]>0)
        {
              print FILEHANDLE ",Yes\n";
        }
        else
        {
         print FILEHANDLE ",No\n";
        }
      }
   }
=pod    
    #Print code files
    foreach my $ent (sort {lc($a->name()) cmp lc($b->name());} $db->ents($codefiles)){
        print FILEHANDLE"CodeFile".",".$ent->relname();
        for my $metric(sort keys %metricList) {
            my $metValue = $metrics{$ent->id}{$metric};
            print FILEHANDLE $metValue?",".$metValue:",0"
        }
		print FILEHANDLE countLines($ent);
        print FILEHANDLE "\n";
    }
    
    #Print Header Files
    foreach my $ent (sort {lc($a->name()) cmp lc($b->name());} $db->ents($headerFiles)){
        print FILEHANDLE"HeaderFile".",".$ent->relname();
        for my $metric(sort keys %metricList) {
            my $metValue = $metrics{$ent->id}{$metric};
            print FILEHANDLE $metValue?",".$metValue:",0"
        }
		print FILEHANDLE countLines($ent);
        print FILEHANDLE "\n";
    }
=cut
    close (FILEHANDLE);
    
}


#Return Hand written vs generated counts
sub countLines{
	my $file = shift;
	
	my $lexer = $file->lexer(0);
	my @body = $lexer->lexemes() if $lexer;
	
	my $computerLineCount=0;
	my $humanLineCount=0;
	my $inHumanCode=0;  #Are we in Human code or computer code
	my $codeOnLine=0; #is there code on this line
	
	foreach my $chunk (@body){
		if ($chunk->token eq "Comment"){
           #look for //#[ or //#] to start or end human code 
				if ( $chunk->text =~ /^\/\/#\[/){
					$inHumanCode=1;
					if ($codeOnLine){
						$computerLineCount++;
						$codeOnLine=0;
					}
				}
				#Switch to computer code
				if ( $chunk->text =~ /^\/\/#\]/){
					$inHumanCode=0;
					if ($codeOnLine){
						$humanLineCount++;
						$codeOnLine=0;
					}
				}
				next;
		}elsif ($chunk->text =~ /\n/){
			next if ! $codeOnLine;
			if ($inHumanCode){
				$humanLineCount ++;
			}else{
				$computerLineCount++;
			}
			$codeOnLine=0;
			next;
		}elsif( $chunk->token eq "Whitespace"){
			next;
		}
			#print "Counting \"".$chunk->text. "\" ".$chunk->token."\n" if !$codeOnLine;
			$codeOnLine=1;
	}
	#print "   Hand-written: $humanLineCount Generated: $computerLineCount\n";
	$computerLinesTotal += $computerLineCount;
	$humanLinesTotal += $humanLineCount;
	return "$humanLineCount,$computerLineCount";
}
sub openDatabase($)
{
    my ($dbPath) = @_;

    my $db = Understand::Gui::db();

    # path not allowed if opened by understand
    if ($db&&$dbPath) {
	die "database already opened by GUI, don't use -db option\n";
    }

    # open database if not already open
    if (!$db) {
	my $status;
	die usage("Error, database not specified\n\n") unless ($dbPath);
	($db,$status)=Understand::open($dbPath);
	die "Error opening database: ",$status,"\n" if $status;
    }
    return($db);
}

sub closeDatabase($)
{
    my ($db)=@_;

    # close database only if we opened it
    $db->close() if $dbPath;
}

