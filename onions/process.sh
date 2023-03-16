#!/bin/bash
FILENAME=$1

#Search onions with a rough pass onions then tansform or control characters to newlines
# and do a second path
F="`basename $FILENAME | sed 's/.gz/.onions/g'`"

zcat $FILENAME | grep -a onion | tr ' "><' '\n' | grep "\.onion$" > ./results/$F
