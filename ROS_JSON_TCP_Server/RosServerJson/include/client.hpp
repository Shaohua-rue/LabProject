#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "useJson.hpp"
#include <iostream>
#include <functional>

class Client
{
public:
    using func = std::function<bool(int)>;
    Client(std::string ip = "192.168.6.130",int port = 1200)
    {

        struct sockaddr_in server_address;
        bzero( &server_address, sizeof( server_address ) );
        server_address.sin_family = AF_INET;
        inet_pton( AF_INET, ip.c_str(), &server_address.sin_addr );
        server_address.sin_port = htons( port );

        sockfd = socket( PF_INET, SOCK_STREAM, 0 );

        if (connect(sockfd , (struct sockaddr *)&server_address , sizeof(server_address)) < 0)
        {
                perror("connect failed. Error");
        }
    }
    ~Client()
    {
        if(sockfd != -1)
            close(sockfd);
    }
    void setTask(func task)
    {
        task_ = task;
    }
    bool handleTask()
    {
        if(task_!=nullptr)
        {
            if(sockfd != -1)
                return task_(sockfd);
        }
        return false;
    }
    void setTaskB(func task)
    {
        taskB_ = task;
    }
    bool handleTaskB()
    {
        if(task_!=nullptr)
        {
            if(sockfd != -1)
                return taskB_(sockfd);
        }
        return false;
    }
   
    
private:
    func task_;
    func taskB_;
    int sockfd = -1;
};