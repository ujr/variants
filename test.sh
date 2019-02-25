#!/bin/sh
# Super simple unit testing: from CASEFILE, pipe lines that
# start with I through variants expansion to OUTFILE, and copy
# lines that start with O to EXPFILE, then compare the two files.

CASEFILE="test.cases"
OUTFILE="test.out"  # actual output
EXPFILE="test.exp"  # expected output

CANDIDATE=${@:?Usage: ${0} CANDIDATE} # ./variants or python variants.py

# Reset output files:
echo "Variant Expansion Test" > $OUTFILE
echo "Variant Expansion Test" > $EXPFILE

while read KEY LINE
do
  case "$KEY" in
  I)
    echo "> $LINE" >> $EXPFILE
    echo "> $LINE" >> $OUTFILE
    $CANDIDATE "$LINE" >> $OUTFILE
    ;;
  O)
    echo "$LINE" >> $EXPFILE
    ;;
  esac
done < $CASEFILE

diff $EXPFILE $OUTFILE && echo "TEST PASSED" || echo "TEST FAILED"

echo "Candidate: $CANDIDATE"
