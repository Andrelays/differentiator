#ifndef DIFFERENTIATOR_H_INCLUDED
#define DIFFERENTIATOR_H_INCLUDED

#include <stdlib.h>

#ifdef DEBUG_OUTPUT_TREE_DUMP

    #define IF_ON_TREE_DUMP(...)   __VA_ARGS__

#else

    #define IF_ON_TREE_DUMP(...)

#endif

extern FILE *Global_logs_pointer_tree;
extern bool  Global_color_output_tree;

const size_t MAX_SIZE_NAME_VARIABLE = 50;
const size_t SIZE_ARRAY_OF_VARIABLE = 100;
const double NEAR_ZERO              = 1e-6;

#define VERIFY_TREE(tree_pointer) verify_tree(tree_pointer, __LINE__, __FILE__, __PRETTY_FUNCTION__)

#define TREE_CONSTRUCTOR(tree_pointer)                                                          \
do {                                                                                            \
    struct debug_info_tree *info = (debug_info_tree *) calloc(1, sizeof(debug_info_tree));      \
                                                                                                \
    info->line = __LINE__;                                                                      \
    info->name = #tree_pointer;                                                                 \
    info->file = __FILE__;                                                                      \
    info->func = __PRETTY_FUNCTION__;                                                           \
                                                                                                \
    tree_constructor(tree_pointer, info);                                                       \
} while(0)

#define FORMAT_SPECIFIERS_TREE   "%g"
typedef char TYPE_ELEMENT_TREE;
const double POISON_TREE = 192;

enum errors_code_tree {
    TREE_NO_ERROR                           = 0,
    POINTER_TO_TREE_IS_NULL                 = 1,
    POINTER_TO_TREE_INFO_IS_NULL            = 1 <<  1,
    POINTER_TO_TREE_NODE_IS_NULL            = 1 <<  2,
    DATABASE_BUFFER_IS_NULL                 = 1 <<  3,
    DATABASE_FILE_IS_NULL                   = 1 <<  4
};

enum type_node {
    NO_TYPE                = 0,
    NUMBER                 = 1,
    OPERATOR               = 2,
    VARIABLE               = 3
};

struct debug_info_tree {
    ssize_t      line;
    const char  *name;
    const char  *file;
    const char  *func;
};

union value_node {                                      //TODO auto-check of type
    double                  number;
    ssize_t                 operator_index;
    ssize_t                 variable_index;
};

struct tree_node {
    value_node               value;
    type_node                type;
    tree_node               *left;
    tree_node               *right;
};

struct variable_parametrs {
    char    name[MAX_SIZE_NAME_VARIABLE];
    double  value;
};

struct tree {
    tree_node               *root;
    ssize_t                  size;
    ssize_t                  error_code;
    variable_parametrs       variable_array[SIZE_ARRAY_OF_VARIABLE];
    ssize_t                  variable_array_position;
    struct debug_info_tree  *info;
};

IF_ON_TREE_DUMP(void       tree_dump           (tree *tree_pointer, ssize_t line, const char *file, const char *func));

tree      *new_pointer_tree    ();
ssize_t    tree_constructor    (tree *tree_pointer, debug_info_tree *info);
ssize_t    tree_destructor     (tree *tree_pointer);
ssize_t    verify_tree         (tree *tree_pointer, ssize_t line, const char *file, const char *func);
tree_node *new_tree_node       ();
void       delete_node         (tree_node *tree_node_pointer);
void       delete_node_without_subtree(tree_node *tree_node_pointer);
tree_node *create_node         (type_node type, value_node value, tree_node *tree_node_left, tree_node *tree_node_right);
tree_node *copying_node        (tree_node *src_tree_node);
bool       check_node_is_number(tree_node *tree_node_pointer);
bool       node_is_equal_number       (tree_node *tree_node_pointer, double number);


ssize_t input_tree_from_database(FILE *database_file, tree *tree_pointer);

void print_node(FILE *file_output, const tree_node *tree_node_pointer, const variable_parametrs *variable_array, const tree_node *parent_tree_node_pointer);

double calcucalate_node(tree_node *tree_node_pointer, const variable_parametrs *variable_array);

tree_node *differentiate_node_by_variable(tree_node *tree_node_pointer, ssize_t variable_index);

tree_node *optimize_node(tree_node *tree_node_pointer);

#endif //DIFFERENTIATOR_H_INCLUDED
