#include "TCPClientConnection.h"




void TCPClientConnection::start()
{
    message_ = make_daytime_string();

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
    boost::bind( &TCPClientConnection::handle_write , shared_from_this() , boost::asio::placeholders::error , boost::asio::placeholders::bytes_transferred));
}

