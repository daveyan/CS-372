#!/usr/bin/python

#Yanda project2

import socket
import sys
import os.path

def userRequest(connection_socket, data_port, server_host, server_port, user_command, file_name):
	if user_command == '-l':
		#send the user's command and recieve response from server
		data = connection_socket.send(user_command.encode())
	
		data = connection_socket.recv(1024)
		
	elif user_command == '-g':
		#send the user's command and receive response from server
		data =  connection_socket.send(user_command.encode())

		data = connection_socket.recv(1024)
		#send the request file name and recieve response from server
		data = connection_socket.send(file_name.encode())

		data = connection_socket.recv(1024)
	else:
		#not -l nor -g
		print("User Command is invalid")
		connection_socket.close()			###
		sys.exit(1)

	return

#check parameters from command line
if (len(sys.argv) < 5) or (len(sys.argv) > 6):
	print("Usage: ftclient.py [Server_host][Server_Port][Command][Filename][Data_Port]")
	sys.exit(1)
#user is requesting directory
if len(sys.argv) == 5:

	#REFERENCE: https://docs.python.org/2/library/socket.html
	#documentation for using gethostbyname
	hostname = socket.gethostbyname(sys.argv[1])
	hostname.strip()

	sPort = sys.argv[2]
	serverport = int(sPort)

	usercommand = sys.argv[3]
	usercommand.strip()

	dPort = sys.argv[4]
	dataport = int(dPort)

	filename = "null"

#user is requesting file
else:
	hostname = socket.gethostbyname(sys.argv[1].strip())

	sPort = sys.argv[2]
	serverport = int(sPort)

	usercommand = sys.argv[3].strip()

	filename = sys.argv[4].strip()
	
	dPort = sys.argv[5]
	dataport = int(dPort)

clientSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

clientSocket.connect(('',serverport))

dataSocket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

#setting socket option so that we can reuse a local address
#Reference: https://docs.python.org/2/library/socket.html
# and http://www.binarytides.com/python-socket-server-code-example/
dataSocket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
dataSocket.bind(('',dataport))
dataSocket.listen(5)

data = clientSocket.send(dPort.encode())

data = clientSocket.recv(1024)

userRequest(clientSocket,dataport,hostname,serverport,usercommand,filename)

connect, address = dataSocket.accept()

if usercommand == '-l':
	print("Requesting directory from server")
	while(True):
		data = connect.recv(1024)

		#everything has been read
		if len(data) == 0: 			
			break
		print("%s" % (data.decode()))

if usercommand == '-g':
	print("Requesting file from server")

	#checking file name with the directory
	data = connect.recv(1024)
	connect.send(data)
	data.rstrip()

	#removing all hidden characters
	filename_return = repr(data.decode().rstrip('\x00'))

	#file not found
	if filename_return == "'MISSING'":
		print("File not found.\nQutting client")
		sys.exit(1)

	#file found
	server_file = open(filename,'w')
	data = connect.recv(1024)

	while(data):
		server_file.write(data.decode())
		data = connect.recv(1024)
	#closing file
	server_file.close()
	print("File transfer commplete")




print("Closing connection")
clientSocket.close()
