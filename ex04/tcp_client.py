import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 20022
BUFFER_SIZE = 1024
MESSAGE = "Hello, World!"

def sendPackage():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((TCP_IP, TCP_PORT))
	s.send(MESSAGE)
	data = s.recv(BUFFER_SIZE)
	s.close()

	print "received data:", data

def menu():
	choice = ''
	while not choice == 0:
		print "1) Send package"
		print "0) Exit"
		choice = input("Choose: ")
		if choice == 1:
			sendPackage()

menu()
