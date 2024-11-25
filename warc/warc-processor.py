#!/usr/bin/python3
import re
import pprint
import argparse
import os
import gzip
from fastwarc import ArchiveIterator

class WARCProcessor:
    def __init__(self, file_path, results):
        """
        Initialize the WARCProcessor with a file path.
        """
        self.file_path = file_path
        self.results = results

        self.short_name = os.path.basename(file_path)
        # Create a directory per WARC to access easier the content
        self.out_dir = self.results + os.sep + self.short_name.replace(".warc.gz","")
        # Create output directory if does not exists
        if os.path.exists(self.out_dir) == False:
            os.makedirs(self.out_dir)
        self.link_ip_url_file=self.out_dir +  os.sep + "link-ip-url.csv.gz"
        # Open gzip file for link-ip-url.csv
        self.link_ip_url_gz = gzip.open(self.link_ip_url_file,"wt")


    def process_record(self, record):
        t=record.headers.get("WARC-TYPE")
        if (t == 'request'):
            target_ip=record.headers.get("WARC-IP-Address")
            target_uri = record.headers.get("WARC-Target-URI")
            warc_date = record.headers.get("WARC-Date")
            #TODO write this in a file 
            #print (f"{warc_date},{target_ip},{target_uri}")
            self.link_ip_url_gz.write(warc_date+","+target_ip +"," + target_uri +"\n")

    def read_warc(self):
        """
        Reads a WARC file using fastwarc and processes each record.
        """
        with open(self.file_path, "rb") as warc_file:
            for record in ArchiveIterator(warc_file):
                #print("\n=== New WARC Record ===")
                self.process_record(record)
        self.link_ip_url_gz.close()
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Read common crawl WARC files and write subset of data in a directory")
    parser.add_argument("-i", "--input", help="Input file path", required=True)
    parser.add_argument("-o", "--output", help="Output directory", required=True)
    args = parser.parse_args()
    obj = WARCProcessor(args.input, args.output)
    obj.read_warc()


