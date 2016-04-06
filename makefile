all: client server

client: client.cpp
	g++ -I /usr/include client.cpp CommandExecutor.h -o client -lboost_system -lboost_filesystem -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so

server: server.cpp
	g++ -I /usr/include server.cpp CommandExecutor.h TCPServer.h TCPServer.cpp TCPServerConnection.h TCPServerConnection.cpp -o server -lboost_system -lboost_filesystem -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so;


