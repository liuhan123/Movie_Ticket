/*************************************************************************
	> File Name: command.h
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Mon 02 May 2016 02:43:39 AM PDT
 ************************************************************************/
           /*定义处理从客户端发送来的状态码相应的处理函数*/
           /*
            *
            *
            *客户端给服务其发送的状态码有
            *GET 请求下载文件
            *PUT 请求上传文件
            *@cd 进入服务器的目录
            *@ls 列出服务器目录的内容
            *
            *file为绝对路径
            * */
#ifndef _COMMAND_H
#define _COMMAND_H
#define REQBUFSIZE 256
int do_put(int client_fd,char *path);//处理用户的get命令 
int do_get(int client_fd,char *path);//除了用户的put命令
int do_cd(int client_fd,char *path);//处理用户@cd命令
int do_ls(int client_fd,char *path);//处理用户@ls命令

//客户端发送请求码 服务器返回错误信息
/*
char EreqBufOpen1[256] = "ErrServerOpenFile";//服务器打开文件失败
char EreqBufStat1[256] = "ErrServerFileStat";//文件状态出错
char EreqBufMode1[256] = "ErrServerFileMode";//文件非普通文件
*/
#endif
