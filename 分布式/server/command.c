/*************************************************************************
	> File Name: command.c
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Wed 04 May 2016 11:14:12 PM CST
 ************************************************************************/

#include <stdio.h>
#include "common.h"	
#include "command.h" 

char EreqBufOpen[256] = "ErrServerOpenFile";//服务器打开文件失败
char EreqBufStat[256] = "ErrServerFileStat";//文件状态出错
char EreqBufMode[256] = "ErrServerFileMode";//文件非普通文件
/*
 * command.c文件处理客户端发来的请求码 后面的信息 
 * 如 GET + PATH 则处理PATH信息（绝对路径）
 *
*/

/*
*（1）处理GET请求 为客户端发送文件 主函数调用do_put命令 
*/
int do_put(int client_fd,char *path)
{
  struct stat statbuf;//文件状态
  int fd;//文件描述符 用于打开服务器端被请求的文件
  char buf[BUFSIZE] = "";
  int real_read_len;//实际读取的字节数
  int size;//文件的大小
  //打开path
  if((fd = open(path,O_RDONLY,0666)) == -1)
  {
      printf("服务器打开文件%s失败!\n",path);
      if((write(client_fd,EreqBufOpen,strlen(EreqBufOpen))) == -1)
      {
          printf("向客户端发送错误信息%s失败!\n",EreqBufOpen);
      }
      else
      {
          printf("已向客户端发送出错信息%s\n",EreqBufOpen);
      }
      close(fd);
      return -1;
  }
  //获取文件状态(得到文件的大小)
  if((fstat(fd,&statbuf)) == -1)
  {
      printf("服务器获取文件%s状态失败!\n",path);
      if((write(client_fd,EreqBufStat,strlen(EreqBufStat))) == -1)
      {
          printf("向服务器发送出错信息%s失败!\n",EreqBufStat);
          
      }
      else
      {
          printf("已向客户端发送出错信息%s\n",EreqBufStat);
      }
      close(fd);
      return -1;
  }
  //查看服务器文件是否为普通文件 如果请求文件非普通文件出错
  if(!S_ISREG(statbuf.st_mode))
  {
      printf("客户端请求的文件%s不是普通文件!\n",path);
      if(write(client_fd,EreqBufMode,strlen(EreqBufMode)) == -1)
      {
         printf("向服务器发送出错信息%s失败!\n",EreqBufMode);
      }
      else
      {
          printf("已向客户端发送出错信息%s\n",EreqBufMode);
      }
      close(fd);
      return -1;
  } 
  //如果一切正常 则向服务器发送OK + 文件大小
  sprintf(buf,"OK %d",statbuf.st_size);
  size = (int)(statbuf.st_size);
  if(write(client_fd,buf,strlen(buf)) == -1)
  {
      printf("向客户端写入请求文件长度信息失败!\n");
      close(fd);
  }
  else
  {
      printf("向客户端已写入请求文件%s的长度,并发送OK状态码!\n",path);
  }
  /*
  *由于是4次握手 所以当服务器收到来自客户端的RDY消息后开始发送数据
  */
  if((read(client_fd,buf,BUFSIZE)) == -1)
  {
      printf("读取客户端RDY状态码失败!\n");
  }
  else
  {
      printf("服务器已收到客户端RDY状态码,并准备发送数据!\n");
  }
  //循环读取数据 并发送数据 依次发送1KB
  while((real_read_len = read(fd,buf,MAX_LINE)) >0 )
  {
      printf("正在向服务器发送数据!\n");
      if((write(client_fd,buf,real_read_len)) == -1)
      {
          printf("向客户端发送%s的数据时失败!\n");
          close(fd);
          return -1;
      }
      
  }
  printf("向客户端发送数据文件%s完毕,共%d字节!\n",path,size);
  close(fd);
  return 0;
}
  
//接受客户端上传数据的请求
int do_get(int client_fd,char *path)
{
  int fd;//文件描述符 打开客户端发来的文件路径
  char buf[BUFSIZE];//接受文件缓冲区
  int len;//存储客户端文件的长度
  int n;//每次读取客户端的文件长度
  if((fd = open(path,O_CREAT|O_WRONLY|O_TRUNC,0666)) == -1)
  {
      printf("服务器打开文件失败!\n");
      if((write(client_fd,EreqBufOpen,strlen(EreqBufOpen))) == -1)
      {
          printf("向客户端发送错误信息%s失败!\n",EreqBufOpen);
      }
      else
      {
          printf("已成功向客户端发送错误信息%s!\n",EreqBufOpen);

      }
      close(fd);
      return -1;

  }
  //上面服务器已经打开了目标路径的文件(相当于已经创建了那个文件(pos))
  //向客户端发送信息
  if((write(client_fd,"OK",strlen("OK"))) == -1)
  {
      printf("服务器向客户端发送OK状态码时出错!\n");
  }
  else
  {
      printf("服务器已经向客户端发送了OK状态码!\n");
  }
  memset(buf,'0',BUFSIZE);
  if((read(client_fd,buf,BUFSIZE)) == -1)
  {
      printf("服务器在收到客户端文件信息时出错!\n");
      return -1;
  }
  else
  {
      printf("服务器已收到客户端传来的状态信息%s!\n",buf);
  }
  //获取文件长度(客户端发来的状态码为RDY + 文件长度)
  buf[strlen(buf)] = '\0';
  len = atoi(&buf[4]);//文件长度
  printf("服务器获得的文件长度为%d\n",len);
  memset(buf,'\0',BUFSIZE);
  //sleep(3);
  //循环读取客户端发来的数据
  while(1)
  {
      
     if((n = read(client_fd,buf,MAX_LINE)) == -1)
     {
         printf("服务器读取文件时出错!\n");
         close(fd);
         return -1;
     }
     else
     {
         printf("服务器正在读取客户端文件%d字节!\n",n);
     }
     if(n>=0)
     {
         if((write(fd,buf,n)) == -1)
         {
             printf("服务器向文件写数据失败!\n");
             return -1;
         }
         else
         {
            printf("服务器正在向文件写数据%d字节!\n",n);
         }
         len = len -n;
         printf("客户端剩余文件长度为%d!\n",len);
     }
     if(len == 0)
     {
         printf("服务器已收到客户端的文件,传输完毕!\n");
         break;
     }
  }
  close(fd);
  return 0;
}
/*
 *
 *
 * 处理客户端进入服务器目录的命令
 * 用户输入@cd + 服务器目录
 * 客户端调用do_ser_cd与服务器进行通信 -> 服务器收到在线程函数中
 * 发现状态码为客户端发来的状态码为CD + path -> 则调用服务器自身的处理
 * (进入服务器目录相当于让服务器自身切换目录)-> 向客户端发送OK
 * 函数do_cd(int client_fd,char *path)
 */
 int do_cd(int client_fd,char *path)
 {
    char buf[BUFSIZE];//接受客户端的发来的命令
    memset(buf,'\0',BUFSIZE);
    //改变服务器的工作目录chdir函数
    if((chdir(path)) == -1)
    {
        printf("客户端进入服务器目录出错!\n");
        return -1;
    }
    else
    {
        write(client_fd,"OK",BUFSIZE);
        printf("客户端进入服务器目录成功!\n");
    }
    return 0;
 }
 /*
 *客户端发出@ls命令 -> 客户端调用 do_ser_ls(向服务器发送LS + path)状态码
 *-> 服务器主程序接受命令 判断命令是LS -> 调用do_ls函数 
 *
 *
 */
int do_ls(int client_fd,char *path)
{

    int fd;//文件描述符
    char buf[BUFSIZE];//设置缓冲区
    char cmd[BUFSIZE];//设置命令缓冲区
    struct stat st;
    int len;//用来存储LS.txt文件的长度
    int real_read_len;//保存每次从文件中真实读取的字节数
    //拼接字符串命令
    sprintf(buf,"ls -l %s>LS.txt",path);
    //系统调用 此时将服务器path目录下的文件信息写入到LS.txt
    system(buf);
    printf("服务器已执行命令!");
    printf("服务器已将客户端所请求的目录信息写入LS.文件中!\n");
    //打开文件
    if((fd = open("LS.txt",O_RDONLY)) == -1) //应该用open 打开文件
    {
        //如果服务器打开文件失败 向客户端写出错信息
        printf("服务器打开LS.txt失败!\n");
        if((write(client_fd,EreqBufOpen,strlen(EreqBufOpen))) == -1)
        {
           printf("服务器向客户端写入出错信息时出错!\n");

        }
        else
        {
            printf("服务器已向客户端写入出错信息%s!\n",EreqBufOpen);
        }
        return -1;
    }
    else
    {
        printf("服务器打开LS.txt成功!\n");
    }
    //得到文件信息
    if((fstat(fd,&st)) == -1)
    {
      printf("获取文件状态失败!\n");
      write(client_fd,EreqBufStat,strlen(EreqBufStat));
      printf("已向客户端写入出错信息%s",EreqBufStat);
      return -1;
    }
    else
    {
        printf("服务器获取LS.txt文件状态成功!\n");
    }

    //向客户端发送OK状态 + 文件长度
    memset(buf,'\0',BUFSIZE);
    len = st.st_size;//得到文件长度
    sprintf(buf,"OK %d",len);
    //向客户端写数据
    if((write(client_fd,buf,BUFSIZE)) == -1)
    {
        printf("服务器向客户端发送状态码%s失败!\n",buf);
        return -1;
    }
    else
    {
        printf("服务器已向客户端发送状态码%s",buf);
    }
    //服务器等待接收客户端的RDY状态码
    memset(buf,'\0',BUFSIZE);
    if((read(client_fd,buf,BUFSIZE)) == -1)
    {
        printf("服务器接收客户端RDY状态码失败!\n");
        return -1;
    }
    else
    {
        printf("服务器已接受到客户端状态码%s",buf);
    }
    //服务器开始发送数据 每次1KB
    memset(buf,'\0',BUFSIZE);
    while((real_read_len = read(fd,buf,BUFSIZE)) >0)
    {
        //向客户端写文件
        if((write(client_fd,buf,real_read_len)) == -1)
        {
            printf("向客户端写数据时失败!\n");
            return -1;
        }
        else
        {
            printf("正在向客户端写数据!\n");
        }
        len = len - real_read_len;
        if(len == 0)
        {
            printf("服务器向客户端传送的目录信息已完成!\n");
            close(fd);
            break;
        }

    }
    return 0;
   


}
