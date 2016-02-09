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
	#for j in range(10000,65535):
	port = 20022
	s.sendto(data, ('129.241.187.20', port))
	print 'sendt:', data
	time.sleep(2)
