/**
 * @file arguments.c
 * @brief This example implement a simple command argument parser
 */
#include "parser.h"
#include <string.h>

/**
 * @brief Consume the buffer, update the actual 
 * pointer to the next item of the argument array
 * @param buffer arguments buffer description 
 * @return parser_consumer_data_t 
 * return OK if the buffer was read with success
 * return END_OF_BUFFER if archived the end of the buffer
 * return ERROR if failed to read the buffer
 */
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

/**
 * @brief compare the matching node with the input argument
 * 
 * @param data array of arguments 
 * @param match_value the value of the node
 * @return parser_match_t if have match return EQUAL if not returns NOT_EQUAL
 */
parser_match_t match_func(void *data, const void *match_value)
{
	LOG_INFO("trying match");
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

	/* Create the buffer description*/
	buffer_t buffer = {
		.begin = argv,
		.actual = argv,
		.end = &argv[argc],
		.consumer = &consumer,
	};

/**
 * Describes the argument tree:
 * -help: output the help message
 * -echo: print the next argument
 */

/* Create a sub root node */
	MAKE_EMPTY_NODES(
		sub_root,
		&match_func
	);
	
/* Create the echo node */
	MAKE_NODES(
		echo,
		&match_func,
		{
			MAKE_WILDCARD_NODE(parse_echo, &sub_root),
		});

/* Fill the sub root node with the childs */
	FILL_EMPTY_NODES(
		sub_root,
		{
			/* Read help node and read next arguments*/
			MAKE_NODE("-help", parse_help, &sub_root),
			MAKE_NODE("-echo", NULL, &echo),
		});

/* Create the root node to ignore the filename */
	MAKE_NODES(
		root,
		&match_func,
		{
			/* Ignore filename */
			MAKE_WILDCARD_NODE(NULL, &sub_root),
		});

	
	parser(&buffer, &root);
}