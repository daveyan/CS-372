#!/usr/bin/python


import sys
import socket

def receiveMessages(connection_socket):
	print ("Chat session started")
	while True:
		client_message = connection_socket.recv(512)
		if client_message == "/quit":
			print ("Chat session ending")
			return 1

		print (client_message)
		return 0

def sendMessages(connection_socket):
	while True:
		input = raw_input()
		message = server_hostname + input
		if input == "/quit":
			connection_socket.send("/quit")
			return 1

		connection_socket.send(message)
		return 0


if len(argv) < 2:
        print "Usage: ./chatserve [port number]\n"
        exit(1)


#port number from the command line
port = int(sys.argv[1])
server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_socket.bind(('',port))
server_socket.listen(1)

server_hostname = "Server >"

print("Server ready to connect to")

while True:
	connection_socket, address = server_socket.accept()
	
	if receiveMessages(connection_socket) == 1:
		break
	if sendMessages(connection_socket) == 1:
		break

	connection_socket.close()