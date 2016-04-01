g++ -I /usr/include server.cpp CommandExecutor.h TCPServer.h TCPServer.cpp TCPServerConnection.h TCPServerConnection.cpp -o server -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so;

g++ -I /usr/include client.cpp CommandExecutor.h TCPClientConnection.h TCPClientConnection.cpp TCPClient.h TCPClient.cpp -o client -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so
