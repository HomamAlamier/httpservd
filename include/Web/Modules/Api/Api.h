#ifndef API_H
#define API_H

typedef int(*stream_get_length)(void*);
typedef int(*stream_seek)(void*, int);
typedef int(*stream_read)(void*, char*, int);
typedef int(*stream_write)(void*, const char*, int);

// Request methods
#define HTTP_GET 1
#define HTTP_POST 2
#define HTTP_CONNECT 3
#define HTTP_PUT 4
#define HTTP_DELETE 5

// Http versions
#define HTTP_V1_0 1
#define HTTP_V1_1 2
#define HTTP_V2_0 3

// Http responses
#define	RESPONSE_OK 200
#define RESPONSE_CREATED 201
#define	RESPONSE_ACCEPTED 202
#define	RESPONSE_NON_AUTHORITATIVE_INFORMATION 203
#define	RESPONSE_NO_CONTENT 204
#define	RESPONSE_RESET_CONTENT 205
#define	RESPONSE_PARTIAL_CONTENT 206
#define	RESPONSE_MULTIPLE_CHOICE 300
#define	RESPONSE_MOVED_PERMANENTLY 301
#define	RESPONSE_FOUND 302
#define	RESPONSE_SEE_OTHER 303
#define	RESPONSE_NOT_MODIFIED 304
#define	RESPONSE_BAD_REQUEST 400
#define	RESPONSE_UNAUTHORIZED 401
#define	RESPONSE_FORBIDDEN 403
#define	RESPONSE_NOT_FOUND 404
#define	RESPONSE_METHOD_NOT_ALLOWED 405
#define	RESPONSE_INTERNAL_SERVER_ERROR 500
#define	RESPONSE_NOT_IMPLEMENTED 501
#define	RESPONSE_BAD_GATEWAY 502
#define	RESPONSE_SERVICE_UNAVAILABLE 503

// configs
#define CONFIG_RETURN_RESULT 0
#define CONFIG_FORWARD_RESULT 1
#define CONFIG_RESULT_IGNORE_IF_EMPTY 2

#define CONFIG_PROCESS_FIRST 0
#define CONFIG_PROCESS_LAST 1

typedef struct
{
    const char* name;
    int result_config;
    int process_priority;
} config_t;

typedef struct 
{
    void* stream_ptr;
    stream_get_length get_length;
    stream_read read;
    stream_write write;
    stream_seek seek;
} stream_t;

typedef struct 
{
    const char* key;
    const char* value;
} tuple_t;

typedef struct 
{
    tuple_t** data;
    int size;
} headers_t;

typedef struct 
{
    int request_method;
    const char* path;
    int http_version;
    headers_t* headers;
    stream_t* in_stream;
} http_request_t;

typedef struct 
{
    int response_code;
    int http_version;
    headers_t* headers;
    stream_t* out_stream;
} http_response_t;

#ifdef API_FUNCTIONS
#include <stdlib.h>
#include <string.h>
void headers_t_add(headers_t* headers, const char* key, const char* value)
{
    if (headers == NULL)
        return;
    tuple_t** ndata = (tuple_t**)malloc(sizeof(tuple_t) * (headers->size + 1));
    for(int i = 0; i < headers->size; ++i)
    {
        ndata[i] = (tuple_t*)malloc(sizeof(tuple_t));
        ndata[i]->key = headers->data[i]->key;
        ndata[i]->value = headers->data[i]->value;
    }
    ndata[headers->size] = (tuple_t*)malloc(sizeof(tuple_t));
    ndata[headers->size]->key = key;
    ndata[headers->size]->value = value;
    if (headers->size > 0)
    {
        for(int i = 0; i < headers->size; ++i)
        {
            free(headers->data[i]);
        }
        free(headers->data);
    }


    headers->data = ndata;
    headers->size++;
}
const char* headers_t_find(headers_t* headers, const char* key)
{
    if (headers == NULL)
        return NULL;
    for(int i = 0; i < headers->size; ++i)
    {
        if (strcmp(headers->data[i]->key, key))
        {
            return headers->data[i]->value;
        }
    }
}
int stream_t_read(stream_t* stream, char* buffer, int size)
{
    if (stream == NULL)
        return 0;
    return stream->read(stream->stream_ptr, buffer, size);
}
int stream_t_write(stream_t* stream, const char* buffer, int size)
{
    if (stream == NULL)
        return 0;
    return stream->write(stream->stream_ptr, buffer, size);
}
int stream_t_seek(stream_t* stream, int to)
{
    if (stream == NULL)
        return 0;
    return stream->seek(stream->stream_ptr, to);
}
int stream_t_get_length(stream_t* stream)
{
    if (stream == NULL)
        return 0;
    return stream->get_length(stream->stream_ptr);
}

#endif // API_FUNCTIONS
#endif // !API_H
