#include "PortScanner.h"
#include <iostream>

namespace {
    struct WsaSession {
        WsaSession() {
            WSADATA wsa{};
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

int PortScanner::Scan()
{
    InitWsa();
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed, Error: " << WSAGetLastError() << std::endl;
        return 1;
    }
}