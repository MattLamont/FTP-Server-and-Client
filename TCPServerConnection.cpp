#include "TCPServerConnection.h"




void TCPServerConnection::start()
{
    message_ = make_daytime_string();

    boost::asio::async_write(socket_, boost::asio::buffer(message_),
    boost::bind( &TCPServerConnection::handle_write , shared_from_this() , boost::asio::placeholders::error , boost::asio::placeholders::bytes_transferred));
}

