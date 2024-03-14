#!/bin/bash
URL="http://data.commoncrawl.org/$1"
FILENAME=`basename $URL`
DFILE=`echo "$FILENAME"  | sed 's/.warc.wat.gz/.txt/g'`
RFILE="./results/$DFILE"
TEMPFILE="./temp/$FILENAME"
wget $URL -O $TEMPFILE
./extract-titles.py -i $TEMPFILE -o $RFILE
rm $TEMPFILE
