/*************************************************************************
	> File Name: command.c
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sun 01 May 2016 12:32:03 AM PDT
 ************************************************************************/
       /*处理客户端的命令*/
#include <stdio.h>
#include "common.h"
#include "command.h"
#include "input.h"

/*
 *处理客户端do_connect命令 与服务器进行连接
 */
 int do_connect(char *ip,struct sockaddr_in *sin,int *sockfd)
 {
     char buf[BUFSIZE];
     struct sockaddr_in server_addr;//服务器地质结构
     int sfd;//临时套接字
     //结构体地址清0
     memset(&server_addr,'0',sizeof(server_addr));
     server_addr.sin_family = AF_INET;//使用协议地址族
     /*
     *int inet_aton(const char *string,struct in_addr *in)
     *返回值为0 ip字符串格式非法 返回值非0 合法
     *将字符串格式的IP地址转换为网络字节序32比特整型IP地址
     */
     if((inet_aton(ip,&(server_addr.sin_addr))) == 0)
     {
         printf("字符串格式非法!\n");
         return -1;
     }
     /*
      * 端口号必须是网络字节序
      * 将一个主机序16位无符号整数转换为网络字节无符号整数
      * unsigned short htons(unsigned short host_short)
      */
     server_addr.sin_port = htons(PORT);
     //建立socket套接字
     if((sfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
     {
         printf("套接字建立失败!\n");
         return -1;
     }
     //使用该套接字与服务器地址结构相连
     if((connect(sfd,(struct sock_addr *)&server_addr,sizeof(server_addr))) == -1)
     {
         printf("套接字连接失败!\n");
         return -1;
     }
     printf("connect to server %s\n",inet_ntoa(server_addr.sin_addr));
    
   
     *sockfd = sfd; //将sockfd改变 为了以后传输文件
     return 0;
   
 }
 
 /*
 *处理do_get命令 从服务器得到文件
 *main函数传的参数argv[1]为服务器文件 argv[2]为本地路径
 *argv[1]  argv[2] 均为绝对路径
 */
 int do_get(const char *src,const char *des,int sock_fd)
 {
    char buf[BUFSIZE];//存储发送给服务器的命令
    char *dst_file;//目的路径
    char *pos;//返回服务器路径的文件
    int fd;//文件描述符
    struct stat stabuf;//文件状态缓冲区
    int n;//客户端读取的字节数
    int len;//得到的服务器文件的大小
    //检查src和des是否合法
    if(src == NULL || des == NULL)
    {
       printf("please check the path!");
       return -1;
    }
    /*
     * 由于是下载源文件 所以src最后一个字符不应该是/
     * /代表目录 
    */
    if(src[strlen(src) - 1] == '/')
    {
      printf("src为目录,传输的应为标准文件!");
      return -1;
    }
    /*
     *为最终的目标文件路径分配内存空间
     *传输文件时 源文件为file 本地路径为/yas/client
     *最终目标文件路径为/home/client/file
     *缓冲区大小为两个文件路径的长度
    */
     if((dst_file = (char *)malloc(strlen(src) + strlen(des))) == -1)
     {
         printf("内存分配失败!\n");
         return -1;
     }
     else
     {
         printf("文件路径内存分配成功!\n");
     }
     strcpy(dst_file,des);
     printf("当前文件路劲为%s\n",dst_file);
     //如果输入时目标路径不是/ 则添加/
     if(dst_file[strlen(dst_file) - 1] != '/')
     {
          
         /*
          * 如用户输入的目标路径是 /home/client 
          * 此时加上/  ->  home/client/
          */
        strcat(dst_file,"/");
     }
     printf("当前客户端的下载路径为%s\n",dst_file);
     /*
      *然后查找服务器路径(src)中出现的最后一个/
      * 因为/后面为要下载的文件名
      * 
     */

     /*
      * rindex 函数 找到最后一个出现/的地方
      * 返回值为该字符出现的地址
      *char *rindex(const char *s,int c)
     */
     pos = rindex(src,'/');//返回/ + 文件名
     printf("要下载的文件为%s\n",pos);
     //拼接字符串 得到最终目标路径
     strcat(dst_file,pos+1);//跳过/ 得到服务器文件名
     printf("当前客户端的下载路径为%s\n",dst_file);
     //打开文件 如果不存在则创建 如果存在O_TRUNC将目标文件覆盖
     if((fd = open(dst_file,O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1)
     {
         printf("文件打开失败!\n");
         return -1;
     }
     else
     {
         printf("目标文件已打开成功!\n");
     }
     //获取目标文件的状态
     if(fstat(fd,&stabuf) == -1)
     {
         printf("获取文件状态失败!\n");
         return -1;
     }
     /*此时客户端已经准备好目标路径(dst_file) 并且已经打开了文件
      *客户端给服务器发送状态码"GET"
      *sprintf() 函数拼接字符串
      */
      sprintf(buf,"GET %s",src);//buf中存放命令
      /*
       * 写数据 write返回值 -1表示失败 0表示未写任何数据 
       * 非0表示所写字节数
      */
      if((write(sock_fd,buf,strlen(buf))) == -1)
      {
          printf("客户端发送GET %s 请求码时失败!\n",buf);
      }
      else
      {
      printf("client has sent %s 请求码 to the server!\n",buf);
      }
      //发送后buf缓冲清0 用来接受数据
      memset(buf,'\0',BUFSIZE);
      if((n = read(sock_fd,buf,BUFSIZE)) == -1)
      {
          printf("Read Error!");
          close(fd);
          return -1;
      }
      if(n == 0)
      {
          printf("服务器已经关闭了连接\n");
          close(fd);
      }
      if(buf[0] == 'E')
      {
          printf("%s\n",buf);//向屏幕打印出错信息
          close(fd);
          return -1;
      }
      /*
      *根据服务器返回的信息 处理
      *
      */
      /*
       * 此时客户端已经正确收到服务器发回的文件信息
       * 收到的格式为 OK + 文件大小
      */
      sleep(2);
      printf("客户端已经收到服务器文件%s的状态码加文件长度%s",src,buf);
      len = atoi(&buf[3]); //&atoi[3]为文件的长度
      //向服务器发送RDY消息
      if((write(sock_fd,"RDY",strlen("RDY"))) == -1)
      {
          printf("向服务器发送RDY状态码失败!\n");
          return -1;
      }
      else
      {
          printf("已经向服务器发送RDY状态码,客户端准备接收数据!\n");
      }
      //循环读取服务器发来的数据
      sleep(2);
      while(1)
      {
        //读取服务器发送的内容
        if((n = read(sock_fd,buf,MAX_LINE)) == -1)
        {
            printf("客户端接受文件时出错!\n");
            return -1;
        }
        else
        {
            printf("客户端正在接收文件!\n");
        }
        //表明正在接收文件
        if(n>=0) 
        {
           if((write(fd,buf,n)) == -1)//向文件中写数据
           { 
               printf("客户端向文件中写数据失败!\n");
               return -1;
           }
           len -= n;//文件总长度减少
        }
        if(len == 0)
        {
            printf("客户端已接受到服务器的文件%s!\n",src);
            break;
        }
        
      }
      close(fd);
      free(dst_file);
      return 0;
  }

/*
*do_put命令用于向服务器传送文件
*命令格式  argv[1] argv[2] 
*argv[1] 本地文件 argv[2] 服务器目录
*绝对路径
*/
int do_put(const char *src,const char *des,int sock_fd)
{
    int fd;//文件描述符 用于打开源文件
    char buf[BUFSIZE];//存储向服务器发送的命令
    struct stat st;//获取文件状态
    char *des_file;//存储最终向服务器发送的字符串
    char *pos;//返回客户端文件地址
    int len;//保存上传文件时的文件长度
    int real_read_len;//每次真正读取的数据
    if(src == NULL || des == NULL)
    {
        printf("文件名不能为空!\n");
        return -1;
    }
    //源文件后面不应该是/
    if(src[strlen(src)-1] == '/')
    {
        printf("传输的应为标准文件!\n");
        return -1;
    }
    //上传的服务器目录des应该是目录 如果没有/则应该加/
    if(des[strlen(des) - 1] != '/')
    {
        strcat(des,"/");
    }
    /*
    *首先分配最终目标路径的内存 
    *example : 客户端要上传的文件为/home/yas/FTP/Tencent  上传到服务器 /home/yas/FTP/fileserver/
    *则我们需要拼接字符串/home/yas/FTP/fileserver/Tencent 发往服务器
    */
    if((des_file = (char *)malloc(strlen(src) + strlen(des))) == -1)
    {
        printf("客户端拼接的字符串内存已分配失败!\n");
    }
    else
    {
        printf("客户端为最终目标路径分配内存已完成!\n");
    }
    strcpy(des_file,des);//将服务器路径复制到内存
    //找到客户端要上传的文件
    pos = rindex(src,'/');//找到最后一次出现‘/’的地方 并返回地址
    //跳过/ 将客户端文件复制到内存
    strcat(des_file,pos+1);
    printf("客户端已经准备好目标路径%s!\n",des_file);
    //打开需要传输的文件
    if((fd = open(src,O_RDONLY,0666)) == -1)
    {
        printf("客户端打开源文件%s失败!\n",src);
        return -1;
    }
    else
    {
        printf("客户端已打开源文件%s\n",des);
    }
    //获取文件状态(得到文件大小)
    if((fstat(fd,&st)) == -1)
    {
      printf("获取上传文件%s状态失败!\n",pos);
      return -1;
    } 
    else
    {
        printf("获取上传文件%s状态成功!\n",pos);
    }
    len = st.st_size;//获取文件长度
    //此时已得到文件大小和要上传的路径
    //拼接字符串命令 PUT + des_file 发送至服务器
    sprintf(buf,"PUT %s",des_file);
    //向服务器发送请求码
    if((write(sock_fd,buf,strlen(buf))) == -1)
    {
        printf("向服务器发送请求码%s失败!\n",buf);
        return -1;
    }
    else
    {
        printf("向服务器发送请求码%s成功!\n",buf);
    }
    //等待服务器的回应 
    //清空接受缓存
    memset(buf,'\0',BUFSIZE);
    if((read(sock_fd,buf,BUFSIZE)) == -1)
    {
        printf("客户端接受状态码时出错!\n");
        return -1;
    }
    else
    {
        printf("客户端已接受到服务器发来的%s状态码!\n",buf);
    }
    //判断服务器端是否出错
    if(buf[0] == 'E')
    {
        printf("服务器打开文件%s时出错!\n",des_file);
        return -1;
    }
    else
    {
        printf("服务器端已打开文件!\n");
    }
    //现在客户端服务器已经可以通信 并且客户端收到了来自服务器发来的状态码OK
    /*
    * 此时相当于两次握手
    * 客户端此时发送RDY + 文件长度信息 服务器得知文件长度信息后 直接读取
    * 此时客户端向服务器发送RDY + 长度
    */
    memset(buf,'\0',BUFSIZE);//清空接收缓存
    sprintf(buf,"RDY %d",len);//拼接字符串指令
    if((write(sock_fd,buf,BUFSIZE)) == -1)
    {
        printf("向服务器写文件状态信息时出错!\n");
        return -1;
    }
    else
    {
        printf("客户端已向服务器写入文件的状态信息%s!\n",buf);
    }
    //上面客户端已经发送了所有信息 客户端可以传送文件
    /*
    *客户端循环发送文件
    */
    memset(buf,'\0',BUFSIZE);
    //每次读取文件1KB
    while((real_read_len = read(fd,buf,MAX_LINE)) >0 )
    {
      
      if((write(sock_fd,buf,real_read_len)) == -1)
      {
          printf("向服务器写文件失败!\n");
      }
      else
      {
          printf("客户端正在向服务器写文件!\n");
          len = len -real_read_len;
      }
      if(len == 0)
      {
          printf("客户端已向服务器发送完%s文件,传输完毕!\n",pos);
      }
    }
    close(fd);
    return 0;
}

/*
*do_cd (处理进入到客户端目录)
*int do_cd(char *path) path 为绝对路径
*用户输入命令 cd argv[1] argv[1] 为目录
*/
int do_cd(char *path)
{
    //系统调用
    if((chdir(path)) == -1)
    {
        printf("改变目录失败!\n");
        return -1;
    }
    system("cls");
    printf("已经进入到目录%s ",path);
    //获取当前工作路径
    printf("当前客户端的工作路径为%s\n",getcwd(NULL,NULL));
    return 0;

}
/*
* do_ls 列出客户端目录文件
* ls arg[1]
* arg1 为绝对路径
* 可用system系统执行命令
* 拼接字符串命令 然后系统调用
* 再从文件中读取
**/
int do_ls(char *path)
{
  char cmd[BUFSIZE];//系统执行命令
  int fd;//文件描述符
  char buf[BUFSIZE];//存放读取的文件(temp.txt)的一行
  sprintf(cmd,"ls -l %s>cmd.txt",path);//拼接命令 cmd.txt不存在则创建
  printf("已拼接命令%s\n",cmd);
  system(cmd);//执行命令
  printf("命令已执行!\n");
  //读取文件
  /*
  if(fd = open("cmd.txt",O_RDWR) == -1)//以读写方式打开文件
  {
      printf("打开cmd.txt文件失败");
      return -1;
  }
  */
  if((fd = fopen("cmd.txt","r")) == -1) //返回文件指针
  {
      printf("打开cmd.txt文件失败!");
      return -1;
  }
  /*
   *循环读取文件用fgets函数
   *函数原型 char *fgets(char *s,int size,FILE *stream)
   *用来将从stream所指的内存读取字符 并存到s中 直到出现换行符
   *或者读到文件尾 或者读取了size-1个字符
   *所以可实现读取一行的作用
   * 
   * */
   while(fgets(buf,NAME_LEN,fd))
   {
      printf("%s\n",buf);
   }
   close(fd);
   return 0;
}


/*
*int do_ser_cd(char *path)  ...需要与服务器通信
*处理@cd 命令进入到服务器目录
* 当用户输入@cd命令后 客户端向服务器发送CD + path 
* 请求服务器的路劲 服务器收到请求后直接发回路径
* 采用两次握手
*
*path 绝对路径
*/
int do_ser_cd(char *path,int sock_fd)
{
    char buf[BUFSIZE];//存放客户端向服务器发送的命令
    memset(buf,'\0',BUFSIZE);
    //拼接字符串命令
    sprintf(buf,"CD %s",path);
    //向服务器发送命令
    if((write(sock_fd,buf,BUFSIZE)) == -1)
    {
        printf("客户端向服务器发送状态码时出错!\n");
        return -1;
    }
    else
    {
        printf("客户端发送状态码信息%s成功!\n",buf);
    
    }
   
    //收到服务器发来的信息 直接读取
    memset(buf,'\n',BUFSIZE);//清空接受缓存
    if((read(sock_fd,buf,BUFSIZE)) == -1)
    {
        printf("客户端进入服务器目录失败!\n");
        return 0;
    }
    else
    {
        printf("客户端已进入到服务器目录,收到状态码%s!\n",buf);
    }
    return 0;
    
}

/*
*do_ser_ls (列出服务器指定目录文件) ...需要与服务器通信
*处理@ls命令 用户此时输入命令为 @ls + 服务器目录
*int do_ser_ls(char *path,int sock_fd) path为绝对路径
*客户端向服务器发送状态码 LS + path
*/
int do_ser_ls(char *path,int sock_fd)
{
    char buf[BUFSIZE];//客户端命令缓冲区
    int len;//获取客户端文件长度
    int real_read_len;//保存每次从服务器读数据的长度
    //因为是列出目录下的文件 所以客户端应该输入的是一个目录
    if(path[strlen(path) - 1] != '/')
    {
        strcat(path,"/");
    }
    //客户端列出服务器目录文件
   
    //拼接客户端命令
    sprintf(buf,"LS %s",path);
    //向服务器写状态码
    if((write(sock_fd,buf,BUFSIZE) == -1))
    {
        printf("向服务器写状态码失败!\n");
        return -1;
    }
    else
    {
        printf("客户端已向服务器发送状态码%s!\n",buf);
    }
    memset(buf,'\0',BUFSIZE);
    //此时客户端应该收到服务器发来的状态码 OK + 文件长度信息
    if((read(sock_fd,buf,BUFSIZE)) == -1)
    {
        printf("客户端接收服务器的OK状态码时出错!\n");
    }
    else
    {
        printf("客户端已经接收到服务器发来的状态码%s!\n",buf);
    }
    //此时客户端应该获取文件的长度
    len = atoi(&buf[3]);
    //向服务器发送状态码RDY
    memset(buf,'\0',BUFSIZE);
    if((write(sock_fd,"RDY",BUFSIZE)) == -1)
    {
        printf("客户端向服务器发送状态码失败!\n");
    }
    else
    {
        printf("客户端已向服务器发送完状态码RDY\n");
    }
    //客户端准备接受数据(直接打印在控制台)
    while(1)
    {
        if((real_read_len = read(sock_fd,buf,BUFSIZE)) == -1)
        {
            printf("客户端读数据时出错!\n");
            return -1;
        }
        if((write(STDOUT_FILENO,buf,real_read_len)) == -1)
        {
            printf("向控制台打印目录信息时出错!\n");
            return -1;
        }
        memset(buf,'\0',BUFSIZE);
        len = len - real_read_len;
        if(len == 0)
        {
            printf("服务器目录信息已获取完成!\n");
            break;
        }

    }
    return 0;

}

/*
*
*
*/
//客户端退出向服务器发送BYE请求码
int do_bye(int sock_fd)
{
  char buf[BUFSIZE] = "BYE";
  if((write(sock_fd,buf,strlen(buf))) == -1)
  {
      printf("客户端关闭失败!\n");
      return -1;
  }
  return 0;
}

