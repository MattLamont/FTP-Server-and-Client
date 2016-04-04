# FTP-Server-and-Client

###Compilation Instructions
1. at the command line type in `./makefile.sh`
2. An executable called "client" will be created in this directory.
3. Another executable called "server" will be created in this directory.
4. Start the server first by typing `./server`
5. Start one or more clients to contact the server by typing `./client [hostname]` where hostname is the name of the machine
   that the server is running on.

###Supported Server Commands
1. catalog
2. spwd
3. upload <source-file> <destination-file>
4. download <source-file> <destination-file>
5. bye

###Supported Client Commands
1. ls
2. pwd
3. upload <source-file> <destination-file>
4. download <source-file> <destination-file>
5. bye
6. All other commands will be routed directly to the local shell program
