#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "dsl.h"
#include "operators.h"

double calcucalate_node(tree_node *tree_node_pointer, const variable_parametrs *variable_array)
{
    MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return 0);

    if (!tree_node_pointer) {
        return 0;
    }

    if (tree_node_pointer->type == NUMBER) {
        return tree_node_pointer->value.number;
    }

    if (tree_node_pointer->type == VARIABLE) {
        return CURRENT_VARIABLE.value;
    }

    double left_value  = calcucalate_node(tree_node_pointer->left,  variable_array);
    double right_value = calcucalate_node(tree_node_pointer->right, variable_array);

    return CURRENT_CALCULATE_FUNC(left_value, right_value);
}
