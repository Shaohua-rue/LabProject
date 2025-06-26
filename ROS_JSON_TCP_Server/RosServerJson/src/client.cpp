#include "client.hpp"
bool sendMessage(int fd)
{
    std::string fucName = "Login";
    std::unordered_map<std::string,std::string> argMap {{"userid","12345"},{"name","shaohua"},{"password","12345"},{"vec","123"}};
    UseJson js;
    js.setFuncAndArgs(fucName,argMap);


    std::string data = js.getSendString() + js.getSendString() + js.getSendString();

    size_t n = send(fd , data.c_str() , data.size() , 0);
    if( n < 0)
    {
        std::cout << "Send failed : " << data << std::endl;
        return false;
    }
}

bool sendMessageB(int fd)
{
    std::string fucName = "Regist";
    std::unordered_map<std::string,std::string> argMap {{"userid","777"},{"name","shaohuaWANG"},{"password","12345"},{"vec","12DD3"}};
    UseJson js;
    js.setFuncAndArgs(fucName,argMap);


    std::string data = js.getSendString() + js.getSendString() + js.getSendString();

    size_t n = send(fd , data.c_str() , data.size() , 0);
    if( n < 0)
    {
        std::cout << "Send failed : " << data << std::endl;
        return false;
    }
}
int main( int argc, char* argv[] )
{

    Client client;
    client.setTask(sendMessage);
    client.setTaskB(sendMessageB);
    int i = 10;

        sleep(1);
        client.handleTask();

        sleep(1);
        client.handleTaskB();
        i--;

     sleep(3);
    return 0;
}