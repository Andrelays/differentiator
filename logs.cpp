#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "operators.h"
#include "dsl.h"

IF_ON_TREE_DUMP
(
    static void print_errors            (const tree *tree_pointer);
    static void print_debug_info        (const tree *tree_pointer, ssize_t line, const char *file, const char *func);
    static void generate_graph_of_tree  (tree *tree_pointer, ssize_t line, const char *file, const char *func);
    static void write_log_to_dot        (const tree *tree_pointer, FILE *dot_file, ssize_t line, const char *file, const char *func);
    static void write_subtree_to_dot    (const variable_parametrs *variable_array, const tree_node *tree_node_pointer, FILE *dot_file);
    static void generate_image          (const tree *tree_pointer, const char *name_dot_file, ssize_t number_graph);
)

IF_ON_TREE_DUMP
(
    void tree_dump(tree *tree_pointer, ssize_t line, const char *file, const char *func)
    {
        MYASSERT(tree_pointer               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info         != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(Global_logs_pointer_tree   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(file                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(func                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        print_errors(tree_pointer);

        print_debug_info(tree_pointer, line, file, func);

        generate_graph_of_tree(tree_pointer, line, file, func);
    }
)

IF_ON_TREE_DUMP
(
    static void print_debug_info(const tree *tree_pointer, ssize_t line, const char *file, const char *func)
    {
        MYASSERT(tree_pointer               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info         != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(Global_logs_pointer_tree   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(file                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(func                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        COLOR_PRINT(MediumBlue, "tree[%p]\n", tree_pointer);

        COLOR_PRINT(BlueViolet, "\"%s\"from %s(%ld) %s\n", tree_pointer->info->name, tree_pointer->info->file, tree_pointer->info->line, tree_pointer->info->func);

        COLOR_PRINT(DarkMagenta, "called from %s(%ld) %s\n", file, line, func);

        fprintf(Global_logs_pointer_tree,  "\nroot = ");
        COLOR_PRINT(Orange, "%p\n", tree_pointer->root);

        fprintf(Global_logs_pointer_tree, "size = ");
        COLOR_PRINT(Crimson, "%ld\n", tree_pointer->size);
    }
)

#define GET_ERRORS_(error)                                                           \
do {                                                                                 \
    if(tree_pointer->error_code & error)                                             \
        COLOR_PRINT(Red, "Errors: %s\n", #error);                                    \
} while(0)

IF_ON_TREE_DUMP
(
    static void print_errors(const tree *tree_pointer)
    {
        MYASSERT(tree_pointer               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info         != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(Global_logs_pointer_tree   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);


    }
)

#undef GET_ERRORS_

IF_ON_TREE_DUMP
(
    static void generate_graph_of_tree(tree *tree_pointer, ssize_t line, const char *file, const char *func)
    {
        MYASSERT(tree_pointer               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info         != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(Global_logs_pointer_tree   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(file                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(func                       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        const char  *NAME_DOT_FILE   = "tree.dot";
        static ssize_t number_graph = 0;

        ++number_graph;

        FILE *dot_file = check_isopen(NAME_DOT_FILE, "w");

        write_log_to_dot(tree_pointer, dot_file, line, file, func);

        MYASSERT(check_isclose(dot_file), COULD_NOT_CLOSE_THE_FILE , return);

        generate_image(tree_pointer, NAME_DOT_FILE, number_graph);

        fprintf(Global_logs_pointer_tree, "<img src = graph/tree_%ld.png height= 75%% width = 75%%>\n\n", number_graph);
    }
)

IF_ON_TREE_DUMP
(
    static void write_log_to_dot(const tree *tree_pointer, FILE *dot_file, ssize_t line, const char *file, const char *func)
    {
        MYASSERT(tree_pointer           != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info     != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(dot_file               != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(file                   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(func                   != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        fprintf(dot_file,   "digraph Tree {\n"
                            "\trankdir = TB;\n"
	                        "\tnode [shape = record];\n"
                            "\tsplines=ortho;\n");


        fprintf(dot_file,  "\tsubgraph cluster0 {\n"
                           "\t\tlabel = \"called  from:    %s(%ld)  %s\";\n", file, line, func);

        write_subtree_to_dot(tree_pointer->variable_array, tree_pointer->root, dot_file);

        fprintf(dot_file,   "\n\n\t\tInfo[shape = Mrecord, label = \"size = %ld \"];\n"
                            "\t}\n"
                            "}\n",
                tree_pointer->size);
    }
)

IF_ON_TREE_DUMP
(
    static void write_subtree_to_dot(const variable_parametrs *variable_array, const tree_node *tree_node_pointer, FILE *dot_file)
    {
        MYASSERT(dot_file       != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(variable_array != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        if (!tree_node_pointer) {
            return;
        }

        switch(tree_node_pointer->type)
        {
            case VARIABLE:
            {
                fprintf(dot_file,  "\t\tnode_%p  [shape = Mrecord, style = filled, fillcolor = \"#87cefa\", label = \"{TYPE\\n VAR | <left> L } | {VALUE\\n [%s = " FORMAT_SPECIFIERS_TREE "] | <right> R} \"];\n", tree_node_pointer, CURRENT_VARIABLE.name, CURRENT_VARIABLE.value);
                break;
            }

            case OPERATOR:
            {
                fprintf(dot_file,  "\t\tnode_%p  [shape = Mrecord, style = filled, fillcolor = \"#8fbc8f\", label = \"{TYPE\\n OPR | <left> L } | {VALUE\\n [%s] | <right> R} \"];\n", tree_node_pointer, OPERATORS[tree_node_pointer->value.operator_index].name);
                break;
            }

            case NUMBER:
            {
                fprintf(dot_file,  "\t\tnode_%p  [shape = Mrecord, style = filled, fillcolor = \"#ba7fa2\", label = \"{TYPE\\n NUM | <left> L } | {VALUE\\n [" FORMAT_SPECIFIERS_TREE "] | <right> R} \"];\n", tree_node_pointer, tree_node_pointer->value.number);
                break;
            }

            case NO_TYPE:
            default:
                fprintf(dot_file,  "\t\tnode_%p  [shape = Mrecord, style = filled, fillcolor = \"#ff4c5b\", label = \"{TYPE\\n ERROR! | <left> L } | {VALUE\\n [" FORMAT_SPECIFIERS_TREE "] | <right> R} \"];\n", tree_node_pointer, tree_node_pointer->value.number);
        }

        if (tree_node_pointer->left != NULL)
        {
            write_subtree_to_dot(variable_array, tree_node_pointer->left, dot_file);

            fprintf(dot_file, "\n\tnode_%p:<left> -> node_%p \t[color = blue];\n", tree_node_pointer, tree_node_pointer->left);
        }

        if (tree_node_pointer->right != NULL)
        {
            write_subtree_to_dot(variable_array, tree_node_pointer->right, dot_file);

            fprintf(dot_file, "\n\tnode_%p:<right> -> node_%p \t[color = blue];\n", tree_node_pointer, tree_node_pointer->right);
        }
    }
)


IF_ON_TREE_DUMP
(
    static void generate_image(const tree *tree_pointer, const char *name_dot_file, ssize_t number_graph)
    {
        MYASSERT(tree_pointer           != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(tree_pointer->info     != NULL, NULL_POINTER_PASSED_TO_FUNC, return);
        MYASSERT(name_dot_file          != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

        const size_t SIZE_NAME_GRAPH = 50;

        char image_generation_command[SIZE_NAME_GRAPH] = "";

        snprintf(image_generation_command, SIZE_NAME_GRAPH, "dot %s -T png -o graph/tree_%ld.png", name_dot_file, number_graph);

        MYASSERT(!system(image_generation_command), SYSTEM_ERROR, return);
    }
)
