#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "info.h"

void command_list_append(struct command_list* new)
{
	if (commands == NULL) {
		commands = new;
		return;
	}

	struct command_list* node = commands;
	for (; node->next != NULL; node = node->next);
	node->next = new;
}

command_func command_list_find(char* name)
{
	if (commands == NULL)
		return NULL;

	struct command_list* node = commands;
	for (; strcmp(node->entry->name, name) != 0; node = node->next)
		if (node->next == NULL)
			return NULL;
	return node->entry->func;
}


void register_command(char* name, command_func func)
{
	struct command_entry* entry = malloc(sizeof(struct command_entry));
	struct command_list* node = malloc(sizeof(struct command_list));

	entry->name = name;
	entry->func = func;
	node->entry = entry;
	node->next = NULL;

	command_list_append(node);
}

char* get_command_body(char* message)
{
	if (strncmp(PREFIX, message, strlen(PREFIX)) == 0)
		return message + strlen(PREFIX);
	else
		return NULL;
}

char* get_command_name(char* body)
{
	return strtok(body, " ");
}

char* get_command_args(char* body)
{
	char* cur = body;
	for (; *cur != ' '; cur++)
		if (*cur == '\0')
			return "";
	return cur + 1;
}

// ----- Command Definitions -----

char* cmd_ping(char* args)
{
	return "Pong!";
}

void init_commands(void)
{
	register_command("ping", cmd_ping);
}
