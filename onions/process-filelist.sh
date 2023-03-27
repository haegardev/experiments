#!/bin/bash
FILELIST=$1
PROCESSEDLIST=$2

P=`cat $PROCESSEDLIST`

for i in `cat $FILELIST`; do
    j="`basename $i`"
    if [ -z "`echo $P | grep $j`" ]; then
        echo "Process $i"
        echo $j >> $PROCESSEDLIST
    fi
done

