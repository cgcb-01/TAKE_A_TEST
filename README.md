# EduVerse: Socket-Based Interactive Quiz System
EduVerse is a C++ client-server application that delivers an interactive, menu-driven Multiple Choice Question (MCQ) quiz experience. It enables communication between client and server using sockets, allows users to select subjects, chapters, and parts from a structured question repository, and provides immediate feedback with explanations.

# Features
Organized question bank by subject, chapter, and part

Socket-based communication for client-server interaction

Real-time quiz experience with answer validation and scoring

Explanations provided after each question

Option to retake tests seamlessly
# Project Structure

EduVerse/
├── server.cpp 
├── client.cpp                
├── questions/                
│   ├── Biology/
│   │   ├── Cell/
│   │   │   ├── part1.txt
│   │   │   └── part2.txt
│   │   └── Cell Division/
│   │       ├── part1.txt
│   │       └── part2.txt
│   └── Mathematics/
│       ├── Linear Inequation/
│       │   ├── part1.txt
│       │   └── part2.txt
│       └── AP GP/
│           ├── part1.txt
│           └── part2.txt

# How It Works
The server initializes and listens for incoming client connections.

The client connects and displays an interactive welcome screen.

Users navigate to choose a subject, chapter, and part.

The server reads the corresponding question file and starts the quiz session.

For each question:

The client submits an answer.

The server verifies the response and sends back the correct answer and explanation.

After the quiz, the server displays the final score.

Users can choose to retake the test or exit.

# Question File Format
Each text file should follow this consistent structure:
Q: What is 2 + 2?
A) 3
B) 4
C) 5
D) 6
Answer: B
Solution: 2 + 2 equals 4.

# Technologies Used
C++ with STL

POSIX Sockets (for client-server communication)

Filesystem library for directory traversal

Terminal-based interface

# Requirements
Linux or Unix-based operating system

g++ compiler (C++17 or higher)

Basic terminal/command-line knowledge

# Compilation and Execution

