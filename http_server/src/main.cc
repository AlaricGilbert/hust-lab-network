#include <iostream>
#include <sstream>
#include <mutex>
#include "http_server.h"
#include "http_request_parser.h"
#include "logger.h"
#include "string_utils.h"
int main()
{
    Logger logger({&std::cout });
    std::mutex logger_mutex;
    HttpServer mserver("127.0.0.1", 8000);
    mserver.on_accept_finished([&logger, &logger_mutex](SOCKET client_socket)->void {
        sockaddr_in client_info;
        auto sunb = &client_info.sin_addr.S_un.S_un_b;
        int client_info_len = sizeof(sockaddr_in);
        char client_ip[16];
        if (getpeername(client_socket, (sockaddr*)(&client_info), &client_info_len) != SOCKET_ERROR) {
            sprintf_s(client_ip, "%d.%d.%d.%d", sunb->s_b1, sunb->s_b2, sunb->s_b3, sunb->s_b4);
            std::string s = "[";
            s += std::to_string(client_socket);
            s += "]Client connected from ";
            logger_mutex.lock();
            logger.info(s + client_ip + ":" + std::to_string(client_info.sin_port));
            logger_mutex.unlock();
        }
    });
    mserver.on_socket_closing([&logger, &logger_mutex](SOCKET client_socket)->void {
        sockaddr_in client_info;
        auto sunb = &client_info.sin_addr.S_un.S_un_b;
        int client_info_len = sizeof(sockaddr_in);
        char client_ip[16];
        if (getpeername(client_socket, (sockaddr*)(&client_info), &client_info_len) != SOCKET_ERROR) {
            sprintf_s(client_ip, "%d.%d.%d.%d", sunb->s_b1, sunb->s_b2, sunb->s_b3, sunb->s_b4);
            std::string s = "[";
            s += std::to_string(client_socket);
            s += "]Client connected from ";
            logger_mutex.lock();
            logger.info(s + client_ip + ":" + std::to_string(client_info.sin_port) + " is closing.");
            logger_mutex.unlock();
        }
    });
    mserver.on_request_finished([&logger, &logger_mutex](HttpRequestParser& parser, SOCKET client_socket)->void {
        sockaddr_in client_info;
        auto sunb = &client_info.sin_addr.S_un.S_un_b;
        int client_info_len = sizeof(sockaddr_in);
        char client_ip[16];
        if (getpeername(client_socket, (sockaddr*)(&client_info), &client_info_len) != SOCKET_ERROR) {
            sprintf_s(client_ip, "%d.%d.%d.%d", sunb->s_b1, sunb->s_b2, sunb->s_b3, sunb->s_b4);
            std::string s = "[";
            s += std::to_string(client_socket);
            s += "]" + parser.get_http_version() + " " + parser.get_method() + " " + parser.get_target() + " received from ";
            logger_mutex.lock();
            logger.info(s + client_ip + ":" + std::to_string(client_info.sin_port));
            logger_mutex.unlock();
        }
    });
    mserver.on_response_finished([&logger, &logger_mutex](HttpResponseBuilder& builder, SOCKET client_socket)->void {
        sockaddr_in client_info;
        auto sunb = &client_info.sin_addr.S_un.S_un_b;
        int client_info_len = sizeof(sockaddr_in);
        char client_ip[16];
        if (getpeername(client_socket, (sockaddr*)(&client_info), &client_info_len) != SOCKET_ERROR) {
            sprintf_s(client_ip, "%d.%d.%d.%d", sunb->s_b1, sunb->s_b2, sunb->s_b3, sunb->s_b4);
            std::string s = "[";
            s += std::to_string(client_socket);
            s += "]" + builder.get_protocol_version() + " " + std::to_string(builder.get_http_code()) + " " +
                 get_httpcode_reason(builder.get_http_code()) + " sent to ";
            logger_mutex.lock();
            logger.info(s + client_ip + ":" + std::to_string(client_info.sin_port));
            logger_mutex.unlock();
        }
    });
    mserver.on_internal_error([&logger, &logger_mutex](std::string& e)->void {

        logger_mutex.lock();
        logger.error(e);
        logger_mutex.unlock();
    });
    loop:
    try {
        std::cout << "Simple Http Server Over Socket (WinSocks2)" << std::endl;
        std::cout << "Server set to listening at 127.0.0.1:8000." << std::endl;
        std::cout << "Command: rebind, start and stop." << std::endl;
        std::string command;
        while (true) {
            std::cin >> command;
            if (command == "rebind") {
                if (mserver.is_running()) {
                    logger.error("Cannot rebind while server is running!");
                    continue;
                }
                std::string ip;
                int  port;
                std::cout << "Please input IP:";
                std::cin >> ip;
                std::cout << "Please input port:";
                std::cin >> port;
                mserver.rebind(ip, port);
                logger.info("Server rebinded to " + mserver.get_ip() + ":" + std::to_string(mserver.get_port()) + ".");
            }
            else if (command == "start") {
                if (mserver.is_running()) {
                    logger.error("Http server has already been runnnig!");
                    continue;
                }
                logger.info("Server started listening at " + mserver.get_ip() + ":" + std::to_string(mserver.get_port())+".");
                mserver.start();
            }
            else if (command == "stop") {
                if (!mserver.is_running()) {
                    logger.error("Http server isn't runnnig!");
                    continue;
                }
                logger.info("Server stopped.");
                mserver.stop();
            }
            else if (command == "quit") {
                if (mserver.is_running()) {
                    mserver.stop();
                    logger.info("Server stopped.");
                }
                break;
            }
            else {
                logger.error("Unknown command.");
            }
        }
    } catch (std::exception e) {
        logger_mutex.lock();
        std::string s = "exception happened:";
        logger.error(s + e.what());
        logger_mutex.unlock();
        goto loop;
    }
}