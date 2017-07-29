#!/usr/bin/python

import sys
import socket


#make sure we have a port number
if len(sys.argv) !=2:
	print("Usage: ./chatserve.py [port number] ")
	sys.exit(1)

#empty server handle string
server_handle = ''
while(len(server_handle) > 10 or len(server_handle) == 0):
	server_handle = input("Enter a server handle (10 characters or less)\n")


def sendMessages(connectionSocket):
	while True:
		server_input = input(server_handle + "> ")
		server_message = server_handle+"> " + input +'\0'
		connectionSocket.send(server_message)



def receiveMessages(connectionSocket):
	while True:
		client_message = connectionSocket.recv(512)
		return client_message.decode()


port = int(sys.argv[1])
server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
server_socket.bind(('',port))
server_socket.listen(1)

print('The server is now listening for a connection')

while True:
	connectionSocket, address = server_socket.accept()
	print("connection made from address {}".format(address))

	client_handle = receiveMessages(connectionSocket)

	connectionSocket.send(server_handle.encode())

	while True:
		receieved_message = connectionSocket.recv(512)
		if receieved_message:
			print(client_handle+"> "+receieved_message.decode())

		send_message = input(server_handle + "> ")
		if send_message == "\quit":
			print("Quitting chat")
			break
		else:
			sendMessages(connectionSocket)
	connectionSocket.close()
	print("Connection is now closed")

