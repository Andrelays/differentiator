#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "dsl.h"
#include "operators.h"

// differentiate_node(tree_node *tree_node_pointer, const variable_parametrs *variable_array)
// {
//
// }

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

    return CURRENT_DIFFERENTIATE_FUNC(tree_node_pointer->left, tree_node_pointer->right, variable_index);
}
