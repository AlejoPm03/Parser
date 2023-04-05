#include "parser.h"
#include <string.h>

parser_consumer_data_t consumer(struct buffer_t *buffer)
{
    if (buffer->actual == NULL)
    {
        LOG_ERROR("error reading buffer: buffer is NULL");
        return ERROR;
    }

    buffer->actual += sizeof(char **);

    if (buffer->actual >= buffer->end)
    {
        LOG_INFO("end of buffer");
        return END_OF_BUFFER;
    }

    return OK;
}



parser_match_t match_func(void *data, const void *match_value)
{
    LOG_INFO("tryng match");
    if (!strcmp(*(char **)data, (char *)match_value))
    {
        LOG_INFO("equal");
        return EQUAL;
    }
    LOG_INFO("not equal: %s, %s", *(char **)data, (char *)match_value);
    return NOT_EQUAL;
}

void parse_help(buffer_t *buffer, const void *value)
{
    printf("arguments:\n\
    -help: this message.\n\
    -echo:  display the next argument\n");
}

void parse_echo(buffer_t *buffer, const void *value)
{
    printf("%s\n", *(char **)buffer->actual);
}


int main(int argc, char *argv[])
{

    buffer_t buffer = {
        .begin = argv,
        .actual = argv,
        .end = &argv[argc],
        .consumer = &consumer,
    };

    MAKE_EMPTY_NODES(
        sub_root,
        &match_func
    );
    
    MAKE_NODES(
        echo,
        &match_func,
        {
            MAKE_WILDCARD_NODE(parse_echo, &sub_root),
        });

    FILL_EMPTY_NODES(
        sub_root,
        {
            /* Read help node and read next arguments*/
            MAKE_NODE("-help", parse_help, &sub_root),
            MAKE_NODE("-echo", NULL, &echo),
        });

    MAKE_NODES(
        root,
        &match_func,
        {
            /* Ignore filename */
            MAKE_WILDCARD_NODE(NULL, &sub_root),
        });

    
    parser(&buffer, &root);
}