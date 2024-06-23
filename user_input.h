#ifndef _USER_INPUT_H_
#define _USER_INPUT_H_

struct input_line {
    char *ln;
    int length;
    int capacity;
    int cursor;
    int last_rendered;
};

void make_buffer(struct input_line *buf);
void destroy_buffer(struct input_line *buf);
void render_line(struct input_line *buf, WINDOW *win);
int retrieve_content(struct input_line *buf, char *target, int max_len);
void add_char(struct input_line *buf, char ch);
int handle_input(struct input_line *buf, char *target, int max_len, int key);
int get_line_non_blocking(struct input_line *buf, char *target, int max_len, WINDOW *win);

#endif