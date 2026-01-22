#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdint.h>

#pragma comment(lib, "Ws2_32.lib")

class PortScanner {
	public:
		PortScanner(const std::string& address);
		
		void SetAddress(const std::string& address);
		void Scan();

	private:
		std::string address;

		bool CanConnect(uint16_t port);
};