#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <assert.h>

/* Also update SONAME in the Makefile whenever you change these. */
#define HTTP_PARSER_VERSION_MAJOR 2
#define HTTP_PARSER_VERSION_MINOR 3
#define HTTP_PARSER_VERSION_PATCH 0

#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER<1600)
#include <BaseTsd.h>
#include <stddef.h>
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/* Compile with -DHTTP_PARSER_STRICT=0 to make less checks, but run
* faster
*/
#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 1
#endif

/* Maximium header size allowed. If the macro is not defined
* before including this header then the default is used. To
* change the maximum header size, define the macro in the build
* environment (e.g. -DHTTP_MAX_HEADER_SIZE=<value>). To remove
* the effective limit on the size of the header, define the macro
* to a very large number (e.g. -DHTTP_MAX_HEADER_SIZE=0x7fffffff)
*/
#ifndef HTTP_MAX_HEADER_SIZE
# define HTTP_MAX_HEADER_SIZE (80*1024)
#endif

typedef struct http_parser http_parser;
typedef struct http_parser_settings http_parser_settings;


/* Callbacks should return non-zero to indicate an error. The parser will
* then halt execution.
*
* The one exception is on_headers_complete. In a HTTP_RESPONSE parser
* returning '1' from on_headers_complete will tell the parser that it
* should not expect a body. This is used when receiving a response to a
* HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
* chunked' headers that indicate the presence of a body.
*
* http_data_cb does not return data chunks. It will be call arbitrarally
* many times for each string. E.G. you might get 10 callbacks for "on_url"
* each providing just a few characters more data.
*/
//typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);
//typedef int (*http_cb) (http_parser*);

typedef std::function<int (http_parser*, const char *, size_t)> http_data_cb;
typedef std::function<int (http_parser*)> http_cb;


/* Request Methods */
#define HTTP_METHOD_MAP(XX)         \
	XX(0,  DELETE,      DELETE)       \
	XX(1,  GET,         GET)          \
	XX(2,  HEAD,        HEAD)         \
	XX(3,  POST,        POST)         \
	XX(4,  PUT,         PUT)          \
	/* pathological */                \
	XX(5,  CONNECT,     CONNECT)      \
	XX(6,  OPTIONS,     OPTIONS)      \
	XX(7,  TRACE,       TRACE)        \
	/* webdav */                      \
	XX(8,  COPY,        COPY)         \
	XX(9,  LOCK,        LOCK)         \
	XX(10, MKCOL,       MKCOL)        \
	XX(11, MOVE,        MOVE)         \
	XX(12, PROPFIND,    PROPFIND)     \
	XX(13, PROPPATCH,   PROPPATCH)    \
	XX(14, SEARCH,      SEARCH)       \
	XX(15, UNLOCK,      UNLOCK)       \
	/* subversion */                  \
	XX(16, REPORT,      REPORT)       \
	XX(17, MKACTIVITY,  MKACTIVITY)   \
	XX(18, CHECKOUT,    CHECKOUT)     \
	XX(19, MERGE,       MERGE)        \
	/* upnp */                        \
	XX(20, MSEARCH,     M-SEARCH)     \
	XX(21, NOTIFY,      NOTIFY)       \
	XX(22, SUBSCRIBE,   SUBSCRIBE)    \
	XX(23, UNSUBSCRIBE, UNSUBSCRIBE)  \
	/* RFC-5789 */                    \
	XX(24, PATCH,       PATCH)        \
	XX(25, PURGE,       PURGE)        \

enum http_method
{
#define XX(num, name, string) HTTP_##name = num,
	HTTP_METHOD_MAP(XX)
#undef XX
};


enum http_parser_type { HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH };


/* Flag values for http_parser.flags field */
enum flags
{ F_CHUNKED               = 1 << 0
, F_CONNECTION_KEEP_ALIVE = 1 << 1
, F_CONNECTION_CLOSE      = 1 << 2
, F_TRAILING              = 1 << 3
, F_UPGRADE               = 1 << 4
, F_SKIPBODY              = 1 << 5
};


/* Map for errno-related constants
* 
* The provided argument should be a macro that takes 2 arguments.
*/
#define HTTP_ERRNO_MAP(XX)                                           \
	/* No error */                                                     \
	XX(OK, "success")                                                  \
	\
	/* Callback-related errors */                                      \
	XX(CB_message_begin, "the on_message_begin callback failed")       \
	XX(CB_url, "the on_url callback failed")                           \
	XX(CB_header_field, "the on_header_field callback failed")         \
	XX(CB_header_value, "the on_header_value callback failed")         \
	XX(CB_headers_complete, "the on_headers_complete callback failed") \
	XX(CB_body, "the on_body callback failed")                         \
	XX(CB_message_complete, "the on_message_complete callback failed") \
	XX(CB_status, "the on_status callback failed")                     \
	\
	/* Parsing-related errors */                                       \
	XX(INVALID_EOF_STATE, "stream ended at an unexpected time")        \
	XX(HEADER_OVERFLOW,                                                \
	"too many header bytes seen; overflow detected")                \
	XX(CLOSED_CONNECTION,                                              \
	"data received after completed connection: close message")      \
	XX(INVALID_VERSION, "invalid HTTP version")                        \
	XX(INVALID_STATUS, "invalid HTTP status code")                     \
	XX(INVALID_METHOD, "invalid HTTP method")                          \
	XX(INVALID_URL, "invalid URL")                                     \
	XX(INVALID_HOST, "invalid host")                                   \
	XX(INVALID_PORT, "invalid port")                                   \
	XX(INVALID_PATH, "invalid path")                                   \
	XX(INVALID_QUERY_STRING, "invalid query string")                   \
	XX(INVALID_FRAGMENT, "invalid fragment")                           \
	XX(LF_EXPECTED, "LF character expected")                           \
	XX(INVALID_HEADER_TOKEN, "invalid character in header")            \
	XX(INVALID_CONTENT_LENGTH,                                         \
	"invalid character in content-length header")                   \
	XX(INVALID_CHUNK_SIZE,                                             \
	"invalid character in chunk size header")                       \
	XX(INVALID_CONSTANT, "invalid constant string")                    \
	XX(INVALID_INTERNAL_STATE, "encountered unexpected internal state")\
	XX(STRICT, "strict mode assertion failed")                         \
	XX(PAUSED, "parser is paused")                                     \
	XX(UNKNOWN, "an unknown error occurred")


/* Define HPE_* values for each errno value above */
#define HTTP_ERRNO_GEN(n, s) HPE_##n,
enum http_errno {
	HTTP_ERRNO_MAP(HTTP_ERRNO_GEN)
};
#undef HTTP_ERRNO_GEN


/* Get an http_errno value from an http_parser */
#define HTTP_PARSER_ERRNO(p)            ((enum http_errno) (p)->http_errno)


struct http_parser {
	/** PRIVATE **/
	unsigned int type : 2;         /* enum http_parser_type */
	unsigned int flags : 6;        /* F_* values from 'flags' enum; semi-public */
	unsigned int state : 8;        /* enum state from http_parser.c */
	unsigned int header_state : 8; /* enum header_state from http_parser.c */
	unsigned int index : 8;        /* index into current matcher */

	uint32_t nread;          /* # bytes read in various scenarios */
	uint64_t content_length; /* # bytes in body (0 if no Content-Length header) */

	/** READ-ONLY **/
	unsigned short http_major;
	unsigned short http_minor;
	unsigned int status_code : 16; /* responses only */
	unsigned int method : 8;       /* requests only */
	unsigned int http_errno : 7;

	/* 1 = Upgrade header was present and the parser has exited because of that.
	* 0 = No upgrade header present.
	* Should be checked when http_parser_execute() returns in addition to
	* error checking.
	*/
	unsigned int upgrade : 1;

	/** PUBLIC **/
	void *data; /* A pointer to get hook to the "connection" or "socket" object */
};


struct http_parser_settings {
	http_cb      on_message_begin;
	http_data_cb on_url;
	http_data_cb on_status;
	http_data_cb on_header_field;
	http_data_cb on_header_value;
	http_cb      on_headers_complete;
	http_data_cb on_body;
	http_cb      on_message_complete;
};


enum http_parser_url_fields
{ UF_SCHEMA           = 0
, UF_HOST             = 1
, UF_PORT             = 2
, UF_PATH             = 3
, UF_QUERY            = 4
, UF_FRAGMENT         = 5
, UF_USERINFO         = 6
, UF_MAX              = 7
};


/* Result structure for http_parser_parse_url().
*
* Callers should index into field_data[] with UF_* values iff field_set
* has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
* because we probably have padding left over), we convert any port to
* a uint16_t.
*/
struct http_parser_url {
	uint16_t field_set;           /* Bitmask of (1 << UF_*) values */
	uint16_t port;                /* Converted UF_PORT string */

	struct {
		uint16_t off;               /* Offset into buffer in which field starts */
		uint16_t len;               /* Length of run in buffer */
	} field_data[UF_MAX];
};


/* Returns the library version. Bits 16-23 contain the major version number,
* bits 8-15 the minor version number and bits 0-7 the patch level.
* Usage example:
*
*   unsigned long version = http_parser_version();
*   unsigned major = (version >> 16) & 255;
*   unsigned minor = (version >> 8) & 255;
*   unsigned patch = version & 255;
*   printf("http_parser v%u.%u.%u\n", major, minor, version);
*/
unsigned long http_parser_version(void);

void http_parser_init(http_parser *parser, enum http_parser_type type);


size_t http_parser_execute(http_parser *parser,
						   const http_parser_settings *settings,
						   const char *data,
						   size_t len);


/* If http_should_keep_alive() in the on_headers_complete or
* on_message_complete callback returns 0, then this should be
* the last message on the connection.
* If you are the server, respond with the "Connection: close" header.
* If you are the client, close the connection.
*/
int http_should_keep_alive(const http_parser *parser);

/* Returns a string version of the HTTP method. */
const char *http_method_str(enum http_method m);

/* Return a string name of the given error */
const char *http_errno_name(enum http_errno err);

/* Return a string description of the given error */
const char *http_errno_description(enum http_errno err);

/* Parse a URL; return nonzero on failure */
int http_parser_parse_url(const char *buf, size_t buflen,
						  int is_connect,
struct http_parser_url *u);

/* Pause or un-pause the parser; a nonzero value pauses */
void http_parser_pause(http_parser *parser, int paused);

/* Checks if this is the final chunk of the body. */
int http_body_is_final(const http_parser *parser);


template<typename T_HEAD = std::map<std::string,std::string>>
class http_copy_buff
{
public:
	http_copy_buff(){};
	~http_copy_buff(){};
public:
	void insert_url(const char *data, size_t s)
	{
		url.assign(data,s);
	}
	void insert_status_code( size_t c)
	{
		code = c;
	}
	void insert_status( const char *data, size_t s)
	{
		status.assign(data,s);
	}
	void insert_field( const char *key, size_t k_s, const char *value, size_t v_s)
	{
		header.insert( std::make_pair(std::string(key,k_s),std::string(value,v_s)));
	}

	void insert_body( const char *data, size_t s)
	{
		auto ptr = std::make_shared<std::vector<char>>();
		ptr->assign(data,data+s);
		body_buff.push_back(ptr);
	}
	void insert_version(size_t major, size_t minor)
	{
		version_major = major;
		version_minor = minor;
	}
	void insert_method( enum http_method m )
	{
		method = m;
	}
protected:
	std::string url;
	size_t version_major;
	size_t version_minor;
	enum http_method method;
	size_t code;
	std::string status;
	T_HEAD header;
	std::list<std::shared_ptr<std::vector<char>>> body_buff;
};


template<typename T_RE = http_copy_buff<>>
class http_work
{
public:
	http_work(enum http_parser_type type)
	{
		http_parser_init(&hp,type);
		hp.data = this;

		hps.on_message_begin = std::bind(&http_work::on_message_begin,this,std::placeholders::_1);
		hps.on_url = std::bind(&http_work::on_url,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		hps.on_status = std::bind(&http_work::on_status,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		hps.on_header_field = std::bind(&http_work::on_header_field,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		hps.on_header_value = std::bind(&http_work::on_header_value,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		hps.on_headers_complete = std::bind(&http_work::on_headers_complete,this,std::placeholders::_1);
		hps.on_body = std::bind(&http_work::on_body,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		hps.on_message_complete = std::bind(&http_work::on_message_complete,this,std::placeholders::_1);
	};
	~http_work(){};

public:
	// Re true: can be continue
	//   false: finished or error occur
	// When return false, should restore messages if needed then release the http_work obj.
	bool insert_buff( const char *data, size_t len )
	{
		size_t ret = http_parser_execute(&hp,&hps,data,len);
		if ( len == ret )
			return true;
		return false;
	}

	size_t message_count()
	{
		return result.size();
	}

	std::shared_ptr<T_RE> get_message(size_t no )
	{
		assert( no < result.size() );
		if ( no < result.size() )
			return result[no];
		return nullptr;
	}

	bool is_interrupter()
	{
		return !message_continue;
	}

	bool is_alive()
	{
		return 
	}

protected:
	http_parser hp;
	http_parser_settings hps;

protected:
	std::vector<std::shared_ptr<T_RE>> result;
	std::shared_ptr<T_RE> op_ptr;
	std::string key_temp;

	bool message_continue;
	bool alive;

protected:
	int on_message_begin(http_parser*)
	{
		op_ptr = std::make_shared<T_RE>();
		message_continue = true;
		alive = true;
		return 0;
	}
	int on_url(http_parser*, const char *data, size_t s)
	{
		op_ptr->insert_url( data, s );
		return 0;
	}
	int on_status(http_parser* h, const char *data, size_t s)
	{
		op_ptr->insert_status( data, s);
		return 0;
	}
	int on_header_field(http_parser*, const char *data, size_t s)
	{
		key_temp.assign(data,s);
	}
	int on_header_value(http_parser*, const char *data, size_t s)
	{
		op_ptr->insert_field( key_temp.c_str(), key_temp.length(), data, s);
		return 0;
	}
	int on_headers_complete(http_parser* h)
	{
		op_ptr->insert_version(h->http_major, h->http_minor);
		op_ptr->insert_status_code( h->status_code );
		op_ptr->insert_method( h->method );

		alive = !(0==http_should_keep_alive(h));
		if ( !alive )
		{
			if ( false == http_body_is_final(h) )
				alive = true;
		}
		return alive?0:1;
	}
	int on_body(http_parser*, const char *data, size_t s)
	{
		op_ptr->insert_body( data, s );
		return 0;
	}
	int on_message_complete(http_parser*)
	{
		alive = !(0==http_should_keep_alive(h));
		message_continue = false;
		if ( op_ptr )
			result.push_back(op_ptr);
		return alive?0:1;
	}

};

