// UDPApplication.cpp : UDP server.
// Copyright (c) Omar Ramirez 11/15/2022
//About: Run this application as well as chat app (Oastes)-1 to able commincation between chap app (Oastes)-1 and console application
#include <string>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;



DWORD WINAPI ReceivingThread(LPVOID printOrderBool) {

    SOCKET* s = static_cast<SOCKET*>(printOrderBool);
    sockaddr_in client;
    int clientLength = sizeof(client);
    char buf[1024];

    while (1) {
        ZeroMemory(&client, clientLength);  // Clear the client structure
        ZeroMemory(buf, 1024);              // Clear the receive buffer

        // Wait for message. Block call
        int bytesIn = recvfrom(*s, buf, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR) {
            cout << "Error receiving from client " << WSAGetLastError() << endl;
            continue;
        }

        // Display message and Work on getting IP info
        char clientIp[256];         // Create enough space to convert the address byte array
        ZeroMemory(clientIp, 256);  // to string of characters

        // Convert from byte array to chars. i.e. get IP info
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

        // Display the message 
        cout << endl << "Message from client. IP address " << clientIp << ": " << buf << endl;
    }

    return 0;
}

int main() {

    const char* pkt = "Connected to the server successfully";
    const char* srcIP = "127.0.0.1";                    //< source IP address >
    const char* destIP = "127.0.0.1";                   //< destination IP address >
    sockaddr_in local;
    sockaddr_in dest;
    WSAData data;
    DWORD ThreadIDA;
    HANDLE ThreadHandlerA;

    //Start up the server
    WSAStartup(MAKEWORD(2, 2), &data);

    // Create a server hint structure for the server
    local.sin_family = AF_INET;
    inet_pton(AF_INET, srcIP, &local.sin_addr.s_addr);  // Convert from string to byte array
    local.sin_port = htons(3515);                       //< recieving port number >

    // Create a hint structure for the server
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, destIP, &dest.sin_addr.s_addr);  // Convert from string to byte array
    dest.sin_port = htons(3514);                        //< destination port number >

    //Socket Creation
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    //Bind socket and comfirm
    bind(s, (sockaddr*)&local, sizeof(local));
    sendto(s, pkt, strlen(pkt), 0, (sockaddr*)&dest, sizeof(dest));

    //Recieving Thread
    ThreadHandlerA = CreateThread(NULL, 0, ReceivingThread, &s, 0, &ThreadIDA);

    //Sending Loop
    cout << "Enter 'end' to exit the program." << endl;
    string inputString;
    while (1) {
        cout << "Please enter your message:  ";
        getline(cin, inputString);
        pkt = inputString.c_str();
        if (inputString == "end") {
            return 0;
        }
        //Sends message to server. Once connected.
        sendto(s, pkt, strlen(pkt), 0, (sockaddr*)&dest, sizeof(dest));
        cout << "Your Message '" << inputString << "' was sent successfully!" << endl;
    }

    if (ThreadHandlerA != NULL) {
        //WaitForSingleObject(ThreadHandlerA, INFINITE);
        CloseHandle(ThreadHandlerA);
    }

    //End
    closesocket(s);
    WSACleanup();

    return 0;
}