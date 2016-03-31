#ifndef TCPCLIENTCONNECTION_H
#define TCPCLIENTCONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;


class TCPClientConnection : public boost::enable_shared_from_this<TCPClientConnection>
{
    public:

      typedef boost::shared_ptr<TCPClientConnection> pointer;

      static pointer create(boost::asio::io_service& io_service)
      {
        return pointer(new TCPClientConnection(io_service));
      }

      tcp::socket& socket()
      {
        return socket_;
      }

      void start();

    private:

      TCPClientConnection(boost::asio::io_service& io_service)
        : socket_(io_service)
      {
      }

      void handle_write(const boost::system::error_code& /*error*/,
          size_t /*bytes_transferred*/)
      {
      }

      std::string make_daytime_string()
      {
        std::time_t now = std::time(0);
        return std::ctime(&now);
      }

      tcp::socket socket_;
      std::string message_;
};


#endif // TCPCONNECTION_H
