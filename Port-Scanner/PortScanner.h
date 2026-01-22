#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class PortScanner {
	public:
		PortScanner(const std::string& address);
		
		void SetAddress(const std::string& address);
		int Scan();

	private:
		std::string address;

		bool CanConnect(SOCKET sock, uint16_t port);
};