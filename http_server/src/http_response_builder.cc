#include "http_response_builder.h"

#include <sstream>
HttpResponseBuilder::HttpResponseBuilder(HttpCode code, const std::string& protocol_version)
{
    this->code = code;
    this->protocol_version = protocol_version;
    headers["Connection"] = "keep-alive";
    headers["Content-Length"] = "0";
}

HttpResponseBuilder& HttpResponseBuilder::header(const std::string& header_key, const std::string& header_value)
{
    headers[header_key] = header_value;
    return *this;
}

HttpResponseBuilder& HttpResponseBuilder::body(const char* body, size_t length)
{
    body_pointer = body;
    body_length = length;
    headers["Content-Length"] = std::to_string(length);
    return *this;
}

std::string HttpResponseBuilder::build()
{
    std::string result;
    result += protocol_version + " " + std::to_string((int)code) + " " + get_httpcode_reason(code) + "\r\n";
    for (auto &header_pair:headers)
    {
        result += header_pair.first + ": " + header_pair.second + "\r\n";
    }
    result += "\r\n";
    if (body_length > 0) {
        result += std::string(body_pointer, body_pointer + body_length);
    }
    return result;
}

std::map<std::string, std::string>& HttpResponseBuilder::get_headers() {
    return headers;
}

std::string HttpResponseBuilder::get_protocol_version() {
    return protocol_version;
}

HttpCode HttpResponseBuilder::get_http_code() {
    return code;
}

std::pair<const char*, size_t> HttpResponseBuilder::get_body() {
    return std::make_pair(body_pointer, body_length);
}