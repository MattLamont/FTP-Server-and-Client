#include "TCPClient.h"

TCPClient::TCPClient( boost::asio::io_service& io_service ) : acceptor_(io_service , tcp::endpoint(tcp::v4() , 3032 ))
{
    this->start_accept();
}

TCPClient::~TCPClient()
{
    //dtor
}

void TCPClient::start_accept()
{
    TCPClientConnection::pointer new_connection = TCPClientConnection::create(acceptor_.get_io_service());

    acceptor_.async_accept( new_connection->socket(),
                            boost::bind(&TCPClient::handle_accept, this, new_connection,
                            boost::asio::placeholders::error));
}

void TCPClient::handle_accept( TCPClientConnection::pointer new_connection ,
                                const boost::system::error_code& error )
{
    if (!error)
    {
        new_connection->start();
        start_accept();
    }
}
