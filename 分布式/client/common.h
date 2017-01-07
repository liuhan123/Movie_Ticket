/*************************************************************************
	> File Name: common.h
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sat 30 Apr 2016 08:56:18 PM PDT
 ************************************************************************/
               /*网络编程之客户端编程 头文件*/
#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>

#define MAX_LINE 1024  //向服务器发送数据的最大长度
#define PORT 2016     //端口号
#define NAME_LEN 256 //linux最大的文件名(用来读取文件时读一行)
#endif
