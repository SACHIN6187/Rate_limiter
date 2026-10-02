#include "core_code.cpp"
// g++ -std=c++17 server.cpp -o rate_limiter_server
//./rate_limiter_server

#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {

  rate_limiter limiter;

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (serverSocket < 0) {
    cerr << "Socket creation failed\n";
    return 1;
  }

  sockaddr_in serverAddress{};

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  int port = 6320;
  if (getenv("PORT")) {
    port = stoi(getenv("PORT"));
  }

  serverAddress.sin_port = htons(port);

  if (::bind(serverSocket, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0) {

    cerr << "Bind failed\n";
    close(serverSocket);
    return 1;
  }

  if (listen(serverSocket, 5) < 0) {

    cerr << "Listen failed\n";
    close(serverSocket);
    return 1;
  }

  cout << "rate limiter running on port " << port << "...\n";

  while (true) {

    cout << "Waiting for connection...\n";

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket < 0) {
      cerr << "Accept failed\n";
      continue;
    }
    cout << "Client connected!\n";
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
      string request(buffer, bytesReceived);
      bool allowed = limiter.check(request);
      string response;
      if (allowed)
        response = "REQUEST_ALLOWED\n";
      else
        response = "RATE_LIMIT_EXCEEDED\n";
      send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
  }

  close(serverSocket);

  return 0;
}