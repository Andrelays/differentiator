#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "myassert.h"
#include "colors.h"
#include "stack.h"
#include "onegin.h"
#include "utilities.h"
#include "differentiator.h"
#include "operators.h"
#include "dsl.h"

const ssize_t NO_OPERATOR = -1;
const ssize_t NO_VARIABLE = -1;

#define syn_assert(condition)                                                                                                           \
 do {                                                                                                                                   \
    if (!(condition))                                                                                                                   \
    {                                                                                                                                   \
        printf(RED  "File: %s\n"                                                                                                        \
                    "line: %d\n"                                                                                                        \
                    "Function: %s\n"                                                                                                    \
                    "The condition is not met: \"%s\"\n"RESET_COLOR, __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition);              \
                                                                                                                                        \
        return 0;                                                                                                                       \
    }                                                                                                                                   \
}  while(0)

static tree_node *get_grammar           (parsing_info_t *, tree *);
static tree_node *get_sum               (parsing_info_t *, tree *);
static tree_node *get_product           (parsing_info_t *, tree *);
static tree_node *get_power             (parsing_info_t *, tree *);
static tree_node *get_unary_functions   (parsing_info_t *, tree *);
static tree_node *get_parenthesis       (parsing_info_t *, tree *);
static tree_node *get_number            (parsing_info_t *, tree *);
static tree_node *get_variable          (parsing_info_t *, tree *);

static parsing_info_t *parsing_info_constructor(tree *tree_pointer, const char *database_buffer);
static void            parsing_info_destructor (parsing_info_t *parsing_info);
static void            parse_data              (parsing_info_t *parsing_info, tree *tree_pointer, const char *database_buffer);

static void check_size_token_array(parsing_info_t *parsing_info);

static bool check_token_parenthesis(parsing_info_t *parsing_info, const char *database_buffer);
static bool check_token_operator   (parsing_info_t *parsing_info, const char *database_buffer);
static bool check_token_number     (parsing_info_t *parsing_info, const char *database_buffer, char **end_number_ptr);
static bool check_token_variable   (parsing_info_t *parsing_info, tree *tree_pointer, const char *database_buffer, size_t *variable_len);

static ssize_t       find_operator           (const char *current_token);

static ssize_t       find_variable           (const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t variable_array_size);
static bool          add_variable            (const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer);
static size_t        calculate_variable_len  (const char *current_token);

static char *saving_buffer (FILE *database_file, size_t *buffer_size);


ssize_t input_tree_from_database(FILE *database_file, tree *tree_pointer)
{
    MYASSERT(database_file      != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_FILE_IS_NULL);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    size_t buffer_size = 0;

    char *database_buffer = saving_buffer(database_file, &buffer_size);
    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return DATABASE_BUFFER_IS_NULL);

    delete_node(tree_pointer->root);
    tree_pointer->root = NULL;
    tree_pointer->size = 0;
    tree_pointer->variable_array_position = 0;

    parsing_info_t *parsing_info = parsing_info_constructor(tree_pointer, database_buffer);
    parse_data(parsing_info, tree_pointer, database_buffer);

    tree_pointer->root = get_grammar(parsing_info, tree_pointer);

    parsing_info_destructor(parsing_info);
    free(database_buffer);

    return VERIFY_TREE(tree_pointer);;
}

static void parse_data(parsing_info_t *parsing_info, tree *tree_pointer, const char *database_buffer)
{
    MYASSERT(database_buffer    != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parsing_info       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    while (*(database_buffer = skip_spaces(database_buffer)) != '\0')
    {

        check_size_token_array(parsing_info);

        if(check_token_parenthesis(parsing_info, database_buffer))
        {
            ++parsing_info->token_array_position;
            ++database_buffer;

            continue;
        }

        if(check_token_operator(parsing_info, database_buffer))
        {
            database_buffer += strlen(OPERATORS[CURRENT_TOKEN.value.operator_index].name);
            ++parsing_info->token_array_position;

            continue;
        }

        char *end_number_ptr = NULL;

        if(check_token_number(parsing_info, database_buffer, &end_number_ptr))
        {

            ++parsing_info->token_array_position;
            database_buffer = end_number_ptr;

            continue;
        }

        size_t variable_len = 0;

        if(check_token_variable(parsing_info, tree_pointer, database_buffer, &variable_len))
        {
            ++parsing_info->token_array_position;
            database_buffer += variable_len;

            continue;
        }

        printf(RED "Error!\n unknown character <%c> %d" RESET_COLOR, *database_buffer, *database_buffer);
        break;
    }

    parsing_info->capacity_token_array = parsing_info->token_array_position;
    parsing_info->token_array_position = 0;

    return;
}

static bool check_token_parenthesis(parsing_info_t *parsing_info, const char *database_buffer)
{
    if (*database_buffer != '(' && *database_buffer != ')') {
        return false;
    }

    CURRENT_TOKEN.type = PARENTHESIS;
    CURRENT_TOKEN.value.is_closing_parenthesis = *database_buffer - '(';

    return true;
}

static bool check_token_operator(parsing_info_t *parsing_info, const char *database_buffer)
{
    ssize_t operator_index = NO_OPERATOR;

    if ((operator_index = find_operator(database_buffer)) == NO_OPERATOR) {
        return false;
    }

    CURRENT_TOKEN.type = OPERATOR;
    CURRENT_TOKEN.value.operator_index = operator_index;

    return true;
}

static bool check_token_number(parsing_info_t *parsing_info, const char *database_buffer, char **end_number_ptr)
{
    double current_number = 0;

    current_number = strtod(database_buffer, end_number_ptr);

    if (database_buffer == *end_number_ptr) {
        return false;
    }

    CURRENT_TOKEN.type = NUMBER;
    CURRENT_TOKEN.value.number = current_number;

    return true;
}

static bool check_token_variable(parsing_info_t *parsing_info, tree *tree_pointer, const char *database_buffer, size_t *variable_len)
{
    if (!isalpha(*database_buffer)) {
        return false;
    }

    ssize_t  variable_index = NO_VARIABLE;

    *variable_len = calculate_variable_len(database_buffer);

    if ((variable_index = find_variable(database_buffer, *variable_len, tree_pointer->variable_array, tree_pointer->variable_array_position)) == NO_VARIABLE)
    {
        if (!add_variable(database_buffer, *variable_len, tree_pointer->variable_array, &tree_pointer->variable_array_position))
        {
            printf(RED "Error! Too many variables\n" RESET_COLOR);
            return false;
        }

        variable_index = tree_pointer->variable_array_position - 1;
    }

    CURRENT_TOKEN.type = VARIABLE;
    CURRENT_TOKEN.value.variable_index = variable_index;

    return true;
}

static parsing_info_t *parsing_info_constructor(tree *tree_pointer, const char *database_buffer)
{
    MYASSERT(database_buffer    != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer       != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer->info != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    const ssize_t INITIAL_CAPACITY_TOKENS_ARRAY = 100;

    parsing_info_t *parsing_info = (parsing_info_t *) calloc(1, sizeof(parsing_info_t));;

    parsing_info->capacity_token_array = INITIAL_CAPACITY_TOKENS_ARRAY;
    parsing_info->token_array_position = 0;

    parsing_info->token_array = (tree_node *) calloc(INITIAL_CAPACITY_TOKENS_ARRAY, sizeof(tree_node));

    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    return parsing_info;
}

static size_t calculate_variable_len(const char *current_token)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return 0);

    size_t index_variable_name = 0;

    while(isalnum(current_token[index_variable_name])) {
        ++index_variable_name;
    }

    return index_variable_name;
}

static void parsing_info_destructor(parsing_info_t *parsing_info)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    free(parsing_info->token_array);

    parsing_info->token_array          = NULL;
    parsing_info->capacity_token_array     = -1;
    parsing_info->token_array_position = -1;

    free(parsing_info);

    parsing_info = NULL;
}

static ssize_t find_operator(const char *current_token)
{
    MYASSERT(current_token != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_OPERATOR);

    ssize_t size_operators_array = sizeof(OPERATORS) / sizeof(*OPERATORS);

    for (ssize_t operator_index = 0; operator_index < size_operators_array; operator_index++)
    {
        if (strncmp(OPERATORS[operator_index].name, current_token, strlen(OPERATORS[operator_index].name)) == 0) {
            return operator_index;
        }
    }

    return NO_OPERATOR;
}

static ssize_t find_variable(const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t variable_array_size)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NO_VARIABLE);

    for (ssize_t variable_index = 0; variable_index < variable_array_size; variable_index++)                            //TODO check variable_len > MAX_LEN
    {
        if (strncmp((variable_array[variable_index]).name, current_token, variable_len) == 0) {
            return variable_index;
        }
    }

    return NO_VARIABLE;
}

static bool add_variable(const char *current_token, size_t variable_len, variable_parametrs *variable_array, ssize_t *variable_array_position_pointer)
{
    MYASSERT(current_token  != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);

    ssize_t variable_array_position = *variable_array_position_pointer;

    if ((size_t) variable_array_position >= SIZE_ARRAY_OF_VARIABLE)
    {
        printf(RED "ERROR! Too many variable. SIZE_ARRAY_OF_VARIABLE = %lu.\n" RESET_COLOR, SIZE_ARRAY_OF_VARIABLE);
        return false;
    }

    strncpy(variable_array[variable_array_position].name, current_token, variable_len);

    ++(*variable_array_position_pointer);

    return true;
}

static void check_size_token_array(parsing_info_t *parsing_info)
{
    MYASSERT(parsing_info != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    if (parsing_info->token_array_position >= parsing_info->capacity_token_array)
    {
        parsing_info->capacity_token_array *= 2;
        parsing_info->token_array = (tree_node *) realloc(parsing_info->token_array, sizeof(tree_node) * (size_t) parsing_info->capacity_token_array);
    }
}

static char *saving_buffer(FILE *database_file, size_t *buffer_size)
{
    MYASSERT(database_file != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    *buffer_size = determine_size(database_file);

    char *database_buffer = (char *)calloc(*buffer_size + 1, sizeof(char));

    MYASSERT(database_buffer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    *buffer_size = fread(database_buffer, sizeof(char), *buffer_size, database_file);
    database_buffer[*buffer_size] = '\0';

    return database_buffer;
}


static tree_node *get_grammar(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_sum(parsing_info, tree_pointer);

    syn_assert(parsing_info->token_array_position == parsing_info->capacity_token_array);

    return tree_node_pointer;
}

static tree_node *get_sum(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_product(parsing_info, tree_pointer);

    while (CURRENT_TOKEN.type == OPERATOR && (CURRENT_TOKEN.value.operator_index == ADD || CURRENT_TOKEN.value.operator_index == SUB))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++parsing_info->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_product(parsing_info, tree_pointer);

        switch(operator_index)
        {
            case ADD:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = ADD}, tree_node_pointer, tree_node_pointer_2);
                break;

            case SUB:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = SUB}, tree_node_pointer, tree_node_pointer_2);
                break;

            default:
                syn_assert(0);
        }
    }

    return tree_node_pointer;
}

static tree_node *get_product(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_power(parsing_info, tree_pointer);

    while (CURRENT_TOKEN.type == OPERATOR && (CURRENT_TOKEN.value.operator_index == MUL || CURRENT_TOKEN.value.operator_index == DIV))
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++parsing_info->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_power(parsing_info, tree_pointer);

        switch(operator_index)
        {
            case MUL:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = MUL}, tree_node_pointer, tree_node_pointer_2);
                break;

            case DIV:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = DIV}, tree_node_pointer, tree_node_pointer_2);
                break;

            default:
                syn_assert(0);
        }
    }

    return tree_node_pointer;
}

static tree_node *get_power(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = get_unary_functions(parsing_info, tree_pointer);

    while (CURRENT_TOKEN.type == OPERATOR && CURRENT_TOKEN.value.operator_index == POW)
    {
        ++parsing_info->token_array_position;
        ++tree_pointer->size;

        tree_node *tree_node_pointer_2 = get_unary_functions(parsing_info, tree_pointer);

        tree_node_pointer = create_node(OPERATOR, {.operator_index = POW}, tree_node_pointer, tree_node_pointer_2);
    }

    return tree_node_pointer;
}

static tree_node *get_unary_functions(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (CURRENT_TOKEN.type == OPERATOR && !OPERATORS[CURRENT_TOKEN.value.operator_index].is_binary)                                     //FIXME log
    {
        ssize_t operator_index = CURRENT_TOKEN.value.operator_index;
        ++parsing_info->token_array_position;
        ++tree_pointer->size;

        // exp exp 21

        tree_node_pointer = get_unary_functions(parsing_info, tree_pointer);

        switch(operator_index)
        {
            case SIN:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = SIN}, NULL, tree_node_pointer);
                break;

            case COS:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = COS}, NULL, tree_node_pointer);
                break;

            case TAN:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = TAN}, NULL, tree_node_pointer);
                break;

            case LOG:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = LOG}, NULL, tree_node_pointer);
                break;

            case EXP:
                tree_node_pointer = create_node(OPERATOR, {.operator_index = EXP}, NULL, tree_node_pointer);
                break;

            default:
                syn_assert(0);
        }

        return tree_node_pointer;
    }

    tree_node_pointer = get_parenthesis(parsing_info, tree_pointer);

    return tree_node_pointer;
}

static tree_node *get_parenthesis(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if (CURRENT_TOKEN.type == PARENTHESIS && CURRENT_TOKEN.value.is_closing_parenthesis == false)
    {
        ++parsing_info->token_array_position;

        tree_node_pointer = get_sum(parsing_info, tree_pointer);

        syn_assert(CURRENT_TOKEN.type == PARENTHESIS && CURRENT_TOKEN.value.is_closing_parenthesis == true);
        ++parsing_info->token_array_position;

        return tree_node_pointer;
    }

    if (!(tree_node_pointer = get_number(parsing_info, tree_pointer))) {
        tree_node_pointer   = get_variable(parsing_info, tree_pointer);
    }

    return tree_node_pointer;
}

static tree_node *get_number(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    if(CURRENT_TOKEN.type == NUMBER)
    {
        tree_node_pointer = create_node(NUMBER, CURRENT_TOKEN.value, NULL, NULL);
        ++parsing_info->token_array_position;
        ++tree_pointer->size;
    }

    return tree_node_pointer;
}

tree_node *get_variable(parsing_info_t *parsing_info, tree *tree_pointer)
{
    MYASSERT(parsing_info              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(parsing_info->token_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);
    MYASSERT(tree_pointer              != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    tree_node *tree_node_pointer = NULL;

    ssize_t string_index_old = parsing_info->token_array_position;

    if(CURRENT_TOKEN.type == VARIABLE)
    {
        tree_node_pointer = create_node(VARIABLE, CURRENT_TOKEN.value, NULL, NULL);
        ++parsing_info->token_array_position;
        ++tree_pointer->size;
    }

    syn_assert(string_index_old < parsing_info->token_array_position);

    return tree_node_pointer;
}
