/*************************************************************************
	> File Name: command.h
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sat 30 Apr 2016 09:23:27 PM PDT
 ************************************************************************/
         /*处理用户输入的命令*/
#ifndef _COMMAND_H
#define _COMMAND_H
#include "common.h"
//处理connnect命令 与服务器建立连接
int do_connect(char *ip,struct sockaddr_in *sin,int *sock_fd);
/*
 * 处理get命令 从服务器得到文件(src,des 为绝对路径 )
 * 从src目录下载到des目录
 */
int do_get(const char *src,const char *dst,int sock_fd);
/*
 * 处理put命令 向服务器传送文件
 * src des为绝对路径 sock_fd:socket描述符
*/
int do_put(const char *src,const char *des,int sock_fd);
/*
*do_cd (处理进入到客户端目录处理cd命令)
*path同为绝对路径
*/
int do_cd(char *path);
/*
*处理列出目录(客户端目录) path 绝对路径
*/
int do_ls(char *path);
/*
*do_ser_cd 处理@cd命令 进入服务器目录 
*/
int do_ser_cd(char *path,int sock_fd);
/*
*do_ser_ls 处理@ls命令 列出服务器目录的内容  path绝对路径
*/
int do_ser_ls(char *path,int sock_fd);
/*
*do_bye 处理bye命令 退出
*/
int do_bye(int sock_fd);
#endif
