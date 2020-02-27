/*
Computer Networks
Programming Assignment 1 -> Client using customized protocol on top of UDP
protocol for sending information to the server.
Name: KAPIL VARMA
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Primitives
#define PORT 8080
#define STARTPACKETID 0XFFFF
#define ENDPACKETID 0XFFFF
#define CLIENTID 0XFF
#define TIMEOUT 5

//Packet Types
#define DATATYPE 0XFFF1
#define ACKPACKETTYPE 0XFFF2
#define REJECTCODETYPE 0XFFF3

//Reject Sub codes
#define OUTOFSEQUENCECODE 0XFFF4
#define LENGTHMISMATCHCODE 0XFFF5
#define ENDOFPACKETMISSINGCODE 0XFFF6
#define DUPLICATECODE 0XFFF7

//Data Packet Definition
struct DataPacket{
	uint16_t startPacketID;
	uint8_t clientID;
	uint16_t data;
	uint8_t segmentNumber;
	uint8_t length;
	char payload[255];
	uint16_t endPacketID;
};

//Acknowledgement Packet Definition
struct ACKPacket {
	uint16_t startPacketID;
	uint8_t clientID;
	uint16_t ACK;
	uint8_t segmentNumber;
	uint16_t endPacketID;
};

//Structure For Reject Definition
struct RejectPacket {
	uint16_t startPacketID;
	uint8_t clientID;
	uint16_t reject;
	uint16_t subCode;
	uint8_t segmentNumber;
	uint16_t endPacketID;
};

// Function to print the data packet
void show(struct DataPacket dp) {
	printf(" \n ******* SERVER RECIEVING ******* \n");
	printf("PACKET ID : %hx\n", dp.startPacketID);
	printf("CLIENT ID : %hhx\n", dp.clientID);
	printf("DATA : %x\n", dp.data);
	printf("SEGMENT NUMBER -> %d\n", dp.segmentNumber);
	printf("LENGTH : %d\n", dp.length);
	printf("PAYLOAD : %s\n", dp.payload);
	printf("END PACKET ID : %x\n", dp.endPacketID);
}

// Create Acknowledgement Packet
struct ACKPacket createACKPacket(struct DataPacket dp) {
	struct ACKPacket ackp;
	ackp.startPacketID = dp.startPacketID;
	ackp.clientID = dp.clientID;
	ackp.segmentNumber = dp.segmentNumber;
	ackp.ACK = ACKPACKETTYPE ;
	ackp.endPacketID = dp.endPacketID;
	return ackp;
}

// Creating Reject Packet
struct RejectPacket createRejectPacket(struct DataPacket dp) {
	struct RejectPacket rp;
	rp.startPacketID = dp.startPacketID;
	rp.clientID = dp.clientID;
	rp.segmentNumber = dp.segmentNumber;
	rp.reject = REJECTCODETYPE;
	rp.endPacketID = dp.endPacketID;
	return rp;
}

int main(int argc, char**argv)
{
	int sockfd,n; //Socket creation variables
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	struct DataPacket dataPacket;
	struct ACKPacket  ackPacket;
	struct RejectPacket rejectPacket;

	// Storing packet contents
	int packetBuffer[20];
	for (int j = 0; j < 20; j++) {
		packetBuffer[j] = 0;
	}
	//Create and Verify Socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	int nextPacketExpected = 1;
	bzero(&serverAddr, sizeof(serverAddr));
	//Assign IP, PORT
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(PORT);
	//Binding created socket to IP
	bind(sockfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
	addr_size = sizeof serverAddr;
	printf("*******THE SERVER IS INITIALIZED*******\n");

	while (1){
		//Receiving the Packet from client
		n = recvfrom(sockfd,&dataPacket,sizeof(struct DataPacket),0,(struct sockaddr *)&serverStorage, &addr_size);
		printf("**************\n");
		show(dataPacket);
		packetBuffer[dataPacket.segmentNumber]++;

		if (dataPacket.segmentNumber == 11 || dataPacket.segmentNumber == 12) {
			packetBuffer[dataPacket.segmentNumber] = 1;
		}
		int length = strlen(dataPacket.payload);

		//Same segment number packeT recived
		if (packetBuffer[dataPacket.segmentNumber] != 1) {
			rejectPacket = createRejectPacket(dataPacket);
			rejectPacket.subCode = DUPLICATECODE;
			sendto(sockfd,&rejectPacket,sizeof(struct RejectPacket), 0, (struct sockaddr *)&serverStorage,addr_size);
			printf("*******RECIEIVED A DUPLICATE PACKET*******\n\n");
		}
		//packet does not have a end of the packet IDENTIFIER
		else if(dataPacket.endPacketID != ENDPACKETID ) {
			rejectPacket = createRejectPacket(dataPacket);
			rejectPacket.subCode = ENDOFPACKETMISSINGCODE ;
			sendto(sockfd,&rejectPacket,sizeof(struct RejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			printf("*******END OF PACKET IDENTIFIER MISSING*******\n\n");
		}

		//length different then the payload field
		else if(length != dataPacket.length) {
			rejectPacket = createRejectPacket(dataPacket);
			rejectPacket.subCode = LENGTHMISMATCHCODE ;
			sendto(sockfd,&rejectPacket,sizeof(struct RejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			printf("*******LENGTH MISMATCH ERROR*******\n\n");
		}

		//if packet incoming is out of sequence
		else if(dataPacket.segmentNumber != nextPacketExpected && dataPacket.segmentNumber != 11 && dataPacket.segmentNumber != 12) {
			rejectPacket = createRejectPacket(dataPacket);
			rejectPacket.subCode = OUTOFSEQUENCECODE;
			sendto(sockfd,&rejectPacket,sizeof(struct RejectPacket),0,(struct sockaddr *)&serverStorage,addr_size);
			printf("*******PACKET IS OUT OF SEQUENCE*******\n\n");
		}
		else {
			//if 11th then sleep no ack
			if(dataPacket.segmentNumber == 11) {
				sleep(7);
			}
			//send ack
			ackPacket = createACKPacket(dataPacket);
			sendto(sockfd,&ackPacket,sizeof(struct ACKPacket),0,(struct sockaddr *)&serverStorage,addr_size);
		}
		nextPacketExpected++; //next packet count
		printf("\n ---------------------------  NEXT PACKET STARTS HERE ---------------------------  \n");
	}
}
