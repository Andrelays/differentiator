#ifndef DSL_H_INCLUDED
#define DSL_H_INCLUDED

#define  CURRENT_VARIABLE            variable_array[tree_node_pointer->value.variable_index]
#define  CURRENT_CALCULATE_FUNC      OPERATORS[tree_node_pointer->value.operator_index].calculate_func
#define  CURRENT_OPERATOR            OPERATORS[tree_node_pointer->value.operator_index]
#define  CURRENT_DIFFERENTIATE_FUNC  OPERATORS[tree_node_pointer->value.operator_index].differentiate_func
#define  VALUE_OF_NODE               tree_node_pointer->value
#define  DIF_BY_CUR_VAR(node)        differentiate_node_by_variable(node, variable_index)


#define  CREATE_NUM(num)                   create_node(NUMBER,   {.number = num},         NULL,      NULL)
#define  CREATE_MUL(left_node, right_node) create_node(OPERATOR, {.operator_index = MUL}, left_node, right_node)
#define  CREATE_ADD(left_node, right_node) create_node(OPERATOR, {.operator_index = ADD}, left_node, right_node)
#define  CREATE_SUB(left_node, right_node) create_node(OPERATOR, {.operator_index = SUB}, left_node, right_node)
#define  CREATE_DIV(left_node, right_node) create_node(OPERATOR, {.operator_index = DIV}, left_node, right_node)
#define  CREATE_POW(left_node, right_node) create_node(OPERATOR, {.operator_index = POW}, left_node, right_node)
#define  CREATE_LOG(node)                  create_node(OPERATOR, {.operator_index = LOG}, node,      NULL)
#define  CREATE_EXP(node)                  create_node(OPERATOR, {.operator_index = EXP}, node,      NULL)
#define  CREATE_COS(node)                  create_node(OPERATOR, {.operator_index = COS}, node,      NULL)
#define  CREATE_SIN(node)                  create_node(OPERATOR, {.operator_index = SIN}, node,      NULL)


#endif
