#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

void handle_client(int client_fd, sockaddr_in *client_socket)
{
    auto ip = inet_ntoa(client_socket->sin_addr);

    std::string username;
    char buffer[256] = {0};

    while (recv(client_fd, buffer, 255, 0))
    {
        if (username.empty())
        {
            username = buffer;
            std::cout << "\033[32m" << username << " has join the chat\033[39m" << std::endl;
        }
        else
        {
            std::cout << username << "> " << buffer << std::endl;
        }

        memset(buffer, '\0', 255);
    }

    close(client_fd);

    std::cout << "\033[31m" << ip << " has quit the chat\033[39m" << std::endl;
}

bool is_number(const char *str)
{
    if (*str == '\0')
    {
        return false;
    }

    while (*str != '\0')
    {
        if (!isdigit(*str))
        {
            return false;
        }

        str++;
    }

    return true;
}

int main(int argc, char const *argv[])
{
    if (argc < 2 || !is_number(argv[1]))
    {
        std::cerr << "Format: server <port>" << std::endl;
        return 1;
    }

    auto port = atoi(argv[1]);
    sockaddr_in server_socket;
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(port);
    server_socket.sin_addr.s_addr = INADDR_ANY;

    auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    auto bind_error = bind(socket_fd, reinterpret_cast<sockaddr *>(&server_socket), sizeof(server_socket));

    if (bind_error == -1)
    {
        std::cerr << "Error when binding socket: " << errno << std::endl;
        return 1;
    }

    if (listen(socket_fd, 64) == -1)
    {
        std::cerr << "Error when listening on socket: " << errno << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    sockaddr_in client_socket;
    auto client_socket_size = sizeof(client_socket);

    int client_fd = 0;

    std::vector<std::thread> threads;

    while ((client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client_socket), reinterpret_cast<socklen_t *>(&client_socket_size))))
    {
        std::thread client_thread(handle_client, client_fd, &client_socket);
        threads.push_back(std::move(client_thread));
    }

    close(socket_fd);

    return 0;
}
