

   include_directories ( ${VLSISAPD_SOURCE_DIR}/src/utilities/src
                         ${VLSISAPD_SOURCE_DIR}/src/bookshelf/src
                         ${Boost_INCLUDE_DIRS}
                         ${PYTHON_INCLUDE_PATH}
                       )

                   set ( includes       vlsisapd/bookshelf/Parser.h
                                        vlsisapd/bookshelf/Node.h
                                        vlsisapd/bookshelf/Pin.h
                                        vlsisapd/bookshelf/Net.h
                                        vlsisapd/bookshelf/Row.h
                                        vlsisapd/bookshelf/Circuit.h
                                        vlsisapd/bookshelf/Exception.h
                       )            
                   set ( cpps           Parser.cpp
                                        Node.cpp
                                        Pin.cpp
                                        Net.cpp
                                        Row.cpp
                                        Circuit.cpp
                                        Exception.cpp
                       )            
                   set ( testcpps       BookshelfTkMain.cpp )
           add_library ( bookshelf      ${cpps} )
 target_link_libraries ( bookshelf      vlsisapdutils )
 set_target_properties ( bookshelf      PROPERTIES VERSION 1.0 SOVERSION 1 )
        add_executable ( bookshelf-tk   ${testcpps} )
 target_link_libraries ( bookshelf-tk   bookshelf ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

               install ( TARGETS        bookshelf      DESTINATION lib${LIB_SUFFIX} )
               install ( TARGETS        bookshelf-tk   DESTINATION bin )
               install ( FILES          ${includes}    DESTINATION include/vlsisapd/bookshelf ) 

