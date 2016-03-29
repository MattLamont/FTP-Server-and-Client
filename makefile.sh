g++ -I /usr/include server.cpp TCPServer.h TCPServer.cpp TCPConnection.h TCPConnection.cpp -o server -lboost_system -lboost_thread -pthread;

g++ -I /usr/include client.cpp -o client -lboost_system -lboost_thread -pthread
