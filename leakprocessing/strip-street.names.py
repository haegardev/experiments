#!/usr/bin/python3
import argparse
import re
import pprint

parser = argparse.ArgumentParser(description="Take street names from PCN and take the largest word to skip small. Skip other stuff numbers small strings")

parser.add_argument("-f","--filename", required=True, help="Street name files")
parser.add_argument("-o","--output", required=True, help="Output file")
parser.add_argument("-l", "--list", required=False, help="List of words that should be skiped")
parser.add_argument("-m", "--min", required=False, type=int, default=3, nargs='?' ,help="Minimal string of a word that is accepted")
args = parser.parse_args()

#Need to filter duplicates
places = dict()

with open (args.filename, "r") as f:
    line = f.readline()
    while line:
        line = line [:-1]
        if line.startswith("#"):
            continue
        words = line.split(' ')
        m = 0
        mw = ""
        for word in words:
            #Skip numbers highways etc such as A13
            if re.findall('[0-9]+',word):
                continue
            l = len(word)
            #Skip small words
            if l  < args.min:
                continue
            if l > m:
                m = l
                mw = word
        if m > 0:
            places[mw] = True
        line = f.readline()
    f.close()

#Need to sort, do not have time to find the best pythonesque approach
buf = []
for k in places.keys():
    buf.append(k)
buf.sort()

#Write to file
with open(args.output,"w") as f:
    for k in buf:
        f.write(k+"\n")
f.close()
