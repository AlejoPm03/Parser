#include "parser.h"
#include <string.h>

parser_match_t match_func(void *data, const void *match_value)
{
    LOG_INFO("tryng match");

    if (!memcmp((char *)data, (const char *)match_value, 5))
    {
        LOG_INFO("equal");
        return EQUAL;
    }
    LOG_INFO("not equal: %s, %s", (char *)data, (char *)match_value);
    return NOT_EQUAL;
}

parser_consumer_data_t consumer(struct buffer_t *buffer)
{
    if (buffer->actual == NULL)
    {
        LOG_ERROR("error reading buffer: buffer is NULL");
        return ERROR;
    }

    /* Message have 5 chars size */
    buffer->actual += sizeof(char) * 5;

    if (buffer->actual >= buffer->end)
    {
        LOG_INFO("end of buffer");
        return END_OF_BUFFER;
    }

    return OK;
}

int buffer2int(buffer_t *buffer)
{
    return atoi((char *)buffer->actual); 
}

void write_var1(buffer_t *buffer, const void *value)
{
    printf("writing var1, value %d\n", buffer2int(buffer));
}

void write_var2(buffer_t *buffer, const void *value)
{
    printf("writing var2, value %d\n", buffer2int(buffer));
}

void write_var3(buffer_t *buffer, const void *value)
{
    printf("writing var3, value %d\n", buffer2int(buffer));
}

void read_allm(buffer_t *buffer, const void *value)
{
    printf("Reading all measurements\n");
}




int main()
{
    char uart_buffer[] = "WRTE:VAR2:VAR3:1000";

    buffer_t buffer = {
        .begin = uart_buffer,
        .actual = uart_buffer,
        .end = &uart_buffer[sizeof(uart_buffer) / sizeof(char)],
        .consumer = &consumer};


    MAKE_NODES(
        var1,
        &match_func,
        {
            MAKE_WILDCARD_NODE(&write_var1, NULL),
        });

    MAKE_NODES(
        var3,
        &match_func,
        {
            MAKE_WILDCARD_NODE(&write_var3, NULL),
        });

    MAKE_NODES(
        var2,
        &match_func,
        {
            MAKE_NODE("VAR3:", NULL, &var3),
            MAKE_WILDCARD_NODE(&write_var2, NULL),
        });

    MAKE_NODES(
        write,
        &match_func,
        {
            MAKE_NODE("VAR1:", NULL, &var1),
            MAKE_NODE("VAR2:", NULL, &var2),
        });

    MAKE_NODES(
        allm,
        &match_func,
        {
            MAKE_WILDCARD_NODE(&read_allm, NULL),
        });

    MAKE_NODES(
        read,
        &match_func,
        {
            MAKE_NODE("ALLM:", NULL, &allm),
        });

    MAKE_NODES(
        root,
        &match_func,
        {
            MAKE_NODE("WRTE:", NULL, &write),
            MAKE_NODE("READ:", NULL, &read),
        });

    parser(&buffer, &root);
}