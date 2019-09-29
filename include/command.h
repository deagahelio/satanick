#pragma once

#include <stddef.h>

typedef char* (*command_func)(char* args);

struct command_entry
{
	char* name;
	command_func func;
};

struct command_list
{
	struct command_entry* entry;
	struct command_list* next;
};

static struct command_list* commands = NULL;

void command_list_append(struct command_list* new);
command_func command_list_find(char* name);
void register_command(char* name, command_func func);
char* get_command_body(char* message);
char* get_command_name(char* body);
char* get_command_args(char* body);

void init_commands(void);
