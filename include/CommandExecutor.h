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

        out_file << buf.data();
        std::cout.write(buf.data(), len);
    }

    socket.close();
    out_file.close();
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
