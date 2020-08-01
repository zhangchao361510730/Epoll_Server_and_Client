#include"Utility.h"
int main(int argc, char const *argv[])
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(SERVER_PORT);
    //serverAddr.sin_addr.s_addr=inet_addr(SERVER_IP);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(connect(sock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
    {
        perror("Connect Error");
        exit(-1);
    }
    int pipe_fd[2];
    if(pipe(pipe_fd)<0)
    {
        perror("Pipe Error");
        exit(-1);
    }
    int epfd=epoll_create(EPOLL_SIZE);
    if(epfd<0)
    {
        perror("Epfd Error");
        exit(-1);
    }
    static struct epoll_event events[2];
    addfd(epfd,sock,true);
    addfd(epfd,pipe_fd[0],true);
    bool isClientWork=true;
    char message[BUF_SIZE]={0};
    int pid=fork();
    if(pid<0)
    {
        perror("Fork Error");
        exit(-1);
    }
    else if(pid==0)
    {
        close(pipe_fd[0]);
        printf("Please input 'exit' to exit the chat room\n");
        while(isClientWork)
        {
            bzero(&message,BUF_SIZE);
            fgets(message,BUF_SIZE,stdin);
            if(strncasecmp(message,EXIT,strlen(EXIT))==0)
            {
                isClientWork=0;
            }
            else{
                if(write(pipe_fd[1],message,strlen(message)-1)<0)
                {
                    perror("Fork Error");
                    exit(-1);
                }
            }
        }
    }
    else
    {
        close(pipe_fd[1]);
        while(isClientWork)
        {
            int epoll_events_count=epoll_wait(epfd,events,2,-1);
            for(int i=0;i<epoll_events_count;++i)
            {
                bzero(&message,BUF_SIZE);
                if(events[i].data.fd==sock)// 来自服务器消息
                {
                    int ret=recv(sock,message,BUF_SIZE,0);
                    if(ret==0){
                        printf("Server closed connection: %d\n",sock);
                        close(sock);
                        isClientWork=0;
                    }else if(ret<0)
                    {
                        printf("接受服务器消息失败，%s",strerror(errno));
                    }
                    else printf("消息来自服务器:%s\n",message);
                }
                else if(events[i].data.fd==pipe_fd[0])
                {
                    int ret=read(events[i].data.fd,message,BUF_SIZE);// 阻塞读管道
                    if(ret==0)
                        isClientWork=0;
                    else
                        send(sock,message,BUF_SIZE,0);
                }
            }
        }
    }
    if(pid)
    {
        close(pipe_fd[0]);
        close(sock);
    }
    else{
        close(pipe_fd[1]);
    }    
    return 0;
}
