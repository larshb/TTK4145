import socket
  
TCP_IP = '129.241.187.23'
TCP_PORT = 33546
BUFFER_SIZE = 1024
msg = ''

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
while msg != 'QUIT':
  msg = raw_input('Message: ')
  s.send(msg + '\0')
  data, addr = s.recvfrom(BUFFER_SIZE)
  print "\nReceived: ", data
s.close()
