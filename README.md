# Expenses-Client-Server

Expenses-Client-Server is a simple TCP/IP Client-Server implementation, written in C programming language.

Server can be hosted on a Linux virtual machine and accessed via the public ip of the host machine through port-forwarding. 
Client is a simple Windows console application adapted to communicate with the server via Winsock2 header library.

Multiple clients are able to:
1. send packets containing expenses data (type,value,date) to the server;
2. track their expenses for each month by calculating their total expenses;
3. check which is their biggest expense.

Server uses fork() to create a new process for each accepted client connection.
Server uses a unique text file ({client ip address}.txt) for every client to store consecutive data.
Server stops receiving client requests when it is terminated in the linux terminal.
