#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include "PortScanner.h"

int main() {
	PortScanner p("127.0.0.1");
	std::cout << std::thread::hardware_concurrency();
	p.Scan();
}