#!/usr/bin/python

import sys
import socket


#make sure we have a port number
if len(sys.argv) !=2:
	print("Usage: ./chatserve.py [port number] ")
	sys.exit(1)

#empty server handle string and get handle from server user
server_handle = ''
while(len(server_handle) > 10 or len(server_handle) == 0):
	server_handle = input("Enter a server handle (10 characters or less)\n")


def sendMessages(connectionSocket,message):
	totalsent = 0
	#send over the message one char at a time until we reach the buffer length
	while totalsent < len(message):
		#encode the byte to str
		sent = connectionSocket.send(message[totalsent:].encode())
		if sent == 0:			
			connectionSocket.close()
		totalsent += sent



def receiveMessages(connectionSocket):
	while True:
		#start with an empty string
		client_message = ''
		client_message = connectionSocket.recv(512)
		#return a decoded version
		return client_message.decode()

port = int(sys.argv[1])

#create and bind the socket
server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_socket.bind(('',port))

#listen for new requests
server_socket.listen(1)

print('The server is now listening for a connection')

while True:
	connectionSocket, addr = server_socket.accept()

	#read the client handle from the socket
	client_handle = receiveMessages(connectionSocket)
	print(client_handle +" has connected to the server\n")

	#use for all returning responses
	client_prepend = client_handle + "> "
			
	#send handle to client
	connectionSocket.send(server_handle.encode())
	#use for all server responses
	server_prepend = server_handle + "> "

	while 1:
		#receive message
		message = receiveMessages(connectionSocket)
		if message:
			message = message.rstrip()
			print (client_prepend + message)
		
		#send message
		sentmessage = input(server_prepend)
		if sentmessage == '\quit':
			print ("Quitting chat")
			break
		sendMessages(connectionSocket, sentmessage)
		
		#close connection
	connectionSocket.close()
	print ("Connection is now closed")
	sys.exit(0)

