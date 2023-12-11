#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "libraries/utilities/myassert.h"
#include "libraries/utilities/colors.h"
#include "libraries/stack/stack.h"
#include "libraries/onegin/onegin.h"
#include "libraries/utilities/utilities.h"
#include "differentiator.h"
#include "dsl.h"

FILE *Global_logs_pointer_tree  = stderr;
bool  Global_color_output_tree  = true;

#define CHECK_ERRORS(tree)                                          \
do {                                                                \
    if (((tree)->error_code = VERIFY_TREE(tree)) != TREE_NO_ERROR)  \
        return (tree)->error_code;                                  \
} while(0)

tree *new_pointer_tree()
{
    struct tree *tree_pointer = (tree *) calloc(1, sizeof(tree));

    MYASSERT(tree_pointer != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    return tree_pointer;
}

ssize_t tree_constructor(tree *tree_pointer, debug_info_tree *info)
{
    MYASSERT(tree_pointer  != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(info          != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    tree_pointer->info = info;
    tree_pointer->size = 0;

    // tree_pointer->root = new_tree_node();

    return (VERIFY_TREE(tree_pointer));
}

tree_node *new_tree_node()
{
    tree_node *tree_node_pointer = (tree_node *) calloc(1, sizeof(tree_node));

    MYASSERT(tree_node_pointer != NULL, FAILED_TO_ALLOCATE_DYNAM_MEMOR, return NULL);

    tree_node_pointer->value.number = POISON_TREE;
    tree_node_pointer->type         = NO_TYPE;

    return tree_node_pointer;
}

ssize_t tree_destructor(tree *tree_pointer)
{
    MYASSERT(tree_pointer          != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(tree_pointer->info    != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    CHECK_ERRORS(tree_pointer);

    delete_node(tree_pointer->root);
    free(tree_pointer->info);

    tree_pointer->info = NULL;
    tree_pointer->root = NULL;

    tree_pointer->size = -1;

    free(tree_pointer);

    return TREE_NO_ERROR;
}

void delete_node(tree_node *tree_node_pointer)
{
    if (!tree_node_pointer) {
        return;
    }

    delete_node(tree_node_pointer->left);
    delete_node(tree_node_pointer->right);

    delete_node_without_subtree(tree_node_pointer);
}

void delete_node_without_subtree(tree_node *tree_node_pointer)
{
    MYASSERT(tree_node_pointer != NULL, NULL_POINTER_PASSED_TO_FUNC, return);

    tree_node_pointer->value.number = POISON_TREE;
    tree_node_pointer->type         = NO_TYPE;
    tree_node_pointer->left         = NULL;
    tree_node_pointer->right        = NULL;

    free(tree_node_pointer);
}

tree_node *create_node(type_node type, value_node value, tree_node *tree_node_left, tree_node *tree_node_right)
{
    tree_node *tree_node_pointer = new_tree_node();

    tree_node_pointer->type  = type;
    tree_node_pointer->left  = tree_node_left;
    tree_node_pointer->right = tree_node_right;
    tree_node_pointer->value = value;

    return tree_node_pointer;
}

bool check_node_is_number(tree_node *tree_node_pointer)
{
    if (!tree_node_pointer || tree_node_pointer->type == NUMBER) {
        return true;
    }

    if (tree_node_pointer->type == VARIABLE) {
        return false;
    }

    if (!check_node_is_number(tree_node_pointer->left) || !check_node_is_number(tree_node_pointer->right)) {
        return false;
    }

    return true;
}

tree_node *copying_node(tree_node *src_tree_node)
{
    if (!src_tree_node) {
        return NULL;
    }

    tree_node *des_tree_node = new_tree_node();
    des_tree_node->value = src_tree_node->value;
    des_tree_node->type  = src_tree_node->type;

    des_tree_node->left  = copying_node(src_tree_node->left);
    des_tree_node->right = copying_node(src_tree_node->right);

    return des_tree_node;
}

bool node_is_equal_number(tree_node *tree_node_pointer, double number)
{
    MYASSERT(tree_node_pointer != NULL, NULL_POINTER_PASSED_TO_FUNC, return false);

    if (tree_node_pointer->type == NUMBER && check_equal_with_accuracy(tree_node_pointer->value.number, number, NEAR_ZERO)) {
        return true;
    }

    return false;
}

ssize_t verify_tree(tree *tree_pointer, ssize_t line, const char *file, const char *func)
{
    MYASSERT(tree_pointer          != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_IS_NULL);
    MYASSERT(tree_pointer->info    != NULL, NULL_POINTER_PASSED_TO_FUNC, return POINTER_TO_TREE_INFO_IS_NULL);

    ssize_t error_code = TREE_NO_ERROR;

    #define SUMMARIZE_ERRORS_(condition, added_error)   \
    do {                                                \
        if((condition))                                 \
            error_code += added_error;                  \
    } while(0)

    // SUMMARIZE_ERRORS_();

    #undef SUMMARIZE_ERRORS_

    tree_pointer->error_code = error_code;

    IF_ON_TREE_DUMP
    (
        if (error_code != TREE_NO_ERROR) {
            tree_dump(tree_pointer, line, file, func);
        }
    )

    tree_dump(tree_pointer, line, file, func);

    return tree_pointer->error_code;
}
