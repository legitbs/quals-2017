#!/usr/bin/python
import sys
import argparse
import socket
import select
import string
import io
import struct

def is_ip_addr(string):
        try:
                socket.inet_aton(string)
        except socket.error:
                raise argparse.ArgumentTypeError("%s is not a valid IP address" % string)
        return string



def send_exec( fileName, ip_addr, port ):
	try:
		curFile = io.FileIO( fileName )

        	fileData = curFile.readall()

        	curFile.close()

        except IOError, e:
                print('Error opening local file "%s"' % fileName )
                exit(1)

	try:
		s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

		s.connect( (ip_addr, port) )

		packed_size = struct.pack("i", len(fileData))
	
		s.send( packed_size )
		s.send( fileData )

		s.send( "abcd" )

		recvData = s.recv( 1024 )

		print "Received: %s" % recvData

	except IOError, e:
		print('Error connecting: %s\n' % e)
		exit(1)
		
	if ( recvData == "a" ):
		exit(0)
	else:
		exit(1)
	
	

if __name__ == '__main__':
	parser = argparse.ArgumentParser( description="This script runs SLA checks on a REESES server",
        epilog='''\
        Example:
                sla_echo.py <ip address>''', usage="%(prog)s [-p port] ipaddr",
        formatter_class = argparse.RawDescriptionHelpFormatter)

	parser.add_argument('-p', type=int, dest='remote_port', help="Port of service", default=8080)
        parser.add_argument('ip_addr', type=is_ip_addr, help="IP address")

        if len (sys.argv)<2:
                parser.print_help()
                exit(1)

	args = parser.parse_args()

	send_exec( 'test_echo.meb', args.ip_addr, args.remote_port )
	

