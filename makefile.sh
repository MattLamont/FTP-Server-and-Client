g++ -I /usr/include server.cpp TCPServer.h TCPServer.cpp TCPConnection.h TCPConnection.cpp -o server -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so;

g++ -I /usr/include client.cpp -o client -lboost_system -pthread -L /usr/lib64 -L /usr/lib/libboost_thread-mt.so
