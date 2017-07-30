#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <fcntl.h> 
#include <unistd.h> 

int main(int argc, char*argv[]){
	//check to see if we have the appropriate arguments
	if (argc !=3){
		printf("Usage: ./chatclient [host name] [port number]");
		exit(1);
	}

	char * hostname;
	char * port;
	//set augments to the correct vars
	hostname = argv[1];
	port = argv[2];

	struct addrinfo hints, *result;
	//reset the hints to 0 following the man pages for get addrinfo
	//http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;

	
	getaddrinfo(hostname,port,&hints,&result);

	int sockfd;
	int socket_connect;
	//socket creation
	sockfd = socket(result->ai_family,result->ai_socktype, result->ai_protocol);
	if(sockfd == -1){
		printf("Error creating socket\n");
		close(sockfd);
		exit(1);
	}
	//socket connection
	socket_connect = connect(sockfd,result->ai_addr,result->ai_addrlen);
	if(socket_connect == -1){
		printf("Error connecting to server\n");
		close(sockfd);
		exit(1);
	}
	printf("Connection Established\n");

	
	char * client_handle;
	int client_length = 0;
	size_t input_length = 11;

	//setting the client handle
	client_handle = malloc(sizeof(char)* 11);
	printf ("Please enter a client handle [10 characters or less]\n");
	
	while(client_length > 10 || client_length == 0){
		client_length = getline(&client_handle,&input_length,stdin);
		//remove the new line character at the end of the getline
		client_handle[strcspn(client_handle, "\n")] = 0;

	}
	
	//send the client handle to the server
	send(sockfd,client_handle,strlen(client_handle),0);

	//recieve the server handle from server
	char* server_handle;
	server_handle = malloc(sizeof(char)*11);
	recv(sockfd,server_handle,input_length,0);

	//printf("%s\n",server_handle);
	fflush(stdout);

	//read and write buffers	
	char read_buff[512];
	char write_buff[512];

	int quit = 1;
	while(quit){
 
        memset(&write_buff, 0, sizeof(write_buff));   
        
        //send response
        fflush(stdout);
        printf("%s> ",client_handle);
        fgets(write_buff, 512, stdin);       
        //check for the quit command
        if (strncmp(write_buff, "\\quit", 5) == 0){
            printf("Quitting chat\n");
            quit = 0;
        }
        //write message to server
        int send_test = 0;
     	send_test = send(sockfd, write_buff, strlen(write_buff), 0);
     	if(send_test == -1){
     		printf("Connection is closed");
     		close(sockfd);
     		exit(0);
     	}
     
        //readint the response from the server.        
        memset(&read_buff, 0, sizeof(read_buff));
        recv(sockfd,read_buff, 512, 0);
        printf("%s> %s\n", server_handle, read_buff);
     
        fflush(stdout);

	}
	//client has enter quit
	//close the connection
	close(sockfd);
	printf("Closing connection to server");
	return 0;
}
