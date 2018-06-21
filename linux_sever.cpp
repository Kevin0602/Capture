
#include "linux_sever.h"
int socket_init()
{
    socklen_t sin_size = sizeof(addr);
    int sockid = 0;
    int new_sockid = 0;
    addr ser_addr,client_addr;
    char bufferw[MAX_LEN]="12.3,-12.5,-0.05";
    char bufferr[MAX_LEN];
    //create socket
    if((sockid = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("create socket is failure\n");
        exit(1);
    }

    //对ser_addr 进行初始化
    bzero(&ser_addr,sizeof(addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int on=1;
     if((setsockopt(sockid,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
     {
         perror("setsockopt failed");
         exit(EXIT_FAILURE);
    }    
  //server bind

    if((bind(sockid,(struct sockaddr *)&ser_addr,sizeof(ser_addr))) == -1)
    {
        fprintf(stderr,"bind error is %s\n",strerror(errno));
        exit(1);

    }
    //listen
    if(listen(sockid,5) == -1)
    {
        fprintf(stderr,"listen error is %s\n",strerror(errno));
        exit(1);
    }
    //while(1)
   // {
        //accept
        if((new_sockid = accept(sockid,(struct sockaddr*)(&client_addr),&sin_size)) == -1)
        {
            fprintf(stderr,"accept error is %s\n",strerror(errno));
            exit(1);
        }
        else
        {
            printf("server and client link success!!\n");
        }

       return new_sockid;
    //}
   // close(sockid);
    //printf("server is over!!\n");
}
