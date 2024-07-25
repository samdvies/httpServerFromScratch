#include <iostream>
#include <winsock2.h>


//initialize winsock
int main() {
    // Initialize WSA variables
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wsaerr != 0) {
        std::cout << "The Winsock dll not found!" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    //Create a tcp socket

    SOCKET serverSocket;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //check if successful
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Socket is OK!"<<std::endl;
    }
    //binding an address to the socket(sort of like assigning a phone num to the line you just built)
    sockaddr_in service;
    service.sin_family = AF_INET;
    //0.0.0.0 allows the OS to choose its own IP
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    //htons converts an integer to a network representation
    service.sin_port = htons(55555); //the port number that clients will connect to
    
    //using the bind function
    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    } else {
        std::cout << "bind() is OK!" <<std::endl;
    }

    //listen for incoming connections
    if (listen(serverSocket, 1) ==SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket: " << WSAGetLastError << std::endl;
    } else {
        std::cout << "listen() is OK! Waiting for connections...";
    }
    
    //ACCEPT incomin connection
    SOCKET acceptSocket;
    //nullptr used as we do not  need the information about the client
    acceptSocket = accept(serverSocket, nullptr, nullptr);

    //check for syccessful connection
    if (acceptSocket == INVALID_SOCKET) {
        std::cout <<"accept failed" <<  WSAGetLastError() <<std::endl;
        WSACleanup();
        return -1;
    } else {
        std::cout << "accept() is OK!" << std::endl;
    }

    bool sendMore = true;
    while (sendMore) {
        //this is recieveing data from the client
        char recieveBuffer[200];
        //recv() is the function used to recieve data from socket
        //recieveBuffer is a pointer to where a buffer will be stored
        //200 is simply the length of the buffer(max number of bytes that will be read)
        int rbyteCount = recv(acceptSocket, recieveBuffer, 200, 0);
        //the value returned is the number of bytes or SOCKET_ERROR if retrieving fails
        if (rbyteCount <0) {
            std::cout <<"Server recv eror" << WSAGetLastError() << std::endl;
            return 0;
        } else {
            std::cout << "Recieved data: " << recieveBuffer << std::endl;
        }


        //this is sending data to the client
        char buffer[200];

        std::cout << "Enter the message: ";
        //takes the entered message with a character limit of 200
        std::cin.getline(buffer, 200);
        //send works similarly to recieve
        int sbyteCount = send(acceptSocket, buffer, 200, 0);
         if (sbyteCount == SOCKET_ERROR) {
         std::cout << "Server send error: " << WSAGetLastError() <<std::endl;
         return -1;
          } 
         else {
            std::cout << "Server: sent" <<sbyteCount << "BYTES" << std::endl;

        }
        int check;
        std::cout <<"do you wish to send more messages yes[1] or no[0]: ";
        std::cin >> check;
        if (check == 1) {
            sendMore = true;
        }
        else {sendMore = false;}
    }
}

