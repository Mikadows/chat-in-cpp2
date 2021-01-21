#include <arpa/inet.h>
#include <cerrno>
#include <iostream>
#include <string>
#include <unistd.h>

bool is_number(const char *str)
{
    if (*str == '\0')
    {
        return false;
    }

    while (*str != '\0')
    {
        if (!isnumber(*str))
        {
            return false;
        }

        str++;
    }

    return true;
}

int main(int argc, char const *argv[])
{
    if (argc < 3 || !is_number(argv[2]))
    {
        std::cerr << "Format: client <host> <port>" << std::endl;
        return 1;
    }

    std::string username;

    auto is_username_valid = [&username]() {
        if (username.empty() || username.size() < 3 || username.size() > 14)
        {
            return false;
        }

        for (char c : username)
        {
            if (!isalnum(c) && c != ' ')
            {
                return false;
            }
        }

        return true;
    };

    while (!is_username_valid())
    {
        std::cout << "Username: ";
        std::getline(std::cin >> std::ws, username);

        auto first = std::find_if(username.begin(), username.end(), [](char c) {
            return !isspace(c);
        });

        auto last = std::find_if(username.rbegin(), username.rend(), [](char c) {
            return !isspace(c);
        });

        username.erase(username.begin(), first);
        username.erase(last.base(), username.end());

        if (!is_username_valid())
        {
            std::cerr << "Username is invalid." << std::endl;
        }
    }

    auto host = argv[1];
    auto port = atoi(argv[2]);

    auto client_socket = sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };

    inet_pton(AF_INET, host, &client_socket.sin_addr);

    auto socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    auto connect_error = connect(socket_fd, reinterpret_cast<sockaddr *>(&client_socket), sizeof(client_socket));
    if (connect_error == -1)
    {
        std::cerr << "Error when connecting to server: " << errno << std::endl;
        return 1;
    }

    send(socket_fd, username.c_str(), username.size(), 0);

    std::string buffer;

    while (1)
    {
        std::cout << username << "> ";
        std::getline(std::cin >> std::ws, buffer);

        if (buffer == "exit")
        {
            break;
        }

        send(socket_fd, buffer.c_str(), buffer.size(), 0);
    }

    close(socket_fd);

    return 0;
}
