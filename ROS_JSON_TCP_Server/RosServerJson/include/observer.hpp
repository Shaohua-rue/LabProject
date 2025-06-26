#pragma once
#include <iostream>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include "useJson.hpp"
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
    virtual void updata(std::string funcName,std::unordered_map<std::string,std::string> recvArgMap) override
    {
        if(funcName == "Login")
        {
            std::cout<<"接收到登陆消息"<<std::endl;
            for(auto u : recvArgMap)
            {
                std::cout<< u.first<<" "<< u.second<<std::endl;
            }
        }
    };
};

class ServerB :public AbstractObserver
{
public:
    virtual void updata(std::string funcName,std::unordered_map<std::string,std::string> recvArgMap) override
    {

        if(funcName == "Regist")
        {
            std::cout<<"接收到注册消息"<<std::endl;
            for(auto u : recvArgMap)
            {
                std::cout<< u.first<<" "<< u.second<<std::endl;
            }
        }
    };
};

