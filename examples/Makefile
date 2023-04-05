SILENT = @
CC = gcc -g

SOURCES = $(shell find ./ -type f -name \*.c)
BINARY = $(patsubst ./%,./%,$(SOURCES:.c=))
INCLUDES_DIR = ../src/
OBJS = $(patsubst ./%,./%,$(SOURCES:.c=.o))

PARSER_DIR = ../bin
PARSER_LIB = parser

% : %.c
	$(SILENT) $(CC) $< $(CFLAGS) -I$(INCLUDES_DIR)/ -L$(PARSER_DIR) -l$(PARSER_LIB) -o $@ 

.PHONY: clean
clean:
	rm -f $(BINARY)

.PHONY: debug
debug: CFLAGS += -DDEBUG -g
debug: $(BINARY)