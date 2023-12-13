#include <string.h>
#include <stdio.h>
#include "libraries/utilities/myassert.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"

int main(int argc, const char *argv[])
{
    const int CORRECT_NUMBER_OF_ARGC = 4;

    if(!check_argc(argc, CORRECT_NUMBER_OF_ARGC)) {
        return INCORRECT_NUMBER_OF_ARGC;
    }

    const char *file_name_logs   = argv[1];
    const char *file_name_input  = argv[2];
    const char *file_name_output = argv[3];


    Global_logs_pointer_tree = check_isopen_html(file_name_logs, "w");
    FILE *file_input         = check_isopen(file_name_input,     "r");
    FILE *file_output        = check_isopen(file_name_output,    "w");


    MYASSERT(Global_logs_pointer_tree != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);
    MYASSERT(file_input               != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);
    MYASSERT(file_output              != NULL, COULD_NOT_OPEN_THE_FILE , return COULD_NOT_OPEN_THE_FILE);

    tree *tree_pointer = new_pointer_tree();

    TREE_CONSTRUCTOR(tree_pointer);

    input_tree_from_database(file_input, tree_pointer);

    printf("%g\n", calcucalate_node(tree_pointer->root, tree_pointer->variable_array));

    tree_node *node_pointer = differentiate_node_by_variable(tree_pointer->root, 0);
    delete_node(tree_pointer->root);
    tree_pointer->root = node_pointer;

    print_node(file_output, tree_pointer->root, tree_pointer->variable_array, tree_pointer->root);

    // print_node(file_output, tree_pointer->root, tree_pointer->variable_array, tree_pointer->root);

    tree_destructor(tree_pointer);

    MYASSERT(check_isclose(Global_logs_pointer_tree), COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);
    MYASSERT(check_isclose(file_input),               COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);
    MYASSERT(check_isclose(file_output),              COULD_NOT_CLOSE_THE_FILE , return COULD_NOT_CLOSE_THE_FILE);

    return 0;
}
