#include <iostream>
#include <Camera.h>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <signal.h>
using namespace std;


CameraNode camera;
bool mainLoop=true;

void sig_exit(int s);
void fileCheck(std::string file_path);


int main(int argc,char *argv[])
{
    
   
   
    // 触发下面的信号就退出
    //SIGABRT 程序异常终止信号  SIGFPE浮点异常信号 SIGILL非法指令信号 SIGINT用户中断信号 SIGQUIT用户退出信号 SIGTERM终止信号
    signal(SIGINT, sig_exit);
    signal(SIGINT, sig_exit);
    signal(SIGQUIT, sig_exit);
    signal(SIGTERM, sig_exit);

    std::string filePath="/home/wsh/aaa.txt";
    fileCheck(filePath);
    // std::thread thValue=std::thread(fileCheck,filePath);
    // thValue.join();
    return 0;
}
void sig_exit(int s)
{
    mainLoop=false;
    camera.closeCamera();
	exit(0);
}
void fileCheck(std::string file_path)
{
    
    std::string last_time="";
    while(mainLoop)
    {
        std::ifstream inputFile(file_path);
		if (!inputFile.is_open()) {
        	std::cerr << "无法打开文件,请检查文件路径" << std::endl;
        	return;
    	}

        //逐个读取文件中的单词
        std::string buff;
        std::vector<std::string> goal_string;
        while(inputFile>>buff)
        {
            goal_string.push_back(buff);
        }
        if(goal_string.size()==5)
        {
            if(goal_string[0]=="camera")   //相机
            {
                if(last_time!=goal_string[4])
                {
                    last_time=goal_string[4];

                    std::string mode = goal_string[1];     //两种模式（claimGoods：取货； deliverGoods：送货）
                    if(mode=="claimGoods")
                    {
                        std::string goodsShelf=goal_string[2]; //ABC
                        std::string goods=goal_string[3];   //abc
                        std::cout<<"*****************************************"<<std::endl;

                        std::cout<<"goodsShelf: "<<goodsShelf<<std::endl;
                        std::cout<<"goods: "<<goods<<std::endl;
                        std::cout<<"time: "<<last_time<<std::endl<<std::endl;



                        sleep(1);
                        bool rec=camera.claimGoodsTask(goodsShelf,goods);
                        if(rec)
                        {
                            std::cout<<"claimGoodsTask success"<<std::endl;
                        }

                    }
                    else if(mode=="deliverGoods")
                    {
                        std::string carShelf=goal_string[2]; //F
                        std::string goodsShelf=goal_string[3];//ABC
                        sleep(1);
                        std::cout<<"*****************************************"<<std::endl;
                        std::cout<<"carShelf: "<<carShelf<<std::endl;
                        std::cout<<"goodsShelf: "<<goodsShelf<<std::endl;
                        std::cout<<"time: "<<last_time<<std::endl<<std::endl;
                        bool rec=camera.deliverGoodsTask(carShelf,goodsShelf);
                        if(rec)
                        {
                            std::cout<<"deliverGoodsTask success"<<std::endl;
                        }



                    }
                    else
                    {
                        std::cerr<<"mode error"<<std::endl;
                    }
                }
            }

            goal_string.clear();
        }
        else
        {
            goal_string.clear();
        }

        usleep(500);
    }
}

