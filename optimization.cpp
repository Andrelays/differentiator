#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "dsl.h"
#include "operators.h"

static tree_node *optimize_constants     (tree_node *tree_node_pointer);
static tree_node *remove_neutral_elements(tree_node *tree_node_pointer, bool *node_is_change);

tree_node *optimize_node(tree_node *tree_node_pointer)
{
    MYASSERT(tree_node_pointer != NULL, NULL_POINTER_PASSED_TO_FUNC, return NULL);

    bool node_is_change = false;

    do {
        node_is_change = false;

        tree_node_pointer = optimize_constants(tree_node_pointer);
        tree_node_pointer = remove_neutral_elements(tree_node_pointer, &node_is_change);

    } while (node_is_change);

    return tree_node_pointer;
}

#define RETURN_AND_DELETE_OPERATOR_(return_value)               \
 do {                                                           \
    delete_node_without_subtree(tree_node_pointer);             \
    return(return_value);                                       \
                                                                \
} while (0)

static tree_node *optimize_constants(tree_node *tree_node_pointer)
{
    if (!tree_node_pointer) {
        return NULL;
    }

    if (tree_node_pointer->type != OPERATOR) {
        return tree_node_pointer;
    }

    tree_node_pointer->left  = optimize_constants(tree_node_pointer->left);
    tree_node_pointer->right = optimize_constants(tree_node_pointer->right);

    if (tree_node_pointer->left->type == NUMBER)
    {
        double left_node_number  = tree_node_pointer->left->value.number;
        double right_node_number = 0;

        calculation_function calculate_func = CURRENT_CALCULATE_FUNC;

        if (!CURRENT_OPERATOR.is_binary)
        {
            delete_node(tree_node_pointer->left);

            RETURN_AND_DELETE_OPERATOR_(CREATE_NUM(calculate_func(left_node_number, 0)));
        }

        if (tree_node_pointer->right->type == NUMBER)
        {
            right_node_number = tree_node_pointer->right->value.number;

            delete_node(tree_node_pointer->left);
            delete_node(tree_node_pointer->right);

            RETURN_AND_DELETE_OPERATOR_(CREATE_NUM(calculate_func(left_node_number, right_node_number)));
        }
    }

    return tree_node_pointer;
}

#undef RETURN_AND_DELETE_OPERATOR_

#define RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(return_value)  \
 do {                                                           \
    delete_node_without_subtree(tree_node_pointer);             \
    *node_is_change = true;                                     \
    return(return_value);                                       \
                                                                \
} while (0)

static tree_node *remove_neutral_elements(tree_node *tree_node_pointer, bool *node_is_change)
{
    if (!tree_node_pointer) {
        return NULL;
    }

    if (tree_node_pointer->type != OPERATOR) {
        return tree_node_pointer;
    }

    tree_node_pointer->left  = remove_neutral_elements(tree_node_pointer->left,  node_is_change);
    tree_node_pointer->right = remove_neutral_elements(tree_node_pointer->right, node_is_change);

    tree_node *left_node = tree_node_pointer->left;
    tree_node *right_node = tree_node_pointer->right;

    switch(tree_node_pointer->value.operator_index)
    {
        case ADD:
        {
            if (node_is_equal_number(left_node, 0))
            {
                delete_node(left_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(right_node);
            }

            if (node_is_equal_number(right_node, 0))
            {
                delete_node(right_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            break;
        }

        case SUB:
        {
            if (node_is_equal_number(left_node, 0))
            {
                delete_node(left_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(CREATE_MUL(CREATE_NUM(-1), right_node));
            }

            if (node_is_equal_number(right_node, 0))
            {
                delete_node(right_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            break;
        }

        case MUL:
        {
            if (node_is_equal_number(left_node, 0))
            {
                delete_node(right_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            if (node_is_equal_number(right_node, 0))
            {
                delete_node(left_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(right_node);
            }

            if (node_is_equal_number(left_node, 1))
            {
                delete_node(left_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(right_node);
            }

            if (node_is_equal_number(right_node, 1))
            {
                delete_node(right_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            break;
        }

        case DIV:
        {
            if (node_is_equal_number(left_node, 0))
            {
                delete_node(right_node);
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            break;
        }

        case POW:
        {
            if (node_is_equal_number(right_node, 0))
            {
                delete_node(left_node);
                delete_node(right_node);
                
                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(CREATE_NUM(1));
            }

            if (node_is_equal_number(right_node, 1))
            {
                delete_node(right_node);

                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            if (node_is_equal_number(left_node, 1))
            {
                delete_node(right_node);

                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            if (node_is_equal_number(left_node, 0))
            {
                delete_node(right_node);

                RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_(left_node);
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return tree_node_pointer;
}

#undef RETURN_AND_DELETE_OPERATOR_WITH_CHANGES_
