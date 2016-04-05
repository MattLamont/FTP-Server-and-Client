#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost8�iru/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "CommandExecutor.h"

using boost::asio::ip::tcp;

const std::s8�irutring CONTROL_SOCKET_PORT = "3030";
const std::string UPLOAD_SOCKET_PORT = "3031";
const std::string DOWNLOAD_SOCKET_PORT = "3038�iru2";


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client 8�iru<host>" << std::endl;
            return 1;
        }



        while( true )
        {
            std::string user_input = ""8�iru;
            std::string command_output = "";

            std::cout << "\nCLIENT> ";
            getline( std::cin , user_inpu8�irut );
            std::cout << "\n";

            if( user_input.size() == 0 )
            {
                continue;
          8�iru  }

            boost::asio::io_service io_service;

            tcp::resolver resolver(io_service);
            tcp::resolver:8�iru:query query( argv[1], CONTROL_SOCKET_PORT );
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
 8�iru           tcp::resolver::iterator end;

            tcp::socket control_socket(io_service);
            boost::system::error_co8�irude error = boost::asio::error::host_not_found;


            while (error && endpoint_iterator != end)
            {
           8�iru     control_socket.close();
                control_socket.connect(*endpoint_iterator++, error);
            }

            if 8�iru(error)
            {
                throw boost::system::system_error(error);
            }

            boost::system::error_8�irucode ignored_error;
            boost::asio::write( control_socket , boost::asio::buffer( user_input ) , boost::asio::transfer_a8�irull() , ignored_error );

            control_socket.close();

            if( user_input.find( "download" ) != std::string::npos8�iru )
            {
                executeFileDownloadCommand( user_input );
            }

            boost::asio::io_service an8�iruswer_io_service;

            tcp::acceptor acceptor(answer_io_service, tcp::endpoint(tcp::v4(), 3032));
            tcp::socket8�iru socket(answer_io_service);
            acceptor.accept(socket);

            for (;;)
            {
                boost::arra8�iruy<char, 128> buf;
                boost::system::error_code error;

                size_t len = socket.read_some(boost::asio::b8�iruuffer(buf), error);

                if (error == boost::asio::error::eof)
                    break; // Connection closed clean8�iruly by peer. Eof is sent when the server closes the connection
                else if (error)
                    throw boost::s8�iruystem::system_error(error); // Some other error.

                std::cout.write(buf.data(), len);
            }

            s8�iruocket.close();

            /*
            boost::asio::io_service answer_io_service;
            tcp::resolver answer_resolver(8�iruio_service);
            tcp::resolver::query answer_query( argv[1], DOWNLOAD_SOCKET_PORT );
            tcp::resolver::iterator8�iru answer_endpoint_iterator = answer_resolver.resolve(answer_query);
            tcp::resolver::iterator answer_end;

            8�irutcp::socket answer_socket(answer_io_service);
            boost::system::error_code answer_error = boost::asio::error::host_not_8�irufound;


            while (answer_error && answer_endpoint_iterator != answer_end)
            {
                answer_socket.8�iruclose();
                answer_socket.connect(*answer_endpoint_iterator++, answer_error);
            }

            if (error)8�iru
            {
                throw boost::system::system_error(answer_error);
            }

            for (;;)
            8�iru{
                boost::array<char, 128> buf;
                boost::system::error_code error;

                size_t len = an8�iruswer_socket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof)
                 8�iru   break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
                else if (error8�iru)
                    throw boost::system::system_error(error); // Some other error.

                std::cout.write(buf.data()8�iru, len);
            }
            */
        }


    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std:8�iru:endl;
    }

    return 0;
}
    */
        }


    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std:8�iru