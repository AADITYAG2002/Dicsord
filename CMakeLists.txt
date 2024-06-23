cmake_minimum_required(VERSION 3.15)

project(Dicsord VERSION 1.0
                DESCRIPTION "Totally not a ripoff, pls dont sue me T-T "
                LANGUAGES C)

add_library(cli_be STATIC client_be.c client_be.h)
add_library(cli_user STATIC user_input.c user_input.h)


add_executable(client client.c)
target_link_libraries(client PUBLIC cli_be cli_user ncurses)

add_executable(server multi_server.c)
