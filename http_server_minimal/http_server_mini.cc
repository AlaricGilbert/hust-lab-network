#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <thread>
#include <fstream>
#include <filesystem>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define BUFFER_SIZE 1024*32
#define RESPONSE_BUILDER(code, code_msg, content, content_len, content_type) std::string("HTTP/1.1 " + std::to_string(code) + " " + code_msg + "\r\nContent-Type: " + content_type + "\r\nContent-Length: "+ std::to_string(content_len)+"\r\nConnection: keep-alive\r\n\r\n"+ content)
class http_server {
    std::string ip;
    int port;
    bool running;
    SOCKET listen_socket;
    std::thread server_thread;
    std::map<SOCKET, std::thread> clients;
    std::map<std::string, std::string> content_types{ {"html","text/html;charset=utf-8"},{"htm","text/html;charset=utf-8"},{"jpg","image/jpeg"},{"jpeg","image/jpeg"},{"png","image/png"} };
public:
    http_server(std::string ip, int port) :ip(std::move(ip)), port(port), running(false), listen_socket(INVALID_SOCKET) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            std::cout << "Startup failed!" << std::endl;
    }
    void start() {
        server_thread = std::thread([&]()->void {
            addrinfo  hints, * addr;
            memset(&hints, 0, sizeof(addrinfo));
            if (getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &addr) != 0) std::cout << "Get address failed!" << std::endl;
            else if ((listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == INVALID_SOCKET) std::cout << "Create listening socket failed!" << std::endl;
            else if (bind(listen_socket, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR) std::cout << "Bind failed!" << std::endl;
            else if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) std::cout << "Listen failed!" << std::endl;
            else {
                running = true;
                while (running) {
                    SOCKET client_socket = accept(listen_socket, NULL, NULL);
                    if (client_socket == INVALID_SOCKET) {
                        std::cout << "Create socket failed!" << std::endl;
                        continue;
                    }
                    clients[client_socket] = std::thread([&]()->void {
                        char* recv_buffer = new char[BUFFER_SIZE];
                        int   bytes_recv = 0;
                        sockaddr_in client_addr;
                        int client_addr_len = sizeof(sockaddr_in);
                        if (getpeername(client_socket, (sockaddr*)(&client_addr), &client_addr_len) == SOCKET_ERROR) {
                            std::cout << "Get client info failed!" << std::endl;
                            goto exit;
                        }
                        while (true) {
                            bytes_recv = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);
                            if (bytes_recv > 0) {
                                char *buffer_ptr = recv_buffer, *buffer_end = recv_buffer + bytes_recv;
                                while (buffer_ptr < buffer_end && *++buffer_ptr != ' ');
                                char* url_start = buffer_ptr;
                                while (buffer_ptr < buffer_end && *++buffer_ptr != ' ');
                                std::filesystem::path file_name(std::string(url_start + 2, buffer_ptr));
                                std::ifstream file(file_name, std::ios::binary | std::ios::ate);
                                if (file.is_open()) {
                                    std::streamsize size = file.tellg();
                                    file.seekg(0, std::ios::beg);
                                    char* buffer = new char[size];
                                    if (file.read(buffer, size)) {
                                        std::string ext(file_name.extension().generic_string());
                                        ext = ext.substr(1, ext.size() - 1);
                                        std::string response = RESPONSE_BUILDER(200, "OK", std::string(buffer, buffer + size), size, content_types[ext]);
                                        send(client_socket, response.c_str(), response.length(), 0);
                                        continue;
                                    }
                                    delete[] buffer;
                                }
                                std::string content = "<html><head><title>404 not found</title></head><body><h1>404 Not Found.</h1><hr><p>Powered by Alaric's Simple Http Server.</p></body></html>";
                                std::string response = RESPONSE_BUILDER(404, "NotFound", content, content.size(), "text/html; charset=UTF-8");
                                send(client_socket, response.c_str(), strlen(response.c_str()), 0);
                            } else break;
                        }
                        exit:
                        closesocket(client_socket);
                        delete[] recv_buffer;
                    });
                    clients[client_socket].detach();
                }
                closesocket(listen_socket);
            }
        });
        server_thread.detach();
    }
    void stop() {
        running = false;
        closesocket(listen_socket);
        for (auto& s : clients) closesocket(s.first);
    }
    ~http_server(){ WSACleanup(); }
};
int main() {
    http_server("127.0.0.1", 8080).start();
    std::cin.get();
}