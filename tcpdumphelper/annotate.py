#!/usr/bin/python3

import re
import sys
import geoip2.database
import argparse
import pprint

parser = argparse.ArgumentParser(description="Read tcpdump output with the -tttt option from stdin and annotate IP addresses")
parser.add_argument("--mmdb", type=str, help="The MMDB file")
args = parser.parse_args()

def annotate_logs_with_country(mmdb_file_path):
    """
    Reads log lines from standard input, appends the country code to the first IP address,
    and writes the annotated lines to standard output.
    """
    # Compile a regex to match the first IP address in each line
    ip_regex = re.compile(r'\b\d{1,3}(?:\.\d{1,3}){3}\b')
    # Open the MaxMind database and process the input
    with geoip2.database.Reader(args.mmdb) as reader:
        for line in sys.stdin:
            line = line.strip()  # Remove trailing whitespace
            match = ip_regex.search(line)
            if match:
                ip_address = match.group()
                try:
                    # Get the country code for the IP address
                    response = reader.country(ip_address)
                    #asn=response.AutonomousSystemNumber
                    country_code = response.country.iso_code
                    #asn=response.country.autonomousSystemNumber
                    # Append the country code to the IP address
                    asn=1
                    annotated_line = line.replace(ip_address, f"{ip_address} [{country_code},{asn}]")
                except geoip2.errors.AddressNotFoundError:
                    annotated_line = line.replace(ip_address, f"{ip_address} [UNKNOWN]")
                #except Exception as e:
                #    annotated_line = line + f" [ERROR: {str(e)}]"
            else:
                annotated_line = line  # If no IP address is found, keep the line unchanged
            # Print the annotated line to standard output
            #print(annotated_line)

# Run the script
if __name__ == "__main__":
    annotate_logs_with_country(args.mmdb)

