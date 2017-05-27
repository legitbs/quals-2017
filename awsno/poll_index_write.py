import sys
import socket


def ru( s, u):
  z = ''

  while z.endswith(u) == False:
    z += s.recv(1)

  return z

s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 9345))

s.send('get /index_write http/1.1\r\n\r\n')

st = "hello world"

for i in range(len(st) ):
  print ru(s, ': ')
  s.send(str(i+1) + '\n')
  print ru(s, ': ')
  s.send( st[i] + '\n')
  print ru(s, '\n')

print ru(s, ': ')
s.send('0\n')

print ru(s, '/html>')

s.close()
