#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TCPClientConnection.h"

using boost::asio::ip::tcp;


class TCPClient
{
    public:

        TCPClient( boost::asio::io_service& io_service );

        virtual ~TCPClient();

    private:

        void start_accept();
        void handle_accept( TCPClientConnection::pointer new_connection,
                            const boost::system::error_code& error);

        tcp::acceptor acceptor_;
};

#endif // TCPClIENT_H
