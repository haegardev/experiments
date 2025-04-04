#!/usr/bin/python3
import argparse
import crypt
import getpass

parser = argparse.ArgumentParser(description='Dispatch users over multiple SSDs')
parser.add_argument("-s", "--ssds", required=True, help="Number of ssds mounted in /ssd0, /ssd1, ...")
parser.add_argument("-u","--users", required=True, help="Number of users to be created")
parser.add_argument("-p","--password",required=True, help="Default password")

args = parser.parse_args()


ssds = int(args.ssds)
users=int(args.users)


# useradd -m -s /bin/bash $valid_user
salt = crypt.mksalt(crypt.METHOD_SHA512)
hashed = crypt.crypt(args.password, salt)

for i in range(users):
    disk=i%ssds
    hd="/ssd"+str(disk) + "/home/user"+str(i)
    print (f"useradd -m -d {hd} -s /bin/bash user{i} -p '{hashed}'")
