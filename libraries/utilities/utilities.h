#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

#include <stdio.h>

FILE *check_isopen            (const char *file_name, const char *opening_mode);
bool  check_isclose           (FILE *file_pointer);
FILE *check_isopen_html       (const char *file_name, const char *opening_mode);
bool  check_argc              (int argc, int correct_number_argc);
void flush_buffer             ();
bool check_equal_with_accuracy(double number_1, double number_2, double accuracy);

#endif // UTILITIES_H_INCLUDED
