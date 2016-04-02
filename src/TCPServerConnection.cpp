#include "TCPServerConnection.h"
#include "CommandExecutor.h"




void TCPServerConnection::start()
{
    try
    {
        for (;;)
        {
            boost::array<char, 128> buf = {};
            boost::system::error_code error;

            size_t len = socket_.receive(boost::asio::buffer(buf), NULL ,  error);

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::string command( buf.data() );

            //Found the download command
            if( command.find( "download" ) != std::string::npos )
            {
                boost::asio::io_service io_service;

                tcp::resolver resolver(io_service);
                tcp::resolver::query query( socket_.remote_endpoint().address().to_string() , "3032" );
                tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
                tcp::resolver::iterator end;

                tcp::socket sendback_socket(io_service);
                error = boost::asio::error::host_not_found;
                boost::system::error_code ignored_error;

                while (error && endpoint_iterator != end)
                {
                    sendback_socket.close();
                    sendback_socket.connect(*endpoint_iterator++, error);
                }

                if (error)
                {
                    throw boost::system::system_error(error);
                }


                std::vector< std::string > tokenized_command;
                boost::algorithm::split( tokenized_command , command , is_any_of( " " ) , token_compress_on );

                if( tokenized_command.size() != 3 )
                {
                    std::string error_message = "Error: could not perform download.";
                    boost::asio::write( sendback_socket , boost::asio::buffer( error_message ) , boost::asio::transfer_all() , ignored_error );
                    sendback_socket.close();
                    return;
                }

                std::string target_file = tokenized_command[2];
                boost::filesystem::path target_path( target_file );

                if( !boost::filesystem::exists( target_path ))
                {
                    std::string error_message = "Error: could not find file.";
                    boost::asio::write( sendback_socket , boost::asio::buffer( error_message ) , boost::asio::transfer_all() , ignored_error );
                    sendback_socket.close();
                    return;
                }

                std::ifstream infile;
                infile.open( target_file.c_str() );
                std::string line;

                while( std::getline( infile , line ))
                {
                    boost::asio::write( sendback_socket , boost::asio::buffer( line ) , boost::asio::transfer_all() , ignored_error );
                }

                infile.close();
                sendback_socket.close();
                return;

            }




            std::string output = executeCommand( std::string(buf.data()) );
            //std::cout << output;


            boost::asio::io_service io_service;


            tcp::resolver resolver(io_service);
            tcp::resolver::query query( socket_.remote_endpoint().address().to_string() , "3032" );
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            tcp::resolver::iterator end;

            tcp::socket sendback_socket(io_service);
            error = boost::asio::error::host_not_found;


            while (error && endpoint_iterator != end)
            {
                sendback_socket.close();
                sendback_socket.connect(*endpoint_iterator++, error);
            }

            if (error)
            {
                throw boost::system::system_error(error);
            }

            boost::system::error_code ignored_error;
            boost::asio::write( sendback_socket , boost::asio::buffer( output ) , boost::asio::transfer_all() , ignored_error );
            sendback_socket.close();

        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    //boost::asio::async_write(socket_, boost::asio::buffer(message_),
    //boost::bind( &TCPServerConnection::handle_write , shared_from_this() , boost::asio::placeholders::error , boost::asio::placeholders::bytes_transferred));
}

