#ifndef COMMANDEXECUTOR_H_INCLUDED
#define COMMANDEXECUTOR_H_INCLUDED

#include <ctime>
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using boost::asio::ip::tcp;
using namespace boost;

void clientSocketWriteHandler( const boost::system::error_code& error , std::size_t bytes_transferred )
{
    std::cout << error.message();
}


std::string executeCommand( std::string command )
{
    char buffer[128];
    std::string output = "";

    command += " 2>&1";

    FILE* pipe = popen( command.c_str() , "r" );

    if( !pipe )
    {
        return "Error: creating pipe failed.\n";
    }

    while( !feof( pipe ))
    {
        if( fgets( buffer , 128 , pipe ) != NULL )
        {
            output += buffer;
        }
    }

    pclose( pipe );

    return output;

}

void executeFileDownloadCommand( std::string command )
{
    std::ofstream out_file;

    boost::asio::io_service answer_io_service;

    /*
    tcp::endpoint endpoint( tcp::v4() , 3032 );
    tcp::acceptor acceptor( answer_io_service );
    acceptor.open( endpoint.protocol() );
    acceptor.set_option( asio::ip::tcp::acceptor::reuse_address( true ));
    acceptor.bind( endpoint );
    acceptor.listen();
    */


    tcp::acceptor acceptor(answer_io_service, tcp::endpoint(tcp::v4(), 3033));
    boost::asio::socket_base::reuse_address option(true);
    acceptor.set_option( option );
    tcp::socket socket(answer_io_service);
    acceptor.accept(socket);


    std::cout << "Accepted Server connection request.\n";

    std::vector< std::string > tokenized_command;
    boost::algorithm::split( tokenized_command , command , is_any_of( " " ) , token_compress_on );

    if( tokenized_command.size() != 3 )
    {
        std::cerr << "Error: usage download <source-filename> <dest-filename>\n";
        return;
    }

    boost::filesystem::path filename_path( tokenized_command[2] );
    if( boost::filesystem::exists( filename_path ))
    {
        std::cerr << "Warning: Overriding file: " << tokenized_command[2] << "\n";
    }

    out_file.open( tokenized_command[2].c_str() );

    for (;;)
    {
        boost::array<char, 10000> buf;
        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (error == boost::asio::error::eof)
            break; // Connection closed cleanly by peer. Eof is sent when the server closes the connection
        else if (error)
            throw boost::system::system_error(error); // Some other error.

        std::string buffer_data( buf.data() );
        if( buffer_data.find( "\x1Error:" ) != std::string::npos )
        {
            std::cout << buffer_data << "\n";
            break;
        }

        out_file << buf.data();
    }

    socket.close();
    acceptor.close();
    out_file.close();
}


void executeFileUploadCommand( std::string host , std::string command )
{

    boost::asio::io_service answer_io_service;


    std::vector< std::string > tokenized_command;
    boost::algorithm::split( tokenized_command , command , is_any_of( " " ) , token_compress_on );

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query( host , "3031" );
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;

    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }

    if (error)
    {
        throw boost::system::system_error(error);
    }

    boost::system::error_code ignored_error;

    if( tokenized_command.size() != 3 )
    {
        std::cerr << "Error: usage upload <source-filename> <dest-filename>\n";
        socket.close();
        return;
    }

    boost::filesystem::path filename_path( tokenized_command[1] );
    if( !boost::filesystem::exists( filename_path ))
    {
        std::cerr << "Error: " << tokenized_command[1] << " does not exist.\n";
        socket.close();
        return;
    }

    std::ifstream infile( tokenized_command[1].c_str() );

    if( !infile.is_open() )
    {
        std::cerr << "Error: unable to open file.\n";
        socket.close();
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
    boost::asio::write( socket , boost::asio::buffer( file_contents ) , boost::asio::transfer_all() , ignored_error );


    socket.close();
    infile.close();
}




void executeControlCommand( std::string hostname , std::string command )
{
    try
    {

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query( hostname , "3032" );
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        tcp::socket download_socket(io_service);
        boost::system::error_code error = boost::asio::error::host_not_found;

        std::cout << "Trying to connect to FTP server at " << hostname << " ......\n";

        while (error && endpoint_iterator != end)
        {
            download_socket.close();
            download_socket.connect(*endpoint_iterator++, error);
            std::cout << "Connection to FTP server at " << hostname << " failed.\n";
        }

        if (error)
        {
            throw boost::system::system_error(error);
        }

        std::cout << "Created download connection to FTP server at " << hostname << " on port 3032.\n";


        for (;;)
        {
            boost::array<char, 128> buf;
            boost::system::error_code error;

            size_t len = download_socket.read_some(boost::asio::buffer(buf), error);

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






#endif // COMMANDEXECUTOR_H_INCLUDED
