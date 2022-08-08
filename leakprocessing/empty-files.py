#!/usr/bin/python3
import argparse
import os
import pprint
parser = argparse.ArgumentParser(description='Check directory for empty files. Gives indication of failed document transforming')
parser.add_argument("-d", "--directory", required=True, help="Directory to look for empty files")
parser.add_argument("-l", "--list", required=False, action='store_true',help="List all empty files")
parser.add_argument("-e","--empty",required=False,action='store_true',help="Show empty and non empty files")
args = parser.parse_args()

cnt = 0
ecnt = 0
if args.list:
    print ("#size,filename")
for subdir,dirs,files in os.walk(args.directory):
    for file in files:
        f = os.path.join(subdir,file)
        r = os.stat(f)
        if args.list:
            print (str(r.st_size) + "," + f)
        if r.st_size == 0:
            ecnt = ecnt + 1
        cnt = cnt + 1


if args.empty:
    print ("#Number of empty files|"+str(ecnt))
    print ("#Number of total files|"+str(cnt))
