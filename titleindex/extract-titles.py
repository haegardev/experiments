#!/usr/bin/python3
import pprint
import json
import sys
from fastwarc.stream_io import *
from fastwarc.warc import ArchiveIterator, WarcRecordType

# GZip:
stream = GZipStream(open('CC-MAIN-20240305124045-20240305154045-00875.warc.wat.gz', 'rb'))
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
                                    sys.stdout.write(u + "|" + data["Envelope"]["Payload-Metadata"]["HTTP-Response-Metadata"]["HTML-Metadata"]["Head"]["Title"]+"\n")

