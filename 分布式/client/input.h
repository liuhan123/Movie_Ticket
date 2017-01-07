/*************************************************************************
	> File Name: input.h
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sat 30 Apr 2016 09:04:31 PM PDT
 ************************************************************************/
         /*input.h 定义输入命令的格式*/
#ifndef _INPUT_H
#define _INPUT_H
#define BUFSIZE 1024  //客户端输入命令的最大长度
#define MAXLENGTH 64//每个参数或命令的最大长度
#define MAX_ARG 10 //命令所使用的最大参数个数
/*
*用户输入的命令 
*命令结构 存储用户输入命令的名字 和所需要的参数
*name 命令名
*argv:存储命令名和参数 其中argv[0] 也为命令名
* 
* */
//命令结构体
struct Command_Line
{
    char *name;//命令的名字 例如 ls
    char *argv[MAX_ARG];//字符串数组 存储命令名和参数
};
/*命令输入后需要切(传入参数cline为用户输入的命令 将命令分割)
 *然后存入结构体
*/
int split(struct Command_Line *command,char cline[]);
#endif
