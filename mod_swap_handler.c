/* vim: set ft=c ts=8 sw=8 noet : */

#include "httpd.h"

#include "ap_config.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#define MODULE_NAME "mod_swap_handler"
#define MODULE_VERSION "0.0.1"

module AP_MODULE_DECLARE_DATA swap_handler_module;

typedef struct {
	const char *from;
	const char *to;
} swaphandler;

typedef struct {
	apr_array_header_t *swaphandlers;
} swap_handler_server_config;

static void *create_swap_handler_server_config(apr_pool_t *p, server_rec *s)
{
	swap_handler_server_config *scfg;
	scfg = (swap_handler_server_config *)apr_pcalloc(
	    p, sizeof(swap_handler_server_config));
	scfg->swaphandlers = apr_array_make(p, 0, sizeof(swaphandler *));
	return scfg;
}

static int swap_handler_fixups_handler(request_rec *r)
{
	if (r->handler == NULL)
		return DECLINED;

	swap_handler_server_config *scfg;
	scfg = (swap_handler_server_config *)ap_get_module_config(
	    r->server->module_config, &swap_handler_module);

	apr_array_header_t *shs;
	shs = scfg->swaphandlers;

	for (int i = 0; i < shs->nelts; i++) {
		swaphandler *sh = APR_ARRAY_IDX(shs, i, swaphandler *);
		if (strcmp(r->handler, sh->from) == 0) {
			ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, NULL,
				     MODULE_NAME
				     ": r->handler changed from %s to %s",
				     r->handler, sh->to);
			r->handler = sh->to;
		}
	}

	return DECLINED;
}

static void swap_handler_register_hooks(apr_pool_t *p)
{
	ap_hook_fixups(swap_handler_fixups_handler, NULL, NULL,
		       APR_HOOK_MIDDLE);
}

static const char *swap_handler_set_swaphandler(cmd_parms *cmd, void *dummy,
						const char *arg1,
						const char *arg2)
{
	swap_handler_server_config *scfg;
	scfg = (swap_handler_server_config *)ap_get_module_config(
	    cmd->server->module_config, &swap_handler_module);

	swaphandler *sh;
	sh	 = (swaphandler *)apr_palloc(cmd->pool, sizeof(swaphandler));
	sh->from = arg1;
	sh->to	 = arg2;

	APR_ARRAY_PUSH(scfg->swaphandlers, swaphandler *) = sh;

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, NULL,
		     MODULE_NAME ": add swap configration from %s to %s",
		     sh->from, sh->to);

	return NULL;
}

static const command_rec swap_handler_cmds[] = {
    AP_INIT_TAKE2("SWAPHandler", swap_handler_set_swaphandler, NULL, RSRC_CONF,
		  "set swap handler."),
    {NULL}};

module AP_MODULE_DECLARE_DATA swap_handler_module = {
    STANDARD20_MODULE_STUFF,
    NULL,			       /* create per-dir    config structures */
    NULL,			       /* merge  per-dir    config structures */
    create_swap_handler_server_config, /* create per-server config structures */
    NULL,			       /* merge  per-server config structures */
    swap_handler_cmds,		       /* table of config file commands       */
    swap_handler_register_hooks	       /* register hooks                      */
};
