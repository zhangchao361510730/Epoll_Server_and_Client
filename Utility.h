#ifndef UTILITY_H
#define UTILITY_H
#include<iostream>
#include<list>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>

using namespace std;

list<int>client_list;
#define SERVER_IP "182.61.146.65"
#define SERVER_PORT 8888
#define EPOLL_SIZE 5000
#define BUF_SIZE 1024
#define SERVER_WELCOME "Welcome you join  to the chat room! Your chat ID is: Client #%d"
//#define SERVER_MESSAGE "ClientID %d say >> %s"
#define EXIT "EXIT"
#define CAUTION "There is only one int the char room!"

int setnonblocking(int &socket)
{
    fcntl(socket,F_SETFL,fcntl(socket,F_GETFD,0)|O_NONBLOCK);
    return 0;
}

void addfd(int &epollfd,int fd,bool enable_et)
{
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=EPOLLIN;
    if(enable_et)
        ev.events=EPOLLIN|EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
    setnonblocking(fd);
}

int sendBroadcastMessage(int clientfd)
{
    char buf[1024]={0};
    char message[2048]={0};
    printf("read from client(clientID =%d)\n",clientfd);
    int len=recv(clientfd,buf,BUF_SIZE,0);
    if(len==0)
    {
        close(clientfd);
        client_list.remove(clientfd);
        printf("ClientID = %d closed.\n now there are %ld client in the char room\n", clientfd, client_list.size());

    }
    else
    {
        if(client_list.size()==1)
        {
            send(clientfd,CAUTION,strlen(CAUTION),0);
            return len;
        }
        sprintf(message,"ClientID %d say :: %s",clientfd,buf);
        list<int>::iterator it;
        for(it=client_list.begin();it!=client_list.end();++it)
        {
            if(*it!=clientfd)
            {
                if(send(*it,message,BUF_SIZE,0)<0)
                {
                    perror("sendBroadcastMessage error");exit(-1);
                }
            }
        }
    }
    return len;
}

#endif // UTILITY_H



























































