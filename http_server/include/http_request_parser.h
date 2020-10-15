#ifndef HUST_LAB_NETWORK_HTTP_REQUEST_PARSER_H
#define HUST_LAB_NETWORK_HTTP_REQUEST_PARSER_H
#include <map>
#include <string>
#define HTTP_PARSER_LINE_BUFSIZE 1024
class HttpRequestParser
{
    std::map<std::string, std::string> headers;
    std::string method;
    std::string target;
    std::string http_version;
    std::size_t body_length;
    char* line_buffer;
    char* line_buffer_end;
    char* body = nullptr;
    char* body_p = nullptr;
    enum ParseStatus {
        STARTLINE,
        HEADERS,
        BODY,
        FINISH
    } parse_status;

public:
    HttpRequestParser();
    ~HttpRequestParser();
    void recv(const char* buf, size_t recv_count);
    void reset();
    std::string& get_method();
    std::string& get_target();
    std::string& get_http_version();
    std::map<std::string, std::string>& get_headers();
    std::pair<char*, size_t> get_body();
    bool is_finish();
};
#endif //HUST_LAB_NETWORK_HTTP_REQUEST_PARSER_H
