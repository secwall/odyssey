#ifndef OD_SCHEME_H
#define OD_SCHEME_H

/*
 * odissey.
 *
 * PostgreSQL connection pooler and request router.
*/

typedef struct od_schemeserver od_schemeserver_t;
typedef struct od_schemeroute  od_schemeroute_t;
typedef struct od_schemeuser   od_schemeuser_t;
typedef struct od_scheme       od_scheme_t;

typedef enum
{
	OD_PUNDEF,
	OD_PSESSION,
	OD_PTRANSACTION
} od_pooling_t;

typedef enum
{
	OD_RUNDEF,
	OD_RFORWARD
} od_routing_t;

typedef enum
{
	OD_AUNDEF,
	OD_ANONE,
	OD_ACLEAR_TEXT,
	OD_AMD5
} od_auth_t;

typedef enum
{
	OD_TDISABLE,
	OD_TALLOW,
	OD_TREQUIRE,
	OD_TVERIFY_CA,
	OD_TVERIFY_FULL
} od_tls_t;

struct od_schemeserver
{
	int        id;
	char      *name;
	char      *host;
	int        port;
	od_tls_t   tls_verify;
	char      *tls_mode;
	char      *tls_ca_file;
	char      *tls_key_file;
	char      *tls_cert_file;
	char      *tls_protocols;
	int        is_default;
	od_list_t  link;
};

struct od_schemeroute
{
	od_schemeserver_t *server;
	int                is_default;
	char              *target;
	char              *route;
	char              *database;
	char              *user;
	int                user_len;
	char              *password;
	int                password_len;
	int                ttl;
	int                cancel;
	int                discard;
	int                rollback;
	int                client_max;
	int                pool_size;
	int                pool_timeout;
	od_list_t          link;
};

struct od_schemeuser
{
	char      *auth;
	od_auth_t  auth_mode;
	char      *user;
	char      *password;
	int        password_len;
	int        is_default;
	int        is_deny;
	od_list_t  link;
};

struct od_scheme
{
	char             *config_file;
	int               server_id;
	/* main */
	int               daemonize;
	int               log_debug;
	int               log_config;
	char             *log_file;
	char             *pid_file;
	int               syslog;
	char             *syslog_ident;
	char             *syslog_facility;
	int               stats_period;
	int               readahead;
	char             *pooling;
	od_pooling_t      pooling_mode;
	/* listen */
	char             *host;
	int               port;
	int               backlog;
	int               nodelay;
	int               keepalive;
	int               workers;
	int               client_max;
	od_tls_t          tls_verify;
	char             *tls_mode;
	char             *tls_ca_file;
	char             *tls_key_file;
	char             *tls_cert_file;
	char             *tls_protocols;
	/* servers */
	od_list_t         servers;
	/* routing */
	char             *routing;
	od_routing_t      routing_mode;
	od_schemeroute_t *routing_default;
	od_list_t         routing_table;
	/* users */
	od_list_t         users;
	od_schemeuser_t  *users_default;
};

void od_scheme_init(od_scheme_t*);
void od_scheme_free(od_scheme_t*);
int  od_scheme_validate(od_scheme_t*, od_log_t*);
void od_scheme_print(od_scheme_t*, od_log_t*);

od_schemeserver_t*
od_schemeserver_add(od_scheme_t*);

od_schemeserver_t*
od_schemeserver_match(od_scheme_t*, char*);

od_schemeroute_t*
od_schemeroute_add(od_scheme_t*);

od_schemeroute_t*
od_schemeroute_match(od_scheme_t*, char*);

od_schemeuser_t*
od_schemeuser_add(od_scheme_t*);

od_schemeuser_t*
od_schemeuser_match(od_scheme_t*, char*);

#endif /* OD_SCHEME_H */