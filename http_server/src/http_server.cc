#include "http_server.h"
#include "string_utils.h"
#include <map>
#include <ios>
#include <iostream>
#include <fstream>
#include <filesystem>
HttpServer::HttpServer(std::string _addr, int _port){
    address = _addr;
    port = _port;
    listen_socket = INVALID_SOCKET;
    running = false;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw "Startup failed!";
    }
}
HttpServer::~HttpServer() {
    WSACleanup();
}

void HttpServer::rebind(std::string _addr, int _port)
{
    if (running) {
        throw "Rebind while running";
    }
    address = _addr;
    port = _port;
}

std::string HttpServer::run() {
    addrinfo  hints;
    addrinfo* listen_addr;
    memset(&hints, 0, sizeof(addrinfo));
    int status = getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &listen_addr);
    if (status != 0) {
        return "Get binding address failed: ";
    }

    listen_socket = socket(listen_addr->ai_family, listen_addr->ai_socktype, listen_addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        return "Create listen socket failed: ";
    }
    if (bind(listen_socket, listen_addr->ai_addr, listen_addr->ai_addrlen) == SOCKET_ERROR) {
        return "Binding listening socket failed: ";
    }
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        return "Listening socket startup failed: ";
    }

    running = true;
    while (running) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            return "Create socket failed!";
        }
        accept_callback(client_socket);
        std::thread client(HttpServer::clientHandler, client_socket, [&client_socket, this](SOCKET s)->void {
                close_callback(client_socket);
                client_threads.erase(s);
        }, request_callback, response_callback);
        client.detach();
        client_threads[client_socket] = std::move(client);

    }
    closesocket(listen_socket);
    return "";
}



void HttpServer::clientHandler(SOCKET client_socket, const std::function<void(SOCKET)>& end_callback, const std::function<void(HttpRequestParser&, SOCKET)>& request_callback, const std::function<void(HttpResponseBuilder&, SOCKET)>& response_callback) {
    char *recvbuf = new char[buffer_size];
    int bytes_received;
    sockaddr_in client_info;
    int client_info_len = sizeof(sockaddr_in);
    HttpRequestParser parser;

    if (getpeername(client_socket, (sockaddr*)(&client_info), &client_info_len) == SOCKET_ERROR) {
        goto exit;
    }

    while (true) {
        bool request_finish = false;
        while (!request_finish) {
            bytes_received = recv(client_socket, recvbuf, buffer_size, 0);
            if (bytes_received > 0) {
                parser.recv(recvbuf, bytes_received);
                if (parser.is_finish())
                    request_finish = true;

            }
            else {
                goto exit;
            }

        }
        request_callback(parser, client_socket);
        std::string file_name = trim(parser.get_target(), "/");
        std::filesystem::path fpath(file_name);

        std::ifstream file(fpath.native(), std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            char* buffer= new char[size];
            if (file.read(buffer, size))
            {
                std::string ext(fpath.extension().generic_string());
                ext = ext.substr(1, ext.size() - 1);
                std::string content_type;
                if (ext == "jpg" || ext == "jpeg")
                    content_type = "image/jpeg";
                if (ext == "png")
                    content_type = "image/png";
                if (ext == "gif")
                    content_type = "image/gif";
                if (ext == "htm" || ext == "html")
                    content_type = "text/html;charset=utf-8";
                auto response_builder = HttpResponseBuilder(HttpCode::OK, parser.get_http_version())
                        .header("Content-Type", content_type)
                        .body(buffer, size);
                response_callback(response_builder, client_socket);
                std::string response = response_builder.build();
                send(client_socket, response.c_str(), response.length(), 0);
            }
            delete[] buffer;
        }
        else {

            std::string content = "<html><head><title>404 not found</title></head><body><h1>404 Not Found.</h1><hr><p>" + get_time_string() + ". Powered by Alaric's Simple Http Server.</p></body></html>";
            auto response_builder = HttpResponseBuilder(HttpCode::NotFound, "HTTP/1.1")
                    .header("Content-Type", "text/html; charset=UTF-8")
                    .body(content.c_str(), content.length());
            response_callback(response_builder, client_socket);
            std::string response = response_builder.build();
            send(client_socket, response.c_str(), strlen(response.c_str()), 0);
        }
        parser.reset();
    }
    exit:
    end_callback(client_socket);
    closesocket(client_socket);
}
size_t HttpServer::buffer_size = 1024;
void HttpServer::set_buffer_size(size_t _buffer_size) {
    buffer_size = _buffer_size;
}

void HttpServer::start()
{
    if (!running) {
        running_thread = std::thread([this]()->void {
            std::string s = run();
            if (!s.empty())
                error_callback(s);
        });
        running_thread.detach();
    }
}

bool HttpServer::is_running() {
    return running;
}


std::string HttpServer::get_ip() {
    return address;
}
int HttpServer::get_port() {
    return port;
}

void HttpServer::stop()
{
    running = false;
    closesocket(listen_socket);
    running_thread.join();
}

void HttpServer::on_internal_error(const std::function<void(std::string&)>& error_callback)
{
    this->error_callback = error_callback;
}

void HttpServer::on_accept_finished(const std::function<void(SOCKET)>& accept_callback) {
    this->accept_callback = accept_callback;
}

void HttpServer::on_socket_closing(const std::function<void(SOCKET)>& close_callback) {
    this->close_callback = close_callback;
}

void HttpServer::on_request_finished(const std::function<void(HttpRequestParser&, SOCKET)>& request_callback) {
    this->request_callback = request_callback;
}

void HttpServer::on_response_finished(const std::function<void(HttpResponseBuilder&, SOCKET)>& response_callback) {
    this->response_callback = response_callback;

}
