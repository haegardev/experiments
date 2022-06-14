#!/bin/bash
#Primitive map reduce script that could run on bare linux servers without many addional software
START=`date "+%y/%m/%d %H:%M:%S"`
#take an url download it, split it
URL=$1
FILENAME=$1
PARTIAL="$HOME/results"
RESULTS="$HOME/datasets"


if [ -z $URL ]; then
    echo "An url must be specified" >$2
    exit 1
fi

#Test if directories exists
CHUNK_SIZE=1000000
CHUNKS="$HOME/chunks"
DOWN="$HOME/down"

#CURL pipe | gzip does not work. Curl stops if to slow or whatever
#wget $URL -o $DOWN/dataset.gz

SHA256=`sha256sum $DOWN/dataset.gz`
cd $CHUNKS
zcat $DOWN/dataset.gz | gzip -c  | split -l  $CHUNK_SIZE -a 6 --numeric-suffixes


#find down/ -type f | sort | parallel -j 4 'cat {} | ~/experiments/cryptocurrencies/kwmatch.py -n {} -t ~/$PARTIAL/`basename {}`.txt'

PROCTIME=`date "+%y/%m/%d %H:%M:%S"`
MTIME=`date "+%y/%m/%d %H:%M:%S"`

#Build a result filename
RN="`echo $URL | sed 's#https://data.commoncrawl.org/crawl-data/##g'`"
RN="`echo $RN | sed 's/.gz/.crypto_onion.txt/g'`"
D="$RESULTS/`dirname $RN`"
if [ ! -d "$D" ]; then
    mkdir -p "$D"
fi

RSN="$D/`basename $RN`"

echo "#Merged results file"  >$RSN
echo "#Processed URL: $URL" >>$RSN
echo "#SHA256 sum of the data: $SHA256" >> $RSN
echo "#Started download: $START" >>$RSN
echo "#Started processing: $PROCTIME" >>$RSN
echo "#Started Merging: $MTIME" >>$RSN
echo "#Chunk size: $CHUNK_SIZE" >>$RSN
echo "#Line numbers are relative to the decompressed chunks" >>$RSN
echo "#Format | seperated list. First column is line number, second column regexp name, third column what has matched" >>$RSN
for i in `ls $PARTIAL| sort`; do
    cat $PARTIAL/$i >>$RSN
done

DTIME=`date "+%y/%m/%d %H:%M:%S"`

echo "#Finished: $DTIME" >> $RSN
gzip $RSN

#TODO remove partial results
#TODO remove downloaded stuff
