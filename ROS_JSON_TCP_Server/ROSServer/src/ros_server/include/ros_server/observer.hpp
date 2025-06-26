#pragma once
#include <iostream>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include "useJson.hpp"
#include "ros/ros.h"
#include "ros_server/login.h"
#include "ros_server/regist.h"
class AbstractObserver
{
public:
    virtual ~AbstractObserver(){};
    virtual void updata(std::string str,std::unordered_map<std::string,std::string> recvArgMap){};
};

class AbstractSubject
{
public:
    virtual ~AbstractSubject(){};
    virtual void registerObserve(std::shared_ptr<AbstractObserver> obs){}
    virtual void removerObserve(std::shared_ptr<AbstractObserver> obs){}
    virtual void notify(std::string str,std::unordered_map<std::string,std::string> recvArgMap){}
};


class Subject: public AbstractSubject
{
public:
    ~Subject(){};
    virtual void registerObserve(std::shared_ptr<AbstractObserver> obs) override
    {
        if(std::find(observers.begin(), observers.end(), obs) == observers.end())
            observers.insert(obs);
    }
    virtual void removerObserve(std::shared_ptr<AbstractObserver> obs)override
    {
        if(std::find(observers.begin(), observers.end(), obs) != observers.end())
            observers.erase(obs);
    }
    virtual void notify(std::string str,std::unordered_map<std::string,std::string> recvArgMap)override
    {
        for(auto& obs : observers)
        {
            obs->updata(str,recvArgMap);
        }
    }
private:
    std::unordered_set<std::shared_ptr<AbstractObserver>> observers;
};

class ServerA :public AbstractObserver
{
public:
    virtual ~ServerA()override{}
    ServerA(std::shared_ptr<ros::NodeHandle> nh):nh_(nh)
    {
        client = nh->serviceClient<ros_server::login>("Login");
    }
    virtual void updata(std::string funcName,std::unordered_map<std::string,std::string> recvArgMap) override
    {
        if(funcName == "Login")
        {
            std::cout<<"接收到登陆消息"<<std::endl;
            ros_server::login Request;
            for(auto u : recvArgMap)
            {
                std::cout<< u.first<<" "<< u.second<<std::endl;
                if(u.first == "userid")
                {
                    Request.request.userid = atoi(u.second.data());
                }
                if(u.first == "password")
                {
                    Request.request.password = atoi(u.second.data());
                }
            }

            bool flag = client.call(Request);
            // 7.处理响应
            if (flag)
            {
                ROS_INFO("请求正常处理,响应结果:%d",Request.response.recv);
            }
            else
            {
                ROS_ERROR("请求处理失败....");
            }


        }
    };
private:
    std::shared_ptr<ros::NodeHandle> nh_;
    ros::ServiceClient client;
};



class ServerB :public AbstractObserver
{
public:
    virtual ~ServerB()override{}
    ServerB(std::shared_ptr<ros::NodeHandle> nh):nh_(nh)
    {
        client = nh->serviceClient<ros_server::regist>("Regist");
    }

    virtual void updata(std::string funcName,std::unordered_map<std::string,std::string> recvArgMap) override
    {
        if(funcName == "Regist")
        {
            std::cout<<"接收到注册消息"<<std::endl;
            ros_server::regist Request;
            for(auto u : recvArgMap)
            {
                std::cout<< u.first<<" "<< u.second<<std::endl;
                if(u.first == "userid")
                {
                    Request.request.userid = atoi(u.second.data());
                }
                if(u.first == "password")
                {
                    Request.request.password = atoi(u.second.data());
                }
            }

            bool flag = client.call(Request);
            // 7.处理响应
            if (flag)
            {
                ROS_INFO("请求正常处理,响应结果:%d",Request.response.recv);
            }
            else
            {
                ROS_ERROR("请求处理失败....");
            }


        }
    };
private:
    std::shared_ptr<ros::NodeHandle> nh_;
    ros::ServiceClient client;
};
