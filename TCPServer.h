#ifndef TCPSERVER_H
#define TCPSERVER_H


#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TCPConnection.h"

using boost::asio::ip::tcp;


class TCPServer
{
    public:

        TCPServer( boost::asio::io_service& io_service );

        virtual ~TCPServer();

    private:

        void start_accept();
        void handle_accept( TCPConnection::pointer new_connection,
                            const boost::system::error_code& error);

        tcp::acceptor acceptor_;
};

#endif // TCPSERVER_H
