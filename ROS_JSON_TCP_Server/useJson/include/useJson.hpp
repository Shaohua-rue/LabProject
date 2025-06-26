#pragma once
#include "json.hpp"
#include <unordered_map>

using namespace nlohmann;
class UseJson
{
public:
    /**
     *@brief 设置发送的json数据
     *@param input funcName 函数名
     *@param input umap 参数列表
    */
    void setFuncAndArgs(std::string& funcName,std::unordered_map<std::string,std::string> umap)
    {
        sendJs_["funcName"] = funcName;
        sendJs_["args"] = umap;
    }

    /**
     *@brief 获取发送的jsonStr数据
     *
     * 注意：为防止粘包，这里的buf前四个字节存储了jsonStr的大小进行组包
     * 拆包时，应该：
     *      uint32_t header_size = 0;
     *      buf.copy((char*)&header_size,4,0);  //从buf的前 4 字节提取出原始消息长度header_size
     *      buf.erase(0,4);
     *      string jsonStr = buf.substr(0, header_size);    //真正的json数据  
     *      buf.erase(0,header_size);
    */
    std::string getSendString()
    {
        std::string jsonStr = sendJs_.dump();
        uint32_t size = jsonStr.size();
        std::string buf;
        buf.insert(0, std::string((char*)&size, 4));//将size的值以二进制形式插入到buf的开头（前4字节）
        buf += jsonStr;
        return buf;
    }

    /**
     *@brief 获取函数名与参数列表
     *@param input str 删去前四字节的实际strJson数据
     *@param output funcName 函数名
     *@param output args 参数列表
    */
    void getRecvJson(std::string str,std::string& funcName,std::unordered_map<std::string,std::string>& args)
    {
        recvJs_ = json::parse(str);
        if(!recvJs_.empty())
        {
            funcName = recvJs_["funcName"];
            args = recvJs_["args"].get<std::unordered_map<std::string, std::string>>();
        }
    }
private:
    json sendJs_;
    json recvJs_;
};

