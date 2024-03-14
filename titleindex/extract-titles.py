#!/usr/bin/python3
import argparse
import pprint
import json
import sys
from fastwarc.stream_io import *
from fastwarc.warc import ArchiveIterator, WarcRecordType


def process_file(inputfile, outputfile):
    g = open(outputfile, 'wb')
    g.write(b'Format: URL|HTML title (encoding utf-8)\n');
    # GZip:
    stream = GZipStream(open(inputfile, 'rb'))
    #for record in ArchiveIterator(stream, record_types=WarcRecordType.request | WarcRecordType.response):
    for record in ArchiveIterator(stream):
        u=None
        l=0
        for (p,q) in record.headers:
            if p=="WARC-Target-URI":
                u=q
            if p=="Content-Length":
                l=int(q)
            if (u is not None and l>0):
                #print ("Got url",u, "length", l)
                body = record.reader.read(l)
                data = json.loads(body)
                if "Envelope" in data:
                    if "Payload-Metadata"  in data["Envelope"]:
                        if "HTTP-Response-Metadata" in data["Envelope"]["Payload-Metadata"]:
                            o  = data["Envelope"]["Payload-Metadata"]
                            if "HTML-Metadata"  in data["Envelope"]["Payload-Metadata"]["HTTP-Response-Metadata"]:
                                if "Head"  in  data["Envelope"]["Payload-Metadata"]["HTTP-Response-Metadata"]["HTML-Metadata"]:
                                    if "Title"  in  data["Envelope"]["Payload-Metadata"]["HTTP-Response-Metadata"]["HTML-Metadata"]["Head"]:
                                        # Title is a string by the library u is a string
                                        g.write(u.encode('utf-8')+b"|" +  data["Envelope"]["Payload-Metadata"]["HTTP-Response-Metadata"]["HTML-Metadata"]["Head"]["Title"].encode('utf-8') + b"\n")



parser = argparse.ArgumentParser(description="Read common crawl WAT files and write html titles with TARGET-URI in the outputfile")
parser.add_argument("-i", "--input", help="Input file path", required=True)
parser.add_argument("-o", "--output", help="Output file path", required=True)
args = parser.parse_args()

process_file(args.input, args.output)
