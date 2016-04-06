#ifndef TCPSERVERCONNECTION_H
#define TCPSERVERCONNECTION_H

#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>



using boost::asio::ip::tcp;
using namespace boost;


class TCPServerConnection : public boost::enable_shared_from_this<TCPServerConnection>
{
    public:

      typedef boost::shared_ptr<TCPServerConnection> pointer;

      static pointer create(boost::asio::io_service& io_service)
      {
        return pointer(new TCPServerConnection(io_service));
      }

      tcp::socket& socket()
      {
        return socket_;
      }

      void start();

    private:

      TCPServerConnection(boost::asio::io_service& io_service)
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


#endif // TCPSERVERCONNECTION_H
