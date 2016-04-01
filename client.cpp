#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "CommandExecutor.h"

using boost::asio::ip::tcp;

const std::string CONTROL_SOCKET_PORT = "3030";
const std::string UPLOAD_SOCKET_PORT = "3031";
const std::string DOWNLOAD_SOCKET_PORT = "3032";


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }



        while( true )
        {
            std::string user_input = "";
            std::string command_output = "";

            std::cout << "\nCLIENT> ";
            getline( std::cin , user_input );
            std::cout << "\n";

            if( user_input.size() == 0 )
            {
                continue;
            }

            boost::asio::io_service io_service;

            tcp::resolver resolver(io_service);
            tcp::resolver::query query( argv[1], CONTROL_SOCKET_PORT );
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            tcp::resolver::iterator end;

            tcp::socket control_socket(io_service);
            boost::system::error_code error = boost::asio::error::host_not_found;


            while (error && endpoint_iterator != end)
            {
                control_socket.close();
                control_socket.connect(*endpoint_iterator++, error);
            }

            if (error)
            {
                throw boost::system::system_error(error);
            }

            boost::system::error_code ignored_error;
            boost::asio::write( control_socket , boost::asio::buffer( user_input ) , boost::asio::transfer_all() , ignored_error );

            control_socket.close();

            if( user_input.find( "download" ) != std::string::npos )
            {
                executeFileDownloadCommand( user_input );
            }

            boost::asio::io_service answer_io_service;

            tcp::acceptor acceptor(answer_io_service, tcp::endpoint(tcp::v4(), 3032));
            tcp::socket socket(answer_io_service);
            acceptor.accept(socket);

            for (;;)
            {
                boost::array<char, 128> buf;
                boost::system::error_code error;

                size_t len = socket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                std::cout.write(buf.data(), len);
            }

            socket.close();

            /*
            boost::asio::io_service answer_io_service;
            tcp::resolver answer_resolver(io_service);
            tcp::resolver::query answer_query( argv[1], DOWNLOAD_SOCKET_PORT );
            tcp::resolver::iterator answer_endpoint_iterator = answer_resolver.resolve(answer_query);
            tcp::resolver::iterator answer_end;

            tcp::socket answer_socket(answer_io_service);
            boost::system::error_code answer_error = boost::asio::error::host_not_found;


            while (answer_error && answer_endpoint_iterator != answer_end)
            {
                answer_socket.close();
                answer_socket.connect(*answer_endpoint_iterator++, answer_error);
            }

            if (error)
            {
                throw boost::system::system_error(answer_error);
            }

            for (;;)
            {
                boost::array<char, 128> buf;
                boost::system::error_code error;

                size_t len = answer_socket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                std::cout.write(buf.data(), len);
            }
            */
        }


    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
