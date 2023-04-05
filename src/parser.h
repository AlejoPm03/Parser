#ifndef PARSER_H
#define PARSER_H

#include "stdio.h"
#include "stdlib.h"

typedef enum parser_consumer_data
{
    OK,
    END_OF_BUFFER,
    ERROR,
} parser_consumer_data_t;

/**
 * Sequential buffer representation
 */
typedef struct buffer_t
{
    /* Points to the begin of the buffer*/
    const void *begin;
    /* Points to the to consume buffer*/
    void *actual;
    /* Points to the end of the buffer*/
    const void *end;
    /* Buffer consumer function */
    parser_consumer_data_t (*consumer)(struct buffer_t *buffer);
} buffer_t;

typedef enum parser_match
{
    NOT_EQUAL,
    EQUAL,
    UNDEFINED,
} parser_match_t;

typedef struct node_t
{
    /* Value pointer */
    const void *value;
    /* Handler function */
    void (*callback)(buffer_t *buffer, const void *value);
    /* Next node of the tree*/
    const struct nodes_t *next;
} node_t;

typedef struct nodes_t
{
    const char *name;
    const int size;
    parser_match_t (*match_func)(void *data, const void *match_value);
    const node_t *nodes;
} nodes_t;

/**
 * LOG prints macros, default log output is printf
 */
#ifndef LOG_INFO
#define LOG_INFO(...)     \
    printf("\033[0;32m"); \
    printf(__VA_ARGS__);  \
    printf("\033[0m\n")
#endif

#ifndef LOG_WARN
#define LOG_WARN(...)     \
    printf("\033[0;33m"); \
    printf(__VA_ARGS__);  \
    printf("\033[0m\n")
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(...)    \
    printf("\033[0;31m"); \
    printf(__VA_ARGS__);  \
    printf("\033[0m\n")
#endif

/**
 * Helper macro for make a array of nodes
 */
#define MAKE_NODES(NAME, MATCH_FUNC, ...)             \
    const node_t NAME##_NODE[] =                      \
        __VA_ARGS__;                                  \
    const nodes_t NAME = {                            \
        .name = #NAME,                                \
        .nodes = NAME##_NODE,                         \
        .match_func = MATCH_FUNC,                     \
        .size = sizeof(NAME##_NODE) / sizeof(node_t), \
    }

/**
 * Helper macro for make a empty array of nodes this is a workaround 
 * to create circular nodes.
 * To create circular nodes, first create an empty node before all nodes that have dependency
 * with this node, and after those nodes use the macro FILL_EMPTY_NODES to create the nodes
 * Example:
 * 
 * MAKE_EMPTY_NODES(foo, &func);
 * MAKE_NODES(bar, &func, {MAKE_NODE("bar", NULL, &foo)});
 * FILL_EMPTY_NODES(foo, {MAKE_NODE("foo", NULL, &bar)})
 * 
 * in this example the next node of foo is bar and the next node of bar is foo
 * 
 *  
 */
#define MAKE_EMPTY_NODES(NAME, MATCH_FUNC) \
    nodes_t NAME = {                       \
        .name = #NAME,                     \
        .match_func = MATCH_FUNC,          \
    }

/**
 * Helper macro to fill a empty node
 * NAME: nodes_t name
 * ...: array of node_t
 */
#define FILL_EMPTY_NODES(NAME, ...)         \
    const node_t NAME##_NODE[] =            \
        __VA_ARGS__;                        \
    NAME.nodes = NAME##_NODE; \
    *(int *)&NAME.size = sizeof(NAME##_NODE) / sizeof(node_t);

/**
 * Helper macro for crate parsing nodes
 * VALUE is the match case that it be compared with the buffer
 * CALLBACK is the callback function
 * NEXT is the next node
 */
#define MAKE_NODE(VALUE, CALLBACK, NEXT) \
    {                                    \
        .value = VALUE,                  \
        .callback = CALLBACK,            \
        .next = NEXT,                    \
    }

/**
 * @brief parser function, receives the buffer and the tree description and parse the buffer
 * @param buffer: buffer to parse
 * @param root_node tree description 
 */
void parser(buffer_t *buffer, const nodes_t *root_node);

#endif