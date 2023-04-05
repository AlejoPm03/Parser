#include "parser.h"

/**
 * @brief Parse one level of the tree receiving the buffer and the node that it will match
 *
 * @param buffer the buffer containing the data
 * @param nodes nodes that the buffer will match
 * @return const nodes_t* the next level of the tree
 */
const nodes_t *node_parser(buffer_t *buffer, const nodes_t *nodes);

const nodes_t *node_parser(buffer_t *buffer, const nodes_t *nodes)
{
    const node_t *node = nodes->nodes;
    LOG_INFO("parsing node %s", nodes->name);
    /* Check for a match in the level of the tree */
    for (int i = 0; i < nodes->size; i++)
    {
        if (nodes->match_func == NULL)
        {
            LOG_ERROR("inexistent match function");
            return NULL;
        }

        /* check if buffer data match with the node or if is a wildcard node*/
        if (node[i].value == NULL || nodes->match_func(buffer->actual, node[i].value) == EQUAL)
        {
            if (node[i].callback != NULL)
                node[i].callback(buffer, node[i].value);

            LOG_INFO("next node is %s", node[i].next != NULL ? node[i].next->name : "NULL");
            return node[i].next;
        }
    }
    LOG_WARN("Unrecognized command\n");
    return NULL;
}

void parser(buffer_t *buffer, const nodes_t *root_node)
{
    const nodes_t *nodes = root_node;
    for (parser_consumer_data_t result = OK; result == OK; result = buffer->consumer(buffer))
    {
        if (nodes == NULL)
        {
            LOG_INFO("end of tree");
            return;
        }
        nodes = node_parser(buffer, nodes);
    }
}