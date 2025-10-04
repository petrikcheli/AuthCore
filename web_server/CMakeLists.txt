cmake_minimum_required(VERSION 3.16)
project(web_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

add_executable(web_server main.cpp templates/index.html)

target_link_libraries(web_server PRIVATE Crow::Crow asio::asio)
target_link_libraries(web_server PRIVATE wsock32 ws2_32)

# Копируем templates рядом с бинарником после сборки
add_custom_target(copy_templates ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/templates
            ${CMAKE_BINARY_DIR}/templates
)

include(GNUInstallDirs)
install(TARGETS web_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
