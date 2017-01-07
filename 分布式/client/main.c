/*************************************************************************
	> File Name: main.c
	> Author: yas
	> Mail: rage_yas@hotmail.com
	> Created Time: Sat 30 Apr 2016 09:44:31 PM PDT
 ************************************************************************/
            /*主函数*/
#include <stdio.h>
#include "command.h"
#include "input.h"
#include "common.h"
int main()
{
    char cline[BUFSIZE];//存储用户输入的命令
    struct Command_Line command;//命令结构 存储分解后的命令
    int sockfd;//socket描述符
    int i,j; //i 返回解析后的字符串个数
    struct sockaddr_in server_addr;//服务器地址
    printf("FTP.......::");//打印提示符
    fflush(stdin);//清空缓冲区
    char buf[BUFSIZE] = "client";
    /*
     * 从 stream所指的文件内读入字符并写入到s里
     *char *fgets(char *s,int size,FILE *stream)
     *fgets命令若成功返回s指针 失败返回NULL
    */
    //循环输入命令
    while(fgets(cline,sizeof(cline),stdin) != NULL)
    {
        /*
        *example :ls -l /server 需要解析命令和参数 
        *调用自定义split函数(函数实现在input.c文件中)
        */
        //首先判断输入的命令是否为NULL
        if(!strcmp("\n",cline))
        {
            printf("The command is not NULL");
            continue;
        }
        //验证split函数解析后的字符串
        i = split(&command,cline);
        printf("命令已解析完毕!\n");
        printf("命令名为%s 参数依次为：",command.name);
        for(j = 1;j<i;j++)
        {
            printf("%s ",command.argv[j]);
        }
        printf("\n");
        /*
         * 然后根据命令名做出相应的处理 strcase函数 忽略大小写
        *
        */
        /*
         * get命令从服务器获取文件 参数argv[1] 服务器目录文件
         * argv[2] 本地客户端目录 
         * 从argv[1] -> argv[2]
        */
        if(!strcasecmp(command.name,"get"))
        {
            do_get(command.argv[1],command.argv[2],sockfd);
        }
        /*
        *put命令向服务器传送文件 argv[1] 本地客户端目录文件
        *argv[2] 服务器目录 绝对路径
        */
        else if(!strcasecmp(command.name,"put"))
        {
            do_put(command.argv[1],command.argv[2],sockfd);
        }
        /*
        *cd 命令 进入客户端目录
        */
        else if(!strcasecmp(command.name,"cd"))
        {
            do_cd(command.argv[1]);
        }
        /*
        *@cd进入服务器目录 (需要通信)
        */
        else if(!strcasecmp(command.name,"@cd"))
        {
           do_ser_cd(command.argv[1], sockfd);
        }
        /*
        *ls 列出客户端目录文件（不需socket通信）
        */
        else if(!strcasecmp(command.name,"ls"))
        {
            do_ls(command.argv[1]);
        }
        /*
        *@ls 列出服务器端的内容 
        */
        else if(!strcasecmp(command.name,"@ls"))
        {
            do_ser_ls(command.argv[1],sockfd);
        }
        /*
        *connect连接服务器 
        */
        else if(!strcasecmp(command.name,"connect"))
        {
            //将sockfd改变 则可用通信
            do_connect(command.argv[1],&server_addr,&sockfd);
        }
        /*
        *客户端主动断开连接
        *bye命令
        */
        else if(!strcasecmp(command.name,"bye"))
        {
            do_bye(sockfd);
            break;//跳出循环
        }
        else
        {
            printf("please check your command and resume load!\n");
            printf("Command Usage: command name + arg[1] + argv[2] + ...\n");
            printf("FTP....... please input next command!\n");
            fflush(stdin);
            continue;
        }
        /*
        
        *在服务器与客户端连接后 测试向服务器写数据
        
        if((write(sockfd,buf,strlen(buf))) == -1)
        {
            printf("客户端发送数据失败\n");
        }
        printf("客户端发送数据成功!\n");
        //验证客户端与服务器之间的通信
        sleep(3);
        memset(buf,'0',sizeof(buf));
        if(read(sockfd,buf,sizeof(buf)) == -1)
        {
            printf("客户端没有收到来自服务器的信息!");
        }
        else
        {
            p buf[strlen(buf)] = '\0';
            printf("客户端收到来自服务器的信息!%s\n",buf);
        }
       */
        printf("FTP....... please input next command!\n");
        fflush(stdin);
    } 
    //跳出循环后关闭socket
    close(sockfd);
    return 0;
}
