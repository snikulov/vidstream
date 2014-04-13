#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PKG_START_KEY 0xAAA9
#define MAX_PKG_SIZE 8192

class transport{
    public:
        transport();
        transport(const char* addr, int port);
        ~transport();
        bool send(const char* addr, int port, char* data, size_t data_len);
        size_t read(char* buf, size_t buf_len);
        int get_free_port();
    private:
        sockaddr_in my_addr;
        sockaddr_in rem_addr;
        int sock;
        int port;
};
#endif // TRANSPORT_H
