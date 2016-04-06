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

            boost::asio::io_service answer_io_service;

            tcp::endpoint endpoint( tcp::v4() , 3032 );
            tcp::acceptor acceptor( answer_io_service );
            acceptor.open( endpoint.protocol() );
            acceptor.set_option( asio::ip::tcp::acceptor::reuse_address( true ));
            acceptor.bind( endpoint );


            /*
            tcp::acceptor acceptor(answer_io_service, tcp::endpoint(tcp::v4(), 3032));
            boost::asio::socket_base::reuse_address option(true);
            acceptor.set_option( option );
            tcp::socket socket(answer_io_service);
            */

            if( user_input.find( "catalog" ) != std::string::npos )
            {
                size_t begin = user_input.find( "catalog" );
                user_input.erase( begin , 7 );
                std::string new_input = "ls ";
                new_input += user_input;
                boost::system::error_code ignored_error;
                boost::asio::write( control_socket , boost::asio::buffer( new_input ) , boost::asio::transfer_all() , ignored_error );
                control_socket.close();
                acceptor.listen();
            }

            else if( user_input.find( "spwd" ) != std::string::npos )
            {
                size_t begin = user_input.find( "spwd" );
                user_input.erase( begin , 4 );
                std::string new_input = "pwd ";
                new_input += user_input;
                boost::system::error_code ignored_error;
                boost::asio::write( control_socket , boost::asio::buffer( new_input ) , boost::asio::transfer_all() , ignored_error );
                control_socket.close();
                acceptor.listen();
            }

            else if( user_input.find( "bye" ) != std::string::npos )
            {
                boost::system::error_code ignored_error;
                boost::asio::write( control_socket , boost::asio::buffer( user_input ) , boost::asio::transfer_all() , ignored_error );
                control_socket.close();
                socket.close();
                std::cout << "Internet copy client is down!\n";
                return 0;
            }


            else if( user_input.find( "download" ) != std::string::npos )
            {
                boost::system::error_code ignored_error;
                boost::asio::write( control_socket , boost::asio::buffer( user_input ) , boost::asio::transfer_all() , ignored_error );
                control_socket.close();
                socket.close();
                executeFileDownloadCommand( user_input );
                continue;
            }

            else if( user_input.find( "upload" ) != std::string::npos )
            {
                boost::system::error_code ignored_error;
                boost::asio::write( control_socket , boost::asio::buffer( user_input ) , boost::asio::transfer_all() , ignored_error );
                control_socket.close();
                socket.close();
                executeFileUploadCommand( std::string( argv[1] ) , user_input );
                continue;
            }

            else
            {
                std::cout << executeCommand( user_input ) << "\n";
                control_socket.close();
                socket.close();
                continue;
            }


            for (;;)
            {
                boost::array<char, 5000> buf;
                boost::system::error_code error;

                size_t len = socket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                std::cout.write(buf.data(), len);
            }

            socket.close();
            acceptor.close();


        }


    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
