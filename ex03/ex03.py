from socket import *
import time

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(("<broadcast>", 20022))

def recieve():
	while True:
		data, addr = sock.recvfrom(1024)
		print "received:", data, "from", addr

def send():
	s = socket(AF_INET, SOCK_DGRAM)
	s.bind(('', 0))
	s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
	while True:
		data = repr(time.time()) + '\n'
		s.sendto(data, ('<broadcast>', 20022))
		time.sleep(2)

def main():
	#recieve()
	send()

main()
