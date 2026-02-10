#include "PortScanner.h"
#include "ThreadPool.h"
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

std::vector<uint16_t> PortScanner::Scan()
{
    InitWsa();
    ThreadPool pool(MAX_THREADS);
    std::mutex results_mutex;
    std::vector<uint16_t> results;

    for (uint16_t start = 1; start <= MAX_PORTS; start += CHUNK) {
        uint16_t end = (std::min)(start + CHUNK - 1, MAX_PORTS);

        pool.Enqueue([start, end, this, &results, &results_mutex] {
            std::vector<uint16_t> local_results;

            for (uint16_t i = start; i <= end; i++) {
                if (CanConnect(i)) {
                    local_results.push_back(i);
                }
            }

            std::unique_lock<std::mutex> lock(results_mutex);
            results.insert(results.end(), local_results.begin(), local_results.end());
            lock.unlock();
        });
    }
    pool.WaitIdle(); // wait for all tasks to finish!
    return results;
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

    std::cout << "Connected succefsully to port: " << port << std::endl;
    closesocket(sock);
    return true;
}
