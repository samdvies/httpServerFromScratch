#include <iostream>
#include <sstream>
#include <string>
#include <winsock2.h>
#include <fstream>



//function to return html as string
std::string readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (!fileStream) {
        return "error";
    }

    std::ostringstream ss;
    ss << fileStream.rdbuf();
    return ss.str();
}



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
    service.sin_port = htons(8080); //the port number that clients will connect to
    
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

    while(1) {
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
    std::string fileName;
    bool fileNameEnd = false;
    bool fileNameStart = false;
    int j=1;
    while (!fileNameEnd) {
        if (fileNameStart == true or recieveBuffer[j-1] == '/') {
            fileNameStart = true;
            fileName = fileName + recieveBuffer[j];

        }
        if (recieveBuffer[j+1] == '.') {
            fileNameEnd = true;
        }
        j+=1;
    }
    std::string sendString;
    std::string myHtml = readFile(fileName +".html");
    if (myHtml == "error") {
        sendString = "HTTP/1.1 404 Incorrect URL\nContent-Type: text/plain\nContent-Length:10\n\nError: 404";
    }
    else {
        std::string bodyS = std::to_string(myHtml.length());
        sendString = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "+ bodyS +"\n\n" +myHtml;
    }
    

    //this is sending data to the client
    //the char seen is sending information as headers (http format)
    //The header shown is the minimum needed
    //200 and OK refer to the status code and ststus message, e.g. Error 404
    

    //the below code converts the string to a char array such that it can be sent via tcp.
    char* sendText = new char[sendString.size() + 1];
    std::copy(sendString.begin(), sendString.end(), sendText);
    sendText[sendString.size()] = '\0';  // Null-terminate the char array

    //std::cout << "Enter the message: ";
    //takes the entered message with a character limit of 200
    //std::cin.getline(buffer, 200);
    //send works similarly to recieve
    int sbyteCount = send(acceptSocket, sendText, strlen(sendText), 0);
    if (sbyteCount == SOCKET_ERROR) {
        std::cout << "Server send error: " << WSAGetLastError() <<std::endl;
        return -1;
    } else {
        std::cout << "Server: sent" <<sbyteCount << "BYTES" << std::endl;
    }
    closesocket(acceptSocket);
    
    }
}

