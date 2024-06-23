#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "user_input.h"
#include "client_be.h"

void print_title_to_center(WINDOW *win, char *title){
    // int center_col = win->_maxx / 2;
    int center_col = getmaxx(win) / 2;
    int half_length = strlen(title) / 2;
    int adjusted_col = center_col - half_length;
    mvwprintw(win, 0, adjusted_col, title);
}

int main(int argc, char** argv){
    char ip_addr[256] = "127.0.0.1";
    int port = 8080;
    char uname[256] = "Admin: ";

    // program start
    if(argc < 7){
        printf("Usage: ./client --ip <IP Address> --port <Port> --name <Username>\n");
        return EXIT_FAILURE;
    }

    strcpy(ip_addr, argv[2]);
    port = atoi(argv[4]);
    strcpy(uname, argv[6]);
    strlcat(uname, ": ", 256);

    // connection intialization
    struct CLIENT client;
    if(initialize(&client, ip_addr, port) < 0){
        return EXIT_FAILURE;
    }
    
    // terminal setup 

    initscr();

    cbreak();             // Immediate key input
    nonl();               // Get return key
    timeout(0);           // Non-blocking input
    keypad(stdscr, 1);    // Fix keypad
    noecho();             // No automatic printing
    curs_set(0);
    intrflush(stdscr, 0); // Avoid potential graphical  ssues
    leaveok(stdscr, 1);   // Don't care where cursor is left

    WINDOW  *MESSAGES, *MESSAGE_BOX, *TEXT_BOX;  
    int TERM_HEIGHT, TERM_WIDTH;
    getmaxyx(stdscr, TERM_HEIGHT, TERM_WIDTH);
    //create 3 windows and position them


    // message box
    MESSAGE_BOX = newwin(TERM_HEIGHT-4, TERM_WIDTH , 0, 0 );
    refresh();
    int MESSAGE_HEIGHT, MESSAGE_WIDTH;
    getmaxyx(MESSAGE_BOX, MESSAGE_HEIGHT, MESSAGE_WIDTH);
    box(MESSAGE_BOX,0,0);
    print_title_to_center(MESSAGE_BOX, "Messages");
    wrefresh(MESSAGE_BOX);

    // messages
    MESSAGES = newwin(MESSAGE_HEIGHT-2, MESSAGE_WIDTH-2 , 1, 1 );
    refresh();


    // scrolling;
    idlok(MESSAGES, 1);
    wscrl(MESSAGES, -1);
    scrollok(MESSAGES, 1);

    // text box
    TEXT_BOX = newwin(4, TERM_WIDTH, TERM_HEIGHT-4, 0 );
    refresh();
    box(TEXT_BOX, 0, 0);
    print_title_to_center(TEXT_BOX, "Text input");
    // wmove(TEXT_BOX,1,1);
    wrefresh(TEXT_BOX);

    struct input_line lnbuffer;
    make_buffer(&lnbuffer);

    int lines_read = 0;
    char recvbuf[1024] = {0};

    while(1){
        char sendbuf[1024] = {0};
        int len = get_line_non_blocking(&lnbuffer, sendbuf, sizeof(sendbuf), stdscr);
        if(len > 0){
            if(strcmp(sendbuf, "exit") == 0){
                break;
            }
            // data to server
            char send_data[2048] = {0};
            strlcat(send_data, uname, 2048);
            strlcat(send_data, sendbuf, 2048);
            transmit(&client, send_data);

        }
        // render text to input box
        wmove(TEXT_BOX, 1,1);
        render_line(&lnbuffer, TEXT_BOX);
        
        // data from server
        if(receive(&client, recvbuf, 1024) > 0 ){
            waddstr(MESSAGES, recvbuf);
            waddch(MESSAGES, '\n');
            wrefresh(MESSAGES);
            memset(recvbuf, 0, sizeof(recvbuf));
        }   
    }
    destroy_buffer(&lnbuffer);
    terminate(&client);
    delwin(TEXT_BOX);
    delwin(MESSAGES);
    endwin();
    return EXIT_SUCCESS;
}
