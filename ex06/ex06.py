from sys import *
from socket import *
import os
import time
import subprocess

BUFF = 1024
number = 0;
data = ""
port = 22000

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', port))
s.settimeout(2)
print 'Initializing'
while True:      
    try:
        data, addr = s.recvfrom(BUFF)
    except:
        print ("LOST CONNECTION TO MASTER, BACKUP CREATED!")
        number +=1
        s.close()
        subprocess.call(['gnome-terminal', '-x', 'python' ,'udp.py'])
        s = socket(AF_INET, SOCK_DGRAM)
        s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
        while True:
            data = "Counter " + str(number)
            s.sendto(data, ('10.20.91.255', port))
            print 'SENT TO BACKUP:', data
            time.sleep(0.5)
            number+=1
    if ("Counter" in data):
        number = int(data[8:])
        print 'RECEIVED FROM MASTER: ' + str(number)
    