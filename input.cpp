#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "operators.h"

const ssize_t NO_OPERATOR = -1;
const ssize_t NO_VARIABLE = -1;

static char *saving_buffer(FILE *database_file);
static tree_node *saving_node_from_database(char *database_buffer, tree *tree_pointer);
static bool process_value(const char *current_token, tree_node *current_tree_node, tree *tree_pointer);
static ssize_t find_operator(const char *current_token);
static ssize_t find_variable(const char *current_token, variable_parametrs *variable_array, ssize_t variable_array_size);
static bool add_variable(const char *current_token, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer);

ssize_t input_tree_from_database(FILE *database_file, tree *tree_pointer)
{
    MYASSERT(database_file      != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_FILE_IS_NULL);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    char *database_buffer = saving_buffer(database_file);

    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_BUFFER_IS_NULL);

    delete_node(tree_pointer->root);
    tree_pointer->root = NULL;

    tree_pointer->root = saving_node_from_database(database_buffer, tree_pointer);

    free(database_buffer);

    return VERIFY_TREE(tree_pointer);;
}

#define STRTOK_AND_CHECK_(str)                                          \
do {                                                                    \
    str = strtok(NULL, " \n\r\t");                                      \
                                                                        \
    if (str == NULL) {                                                  \
        printf(RED "ERROR! Incorrect input from file.\n" RESET_COLOR);  \
        return NULL;                                                    \
    }                                                                   \
                                                                        \
} while (0)

static tree_node *saving_node_from_database(char *database_buffer, tree *tree_pointer)
{
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    static char *current_token = NULL;

    if (database_buffer != NULL) {
        current_token = strtok(database_buffer, " \n\r\t");

        if (!current_token) {
            printf(RED "ERROR! No separators found\n" RESET_COLOR);
            return NULL;
        }
    }

    if (*current_token == '(')
    {
        STRTOK_AND_CHECK_(current_token);

        tree_node *current_tree_node = new_tree_node();

        current_tree_node->left = saving_node_from_database(NULL, tree_pointer);

        while(*current_token == ')') {
            STRTOK_AND_CHECK_(current_token);
        }

        if (!process_value(current_token, current_tree_node, tree_pointer))
        {
            printf(RED "ERROR! Incorrect value input from file.\n" RESET_COLOR);
            return NULL;
        }

        ++tree_pointer->size;

        STRTOK_AND_CHECK_(current_token);

        current_tree_node->right = saving_node_from_database(NULL, tree_pointer);

        return current_tree_node;
    }

    if (*current_token == '_')
    {
        STRTOK_AND_CHECK_(current_token);
        return NULL;
    }

    printf(RED "ERROR! Incorrect input from file.\n" RESET_COLOR);
    return NULL;
}

#undef STRTOK_AND_CHECK_

static bool process_value(const char *current_token, tree_node *current_tree_node, tree *tree_pointer)
{
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(current_token      != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(current_tree_node  != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);

    char *pointer_end_of_number = NULL;

    double number = strtod(current_token, &pointer_end_of_number);
    ssize_t operator_index = NO_OPERATOR;
    ssize_t variable_index = NO_VARIABLE;

    if (pointer_end_of_number == current_token + strlen(current_token))
    {
        current_tree_node->type = NUMBER;
        current_tree_node->value.number = number;

        return true;
    }

    if ((operator_index = find_operator(current_token)) != NO_OPERATOR)
    {
        current_tree_node->type = OPERATOR;
        current_tree_node->value.operator_index = operator_index;

        return true;
    }

    if ((variable_index = find_variable(current_token, tree_pointer->variable_array, tree_pointer->variable_array_position)) == NO_VARIABLE)
    {
        if (!add_variable(current_token, tree_pointer->variable_array, &tree_pointer->variable_array_position)) {
            return false;
        }

        variable_index = tree_pointer->variable_array_position - 1;
    }

    current_tree_node->type = VARIABLE;
    current_tree_node->value.variable_index = variable_index;

    return true;
}

static ssize_t find_variable(const char *current_token, variable_parametrs *variable_array, ssize_t variable_array_size)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);

    for (ssize_t variable_index = 0; variable_index < variable_array_size; variable_index++)
    {
        if (strcmp((variable_array[variable_index]).name, current_token) == 0) {
            return variable_index;
        }
    }

    return NO_VARIABLE;
}

static ssize_t find_operator(const char *current_token)
{
    MYASSERT(current_token != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_OPERATOR);

    ssize_t size_operators_array = sizeof(OPERATORS) / sizeof(*OPERATORS);

    for (ssize_t operator_index = 0; operator_index < size_operators_array; operator_index++)
    {
        if (strcmp(OPERATORS[operator_index].name, current_token) == 0) {
            return operator_index;
        }
    }

    return NO_OPERATOR;
}

static bool add_variable(const char *current_token, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);

    ssize_t variable_array_position = *variable_array_position_pointer;

    if ((size_t) variable_array_position >= SIZE_ARRAY_OF_VARIABLE)
    {
        printf(RED "ERROR! Too many variable. SIZE_ARRAY_OF_VARIABLE = %lu.\n" RESET_COLOR, SIZE_ARRAY_OF_VARIABLE);
        return false;
    }

    printf("What is the value of the variable <%s>?\n", current_token);

    scanf("%lf", &(variable_array[variable_array_position].value));
    flush_buffer();

    strncpy(variable_array[variable_array_position].name, current_token, MAX_SIZE_NAME_VARIABLE);

    ++(*variable_array_position_pointer);

    return true;
}


static char *saving_buffer(FILE *database_file)
{
    MYASSERT(database_file != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    size_t size_file       = determine_size(database_file);
    char  *database_buffer = (char *)calloc(size_file + 1, sizeof(char));

    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    size_file = fread(database_buffer, sizeof(char), size_file, database_file);
    database_buffer[size_file] = '\0';

    return database_buffer;
}
