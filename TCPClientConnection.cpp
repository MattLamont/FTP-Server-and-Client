#include "TCPClientConnection.h"




void TCPClientConnection::start()
{
    try
    {
        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = socket_.receive(boost::asio::buffer(buf), NULL ,  error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout.write(buf.data(), len);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

