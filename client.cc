Finshed for C++ Client unsigned char
//  main.cpp
//  Server(int)
//
//  Created by 刘祚秋 on 2018-12-26.
//  Copyright © 2018 刘祚秋. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

char buffer[40];
unsigned char temp[1300];
unsigned char package[255];
int total = 0;
/*void move(int length)
 {
 int i;
 for(i=length;i>=0;i--)
 temp[length+1]=temp[length];
 }*/
int addt() //求temp内 0-1299号元素的总和
{
    for(int i=0;i<=1299;i++)
        total += temp[i];
    return total;
}
int sum(int i,int n) //求和，i为开始累加的元素编号，n为累加数量
{
    int j;
    int tot = 0;
    for(j = i*255; j <= i*255 + (n-1); i++)
    {
        tot+=temp[j];
    }
    return tot;
}
int time(int length) //打多少个包裹
{
    int tot;
    if(length % 250 == 0)
    {
        tot = length / 250 + 2;
    }
    else
        tot = length/250 +3;
    return tot;
}

int main(){
    int number,i;
    for(i=0;i<=1299;i++)
    {
        temp[i]=i%256;
    }
    
    number = time(sizeof(temp));
    int x=1300;
    
    
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(9999);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    
    //制作第一个package
    package[0] = 5;
    package[1] = 80;
    memcpy(package+2,&x,sizeof(x));
    package[6] = x % 256;
    sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
    recv(sock, buffer, sizeof(buffer),0);
    while(strcmp(buffer,"findheader") != 0)
    {
        sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
        recv(sock, buffer, sizeof(buffer),0);
    }
    //循环发送所有包裹
    for(i=0;i<number-2;i++)
    {
        if(i == number-3)
        {
            memset(package,0,sizeof(package));
            memset(buffer,0,sizeof(buffer));
            package[0] = x - (x/250)*250 + 3;
            package[1] = 81;
            package[2] = i;
            package[3] = x - (x/250)*250;
            for(int j=4;j<=package[3]+3;j++)
                package[j]=temp[i*250+(j-4)];
            
            package[x-(x/250)*250 + 4] = (sum(i,x - (x/250)*250) + package[1]+package[2]+package[3])% 256;
            sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
            recv(sock, buffer, sizeof(buffer),0);
//            printf("第几次%d=%s\n",i,buffer);
//            printf("%d\n",package[4]);
            while(strcmp(buffer,"findpackage") != 0)
            {
                sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
                recv(sock, buffer, sizeof(buffer),0);
            }
        }
        else
        {
            memset(package,0,sizeof(package));
            memset(buffer,0,sizeof(buffer));
            package[0] = 253;
            package[1] = 81;
            package[2] = i;
            package[3] = 250;
            for(int j=4;j<=253;j++)
                package[j]=temp[i*250+(j-4)];
            package[254] = (sum(i,250) + package[1]+package[2]+package[3]) % 256;
            sendto(sock,package,255,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
            recv(sock, buffer, sizeof(buffer),0);
//            printf("第几次%d=%s\n",i,buffer);
//            printf("%d\n",package[4]);
            while(strcmp(buffer,"findpackage") != 0)
            {
                sendto(sock,package,255,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
                recv(sock, buffer, sizeof(buffer),0);
            }
            
        }
    }
    //制作最后一个包裹
    memset(package,0,sizeof(package));
    memset(buffer,0,sizeof(buffer));
    package[0] = 5;
    package[1] = 82;
    total = addt();
    memcpy(package+2,&total,sizeof(total));
    package[6] = total % 256;
    sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
    while(strcmp(buffer,"findlast") != 0)
    {
        sendto(sock,package,package[0]+2,0,(sockaddr*)&serv_addr,sizeof(sockaddr));
        recv(sock, buffer, sizeof(buffer),0);
    }
    //send(sock , temp , strlen(temp) , 0);b
    //读取服务器传回的数据
    //关闭套接字

    printf("done\n");
    close(sock);
    return 0;
}
