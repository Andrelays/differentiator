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


void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer)
{
    MYASSERT(file_output                != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(parent_tree_node_pointer   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
    MYASSERT(variable_array             != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    if (!tree_node_pointer) {
        return;
    }

    if (tree_node_pointer->type == OPERATOR && (OPERATOR_PRIORITY(tree_node_pointer) > OPERATOR_PRIORITY(parent_tree_node_pointer))) {
        fprintf(file_output,  "( ");
    }

    print_node(file_output, tree_node_pointer->left, variable_array, tree_node_pointer);

    switch(tree_node_pointer->type)
    {
        case VARIABLE:
        {
            fprintf(file_output,  "%s ", variable_array[tree_node_pointer->value.variable_index].name);
            break;
        }

        case OPERATOR:
        {
            fprintf(file_output,  "%s " , OPERATORS[tree_node_pointer->value.operator_index].name);
            break;
        }

        case NUMBER:
        {
            fprintf(file_output, FORMAT_SPECIFIERS_TREE " ", tree_node_pointer->value.number);
            break;
        }

        case PARENTHESIS:
        case NO_TYPE:
        default:
        {
            printf(RED "ERROR! Incorrect data type when output to a file!\n" RESET_COLOR);
            return;
        }
    }

    print_node(file_output, tree_node_pointer->right, variable_array, tree_node_pointer);

    if (tree_node_pointer->type == OPERATOR && (OPERATOR_PRIORITY(tree_node_pointer) > OPERATOR_PRIORITY(parent_tree_node_pointer))) {
        fprintf(file_output, ") ");
    }
}
