#define API_FUNCTIONS 
#include <Web/Modules/Api/Api.h>
#include <stdio.h>
int init()
{
    return 1;
}


int can_handle(const http_request_t* r)
{
    int i = strcmp(r->path, "/redirect");
    if (i == 0)
    {
        return 1;
    }
    return 0;
}

int get_config(config_t* conf)
{
    conf->name = "HelloWorldModule";
    conf->result_config = CONFIG_RETURN_RESULT;
    conf->process_priority = CONFIG_PROCESS_LAST;
    return 0;
}

int process_request(http_request_t* req, http_response_t* resp, const char* requestFilePath)
{

    int x = stream_t_get_length(req->in_stream);
    char* b = (char*)malloc(sizeof(char) * x);
    stream_t_read(req->in_stream, b, x);

    printf("%s\n", b);
    
    free(b);

    resp->response_code = RESPONSE_MOVED_PERMANENTLY;
    resp->http_version = HTTP_V1_1;

    resp->headers = (headers_t*)malloc(sizeof(headers_t));
    resp->headers->size = 0;

    headers_t_add(resp->headers, "Location", "http://google.com");

    //stream_t_write(resp->out_stream, "<html><head><h1>Hello World !</h1></head></html>", 49);

    
    return 0;
}
