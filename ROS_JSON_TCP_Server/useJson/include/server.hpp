#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "useJson.hpp"
#include <functional>
#define  BUF_SIZE (4096) 

class Service
{
public:
    using func = std::function<void(std::string&)>;
    Service(std::string ip = "192.168.6.130",int port = 1200)
    {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(sock_fd < 0)
        {
            std::cout << "create socket error" << std::endl;
        }
        
        // 设置端口可复用
        int opt = 1;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Failed to set socket option" << std::endl;
            close(sock_fd);
        }

        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = inet_addr(ip.c_str());
        local.sin_port = htons(port);

        ret = bind(sock_fd, (struct sockaddr*)&local, sizeof(local));
        if(ret < 0)
        {
            std::cout << "bind error" << std::endl;
        }
        ret = listen(sock_fd, 5);
        if(ret < 0)
        {
            std::cout << "listen error" << std::endl;
            close(sock_fd);
        }

        epoll_fd = epoll_create(100);
        if(epoll_fd < 0)
        {
            std::cout << "epoll create error" << std::endl;
        }
       

        ev.data.fd = sock_fd;
        ev.events = EPOLLIN;

        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) < 0)
        {
            std::cout << "epoll add error" << std::endl;
        }
    }
    ~Service()
    {
        if(sock_fd ==- 1)
        {
            close(sock_fd);
        }
    }
    void Run()
    {
        while(1)
        {
            int nfds = epoll_wait(epoll_fd, events, 10, timeout);
            if (nfds ==  -1)
            {
                std::cout << "epoll wait error" << std::endl;
            }
            else if(nfds == 0)
            {
                //std::cout << "epoll wait timeout" << std::endl;
                continue;
            }
            
            for(int i = 0; i < nfds; i++)
            {
                int fd = events[i].data.fd;
                
                if(fd == sock_fd)
                {
                    new_fd = accept(sock_fd, (struct sockaddr*)&peer, &addrlen);
                    if(new_fd < 0)
                    {
                        // 在非阻塞模式下，accept可能返回EAGAIN或EWOULDBLOCK
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue;
                        }
                        
                        std::cout << "accept error" << std::endl;
                    }
                    
                    setNonBlocking(new_fd);
                    ev.data.fd = new_fd;
                    ev.events = EPOLLIN | EPOLLET;  // 使用边缘触发模式
                    
                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &ev) < 0)
                    {
                        std::cout << "epoll add error" << std::endl;
                        close(new_fd);
                        continue;
                    }
                    
                    std::cout << "New connection from " 
                            << inet_ntoa(peer.sin_addr) 
                            << ":" << ntohs(peer.sin_port) 
                            << std::endl;
                }
                else
                {
                    if(events[i].events & EPOLLIN)
                    {
                        memset(recv_buf, 0, BUF_SIZE);
                        unsigned int recv_len = 0;
                        bool connection_closed = false;
                        
                        // 在边缘触发模式下，必须读完所有数据
                        while(1)
                        {
                            ret = recv(fd, recv_buf + recv_len, BUF_SIZE - recv_len, 0);
                            if(ret == 0)
                            {
                                std::cout << "client close" << std::endl;
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                                close(fd);
                                connection_closed = true;
                                break;
                            }
                            else if(ret < 0)
                            {
                                if(errno == EAGAIN || errno == EWOULDBLOCK)
                                {
                                    // 已经读取完所有数据
                                    //std::cout << "recv data len:" << recv_len << std::endl;
                                    break;
                                }
                                else if(errno == EINTR)
                                {
                                    // 系统调用被中断，继续读取
                                    continue;
                                }
                                else
                                {
                                    // 其他错误，关闭连接
                                    std::cout << "recv error: " << strerror(errno) << std::endl;
                                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                                    close(fd);
                                    connection_closed = true;
                                    break;
                                }
                            }
                            else
                            {
                                recv_len += ret;
                                
                                // 如果缓冲区已满，可能还有更多数据
                                if (recv_len >= BUF_SIZE) {
                                    std::cout << "Buffer is full, but there might be more data" << std::endl;
                                    break;
                                }
                            }
                        }
                        
                        if (!connection_closed && recv_len > 0) {
                            std::string recvBuf(recv_buf, recv_len);
                            if(task_!= nullptr)
                                task_(recvBuf);
                        
                        }
                    }
                    
                    if(events[i].events & (EPOLLERR | EPOLLHUP))
                    {
                        std::cout << "epoll error or hangup" << std::endl;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                    }
                }
            }
        }
    }

    void setTask(func task)
    {   
        task_ = task;
    }
private:
    void setNonBlocking(int fd)
    {
        int flag = fcntl(fd, F_GETFL,0);
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
private:
    func task_;
    int timeout = 1000;
    char recv_buf[BUF_SIZE] = {0};
    
    struct sockaddr_in local;
    struct sockaddr_in peer;

    socklen_t addrlen = sizeof(peer);
    int sock_fd = 0, new_fd = 0;
    int ret = 0;

    int epoll_fd = -1;
    struct epoll_event ev, events[10];
};