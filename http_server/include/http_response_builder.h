#ifndef HUST_LAB_NETWORK_HTTP_RESPONSE_BUILDER_H
#define HUST_LAB_NETWORK_HTTP_RESPONSE_BUILDER_H
#include <map>
#include <string>
#include "http_code.h"
class HttpResponseBuilder
{
    std::map<std::string, std::string> headers;
    std::string protocol_version;
    HttpCode    code;
    const char* body_pointer;
    size_t      body_length;
public:
    HttpResponseBuilder(HttpCode code, const std::string& protocol_version);
    HttpResponseBuilder& header(const std::string& header_key, const std::string& header_value);
    HttpResponseBuilder& body(const char* body, size_t length);
    std::map<std::string, std::string>& get_headers();
    std::string get_protocol_version();
    HttpCode    get_http_code();
    std::pair<const char*, size_t> get_body();
    std::string build();
};
#endif //HUST_LAB_NETWORK_HTTP_RESPONSE_BUILDER_H
