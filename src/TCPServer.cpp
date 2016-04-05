#include "TCPServer.h"

TCPServer::TCPServer( boost::asio::io_service& io_service ) : acceptor_(io_service , tcp::endpoint(tcp::v4() , 3030 ))
{
    this->start_accept();
}

TCPServer::~TCPServer()
{
    //dtor
}

void TCPServer::start_accept()
{
    TCPServerConnection::pointer new_connection = TCPServerConnection::create(acceptor_.get_io_service());

    acceptor_.async_accept( new_connection->socket(),
                            boost::bind(&TCPServer::handle_accept, this, new_connection,
                            boost::asio::placeholders::error));
}

void TCPServer::handle_accept( TCPServerConnection::pointer new_connection ,
                                const boost::system::error_code& error )
{
    if (!error)
    {
        new_connection->start();
        start_accept();
    }
}
