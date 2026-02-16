#include "PortScanner.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

int main() {
	PortScanner p("127.0.0.1");
	p.Scan();
}