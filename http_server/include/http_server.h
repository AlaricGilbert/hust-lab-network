//
// Created by Alaric on 2020/10/15.
//

#ifndef HUST_LAB_NETWORK_HTTP_SERVER_H
#define HUST_LAB_NETWORK_HTTP_SERVER_H
#include "http_request_parser.h"
#include "http_response_builder.h"
#include <string>
#include <thread>
#include <functional>
#include <WinSock2.h>
#include <WS2tcpip.h>

class HttpServer
{
    std::string   address;
    std::thread   running_thread;
    int           port;
    bool          running;
    SOCKET        listen_socket;
    std::map<SOCKET, std::thread> client_threads;
    std::function<void(SOCKET)> accept_callback;
    std::function<void(SOCKET)> close_callback;
    std::function<void(HttpRequestParser&, SOCKET)> request_callback;
    std::function<void(HttpResponseBuilder&, SOCKET)> response_callback;
    std::function<void(std::string&)> error_callback;
    static size_t buffer_size;
    static void clientHandler(SOCKET client_socket, const std::function<void(SOCKET)>& end_callback, const std::function<void(HttpRequestParser&, SOCKET)>& request_callback, const std::function<void(HttpResponseBuilder&, SOCKET)>& response_callback);
public:
    HttpServer(std::string _addr, int _port);
    ~HttpServer();
    void rebind(std::string _addr, int _port);
    void start_watch();
    void start();
    void stop();
    void on_internal_error(const std::function<void(std::string&)>& error_callback);
    void on_accept_finished(const std::function<void(SOCKET)>& accept_callback);
    void on_socket_closing(const std::function<void(SOCKET)>& close_callback);
    void on_request_finished(const std::function<void(HttpRequestParser&, SOCKET)> &request_callback);
    void on_response_finished(const std::function<void(HttpResponseBuilder&, SOCKET)> &response_callback);
    bool is_running();
    std::string get_ip();
    std::string run();
    int get_port();
    static void set_buffer_size(size_t _buffer_size);
};

#endif //HUST_LAB_NETWORK_HTTP_SERVER_H
