#!/usr/bin/python3
import argparse
import pprint


parser = argparse.ArgumentParser(description='Process text files to identify basic leak data')


parser.add_argument("-f", "--filename", required=True, help="Text file name")
parser.add_argument("-p","--processed", required=True, help="Procssed files list. To avoid to process the files over and over ")
parser.add_argument("-c","--corpus", required=True, help="List of known places")
parser.add_argument("-e", "--exclude",required=False, help="Exclude list. If a word in this list is in the text block, the text block is skipped")

args = parser.parse_args()

#Load exclude list
exclist = dict()
if args.exclude is not None:
    with open(args.exclude,"r") as f:
        for line in f.readlines():
            line = line[:-1]
            exclist[line]=True


#Return True if line matches exclude
#Exact match gives bad results
#Maybe more flexible matching must be done
def check_exclude(line):
    for e in exclist:
        if line.find(e) > -1:
            return True
    return False

#Load corpus of known places
corpus = dict()
with open(args.corpus, "r") as f:
    for line in f.readlines():
        line = line[:-1]
        corpus[line]=True

#TODO length parameter

#Cleanup address and write it
def handle_address(buf):
    out = []
    for i in buf:
        if len(i) > 0:
            out.append(i)
    #TODO line number or offset where match happend
    print  ("ADDRESS|"+args.filename+"|"+",".join(out))
#minbufflen block needs to have at least 3 lines (name, street name, village)
def process_buffer(buf, minbufflen=3, maxbufflen=9):
    l  = len(buf)
    #Give a score for each block
    #+5 if there is a known place insite
    #+1 if there is a number
    score = 0
    if (l > minbufflen and len(buf) < maxbufflen):
       for line in buf:
            if check_exclude(line):
                return None
            words = line.split(' ')
            for word in words:
                word = word.upper()
                if word in corpus:
                    score=score  + 5
       #Ranking of highest scores needed?
       if score > 0:
            handle_address(buf)

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
