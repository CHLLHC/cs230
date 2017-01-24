#!/bin/bash

# ./grading-script.sh <test-dir> <grading-scheme.txt>

test_dir="$1"

# Set up scratch space for grading
dir="grading"
mkdir -p $dir
cp *.h *.cpp SConstruct $dir

# Check for cheating
token=`mktemp XXXXXXXXXXXXXXXXXXXXXXXX`
rm $token

# Compile project
{
    pushd $dir
    if ! scons -j 8 ; then
        echo "FAIL: Did not compile"
        exit 1
    fi
    popd >/dev/null
}

# Check results against grading criteria
./grading-helper.pl $dir "$test_dir" $token
