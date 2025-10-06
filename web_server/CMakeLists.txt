cmake_minimum_required(VERSION 3.16)
project(web_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(web_server
    main.cpp
    password_hasher.h
    data_base.h
)

target_include_directories(web_server PRIVATE ${SQLITE_MODERN_CPP_INCLUDE_DIRS})
target_link_libraries(web_server PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(web_server PRIVATE Crow::Crow asio::asio)
target_link_libraries(web_server PRIVATE unofficial-sodium::sodium)
# todo: сделать условие для unix
target_link_libraries(web_server PRIVATE wsock32 ws2_32)

# Копируем templates и static рядом с бинарником после сборки
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/templates
            ${CMAKE_BINARY_DIR}/templates
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/static
            ${CMAKE_BINARY_DIR}/static
)


include(GNUInstallDirs)
install(TARGETS web_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
