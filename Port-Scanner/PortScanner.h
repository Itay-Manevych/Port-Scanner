#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdint.h>
#include <mutex>
#include <vector>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

constexpr auto MAX_PORTS = 65535;
constexpr auto CHUNK = 1024;
constexpr auto MAX_THREADS = 8;

class PortScanner {
	public:
		PortScanner(const std::string& address);
		
		void SetAddress(const std::string& address);
		std::vector<uint32_t> Scan();

	private:
		std::string address;

		bool CanConnect(uint32_t port);
};