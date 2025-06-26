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
int main( int argc, char* argv[] )
{

    Client client;
    client.setTask(sendMessage);

    int i = 100;
    while(i > 0)
    {
        usleep(5);
        client.handleTask();
        i--;
    }

    return 0;
}