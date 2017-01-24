#!/usr/bin/perl -w

require strict;

my ($dir, $test_dir, $token) = @ARGV;

open S, "grading-scheme.txt" || die "FAIL: could not open grading scheme.";

sub run_test
{
    my ($file,$max_error)=@_;
    my $pass_error = 0;

    if(!defined $hashed_tests{$file})
    {
        `rm -f $dir/file.txt ; cp $test_dir/$file.txt $dir/file.txt`;
        `rm -f $dir/sol.png ; cp $test_dir/$file.png $dir/sol.png`;
        `cd $dir ; timeout 10 ./ray_tracer -i file.txt -s sol.png -o $token.txt ; [ "$?" = "124" ] && echo TIMEOUT >> $token.txt ; echo LAST >> $token.txt`;
        open(my $T, '<', "$dir/$token.txt");
        $_=<$T>;
        my $diff = 1000;
        if(/diff: (.*)/)
        {
            $diff = $1;
        }
        if(/TIMEOUT/)
        {
            $diff="TIMEOUT";
        }
        if(/LAST/)
        {
            $diff="LAST";
        }
        $hashed_tests{$file}=$diff;
        close($T);
        `rm $dir/$token.txt`;
    }
    my $diff = $hashed_tests{$file};
    if($diff eq "TIMEOUT")
    {
        print "FAIL: ($file) Test timed out.\n";
    }
    elsif($diff eq "LAST")
    {
        print "FAIL: ($file) Command failed to report errer.\n";
    }
    elsif($diff>$max_error)
    {
        print "FAIL: ($file) Too much error. Actual: $diff  Max: $max_error.\n";
    }
    else
    {
        print "PASS: ($file) diff $diff vs $max_error.\n";
        return 1;
    }
    return 0;
}

my $total_score=0;
while(<S>)
{
    /^\s*\#/ && next;
    /^\s*$/ && next;
    if(/^(\S+)\s+(\S+)\s+(\S+)\s*$/)
    {
        my $points=$1;
        my $max_error=$2;
        my $file=$3;

        if(&run_test($file,$max_error))
        {
            print "+$points points\n";
            $total_score+=$points;
        }
        else
        {
            print "no points\n";
        }
        
        next;
    }

    print "Unrecognized command: $_";
}

print "FINAL SCORE: $total_score\n";

