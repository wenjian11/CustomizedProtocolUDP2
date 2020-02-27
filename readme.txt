/*
Computer Networks Programming Assignment 1
Programming Assignment 2: CClient using customised protocol on top of UDP protocol for sending information to the server. Different types of packets with errors and also correct ones are transmitted and the errors are handled.

Note: I ran this program in the Design Center: Linux Lab
Name: KAPIL VARMA
ID: 1483983
*/



The program contains two files one for the server and one for the client. Other than this it contains screenshots for the output of the program and also the input file for the data to be given by the client.

To run the program follow these steps

1.Compile the server so that the client can send its data. Compile Assignment1Server.c file by using the comand “gcc -o Assignment1Server Assignment1Server.c”  (if this command throws error then use option -std=c99 or -std=gnu99 to compile your code)

 
2. The next step is to open another terminal and then compile the client by running the command: “gcc -o Assignment1Client Assignment1Client.c" (if this command throws error then use option -std=c99 or -std=gnu99 to compile your code)

3.Now we need to execute the server first by using the command "./Assingment1Server"

4.Similarly we need to also run the client and begin transmitting packets by using the command "./Assignment1client"

The program has now run successfully.
