#include <client.hpp>
#include <fileIO.hpp>


int main()
{
    char ip[50] = "127.0.0.1";
    char username[50] = "Admin";
    char password[50] = "Password";
    char rcpt[50] = "User";
    ClientTransaction conn(ip, 12345, username, password, rcpt);
    conn.handshakeTransaction();
    conn.fetchBufferMessages();
    conn.threadHandler();
}