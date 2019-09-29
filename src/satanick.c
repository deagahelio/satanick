#include <signal.h>
#include <REST.h>

#include "libdiscord.h"
#include "command.h"
#include "info.h"

#define REPLY(text) ld_send_basic_message(context, message.channel_id, text)

struct ld_context_info info;
struct ld_context context;
CURL* handle;
int bot_die;

int callback(struct ld_context* context, enum ld_callback_reason reason, void* data, int len)
{
	if (reason != LD_CALLBACK_MESSAGE_CREATE)
		return 0;

	struct ld_json_message message;
	ld_json_message_init(&message);
	ld_json_pack_message(&message, (json_t*) data);

	if (message.author->id == context->current_user->id ||
	    message.author->bot == 1 ||
	    message.channel_id == 0)
		return 0;

	char* body = get_command_body(message.content);

	if (body == NULL)
		return 0;

	char* name = get_command_name(body);

	if (name == NULL) {
		REPLY("error: invalid command name");
		return 0;
	}

	char* args = get_command_args(body);

	command_func func = command_list_find(name);

	if (func == NULL) {
		REPLY("error: unknown command");
		return 0;
	}

	char* reply = func(args);

	if (reply != NULL)
		REPLY(reply);

	return 0;
}

void die()
{
	bot_die = 1;
}

int main()
{
	init_commands();

	ld_set_logging_level(31);
	ld_debug("Initializing");

	signal(SIGINT, die);

	ld_init_context_info(&info);

	info.bot_token = getenv("BOT_TOKEN");
	info.user_callback = callback;
	info.gateway_ringbuffer_size = 8;
	info.init_presence = NULL;

	if (ld_init_context(&info, &context) != 0) {
		ld_error("Couldn't create libdiscord context");
		return 1;
	}

	handle = curl_easy_init();

	int ret;

	while (!bot_die) {
		ret = ld_service(&context, 20);

		if (ret != LDS_OK) {
			ld_error("Died");
			bot_die = 1;
		}
	}

	ld_info("Disconnecting");

	return 0;
}
