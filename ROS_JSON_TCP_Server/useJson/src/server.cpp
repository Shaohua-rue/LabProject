#include "server.hpp"

void handleRecvMessage(std::string& recvBuf)
{
    uint32_t header_size = 0;
    // 处理接收到的数据
    if(header_size == 0 && recvBuf.size() >= 4)
    {
        recvBuf.copy((char*)&header_size,4,0);  //从recvBuf的前 4 字节提取出原始消息长度header_size
        recvBuf.erase(0,4);
    }
    if(header_size > 0 && recvBuf.size()>= header_size)
    {
        std::string recvMessage = recvBuf.substr(0, header_size);
        
        recvBuf.erase(0,header_size);

        UseJson recvJs;
        std::string funcName;
        std::unordered_map<std::string,std::string> recvArgMap;
        recvJs.getRecvJson(recvMessage,funcName,recvArgMap);
        
    
        std::cout<<funcName<<std::endl;
        for(auto u : recvArgMap)
        {
            std::cout<< u.first<<" "<< u.second<<std::endl;
        }
        header_size = 0;
        std::cout<<"===========================================>\n\n"<<std::endl;
        
    }
}

int main()
{
    Service server;
    server.setTask(handleRecvMessage);
    server.Run();
    return 0;
}