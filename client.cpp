#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << " Socket creation failed\n";
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8050);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        cerr << "Connection failed\n";
        return 1;
    }

    cout << "Connected to the server!\n";

    char buffer[4096];
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Server disconnected.\n";
            break;
        }

        cout << buffer; 

        if (strstr(buffer, "Choose:") || strstr(buffer, "Enter") || strstr(buffer, "Choice:") || strstr(buffer, "Input:") || strstr(buffer, ":")) {
            string input;
            cout << "> ";
            getline(cin, input);

            if (send(sock, input.c_str(), input.length(), 0) == -1) {
                cerr << "Failed to send input\n";
                break;
            }
        }
    }

    close(sock);
    return 0;
}

