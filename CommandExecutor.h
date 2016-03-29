#ifndef COMMANDEXECUTOR_H_INCLUDED
#define COMMANDEXECUTOR_H_INCLUDED

#include <string>
#include <iostream>
#include <cstdio>


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


#endif // COMMANDEXECUTOR_H_INCLUDED
