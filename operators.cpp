#include <stdio.h>
#include <math.h>
#include "operators.h"
#include "differentiator.h"
#include "libraries/utilities/colors.h"
#include "libraries/utilities/utilities.h"
#include "dsl.h"

#define UNUSED(x)   \
do {                \
    (void)(x);      \
} while(0)

const double PI = 3.14159265;

double calculation_mul(double left_value, double right_value)
{
    return left_value * right_value;
}

double calculation_add(double left_value, double right_value)
{
    return left_value + right_value;
}

double calculation_sub(double left_value, double right_value)
{
    return left_value - right_value;
}

double calculation_div(double left_value, double right_value)
{
    if (check_equal_with_accuracy(right_value, 0, NEAR_ZERO))
    {
        printf(RED "ERROR! Division by zero.\n" RESET_COLOR);
        return 0;
    }

    return left_value / right_value;
}

double calculation_pow(double left_value, double right_value)
{
    if (check_equal_with_accuracy(left_value, 0, NEAR_ZERO) && check_equal_with_accuracy(right_value, 0, NEAR_ZERO))
    {
        printf(RED "ERROR! Zero to the power of zero.\n" RESET_COLOR);
        return 0;
    }

    return pow(left_value, right_value);
}

double calculation_sin(double left_value, double right_value)
{
    UNUSED(right_value);

    return sin(left_value);
}

double calculation_cos(double left_value, double right_value)
{
    UNUSED(right_value);

    return cos(left_value);
}

double calculation_tan(double left_value, double right_value)
{
    UNUSED(right_value);

    if (check_equal_with_accuracy(left_value, PI / 2, NEAR_ZERO))
    {
        printf(RED "ERROR! Tangent of pi/2.\n" RESET_COLOR);
        return 0;
    }

    return tan(left_value);
}

double calculation_log(double left_value, double right_value)
{
    UNUSED(right_value);

    if (left_value <= 0)
    {
        printf(RED "ERROR! Logarithm of a number less than 0.\n" RESET_COLOR);
        return 0;
    }

    return log(left_value);
}

double calculation_exp(double left_value, double right_value)
{
    UNUSED(right_value);

    return exp(left_value);
}

tree_node *differentiation_add(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    return CREATE_ADD(DIF_BY_CUR_VAR(left_node), DIF_BY_CUR_VAR(right_node));
}

tree_node *differentiation_mul(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    return CREATE_ADD(CREATE_MUL(DIF_BY_CUR_VAR(left_node), copying_node(right_node)), CREATE_MUL(copying_node(left_node), DIF_BY_CUR_VAR(right_node)));
}

tree_node *differentiation_sub(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    return CREATE_SUB(DIF_BY_CUR_VAR(left_node), DIF_BY_CUR_VAR(right_node));
}

tree_node *differentiation_div(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    return CREATE_DIV(CREATE_SUB(CREATE_MUL(DIF_BY_CUR_VAR(left_node), copying_node(right_node)), CREATE_MUL(copying_node(left_node), DIF_BY_CUR_VAR(right_node))), CREATE_POW(copying_node(right_node), CREATE_NUM(2)));
}

tree_node *differentiation_exp(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    UNUSED(right_node);

    return CREATE_MUL(CREATE_EXP(copying_node(left_node)), DIF_BY_CUR_VAR(left_node));
}

tree_node *differentiation_log(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    UNUSED(right_node);

    return CREATE_MUL(CREATE_DIV(CREATE_NUM(1), copying_node(left_node)), DIF_BY_CUR_VAR(left_node));
}

tree_node *differentiation_sin(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    UNUSED(right_node);

    return CREATE_MUL(CREATE_COS(copying_node(left_node)), DIF_BY_CUR_VAR(left_node));
}

tree_node *differentiation_cos(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    UNUSED(right_node);

    return CREATE_MUL(CREATE_MUL(CREATE_NUM(-1), CREATE_SIN(copying_node(left_node))), DIF_BY_CUR_VAR(left_node));
}

tree_node *differentiation_tan(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    UNUSED(right_node);

    return CREATE_MUL(CREATE_DIV(CREATE_NUM(1), CREATE_POW(CREATE_COS(copying_node(left_node)), CREATE_NUM(2))), DIF_BY_CUR_VAR(left_node));
}

tree_node *differentiation_pow(tree_node *left_node, tree_node *right_node, ssize_t variable_index)
{
    if (check_node_is_number(right_node)) {
        return CREATE_MUL(CREATE_MUL(copying_node(right_node), CREATE_POW(copying_node(left_node), CREATE_NUM(right_node->value.number - 1))), DIF_BY_CUR_VAR(left_node));
    }

    if (check_node_is_number(left_node)) {
        return CREATE_MUL(CREATE_MUL(CREATE_LOG(copying_node(left_node)), CREATE_POW(copying_node(left_node), copying_node(right_node))), DIF_BY_CUR_VAR(right_node));
    }

    tree_node *exp_node = CREATE_EXP(CREATE_MUL(copying_node(right_node), CREATE_LOG(copying_node(left_node))));
    tree_node *dif_node = DIF_BY_CUR_VAR(exp_node);

    delete_node(exp_node);

    return dif_node;
}

