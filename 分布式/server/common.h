/*************************************************************************
	> File Name: common.h
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Mon 02 May 2016 02:35:10 AM PDT
  ************************************************************************/
              /*网络编程之服务器分布式存储系统*/
              
              /*common.h文件存储服务器所需的头文件和一些定义的宏变量*/
#ifndef _COMMON_H
#define _COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#define MAX_QUE_CONN_NM 10 //设置服务器监听的队列长度
#define MAX_LINE 1024//向客户端发送数据的最大长度
#define PORT 2016//端口号
#define NAME_LEN 256//linux最大的文件名(读取文件时读一行)
#define BUFSIZE 1024//设置接受缓存大小
#endif
