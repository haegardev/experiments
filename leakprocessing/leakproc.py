#!/usr/bin/python3
import argparse
import pprint


parser = argparse.ArgumentParser(description='Process text files to identify basic leak data')


parser.add_argument("-f", "--filename", required=True, help="Text file name")
parser.add_argument("-p","--processed", required=True, help="Procssed files list. To avoid to process the files over and over ")
args = parser.parse_args()
#TODO length parameter

#minbufflen block needs to have at least 3 lines (name, street name, village)
def process_buffer(buf, minbufflen=3, maxbufflen=9):
    l  = len(buf)
    print (l)
    if (l > minbufflen and len(buf) < maxbufflen):
        pprint.pprint(buf)

with open (args.filename,"r") as f:
    line  = f.readline()
    buf = []
    minlen = 40
    lastlen = 0
    while line:
        try:
            #Strip newlines
            line = line[:-1]
            l = len(line)
            if l < minlen and lastlen < minlen:
                buf.append(line)
                lastlen = l

            if l == 0:
                process_buffer(buf)
                del (buf)
                buf = []
            line  = f.readline()
        except UnicodeDecodeError as u:
            pass
