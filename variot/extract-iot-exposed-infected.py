#!/usr/bin/python3
# Fetch device stats push it to minio
#
#    Copyright (C) 2022  Gerard Wagener
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

import os
import syslog
#https://pypi.org/project/rt/https://pypi.org/project/rt/
import rt
import config
import pprint
import codecs
import sys
import datetime
from minio import Minio
from minio.error import S3Error
import urllib.request
from io import BytesIO
import argparse

#As subject lines are not constant. guess date

def extract_date(subject):
    ar = subject.split(' ')
    date = None
    for  item in ar:
        try:
            do = datetime.datetime.strptime(item, '%Y-%m-%d')
            date =  str(do.year).zfill(4)  + "-"  + (str(do.month).zfill(2)) + "-"  + (str(do.day).zfill(2))
        except ValueError as v:
            pass
    return date


# Day is the day where the ticket is arrived that is not nessarily the day
# inside the data

def extract_iot_exposed_link(tracker, day):
    link = None
    date = None
    tracker.default_queue = "VARIOT"
    #sets default queue to general
    # FIXME timestamps in csv files are not the same than the reception of the ticket
    for t in tracker.search(raw_query="Created = '" + day+"'"):
    #Does not work on queue VARIOT
        s=t['Subject'].upper()
        date = extract_date(t['Subject'])
        if s.find("IOT EXPOSED STATS") > 0:
            #URL is sometimes split over multiple bytes objects returned by obj'[Content]
            buf = []
            for (aid, filename, atype, size)  in tracker.get_attachments(t['numerical_id']):
                obj = tracker.get_attachment(t['numerical_id'], aid)
                #Shadowserver URLs might include non ascii
                buf.append(codecs.decode(obj['Content'], "UTF-8"))
            text = ''.join(buf)
            te = text.split('\n')
            #Email sometimes has multiple links keep the last one
            for e in te:
                if e.find("https") >= 0:
                    if (e.find("dl.shadowserver.org") > 0):
                        l=e.strip()
                        if link is not None and l  != link:
                            syslog.syslog ("Error got different links do not know which one is the right one in ticket " + str(t['numerical_id']))
                            return (None, None)
                        else:
                            link = l
    return (date, link)

def check_exposed_csv(data):
    #TODO check timestamps in CSV if they correspond to the right date
    eline = '"vendor","model","type","geo","country","count","possible_vulnerability"'
    lines = data.split('\n')
    if lines[0] != eline:
        syslog.syslog ("Got an unexpected csv content. do not publish. Got fields: "+lines[0])
        return False
    return True

def is_csv_in_bucket(minio, objectname,force):
    if force == True:
        return False
    try:
        result = client.stat_object(config.minio_bucket, objectname)
        return True
    except S3Error as a:
        return False
    #No exception, ass
    return False

#exposed=True or False to know what should be checked
def download_url_push_minio(minio, date, link,exposed,force):
    if date is None or link is None:
        syslog.syslog("Cannot download IoT stats as no link or no date is available")
        #Do not want to break the loop
        return False
    response = urllib.request.urlopen(link)
    l = response.length
    data = response.read()
    text = data.decode('utf-8')
    stream = BytesIO(data)
    if exposed:
        if check_exposed_csv(text) == False:
            return False
    target =  date+"-iot-exposed-device-stats.csv"
    #Do not put object over and over in bucket due to versioning and replication
    if is_csv_in_bucket(minio, target, force):
        syslog.syslog("Object " + target + " is already in bucket do not overwrite")
        return False

    minio.put_object(config.minio_bucket, target, stream,l)
    syslog.syslog("Created on minio:" + target)

tracker = rt.Rt(config.rt_url, config.rt_user, config.rt_pass, verify_cert=False)
tracker.login()


client = Minio(config.minio_url, access_key=config.minio_access_key,
               secret_key=config.minio_secret_key,secure=False)

parser = argparse.ArgumentParser(description='Fetch ticket from iot-exposed-device stats from VARIOT queue and push them to minio')
parser.add_argument("-d", "--date", required=False, help="Date tickets were received.")
parser.add_argument("-s", "--start", required=False, help="Start date to process tickets")
parser.add_argument("-e", "--end", required=False, help="End date to process tickets")
parser.add_argument("-f", "--force", required=False, action='store_true', help="Force overwrite")
parser.add_argument("-l","--last", action='store_true', required=False, help="Put the last 3 days in minio as reports arrive sometimes late")

args = parser.parse_args()

if args.date:
    #test date format
    do = datetime.datetime.strptime(args.date, '%Y-%m-%d')
    date =  str(do.year).zfill(4)  + "-"  + (str(do.month).zfill(2)) + "-"  + (str(do.day).zfill(2))
    (date, link) = extract_iot_exposed_link(tracker, date)
    download_url_push_minio(client, date, link, exposed=True, force=args.force)


if args.start:
    if args.end is None:
        print ("Error. An end date must be specified")
        sys.exit(1)
    #Check start date
    so  = datetime.datetime.strptime(args.start, "%Y-%m-%d")
    eo  = datetime.datetime.strptime(args.end, "%Y-%m-%d")

    day_delta = datetime.timedelta(days=1)

    start_date = so.date()
    end_date = eo.date()
    for i in range((end_date - start_date).days+1):
        o = start_date  + i*day_delta
        date =  str(o.year).zfill(4)  + "-"  + (str(o.month).zfill(2)) + "-"  + (str(o.day).zfill(2))
        #Date in data (ddate) might be older than date
        (ddate, link) = extract_iot_exposed_link(tracker, date)
        download_url_push_minio(client, ddate, link, exposed=True, force=args.force)

if args.last:
    day_delta = datetime.timedelta(days=1)
    start_date=datetime.date.today()
    for i in range(0,3):
        o = start_date - i*day_delta
        date =  str(o.year).zfill(4)  + "-"  + (str(o.month).zfill(2)) + "-"  + (str(o.day).zfill(2))
        #Date in data (ddate) might be older than date
        (ddate, link) = extract_iot_exposed_link(tracker, date)
        download_url_push_minio(client, ddate, link, exposed=True, force=args.force)

tracker.logout()
