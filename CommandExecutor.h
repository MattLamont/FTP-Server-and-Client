#ifndef COMMANDEXECUTOR_H_INCLUDED
#define COMMANDEXECUTOR_H_INCLUDED

#include <ctime>
#include <iostream>
#include <string>
#include <cstdio>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>


std::string executeCommand( const std::string command )
{
    char buffer[128];
    std::string output = "";

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

void executeFileCommand()
{

}

void executeControlCommand()
{

}

void clientSocketWriteHandler( const boost::system::error_code& error , std::size_t bytes_transferred )
{
    std::cout << error.message();
}


#endif // COMMANDEXECUTOR_H_INCLUDED
