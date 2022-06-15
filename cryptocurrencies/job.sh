#!/bin/bash
#Job that is execute on a data chunk

FILENAME=$1
URL=$2
#Other parameters can be passed here

if [ ! -e "process_config.sh" ]; then
    echo "Config file is missing" >2
    exit 1
fi

. process_config.sh

SN=`basename $FILENAME`
S=".txt"
RES=$PARTIAL/$SN$S

cat $FILENAME | $PROG -n $SN -o $URL -t $RES
