#!/usr/bin/python3 -u
import random
from pathlib import Path
from os import environ
from sys import exit
from subprocess import Popen, PIPE
import signal
from base64 import b64decode

def alarm_handler(signum, frame):
    print("timed out, sorry")
    exit(-1)
signal.signal(signal.SIGALRM, alarm_handler)

input_timeout = int(environ.get('INPUT_TIMEOUT', 15))
crash_timeout = int(environ.get('CRASH_TIMEOUT', 5))

chall_paths = [c for c in Path('/app/witchcraft_server').iterdir() if '' == c.suffix]

all_challs = set(chall_paths)

picked = random.sample(all_challs, 10)

print("send your solution as base64, followed by a newline")

for c in picked:
    print(c.name)
    signal.alarm(input_timeout)
    crasher = b64decode(input())
    signal.alarm(0)

    signal.alarm(crash_timeout)
    proc = Popen(str(c),
                 stdin=PIPE, stdout=PIPE, stderr=PIPE)
    (out, err) = proc.communicate(crasher)
    signal.alarm(0)

    if 0 != proc.returncode:
        print("didn't exit happy, sorry")
        exit(-1)

print("The flag is: {}".format(
    environ.get('FLAG', "bustin makes me feel good scengoybEm")))
