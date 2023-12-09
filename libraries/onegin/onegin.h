#ifndef ONEGIN_H_INCLUDED
#define ONEGIN_H_INCLUDED

#include<stdio.h>
#include"../utilities/myassert.h"

struct text_parametrs {
    struct string_parametrs *string_array;
    char                    *buffer;
    size_t                   number_lines;
};

struct string_parametrs {
    char            *string_pointer;
    size_t           size_string;
};

typedef int (*compare_func) (const void *, const void *);

void text_parametrs_constructor(struct text_parametrs *text, FILE *file_pointer);

const char *skip_spaces(const char *string);
errors_code replace_chars_to_null_character(char *text, const char *replaceable_characters);

char *input_data(FILE *file_pointer);
size_t determine_size(FILE *file_pointer);

void search_strings(struct text_parametrs *text);
size_t count_strings(const char *buffer);

int compare_line(const void *string_parametrs_1, const void *string_parametrs_2);
int reverse_compare_line(const void *string_parametrs_1, const void *string_parametrs_2);
const char *move_to_alpha(const char * char_string);
const char *back_to_alpha(const char *string, const char *end_string);

void output_data(const struct text_parametrs *text, FILE *file_pointer);
void print_string(const char *string, FILE *file_pointer);

void text_parametrs_destructor(struct text_parametrs *text);

int compare_int (const void *number_1, const void *number_2);
void swap_values(void* value_1, void* value_2, const size_t size_elements);

void quick_sort( void *array,
                 size_t number_elements,
                 size_t size_elements,
                 compare_func comparator);

void quick_sort_loop( void *left_border,
                      void *right_border,
                      size_t size_elements,
                      compare_func comparator);

void *partition( void *left_border,
                 void *right_border,
                 size_t size_elements,
                 compare_func comparator);

void sort_three_values(void* left_border,
                  void* right_border,
                  size_t size_elements,
                  compare_func comparator);

#endif // ONEGIN_H_INCLUDED
