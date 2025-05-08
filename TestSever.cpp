#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <arpa/inet.h>
#define BROADCAST_PORT 9000
#define TCP_PORT 8050
using namespace std;


// Read from a file
vector<vector<string>> readFile(const string &filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        string cell;
        while (getline(ss, cell, ','))
            row.push_back(cell);
        data.push_back(row);
    }
    return data;
}

//sending To client
void sendPrompt(int sock, const string &msg) {
    send(sock, msg.c_str(), msg.length(), 0);
    cout << "[SEND] " << msg << endl;
}

//Receiving from Client
string receiveInput(int sock) {
    char buffer[4096];
    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived <= 0) {
        cout << "[RECV] Connection closed or no data.\n";
        return "";
    }

    buffer[bytesReceived] = '\0';
    cout << "[RECV] " << buffer << endl;
    return string(buffer);
}


//---------Entrance Function----------------
void entrance(int sock)
{
    stringstream ss;
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|       EDUVERSE - YOUR GATEWAY TO KNOWLEDGE         |";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|----------------------------------------------------|";
  ss<<"\n\n Welcome You to this Automated Question Answer Shift  ";
  SS<<"\n\n             LET'S BEGIN THE SESSION \n";
  ss<<"\n\n|----------------------------------------------------|";
    ss<<" \nTO EXIT PRESS: CTRL+C";
    ss<<"\n|-----------------------------------------------------|";
    ss<<"\n\n                   GET READY;
    sendPrompt(sock,ss);
    
    //Clearing the stringStream
    ss.str("");     
    ss.clear(); 
    
    //Subject Choice
    ss<<"\n\n";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|        WHICH SUBJECT YOU WANT TO PREPARE ?         |";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n|----------------------------------------------------|";
  ss<<"\n\n|  1. MATHEMATICS       2.HISTORY       3.GEOGRAPHY  |";
  ss<<"\n\n|  4. BIOLOGY           5.PHYSICS       6.CHEMISTRY  |";
  ss<<"\n\n|  7. LITERATURE        8.COMPUTER      10.LANGUAGE  |";
    ss<<"\n|----------------------------------------------------|";
    ss<<"\n\n CHOOSE A NO.";
    sendPrompt

    
}


// --- MAIN ---
// ---------- UDP Broadcast Function ----------
void broadcastServerIP() {
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int broadcast = 1;
    setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    sockaddr_in broadcast_addr{};
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(BROADCAST_PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    string message = "SERVER:" + to_string(TCP_PORT);

    while (true) {
        sendto(udp_socket, message.c_str(), message.length(), 0,
               (sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        cout << "[Broadcasting] " << message << "\n";
        sleep(2);
    }
}

// ---------- Main Function ----------
int main() {
    // Start UDP broadcasting
    thread broadcaster(broadcastServerIP);
    broadcaster.detach();

    // Setup TCP server
    int server_fd, new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TCP_PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "✅ Server is running on port " << TCP_PORT << " and broadcasting..." << endl;

    // Accept multiple clients
    while (true) {
        new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        thread(entrance, new_sock).detach();
    }

    return 0;
}

