#!/usr/bin/env python 
import socket
  
TCP_IP = '129.241.187.23'
TCP_PORT = 33546
BUFFER_SIZE = 1024
MESSAGE = ''

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
while MESSAGE != 'QUIT':
  MESSAGE = raw_input('->')
  s.send(MESSAGE + '\0')
  data, addr = s.recvfrom(BUFFER_SIZE)
  print "received data:", data, addr
  MESSAGE = ''
  data = ''
s.close()
