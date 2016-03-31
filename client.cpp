#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "CommandExecutor.h"

using boost::asio::ip::tcp;


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], "3030");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        tcp::socket control_socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;

        std::cout << "Trying to connect to FTP server at " << argv[1] << " ......\n";

        while (error && endpoint_iterator != end)
        {
            control_socket.close();
            control_socket.connect(*endpoint_iterator++, error);
            std::cout << "Connection to FTP server at " << argv[1] << " failed.\n";
        }

        if (error)
        {
            throw boost::system::system_error(error);
        }





        std::cout << "Created control connection to FTP server at " << argv[1] << " on port 3030.\n";



        while( true )
        {
            std::string user_input = "";
            std::string command_output = "";

            std::cout << "\nCLIENT> ";
            getline( std::cin , user_input );
            std::cout << "\n";

            boost::asio::async_write( control_socket , boost::asio::buffer( user_input ), boost::bind( clientSocketWriteHandler , boost::asio::placeholders::error , boost::asio::placeholders::bytes_transferred));
        }


        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = control_socket.read_some(boost::asio::buffer(buf), error);

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

    return 0;
}
