#!/bin/bash
FILENAME=$1

#Search onions with a rough pass onions then tansform or control characters to newlines
# and do a second path
F="`basename $FILENAME | sed 's/.gz/.onions/g'`"
TF="./results/$F"


if [ -e $TF ]; then
        echo "[INFO] File $FILENAME was already processed"
        exit 0
fi

nice -n 19 ionice -c 3 zcat $FILENAME | grep -a onion | tr ' "><' '\n' | grep "\.onion$" > ./results/$F


# Need to tell download script to not download it again if it is processed
FN=$(basename $FILENAME)
url="$(zcat warc.paths.gz | grep $FN)"
#FIXME Not multiproc safe
echo $url >> processed-urls.txt
#Delete processed WARC file
rm $FILENAME
