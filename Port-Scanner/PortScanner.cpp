#include "PortScanner.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

namespace {
    struct WsaSession {
        WsaSession() {
            WSADATA wsa {};
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                throw std::runtime_error("WSAStartup failed\n");
            }
        }

        ~WsaSession() {
            WSACleanup();
        }
    };

    // function local static variable - constructed only once and then reused
    void InitWsa() {
        static WsaSession wsa;
    }
}

PortScanner::PortScanner(const std::string& address) : address(address)
{}

void PortScanner::SetAddress(const std::string& address)
{
	this->address = address;
}

void PortScanner::Scan()
{
    InitWsa();

    for (int i = 7990; i < 9250; i++) {
        if (CanConnect(i)) {
            std::cout << address << ":" << i << "Is Open!" << std::endl;
        }
    }
}

bool PortScanner::CanConnect(uint16_t port) 
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket number " << port << " creation failed, Error: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    // convert string address to bytes
    int result = inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    if (result != 1) {
        std::cout << "inet_pton failed!, Result: " << result 
        << " Error: " << WSAGetLastError() << std::endl;

        closesocket(sock);
        return false;
    }

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == INVALID_SOCKET) {
        closesocket(sock);
        return false;
    }

    closesocket(sock);
    return true;
}
