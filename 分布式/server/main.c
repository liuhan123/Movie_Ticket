/*************************************************************************
	> File Name: main.c
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Mon 02 May 2016 02:53:23 AM PDT
 ************************************************************************/
         /*服务器的主函数*/
#include  <stdio.h>
#include "command.h"
#include "common.h"
void *th(void *f);//声明线程处理函数
int main()
{
    int sockfd;//服务器套接字
    int *client_fd;//服务器连接套接字
    struct sockaddr_in server_addr;//服务器监听套接字
    struct sockaddr_in client_addr;//客户端IP地址
    int sin_size;
    char buf[BUFSIZE] = "thanks server";
    //创建服务器套接字
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
      printf("服务器套接字创建失败!\n");
      exit(1);
    }
    //服务器监听套接字信息清0
    memset(&server_addr,'0',sizeof(server_addr));
    //设置服务器监听套接字信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT); //2016
    //设置服务器的IP地址 inet_pton将ip字符串转换为网络字节序
    //inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);
    server_addr.sin_addr.s_addr = INADDR_ANY;//绑定本机ip地址
    //绑定套接字与服务器
    if((bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))) == -1)
    {
        printf("绑定套接字失败!\n");
        exit(1);
    }
    if((listen(sockfd,MAX_QUE_CONN_NM)) == -1)//服务器监听
    {
      printf("服务器监听失败!\n");
      exit(1);
    }
    sin_size = sizeof(struct sockaddr_in);
    printf("服务器监听成功,并正等待客户端的连接......!\n");
    while(1)
    {
       pthread_t thread;//定义线程
       //client_addr 保存客户端地址信息
       client_fd = (int *)malloc(sizeof(int));
       if((*client_fd = accept(sockfd,(struct sock_addr *)&client_addr,&sin_size)) == -1)
       {
          printf("服务器接受客户端连接失败!\n");
       }
       /*
        *char *inet_ntoa(struct in_addr addr);
        *将网络字节序32比特转换为字符串格式的IP地址
       */
       printf("服务器收到来自%s的连接!\n",inet_ntoa(client_addr.sin_addr));//到这里服务器与客户端已经建立连接
   
       /* 这里验证服务器和客户端已经连接上并且可以相互发送消息
       sleep(3);
       read(client_fd,buf,sizeof(buf));
       printf("%s\n",buf); 
       */
       //创建线程并发处理(在连接到客户端后 服务器处于阻塞状态 此时通过向服务器写数据 线程创建)
       
       if((pthread_create(&thread,NULL,th,client_fd) != 0))
       {
          printf("线程创建失败!\n");
          break;
       }
       else
       {
           printf("线程创建成功!已经转入线程处理函数处理!\n");
       }
      
    }
    close(sockfd);
    return 0;
}

//定义线程处理函数
void *th(void *f)
{
  int client_fd = *(int *)f;
  char buf[BUFSIZE]; //设置接受缓存
  /*
  char reqBuf[BUFSIZE] = "I'm server!";
  // 这里验证服务器和客户端已经连接上并且可以相互发送消息
  sleep(3);
  if(read(client_fd,buf,sizeof(buf)) == -1)
  {
      printf("服务器没有收到客户端发来的消息!");
  }
  else
  {
      printf("服务器收到客户端的信息%s\n",buf);
  }
  memset(buf,'0',sizeof(buf));
  if((write(client_fd,reqBuf,strlen(buf))) == -1)
  {
      printf("向客户端发送数据失败!\n");
  }
  else
  {
    
     printf("已经向客户端发送了数据%s!\n",reqBuf);
  }
  */

  /*
   *上面已经验证了客户端可以与服务器通信 并且采用多线程并发处理
   *收到客户端的信息后转入到线程处理函数中处理
   *由于客户端与服务器之间的交互 get命令用于获取服务器目录中的文件
   *put 命令用于上传至服务器 @cd 用于进入服务器的目录 @ls 用于列出
   *服务器的文件 所以以下用for循环进行处理 若用户发送get请求 则拼接
   *的命令为 get 源文件 目标路径  发送的状态码为 GET + 源文件(绝对路径)
   *所以我们用strstr函数 判断字符串中是否存在状态码信息 若存在则转入相
   *应的处理函数 客户端 GET + 源文件  服务器转入do_put函数进行处理
   */
    /*
     *
     * 读取客户端的命令 根据客户端的请求码 服务器进行相应的处理
     * 客户端如果需要请求数据 则发送GET + SRC 
     * 客户端如果需要上传数据 则发送PUT + DES
     * 客户端如果需要进入服务器目录 则发送 CD + SRC
     * 客户端如果需要列出服务器目录文件 则发送 LS +SRC
     */
    for(;;)
    {

       if((read(client_fd,buf,BUFSIZE)) == -1)
       {
           printf("读取客户端的命令失败!\n");
           exit(1);

       }
       else
       {
           printf("服务器已收到客户端的请求命令%s!\n",buf);
       }

       //根据客户端的请求码选择相应的处理函数
       //(1) 客户端发出请求下载数据
       if(buf[0] == 'G')
       {
          if((do_put(client_fd,buf+4)) == -1)
          {
              printf("向客户端发送数据失败!\n");
          }

       }
       //(2) 客户端发出请求上传数据
       else if(buf[0] == 'P')
       {
           if((do_get(client_fd,buf+4)) == -1)
           {
              printf("客户端上传数据失败!\n");
           }
       }
      //(3)客户端发出请求 进入服务器目录
      //客户端此时发送请求为CD + path
      else if(buf[0] == 'C')
      {
          //调用do_cd命令处理
          if((do_cd(client_fd,buf+3)) == -1)
          {
              printf("客户端进入服务器目录失败!\n");
          }
      }
      /*
       *
       * 判断是客户端发送@ls命令 请求获取服务器目录文件
       * 此时客户端发来的状态码为LS + 目录
       */
      else if(buf[0] == 'L')
      {
          //调用do_ls处理
          if(do_ls(client_fd,buf + 3) == -1)
          {
              printf("列出服务器目录文件失败!\n");
          }
      }

    }
    close(client_fd);

   
}
