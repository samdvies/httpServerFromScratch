#include <iostream>
#include <winsock2.h>

int main() {
    // Initialize WSA variables
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wsaerr != 0) {
        std::cout << "The winsock dll not found" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    //Create a socket
    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): "<< WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "Socket is ok!"<< std::endl;
    }


    //connect to the server(setup sockaddr structure)
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(55555);

    //using the actual connect function
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "CLient: connect() - Failed to connect: "<<WSAGetLastError()<<std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "client connection is ok!" << std::endl;
        std::cout << "Client can start sending and recieving data" <<std::endl;
    }
    //Sending data to the server
    char buffer[200];
    std::cout << "Enter the message: ";
    std::cin.getline(buffer, 200);
    int sbyteCount = send(clientSocket, buffer, 200, 0);
    if (sbyteCount == SOCKET_ERROR) {
        std::cout << "Client send error:" << WSAGetLastError() <<std::endl;
        return 0;
    } else {
        std::cout << "Client: sent "<< sbyteCount << " BYTES"<<std::endl;
    }


    // Receiving data from the server
    char receiveBuffer[200];
    int rbyteCount = recv(clientSocket, receiveBuffer, 200, 0);
    if (rbyteCount < 0) {
        std::cout << "Client recv error: " << WSAGetLastError() << std::endl;
        return 0;
    } else {
        std::cout << "Client: Received data: " << receiveBuffer << std::endl;
    }
    

    

}