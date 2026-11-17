#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

struct Player;

void clear_input(void);

int read_int(int* out);

int read_word(char* buf, int max_len);

void debug_free(void* ptr);

int  utils_read_items(char names[][32], int max_items);

void utils_write_diary(const char* hero_name,
    int day, int hour,
    const char* weather_name, int temp, int wind,
    const struct Player* p,
    const char* note);

void utils_count_supplies(void);

int  hours_until_2am(int current_hour);
void advance_hours(int* day, int* hour, int delta);

#endif