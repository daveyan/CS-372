#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

//Yanda project2

//reading the command and the file name
//return value indicates success
int userRequest(int socket, char* command, char*filename){
	int data = read(socket,command,512);
	if ((strcmp(command,"-l") == 0)){
		write(socket,"good",5);
		return 1;
	}
	else if ((strcmp(command,"-g") == 0)){
		write(socket,"good",5);
		//we also need the filename
		read(socket, filename,512);
		write(socket,"good",5);
		return 1;
	}
	else{
		write(socket,"error",6);
		printf("Error with user command\n");
		return 0;

	}


}

void getData(int socket, char*command, char*filename,char*hostname, int portNum){
	//REFERENCE: Vinod K - https://stackoverflow.com/questions/3554120/open-directory-using-c 
	//char array to store directory
	char currentDir[1024] = "";
	DIR *dir;
	struct dirent *entry;
	char nline[2] = "\n";

	//user requested the directory
	if((strcmp(command,"-l") == 0)){
		char cwd[1024];
		getcwd(cwd,sizeof(cwd));
		if((dir = opendir(cwd)) != NULL){
			while ((entry = readdir (dir)) != NULL) {
                strcat(currentDir, entry->d_name); 
                strcat(currentDir, nline); 
            }

            closedir (dir);
            write(socket,currentDir,strlen(currentDir));
		}
	}
	//user is requesting a file
	if((strcmp(command,"-g") == 0)){
		FILE *fileDesc;
		fileDesc = fopen(filename,"r");
		
		if(fileDesc == 0){
			//send missing response to indicate missing file
			write(socket,"MISSING",15);
			read(socket,"error",6);
		}
		else{
			char response[1024];
            write(socket,"Sending File",13);

            read(socket,response,1024);
            
            printf("Sending file to client");
            
            while(1){ 
                unsigned char buff[1024]={0};

                int data = fread(buff,1,1024,fileDesc);      
                if(data > 0){
                        write(socket, buff, data); 
                }
                if (data < 1024){
                	//check if at the end of the file
                    if (feof(fileDesc)){
                    	//done with the writing
                        break; 
                    }
                }
            }

		}

	}
	close(socket);
}


int main(int argc, char*argv[]){

	struct hostent *server;

	if (argc != 2){
		printf("Usage: ./ftserver [Port Number]");
		exit(1);
	}

	int run = 1;

	while(run){

	int portNum;

	portNum = atoi(argv[1]);

	
	
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0){
		printf("Error with socket");
		exit(1);
	}
	//REFERENCE: http://www.binarytides.com/socket-programming-c-linux-tutorial/
	struct sockaddr_in server_address, client_address, data_address;

	memset(&server_address,0,sizeof(struct sockaddr_in));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	//convert hostbyte to network order
	server_address.sin_port = htons(portNum); 

	//https://linux.die.net/man/3/setsockopt
	//user with bind to allow resuse of local addresss
	int optval = 1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval, sizeof(optval)) == -1){
		printf("Error with socket options");
		exit(1);
	}

	if(bind(sockfd,(struct sockaddr*)&server_address, sizeof(struct addrinfo))){
		printf("Error with bind");
		exit(1);
	}

	listen(sockfd,10);

	printf("Server is now running\n");
	//bool to control loops
		socklen_t clilen;
		clilen = sizeof(struct sockaddr_in);
		//REFERENCE: http://beej.us/guide/bgnet/output/html/multipage/acceptman.html
		int newsockfd = accept(sockfd,(struct sockaddr *)&client_address,&clilen);
		if (newsockfd < 0){
			printf("Error with accept");
			close(newsockfd);
			close(sockfd);
			exit(1);
		}

		//no errors means that the client has been connected
		printf("Client has been connected\n");

		//string to store variables
		char userhostname[512] = "";
		char usercommand[512] = "";
		char userfilename[512] = "";
		char userdataport[512] = "";
		
		//read data port number
		read(newsockfd,userdataport,sizeof(userdataport));
		write(newsockfd,"good",5);

		printf("Ready to transfer\n");

		int result;
		result = userRequest(newsockfd,usercommand,userfilename);

		if(result == 1){
			int datasocketport = atoi(userdataport);

			int datafd = socket(AF_INET,SOCK_STREAM,0);


			//get hostname
			gethostname(userhostname,512);
			//check hostname
			server = gethostbyname(userhostname);

			if(server == NULL){
				printf("Error with host name");
				exit(1);
			}

			memset(&data_address,0,sizeof(data_address));
			data_address.sin_family = AF_INET;
			//REFERENCE: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
			bcopy((char *)server->h_addr, 
        		(char *)&data_address.sin_addr.s_addr,
         		server->h_length);
    		data_address.sin_port = htons(datasocketport);


    		if (connect(datafd,(struct sockaddr *) &data_address,sizeof(data_address)) < 0) { 
    				printf("ERR");
	                close(datafd);
	                close(newsockfd);
	                close(sockfd);
	                exit(1);
	        }
	        printf("Getting data\n");
	        getData(datafd,usercommand,userfilename,userhostname,datasocketport);
	        close(datafd);  
            close(sockfd);

		}
		else{
			close(sockfd);
			close(newsockfd);
		}


	}


}