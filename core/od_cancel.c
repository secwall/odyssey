
/*
 * odissey.
 *
 * PostgreSQL connection pooler and request router.
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <flint.h>
#include <soprano.h>

#include "od_macro.h"
#include "od_list.h"
#include "od_log.h"
#include "od_scheme.h"
#include "od_lex.h"
#include "od_config.h"
#include "od_server.h"
#include "od_server_pool.h"
#include "od_route_id.h"
#include "od_route.h"
#include "od_route_pool.h"
#include "od_client.h"
#include "od_client_pool.h"
#include "od.h"
#include "od_io.h"
#include "od_pooler.h"
#include "od_cancel.h"
#include "od_fe.h"
#include "od_be.h"

static inline int
od_cancel_cmp(odserver_t *server, void *arg)
{
	sokey_t *key = arg;
	return so_keycmp(&server->key_client, key);
}

int od_cancel_of(odpooler_t *pooler,
                 odscheme_server_t *server_scheme,
                 sokey_t *key)
{
	ftio_t io = ft_io_new(pooler->env);
	if (io == NULL)
		return -1;
	/* connect to server */
	int rc;
	rc = ft_connect(io, server_scheme->host,
	                server_scheme->port, 0);
	if (rc < 0) {
		od_error(&pooler->od->log, "(cancel) failed to connect to %s:%d",
		         server_scheme->host,
		         server_scheme->port);
		ft_close(io);
		return -1;
	}
	/* send cancel and disconnect */
	sostream_t stream;
	so_stream_init(&stream);
	rc = so_fewrite_cancel(&stream, key->key_pid, key->key);
	if (rc == -1) {
		ft_close(io);
		so_stream_free(&stream);
		return -1;
	}
	od_write(io, &stream);
	ft_close(io);
	so_stream_free(&stream);
	return 0;
}

int od_cancel(odpooler_t *pooler, sokey_t *key)
{
	/* match server by client key (forge) */
	odserver_t *server;
	server = od_routepool_foreach(&pooler->route_pool, OD_SACTIVE,
	                              od_cancel_cmp, key);
	if (server == NULL)
		return -1;
	odroute_t *route = server->route;
	odscheme_server_t *server_scheme = route->scheme->server;
	sokey_t cancel_key = server->key;
	return od_cancel_of(pooler, server_scheme, &cancel_key);
}