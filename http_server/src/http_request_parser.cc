#include "http_request_parser.h"
#include "string_utils.h"
HttpRequestParser::HttpRequestParser() {
    line_buffer = new char[HTTP_PARSER_LINE_BUFSIZE];
    reset();
}
HttpRequestParser::~HttpRequestParser() {
    delete[] line_buffer;
    delete[] body;
}
void HttpRequestParser::recv(const char* buf, size_t recv_count)
{
    const char* buf_p = buf;
    while ((buf_p - buf) < recv_count)
    {
        switch (parse_status)
        {
            case STARTLINE: {

                while ((*line_buffer_end++ = *buf_p++) != '\n' && (buf_p - buf) < recv_count);
                if (line_buffer_end[-1] == '\n') {
                    line_buffer_end = line_buffer;
                    while (*++line_buffer_end != ' ');
                    method = std::string(line_buffer, line_buffer_end);
                    trim(method);
                    trim(method,"\r");
                    char* targetStart = line_buffer_end + 1;
                    while (*++line_buffer_end != ' ');
                    target = std::string(targetStart, line_buffer_end);
                    trim(target);
                    trim(target, "\r");
                    char* protVerStart = line_buffer_end + 1;
                    while (*++line_buffer_end != '\n');
                    http_version = std::string(protVerStart, line_buffer_end);
                    trim(http_version);
                    trim(http_version, "\r");
                    parse_status = HEADERS;
                    line_buffer_end = line_buffer;
                }
                break;
            }
            case HEADERS: {
                while ((*line_buffer_end++ = *buf_p++) != '\n' && (buf_p - buf) < recv_count);
                if (line_buffer_end[-1] == '\n') {
                    if (line_buffer_end - line_buffer == 1) {
                        if (line_buffer[0] == '\n')
                            parse_status = body_length == 0 ? FINISH : BODY;
                    }
                    if (line_buffer_end - line_buffer == 2) {
                        if (line_buffer[0] == '\r' && line_buffer[1] == '\n')
                            parse_status = body_length == 0 ? FINISH : BODY;

                    }
                    else {
                        char* headerKey = line_buffer;
                        while (*++headerKey != ':');
                        std::string header(line_buffer, headerKey);
                        std::string value(headerKey + 1, line_buffer_end - 1);
                        trim(header);
                        trim(header, "\r");
                        trim(value);
                        trim(value, "\r");
                        headers[header] = value;
                        if (lower(header) == "content-length") {
                            body_length = std::stoull(value);
                            body = new char[body_length];
                            body_p = body;
                        }
                        line_buffer_end = line_buffer;
                    }
                }
                break;
            }
            case BODY: {
                size_t count = recv_count - (buf_p - buf);
                memcpy(body_p, buf_p, count);
                body_p += count;
                buf_p += count;

                if (body_p - body >= body_length) {
                    parse_status = FINISH;
                }
                break;
            }
            default:
                break;
        }
    }
}

void HttpRequestParser::reset()
{
    headers.clear();
    line_buffer_end = line_buffer;
    if(body != nullptr)
        delete[] body;
    body = nullptr;
    body_p = nullptr;
    body_length = 0;
    parse_status = STARTLINE;
}

std::string& HttpRequestParser::get_method()
{
    return method;
}

std::string& HttpRequestParser::get_target()
{
    return target;
}

std::string& HttpRequestParser::get_http_version()
{
    return http_version;
}

std::map<std::string, std::string>& HttpRequestParser::get_headers()
{
    return headers;
}

std::pair<char*, size_t> HttpRequestParser::get_body()
{
    return std::make_pair(body, body_length);
}

bool HttpRequestParser::is_finish()
{
    return parse_status == FINISH;
}
