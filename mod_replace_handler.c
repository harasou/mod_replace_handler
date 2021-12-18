/* vim: set ft=c ts=8 sw=8 noet : */

#include "httpd.h"

#include "ap_config.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

#define MODULE_NAME "mod_replace_handler"
#define MODULE_VERSION "0.0.1"

module AP_MODULE_DECLARE_DATA replace_handler_module;

typedef struct {
	const char *from;
	const char *to;
} replacehandler;

typedef struct {
	apr_array_header_t *replacehandlers;
} replace_handler_server_config;

static void *create_replace_handler_server_config(apr_pool_t *p, server_rec *s)
{
	replace_handler_server_config *scfg;
	scfg = (replace_handler_server_config *)apr_pcalloc(
	    p, sizeof(replace_handler_server_config));
	scfg->replacehandlers = apr_array_make(p, 0, sizeof(replacehandler *));
	return scfg;
}

static int replace_handler_fixups_handler(request_rec *r)
{
	if (r->handler == NULL)
		return DECLINED;

	replace_handler_server_config *scfg;
	scfg = (replace_handler_server_config *)ap_get_module_config(
	    r->server->module_config, &replace_handler_module);

	apr_array_header_t *shs;
	shs = scfg->replacehandlers;

	for (int i = 0; i < shs->nelts; i++) {
		replacehandler *sh = APR_ARRAY_IDX(shs, i, replacehandler *);
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

static void replace_handler_register_hooks(apr_pool_t *p)
{
	ap_hook_fixups(replace_handler_fixups_handler, NULL, NULL,
		       APR_HOOK_MIDDLE);
}

static const char *replace_handler_set_replacehandler(cmd_parms *cmd,
						      void *dummy,
						      const char *arg1,
						      const char *arg2)
{
	replace_handler_server_config *scfg;
	scfg = (replace_handler_server_config *)ap_get_module_config(
	    cmd->server->module_config, &replace_handler_module);

	replacehandler *sh;
	sh = (replacehandler *)apr_palloc(cmd->pool, sizeof(replacehandler));
	sh->from = arg1;
	sh->to	 = arg2;

	APR_ARRAY_PUSH(scfg->replacehandlers, replacehandler *) = sh;

	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, NULL,
		     MODULE_NAME ": add replace configration from %s to %s",
		     sh->from, sh->to);

	return NULL;
}

static const command_rec replace_handler_cmds[] = {
    AP_INIT_TAKE2("ReplaceHandler", replace_handler_set_replacehandler, NULL,
		  RSRC_CONF, "set replace handler."),
    {NULL}};

module AP_MODULE_DECLARE_DATA replace_handler_module = {
    STANDARD20_MODULE_STUFF,
    NULL, /* create per-dir    config structures */
    NULL, /* merge  per-dir    config structures */
    create_replace_handler_server_config, /* create per-server config structures
					   */
    NULL,			   /* merge  per-server config structures */
    replace_handler_cmds,	   /* table of config file commands       */
    replace_handler_register_hooks /* register hooks                      */
};
