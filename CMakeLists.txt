CMAKE_MINIMUM_REQUIRED( VERSION 2.8 FATAL_ERROR )

find_package( Boost COMPONENTS system filesystem thread  )
IF( Boost_FOUND )
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS})
    add_definitions( "-DHAS_BOOST" )
endif()

File( GLOB Headers "include/*.h" )

Set( include_dirs "include" )

include_directories( ${include_dirs})

link_directories( ${Boost_LIBRARIES})

add_executable( server
                "src/server.cpp"
                "src/TCPServer.cpp"
                "src/TCPServerConnection.cpp"
                ${Headers} )

target_link_libraries( server ${Boost_LIBRARIES} pthread )

add_executable( client
                "src/client.cpp"
                "src/TCPClient.cpp"
                "src/TCPClientConnection.cpp"
                ${Headers} )

target_link_libraries( client ${Boost_LIBRARIES} pthread )




#COMPONENTS system filesystem thread algorithm asio array
