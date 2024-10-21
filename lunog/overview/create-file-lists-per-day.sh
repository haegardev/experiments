#!/bin/bash
SOURCE=$1
OUT=$2

if [ -z "$SOURCE" ]; then
    echo "A source with the tshark output must be there"
    exit 1
fi

if [ -z "$OUT" ]; then
    echo "A directory where the filelist should be generated must be specified"
    exit 1
fi


FILELIST=`mktemp`
mkdir -p $OUT

find $SOURCE -type f | sort >$FILELIST

START=`head -n 1 $FILELIST | cut -d '-' -f 2,3,4`
END=`tail -n 1 $FILELIST | cut -d '-' -f 2,3,4`

T0=`date -d $START +%s`
T1=`date -d  $END +%s`

I=$T0
SUF=".lst"
while [ $I -le $T1 ]; do
    DAY=`date -d @$I +%Y-%m-%d`
    cat $FILELIST  |  grep "$DAY" > "$OUT/$DAY$SUF"
    let I+=86400
done

