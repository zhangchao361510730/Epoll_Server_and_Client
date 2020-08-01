#include"Utility.h"
int main(int argc, char *argv[])
{
	  struct sockaddr_in serverAddr;
	  serverAddr.sin_family=PF_INET;
	  serverAddr.sin_port=htons(SERVER_PORT);
	  // serverAddr.sin_addr.s_addr=inet_addr(SERVER_IP);
	  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	 
	  int listener=socket(PF_INET,SOCK_STREAM,0);
	  if(listener<0){perror("socket create error");exit(-1);}
	  int ret=bind(listener,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
	  if(ret<0)
	  {
		    cout<<"Bind Error Msg::"<<strerror(errno)<<endl;
	  }
	  ret=listen(listener,5);
	  if(ret<0)
	  {
		    cout<<"Listen Error Msg::"<<strerror(errno)<<endl;
	  }
	  int epfd=epoll_create(EPOLL_SIZE);
	  if(epfd<0)
	  {
		    cout<<"EPOLL Create Error";
		    exit(0);
	  }
	  static struct epoll_event events[EPOLL_SIZE];
	  addfd(epfd,listener,true);
	  while(1)
	  {
		    printf("Start Epoll_Wait\n");
		    int epoll_events_count=epoll_wait(epfd,events,EPOLL_SIZE,-1);
		    printf("End Epoll_Wait\n");
		    if(epoll_events_count<0)
		    {
				cout<<"EPOLL failture";
				break;
		    }
		    printf("epoll_events_count = %d\n", epoll_events_count);
		    for (int i=0;i<epoll_events_count;++i)
		    {
				int sockfd=events[i].data.fd;
				if(sockfd==listener)
				{
					  struct sockaddr_in client_address;
					  socklen_t client_addrLength=sizeof(struct sockaddr_in);
					  cout<<"Wait for accept"<<endl;
					  int clientfd=accept(listener,(struct sockaddr*)&client_address,&client_addrLength);

					  printf("client connection from: %s : % d(IP : port), clientfd = %d \n",
								inet_ntoa(client_address.sin_addr),
								ntohs(client_address.sin_port),
								clientfd);
					  addfd(epfd,clientfd,true);
					  client_list.push_back(clientfd);
					  printf("Add new clientfd = %d to epoll\n", clientfd);
					  printf("Now there are %d clients int the chat room\n", (int)client_list.size());
					  char message[1024]={0};
					  sprintf(message,SERVER_WELCOME,clientfd);
					  send(clientfd,message,1024,0);
				}
				else {
					  printf("属于广播消息\n");
					  int ret=sendBroadcastMessage(sockfd);
					  if(ret<0){
						    perror("sendBroadcastMessage");
						    exit(-1);
					  }
				}
		    }
	  }
	  close(listener);
	  close(epfd);
	  return 0;
}
























