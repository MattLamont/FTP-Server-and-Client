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
                std::cout << "got download request.\n";
                boost::asio::io_service io_service;

                tcp::resolver resolver(io_service);
                tcp::resolver::query query( socket_.remote_endpoint().address().to_string() , "3032" );
                tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
                tcp::resolver::iterator end;

                tcp::socket sendback_socket(io_service);
                error = boost::asio::error::host_not_found;
                boost::system::error_code ignored_error;

                for( int i = 0 ; i < 10 ; i++ )
                {
                    std::this_thread::sleep_for( std::chrono::seconds(i));

                    while (error && endpoint_iterator != end)
                    {
                        sendback_socket.close();
                        sendback_socket.connect(*endpoint_iterator++, error);
                    }
                }

                if (error)
                {
                    throw boost::system::system_error(error);
                }



                std::vector< std::string > tokenized_command;
                boost::algorithm::split( tokenized_command , command , is_any_of( " " ) , token_compress_on );

                std::cout << "Recieved request to download " << tokenized_command[1] << "\n";

                if( tokenized_command.size() != 3 )
                {
                    std::string error_message = "\x1Error: could not perform download.";
                    std::cerr << error_message << "\n";
                    boost::asio::write( sendback_socket , boost::asio::buffer( error_message ) , boost::asio::transfer_all() , ignored_error );
                    sendback_socket.close();
                    return;
                }

                std::string target_file = tokenized_command[1];
                boost::filesystem::path target_path( target_file );

                if( !boost::filesystem::exists( target_path ))
                {
                    std::string error_message = "\x1Error: could not find file.";
                    std::cout << error_message << "\n";
                    boost::asio::write( sendback_socket , boost::asio::buffer( error_message ) , boost::asio::transfer_all() , ignored_error );
                    sendback_socket.close();
                    return;
                }


                std::ifstream infile( target_file.c_str() );

                if( !infile.is_open() )
                {
                    std::cerr << "Error: unable to open file.\n";
                    boost::asio::write( sendback_socket , boost::asio::buffer( std::string( "\x1Error: unable to open file." ) ) , boost::asio::transfer_all() , ignored_error );
                    sendback_socket.close();
                    return;
                }

                std::string file_contents = "";
                std::string line = "";

                std::cout << "Reading in file...\n";
                while( std::getline( infile , line ) )
                {
                    file_contents += line;
                    file_contents += "\n";
                    line = "";
                }

                std::cout << "sending " << file_contents.size() << " bytes over socket\n";
                boost::asio::write( sendback_socket , boost::asio::buffer( file_contents ) , boost::asio::transfer_all() , ignored_error );

                infile.close();
                sendback_socket.close();

                std::cout << "finished download\n";
                return;

            }



            if( command.find( "upload" ) != std::string::npos )
            {
                std::cout << "got upload request.\n";

                boost::asio::io_service answer_io_service;

                tcp::endpoint endpoint( tcp::v4() , 3031 );
                tcp::acceptor acceptor( answer_io_service );
                acceptor.open( endpoint.protocol() );
                acceptor.set_option( asio::ip::tcp::acceptor::reuse_address( true ));
                acceptor.bind( endpoint );
                acceptor.listen();

                /*
                tcp::acceptor acceptor(answer_io_service, tcp::endpoint(tcp::v4(), 3031));
                boost::asio::socket_base::reuse_address option(true);
                acceptor.set_option( option );
                tcp::socket socket(answer_io_service);
                acceptor.accept(socket);
                */

                std::cout << "Accepted client connection request.\n";

                std::vector< std::string > tokenized_command;
                boost::algorithm::split( tokenized_command , command , is_any_of( " " ) , token_compress_on );

                if( tokenized_command.size() != 3 )
                {
                    socket.close();
                    return;
                }

                std::ofstream outfile;
                outfile.open( tokenized_command[2].c_str() );

                for (;;)
                {
                    boost::array<char, 10000> buf;
                    boost::system::error_code error;

                    size_t len = socket.read_some(boost::asio::buffer(buf), error);

                    if (error == boost::asio::error::eof)
                        break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
                    else if (error)
                        throw boost::system::system_error(error); // Some other error.


                    outfile << buf.data();
                }

                socket.close();
                acceptor.close();
                outfile.close();
                std::cout << "finished uploading file.\n";
                return;

            }


            if( command.find( "bye" ) != std::string::npos )
            {
                std::cout << "File copy server is down!\n";
                exit( 0 );
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

