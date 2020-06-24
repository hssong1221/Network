/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
	int portno; // port number
	socklen_t clilen;

	char buffer[1000];

	/*sockaddr_in: Structure Containing an Internet Address*/
	struct sockaddr_in serv_addr, cli_addr;

	int n;
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	/*Create a new socket
	AF_INET: Address Domain is Internet 
	SOCK_STREAM: Socket Type is STREAM Socket */
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]); //atoi converts from String to Integer
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //for the server the IP address is always the address that the server is running on
	serv_addr.sin_port = htons(portno); //convert from host to network byte order

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
	    error("ERROR on binding");

	listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5

	
	/*accept function: 
	1) Block until a new connection is established
	2) the new socket descriptor will be used for subsequent communication with the newly connected client.
	*/
	
	char *header1; 
	header1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";			//response header(html)
	char *header2; 
	header2 = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";		//response header(jpeg)
	char *header3; 
	header3 = "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n";			//response header(gif)
	char *header4; 
	header4 = "HTTP/1.1 200 OK\r\nContent-Type: audio/mpeg\r\n\r\n";		//response header(mp3)
	char *header5; 
	header5 = "HTTP/1.1 200 OK\r\nContent-Type: application/pdf\r\n\r\n";	//response header(pdf)


	while(1)
	{
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");

		bzero(buffer,1000);
		n = read(newsockfd,buffer,1000); //Read is a block function. It will read at most 255 bytes
		if (n < 0) 
			error("ERROR reading from socket");
		printf("Here is the message: \n%s\n",buffer);

		char *ptr1;							//where to store separated tokens.(file name)
		ptr1 = strtok(buffer, "/");			//string tokenize
		ptr1 = strtok(NULL, " ");			//detatch the requested file name from the header.

		if(strstr(ptr1, "html") != NULL)					//identify the requested file.
			send(newsockfd, header1, strlen(header1), 0);	//send a response header that matches the requested header
		else if(strstr(ptr1, "jpeg") != NULL)
			send(newsockfd, header2, strlen(header2), 0);
		else if(strstr(ptr1, "gif") != NULL)
			send(newsockfd, header3, strlen(header3), 0);
		else if(strstr(ptr1, "mp3") != NULL)
			send(newsockfd, header4, strlen(header4), 0);
		else if(strstr(ptr1, "pdf") != NULL)
			send(newsockfd, header5, strlen(header5), 0);

		FILE *fp;										//create file stream
		char *body;										//save the contents of the file
		int length = 0;									//file size
		if((fp = fopen(ptr1, "rb"))!= NULL)				//open the binary file in read mode, 
		{
			fseek(fp, 0, SEEK_END);						//go to the end of the file.
			length = ftell(fp);							//check the size of the file
			fseek(fp, 0, SEEK_SET);						//return to the beginning of the file.
			body = malloc(sizeof(char) * length);		//allocate a space to store the files.
			fread(body, length, 1 , fp);				//read the file.
			send(newsockfd, body, length, 0);			//send the contents of the file
			fclose(fp);									//close file stream.
		}
		free(body);										//free the memory
		close(newsockfd);								//close the socket
	}
	close(sockfd);

	return 0; 
}
