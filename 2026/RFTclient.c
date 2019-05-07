#include "RFTheader.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h> 
#include <sys/select.h> 

/* error message format for fatalerror - Source Nick Cook*/
static const char *ERR_FMT = "%s:%d - %s, errno %d: %s\n";

/**************************************************************************
**************************************************************************
 All functions' signature
**************************************************************************
**************************************************************************/
int checksum(char *content, int len);
FILE * check_file(char *fileName);
int file_size(FILE *file);
void fatalerror(int line, char *msg);
bool isCorrupted(float prob);

struct sockaddr_in create_server_data(int sockfd , char* host, int port);
void send_meta_data(FILE *file,char *output, int sockfd, struct sockaddr_in server);
void send_file_normal(FILE *fp,int sockfd, struct sockaddr_in server);
void send_file_with_timeout(FILE *fp,int sockfd, struct sockaddr_in server, float prob_loss);

/**************************************************************************
**************************************************************************
 Functions that you need to implement
**************************************************************************
**************************************************************************/
/* arguments: socket id, host name, port number 
   return: The internet socket address of the server */ 
struct sockaddr_in create_server_data(int sockfd , char* host, int port)
{  
    struct sockaddr_in server; 
	
    server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(host);
	server.sin_port = htons(port);
	
	return server;
} 
  
/* arguments: input file, the name of the output file, socket id, 
              port number, structure containing the server internet address */ 
void send_meta_data(FILE *file,char *output, int sockfd, struct sockaddr_in server)
{ 
	meta_data metadata;
	strcpy(metadata.name, output);
	metadata.size = file_size(file);
	
	int val;
	if ((val=sendto(sockfd, &metadata, sizeof(metadata), 0,(struct sockaddr*) &server, sizeof(server))<0))
					    perror("SENDER: Error sending message\n");
} 
  
/* arguments: input file, socket id, internet socket address of the receiver  */
void send_file_normal(FILE *fp,int sockfd, struct sockaddr_in server)
{
	segment seg, ack;
	int MAXSIZE = TOTALCHAR;
	int nopackets = (file_size(fp)/MAXSIZE);
	int finalsize = file_size(fp) % MAXSIZE;	
	int i,val, packetnum = 0, serversize = sizeof(server);
	char line [MAXSIZE];
	
	fseek(fp, 0L, SEEK_SET); // set file to start for read
	
	for(i = 0; i < nopackets; i++) {
		printf("------------------------------------------------------------------\n");
		seg.size = MAXSIZE;               			//size of the payload
		seg.sq = packetnum;            				//sequence number
		packetnum = (packetnum + 1) % 2;			// switch seq num between 0 and 1
		seg.type = TYPE_DATA;        				//segment type
		
		fread(line, MAXSIZE, 1, fp);			//get data in
		strcpy(seg.payload, line);					//copy to payload
		
		seg.checksum = checksum(seg.payload, MAXSIZE);  //checksum
		
		printf("SENDER: Sending segment: (sq:%d, size:%d, checksum:%d, content:'%s')\n", seg.sq, seg.size, seg.checksum, seg.payload);
		if ((val=sendto(sockfd, &seg, sizeof(seg), 0, (struct sockaddr*) &server, serversize)<0))
					    perror("SENDER: Error sending message\n");
					
		printf("SENDER: Waiting for an ack\n");
		
		
		do {
			val = recvfrom(sockfd, &ack, sizeof(ack),0, (struct sockaddr *)&server, &serversize);
		} while(val != 0); // wait for ack
		
		if(ack.sq == seg.sq)
			printf("SENDER: Ack sq = %d RECIEVED.\n", ack.sq);
		else perror("invalid ack recived");
	}
	
	if(finalsize > 0) { // extra packet needed 
		printf("------------------------------------------------------------------\n");
		seg.size = finalsize;          					//size of the payload
		seg.sq = packetnum;                 			//sequence number
		seg.type = TYPE_DATA;     					   	//segment type
		
		fread(line, MAXSIZE, 1, fp);				//get data in
		line[finalsize] = '\0';							// delimiter character
		strcpy(seg.payload, line);						// copy to payload
		
		seg.checksum = checksum(seg.payload, finalsize);  //checksum
		
		printf("SENDER: Sending segment: (sq:%d, size:%d, checksum:%d, content:'%s')\n", seg.sq, seg.size, seg.checksum, seg.payload);
		if ((val=sendto(sockfd, &seg, sizeof(seg), 0, (struct sockaddr*) &server, sizeof(server))<0))
					    perror("SENDER: Error sending message\n");	
					
		printf("SENDER: Waiting for an ack\n");
		
		do {
			val = recvfrom(sockfd, &ack, sizeof(ack),0, (struct sockaddr *)&server, &serversize); 
		} while(val != 0); // wait for ack
		
		if(ack.sq == seg.sq)
			printf("SENDER: Ack sq = %d RECIEVED.\n", ack.sq);
		else perror("invalid ack recived");
	}
		
	printf("------------------------------------------------------------------\n");	
}


 /* arguments: input file, socket id, internet socket address of the receiver  */
void send_file_with_timeout(FILE *fp,int sockfd, struct sockaddr_in server, float prob_loss)
{ 
    segment seg, ack;
	int MAXSIZE = TOTALCHAR;
	int nopackets = (file_size(fp)/MAXSIZE);
	int finalsize = file_size(fp) % MAXSIZE;	
	int i,val, packetnum = 0, serversize = sizeof(server), sentcount;
	char line [MAXSIZE];
	bool packsent = false;
	fd_set rset; 	
	
	struct timeval tv;
	
	fseek(fp, 0L, SEEK_SET); // set file to start for read
	
	// bind listener to server with backlog of 10
	bind(sockfd, (struct sockaddr*)&server, sizeof(server)); 
    listen(sockfd, 10); 
	
	FD_SET(sockfd, &rset);
	
	
	for(i = 0; i < nopackets; i++) {
		printf("------------------------------------------------------------------\n");
		packsent = false; // reset variables
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		sentcount = 0;
	
		seg.size = MAXSIZE;               			//size of the payload
		seg.sq = packetnum;            				//sequence number
		packetnum = (packetnum + 1) % 2;			// switch seq num between 0 and 1
		seg.type = TYPE_DATA;        				//segment type
		
		fread(line, MAXSIZE, 1, fp);			//get data in
		strcpy(seg.payload, line);					//copy to payload
		
		while(packsent == false && sentcount <= 3){
			sentcount ++;
			if(isCorrupted(prob_loss)){ // if isCorrupted is true checksum is corrupted and a message is printed
				seg.checksum =  0;
				printf(">>>>>>>>Network: segment checksum has been corrupted<<<<<<<<\n");
			}else {
				seg.checksum = checksum(seg.payload, MAXSIZE);  // else normal checksum
			}
			
			printf("SENDER: Sending segment: (sq:%d, size:%d, checksum:%d, content:'%s')\n", seg.sq, seg.size, seg.checksum, seg.payload);
			
			if ((val=sendto(sockfd, &seg, sizeof(seg), 0, (struct sockaddr*) &server, sizeof(server))>=0)) {
				printf("SENDER: Waiting for an ack\n");
			
				val = select(sockfd + 1, &rset, NULL, NULL, &tv); // block while waiting
			}
			if(val == -1) {
				perror("SENDER: error occured with sendto or select");
			}else if (val > 0) {
				packsent = true;
				printf("SENDER: Ack sq = %d RECIEVED.\n", seg.sq);
			}else{
				printf("*****************************");
				printf("SENDER: TIME OUT: Re-sending the same segment\n");
			}
		}
		if(sentcount == 3){
			printf("SENDER EXITING");
			exit(0);
		}
	}
	
	if(finalsize > 0) { // extra packet needed 
		printf("------------------------------------------------------------------\n");
		packsent = false;
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		sentcount = 0;
		
		seg.size = finalsize;          					//size of the payload
		seg.sq = packetnum;                 			//sequence number
		seg.type = TYPE_DATA;     					   	//segment type
		
		fread(line, MAXSIZE, 1, fp);				//get data in
		line[finalsize] = '\0';							// delimiter character
		strcpy(seg.payload, line);						// copy to payload
		
		while(packsent == false && sentcount <= 3){
			sentcount ++;
			
			if(isCorrupted(prob_loss)){ // if isCorrupted is true checksum is corrupted and a message is printed
				seg.checksum =  0;
				printf(">>>>>>>>Network: segment checksum has been corrupted<<<<<<<<\n");
			}else {
				seg.checksum = checksum(seg.payload, finalsize);  // else normal checksum set
			}
			
			printf("SENDER: Sending segment: (sq:%d, size:%d, checksum:%d, content:'%s')\n", seg.sq, seg.size, seg.checksum, seg.payload);
			 
			if ((val=sendto(sockfd, &seg, sizeof(seg), 0, (struct sockaddr*) &server, sizeof(server)) >= 0)) {			
				printf("SENDER: Waiting for an ack\n");
				
				val = select(sockfd + 1, &rset, NULL, NULL, &tv);
			}
			
			if(val == -1) {
				perror("select()");
			}else if (val > 0) {
				packsent = true;
				printf("SENDER: Ack sq = %d RECIEVED.\n", seg.sq);
			}else{
				printf("*****************************");
				printf("SENDER: TIME OUT: Re-sending the same segment\n");
			}
		}
		if(sentcount == 3){
			printf("SENDER EXITING");
			exit(0);
		}
	}
	printf("------------------------------------------------------------------\n");	 
}

/**************************************************************************
**************************************************************************
                  The main function  
**************************************************************************
**************************************************************************/
int main(int argc,char *argv[])
{
	int sockfd ;
	FILE *file;
    struct sockaddr_in server;
	
	/* 	accept input from console in the form of 
	./client inputFile outputFile localhost portNumber 	*/
	if (argc != 5) {
		fprintf(stderr, "Usage: <inputFile> <outputFile> <localhost> <portNumber> \n");
		exit(1);
	}
		
	/* check the file actually exist	*/
	file=check_file(argv[1]);
	printf ("----------------------------------------------------\n");
	printf ("SENDER: File (%s) exists with a size of (%d) bytes\n",argv[1], file_size(file));
	printf ("----------------------------------------------------\n");
	printf ("----------------------------------------------------\n");

	/* create a socket */
	sockfd =socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd <0)
	{
		perror("SENDER: Failed to open socket"); 
		close (sockfd );
		exit(1);
	}
	printf ("SENDER: Socket is created\n");

	/* create a connection */
	server = create_server_data(sockfd, argv[3], atoi(argv[4]));
 	printf ("SENDER: Server data is prepared\n");
 	
	/* Send meta data */
	send_meta_data(file,argv[2],sockfd,server);
	
	/* transmission options */
	int choice=0;
    float loss=0;
	printf ("----------------------------------------------------\n");
	printf ("----------------------------------------------------\n");
	printf("Choose one of the following options (1 or 2):\n ");
	printf("1. Normal transmission (no data segment is lost) \n ");
	printf("2: Transmission with time-out capabilities\n ");
    scanf("%d", &choice);
    switch(choice)
    {
        case 1:
            send_file_normal(file,sockfd, server);
            break;
        
        case 2:
			printf("Enter the probability of a corrupted checksum (between 0 and 1):\n ");
			scanf("%f", &loss);
			send_file_with_timeout(file,sockfd, server, loss);
            break;

         default:
            printf("Error! enter 1 or 2 \n");
    } 

	printf("SENDER: File is sent\n");
	
	/* Close the file */
	fclose(file);
	
	/* Close the socket */
	close (sockfd );
	
 	return 0;
} 

/*************************************************************************
**************************************************************************
 These functions are implemented for you .. Do NOT Change them
**************************************************************************
**************************************************************************/


/* calculate the segment checksum by adding the payload */
int checksum(char *content, int len)
{
	int i;
	int sum = 0;
	for (i = 0; i < len; i++)
		sum += (int)(*content++);
	return sum;
}

/* check if the input file does exist */
FILE * check_file(char *fileName)
{
	FILE *file = fopen(fileName, "rb");
	if(!file) {
		perror("SENDER: File does not exists");
 		fclose(file);
		exit(1);
	}
	return file;
}

/* return file size */
int file_size(FILE *file)
{
	fseek(file, 0L, SEEK_END);
	int size = ftell(file);
	return size;
}

void fatalerror(int line, char *msg) {
    printf("Line %d : %s\n", line, msg);
    exit(0);
}

/* decide whether the segment is corrupted or not given a certain probability */
bool isCorrupted(float prob)
{
    srand(time(NULL)); 
    if(((float)(rand())/(float)(RAND_MAX)) < prob)
	{
	return true;
	}
    else return false;
}
/*************************************************************************** 
 *************************************************************************** 
 *************************************************************************** 
 ***************************************************************************/

