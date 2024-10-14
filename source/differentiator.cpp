#include "myassert.h"
#include "colors.h"
#include "stack.h"
#include "onegin.h"
#include "utilities.h"
#include "differentiator.h"
#include "dsl.h"
#include "operators.h"

tree_node *differentiate_node_by_variable(tree_node *tree_node_pointer, ssize_t variable_index)
{
    MYASSERT(tree_node_pointer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    if (tree_node_pointer->type == VARIABLE && VALUE_OF_NODE.variable_index == variable_index)
    {
        return CREATE_NUM(1);
    }

    if (tree_node_pointer->type == NUMBER || tree_node_pointer->type == VARIABLE)
    {
        return CREATE_NUM(0);
    }

    MYASSERT(CURRENT_DIFFERENTIATE_FUNC != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    return optimize_node(CURRENT_DIFFERENTIATE_FUNC(tree_node_pointer->left, tree_node_pointer->right, variable_index));
}
