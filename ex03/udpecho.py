from sys import *
from socket import *

BUFF = 1024

if len(argv) == 2: port = int(argv[1])
else: port = input('Please enter port number: ')
s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', port))
print 'UDP: Waiting to recieve...'
while True:
    data, addr = s.recvfrom(BUFF)
    print 'Message: %r\nAddress: %r\n-' % (data, addr)
    s.sendto(data, addr)
