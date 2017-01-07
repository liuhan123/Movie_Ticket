/*************************************************************************
	> File Name: input.c
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sat 30 Apr 2016 10:17:17 PM PDT
 ************************************************************************/
    /*
    *input.c文件完成对输入命令的分割 
    *并存储到Command_Line结构体中
    */     
#include <stdio.h>
#include "input.h"
#include "common.h"
#include "command.h"
/*
*split函数定义在input.h文件中
函数原型 int split(struct Command_Line *command,char *cline)
函数成功返回解析后的命令参数的个数 解析出错返回-1
Command_Line 存储解析后的命令 cline 用户输入的命令
*/
//切割字符串
int split(struct Command_Line *command,char *cline)
{
    int pos = 0;
    //将存储命令的结构体清空
    memset(command,'0',sizeof(struct Command_Line));
    //得到用户输入的命令 将最后一个字符'\n' 替换为'\0'
    cline[strlen(cline)-1] = '\0';
    /*
    *使用c库函数 char *strtok(char *s,const char *delim)
    *strtok用来将字符串分割成一个个片段
    *参数s指向欲分割的字符串 delim为分割字符串
    *例如s为"ls -l   /bin" 则分割后ls -l /bin 
    *第一次调用时必须给定s字符串 往后的调用将参数s置为NULL
    *每次调用成功返回下一次分割字符串的指针
    */  
    char *p = strtok(cline," ");//按照空格切割
    /*
     * 给struct Command_Line command 分配存储空间
     *input.h文件中定义的MAXLENGTH = 64为每个参数的存储空间
    */
     //给第一个参数分配空间(argv[0]和name相同)
     command->name = (char *)malloc(MAXLENGTH);
     command->argv[0] = (char *)malloc(MAXLENGTH);
     strcpy(command->name,p);
     strcpy(command->argv[0],p);
     while((p = strtok(NULL," ")))
     {
         pos++;//指向下一个参数
         //分配空间
         command->argv[pos] = (char *)malloc(MAXLENGTH);
         strcpy(command->argv[pos],p);
     }
      //将存储参数的最后一个元素置为NULL
      command->argv[++pos] = NULL;
      //每次分割完字符串后将字符串清空
      memset(cline,'0',strlen(cline));
      return pos;//返回分割的字符串个数

}




