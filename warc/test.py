#!/usr/bin/python3
import re
import pprint
from fastwarc import ArchiveIterator

def process_record(record):
    """
    Processes a WARC record, extracts specific header fields,
    and optionally applies regex to the payload.
    """
    # Extract headers
    warc_payload_digest = record.headers.get("WARC-Payload-Digest", "N/A")
    warc_block_digest = record.headers.get("WARC-Block-Digest", "N/A")
    warc_payload_type = record.headers.get("WARC-Identified-Payload-Type", "N/A")

    pprint.pprint(record.headers)
    print("=== WARC Header Fields ===")
    print(f"WARC-Payload-Digest: {warc_payload_digest}")
    print(f"WARC-Block-Digest: {warc_block_digest}")
    print(f"WARC-Identified-Payload-Type: {warc_payload_type}")

    # Get payload if it exists
    #payload = record.payload.read().decode("utf-8", errors="ignore") if record.payload else ""
    #if payload:
        #print("\n=== Payload Content ===")
        #print(payload[:500])  # Print the first 500 characters of the payload

        # Example: Regex to find HTML <title> tag in payload
        #title_match = re.search(r"<title>(.*?)</title>", payload, re.IGNORECASE)
        #if title_match:
        #    print("\nExtracted Title:")
        #    print(title_match.group(1))

def read_warc(file_path):
    """
    Reads a WARC file using fastwarc and processes each record.
    """
    with open(file_path, "rb") as warc_file:
        for record in ArchiveIterator(warc_file):
            print("\n=== New WARC Record ===")
            process_record(record)

if __name__ == "__main__":
    # Replace 'sample.warc' with the path to your WARC file
    warc_file_path = "CC-MAIN-20241101184224-20241101214224-00002.warc.gz"
    read_warc(warc_file_path)

