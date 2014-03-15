#include "transport.h"

//=============================================================================================
transport::~transport(){
    close(sock);
}
//=============================================================================================
transport::transport(){

            if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                printf("%s","can't create socket\n");
                return;
            }

            memset((char *)&my_addr, 0, sizeof(my_addr));
            my_addr.sin_family = AF_INET;
            my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            my_addr.sin_port = htons(0);

            if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0){
                printf("%s %d","can't bind socket: \n", errno);
                return;
            }

            socklen_t rem_addr_len = sizeof(my_addr);
            getsockname(sock, (struct sockaddr *)&my_addr, &rem_addr_len);

	    struct timeval tv;
	    tv.tv_sec = 1;
	    tv.tv_usec = 0;
	    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
	       perror("Error");
//            printf("%s : %d\n",inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port));
}
//=============================================================================================
transport::transport(char* addr, int port){
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("can't create %s : %d socket\n",addr,port);
        return;
    }

    memset((char *)&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
//    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (inet_aton(addr, &my_addr.sin_addr)==0)
        return;

    my_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0){
            printf("can't bind %s : %d socket : %d\n",addr,port, errno);
           return;
    }

    socklen_t rem_addr_len = sizeof(my_addr);
    getsockname(sock, (struct sockaddr *)&my_addr, &rem_addr_len);
    
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) )  < 0) 
       perror("Error");
//    printf("created %s : %d socket\n",inet_ntoa(my_addr.sin_addr), ntohs(my_addr.sin_port));
}

//=============================================================================================
int transport::get_free_port(){
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("can't create socket\n");
        return -1;
    }

    memset((char *)&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(0);

    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0){
            printf("can't bind socket : %d\n", errno);
           return -1;
    }

    socklen_t rem_addr_len = sizeof(my_addr);
    getsockname(sock, (struct sockaddr *)&my_addr, &rem_addr_len);
    close(sock);
    return ntohs(my_addr.sin_port);
}

//=============================================================================================
bool transport::send(char* addr, int port, char* data, size_t data_len){

    memset((char *) &rem_addr, 0, sizeof(rem_addr));
    rem_addr.sin_family = AF_INET;
    rem_addr.sin_port = htons(port);

    if (inet_aton(addr, &rem_addr.sin_addr)==0)
        return false;

        if (sendto(sock, data, data_len, 0, (struct sockaddr *)&rem_addr, sizeof(rem_addr)) < 0)
            return false;
        else
            return true;
}
//=============================================================================================
size_t transport::read(char* buf, size_t buf_len){
        socklen_t rem_addr_len = sizeof(rem_addr);
        return recvfrom(sock, buf, buf_len, 0, (struct sockaddr *)&rem_addr, &rem_addr_len);
}

