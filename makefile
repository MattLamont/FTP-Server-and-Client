all: client server

client: src/client.cpp
	g++ -I /usr/include src/client.cpp include/CommandExecutor.h -o client -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so

server: src/server.cpp
	g++ -I /usr/include src/server.cpp include/CommandExecutor.h include/TCPServer.h src/TCPServer.cpp include/TCPServerConnection.h src/TCPServerConnection.cpp -o server -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so;


