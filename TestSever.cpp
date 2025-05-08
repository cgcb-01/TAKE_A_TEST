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

//--------------TEST HANDLER--------------------------
class QuizManager {
public:
    void loadQuestions(const string& filepath, vector<MCQ>& questions) {
        ifstream file(filepath);
        if (!file) {
            cerr << "Could not open file: " << filepath << endl;
            return;
        }

        string line;
        MCQ mcq;
        while (getline(file, line)) {
            if (line.rfind("Q:", 0) == 0) {
                mcq.question = line.substr(2);
            } else if (line.rfind("A)", 0) == 0) {
                mcq.options[0] = line;
            } else if (line.rfind("B)", 0) == 0) {
                mcq.options[1] = line;
            } else if (line.rfind("C)", 0) == 0) {
                mcq.options[2] = line;
            } else if (line.rfind("D)", 0) == 0) {
                mcq.options[3] = line;
            } else if (line.rfind("Answer:", 0) == 0) {
                mcq.correctAnswer = toupper(line.back());
            } else if (line.rfind("Solution:", 0) == 0) {
                mcq.solution = line.substr(9); 
                questions.push_back(mcq); 
            }
        }
    }

    void runQuiz(const vector<MCQ>& questions) {
        int score = 0;
        for (const auto& q : questions) {
            cout << "\n" << q.question << "\n";
            for (int i = 0; i < 4; ++i) {
                cout << q.options[i] << "\n";
            }

            cout << "Your answer: ";
            char ans;
            cin >> ans;

            if (toupper(ans) == q.correctAnswer) {
                cout << "Correct!\n";
                score++;
            } else {
                cout << "Wrong. Correct answer is: " << q.correctAnswer << "\n";
            }
            cout << "Explanation: " << q.solution << "\n";
        }
        cout << "\nYour score: " << score << "/" << questions.size() << "\n";
    }
};

//---------Entrance Function----------------
void entrance(int sock)
{
    testHandler test;
    string basePath = "./questions";

    while (true) {
        stringstream ss;
        ss<<"\n|----------------------------------------------------|";
        ss<<"\n|----------------------------------------------------|";
        ss<<"\n|       EDUVERSE - YOUR GATEWAY TO KNOWLEDGE         |";
        ss<<"\n|----------------------------------------------------|";
        ss<<"\n|----------------------------------------------------|";
        ss<<"\n\n Welcome You to this Automated Question Answer Shift  ";
        ss<<"\n\n             LET'S BEGIN THE SESSION \n";
        ss<<"\n\n|----------------------------------------------------|";
        ss<<" \nTO EXIT PRESS: CTRL+C";
        ss<<"\n|-----------------------------------------------------|";
        ss<<"\n\n                   GET READY";
        sendPrompt(sock, ss.str());

        // Clear stream
        ss.str(""); ss.clear();

        // Step 1: Show Subjects
        vector<string> subjects;
        for (const auto& entry : fs::directory_iterator(basePath)) {
            if (entry.is_directory()) {
                subjects.push_back(entry.path().filename().string());
            }
        }
        int subjIndex = getChoiceFromList(sock, subjects, "AVAILABLE SUBJECTS");
        string subject = subjects[subjIndex];

        // Step 2: Show Chapters
        string subjectPath = basePath + "/" + subject;
        vector<string> chapters;
        for (const auto& entry : fs::directory_iterator(subjectPath)) {
            if (entry.is_directory()) {
                chapters.push_back(entry.path().filename().string());
            }
        }
        int chapIndex = getChoiceFromList(sock, chapters, "AVAILABLE CHAPTERS");
        string chapter = chapters[chapIndex];

        // Step 3: Show Parts
        string chapterPath = subjectPath + "/" + chapter;
        vector<string> parts;
        for (const auto& entry : fs::directory_iterator(chapterPath)) {
            if (entry.is_regular_file()) {
                parts.push_back(entry.path().filename().string());
            }
        }
        int partIndex = getChoiceFromList(sock, parts, "AVAILABLE PARTS");
        string part = parts[partIndex];

        string finalFile = chapterPath + "/" + part;
        stringstream finalMsg;
        finalMsg << "\nLoading Your questions from " << finalFile << "\n";
        sendPrompt(sock, finalMsg.str());

        // Run quiz
        vector<MCQ> questions;
        test.loadQuestions(finalFile, questions);
        test.runTest(sock, questions); 

        // Ask if user wants to try another quiz
        sendPrompt(sock, "\nDo you want to take another test? (yes/no): ");
        string choice = receiveInput(sock);
        if (choice != "yes" && choice != "Yes" && choice != "y" && choice != "Y") {
            sendPrompt(sock, "\nThank you for using EDUVERSE. Goodbye!\n");
            break;
        }
    }
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

