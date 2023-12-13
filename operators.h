#ifndef OPERATORS_H_INCLUDED
#define OPERATORS_H_INCLUDED

#include "differentiator.h"

typedef double     (*calculation_function)     (double,      double);
typedef tree_node *(*differentiation_function) (tree_node *, tree_node *, ssize_t);

struct operator_parametrs {
    const char              *name;
    bool                     is_binary;
    calculation_function     calculate_func;
    differentiation_function differentiate_func;
    size_t                   priority;
};

double calculation_mul(double left_value, double right_value);
double calculation_add(double left_value, double right_value);
double calculation_sub(double left_value, double right_value);
double calculation_div(double left_value, double right_value);
double calculation_pow(double left_value, double right_value);
double calculation_sin(double left_value, double right_value);
double calculation_cos(double left_value, double right_value);
double calculation_tan(double left_value, double right_value);
double calculation_log(double left_value, double right_value);
double calculation_exp(double left_value, double right_value);

tree_node *differentiation_add(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_mul(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_sub(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_div(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_pow(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_exp(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_log(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_tan(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_sin(tree_node *left_node, tree_node *right_node, ssize_t variable_index);
tree_node *differentiation_cos(tree_node *left_node, tree_node *right_node, ssize_t variable_index);

enum operators_index {
    MUL         = 0,
    ADD         = 1,
    SUB         = 2,
    DIV         = 3,
    POW         = 4,
    SIN         = 5,
    COS         = 6,
    TAN         = 7,
    LOG         = 8,
    EXP         = 9
};

const operator_parametrs OPERATORS[] = {[MUL] = {"*",   true,  calculation_mul, differentiation_mul, 3},
                                        [ADD] = {"+",   true,  calculation_add, differentiation_add, 4},
                                        [SUB] = {"-",   true,  calculation_sub, differentiation_sub, 4},
                                        [DIV] = {"/",   true,  calculation_div, differentiation_div, 3},
                                        [POW] = {"^",   true,  calculation_pow, differentiation_pow, 2},
                                        [SIN] = {"sin", false, calculation_sin, differentiation_sin, 1},
                                        [COS] = {"cos", false, calculation_cos, differentiation_cos, 1},
                                        [TAN] = {"tan", false, calculation_tan, differentiation_tan, 1},
                                        [LOG] = {"log", false, calculation_log, differentiation_log, 1},
                                        [EXP] = {"exp", false, calculation_exp, differentiation_exp, 1}};

#endif //OPERATORS_H_INCLUDED

