from socket import *
import time

s = socket(AF_INET, SOCK_DGRAM)
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
i = 1
while True:
	data = str(i)
	if i % 3 == 0:
		data += ' indianere'
	else:
		data += ' og'
	i += 1
	for j in range(10000,50000):
		s.sendto(data, ('<broadcast>', j))
	print 'sendt:', data
	#data, addr = s.recvfrom(1024)
	#print 'recieved:', data, 'from', addr
	time.sleep(1)
