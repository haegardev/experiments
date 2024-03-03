#!/bin/bash
FILENAME=$1
TARGET="$HOME/pcap-processing/counts"
TD="$TARGET/$FILENAME"
mkdir -p $TD

tshark -n -r $FILENAME -E occurrence=f -E separator=, -T fields -e frame.time_epoch -e ip.src -e ip.dst -e ip.proto | ./countpcapitems -s $FILENAME -t $TD -i

