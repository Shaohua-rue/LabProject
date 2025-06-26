#include <Camera.h>
CameraNode:: CameraNode():cameraStatus{false},cameraInitStatus{true},sp{iosev}
 {
      serialInit();
      th=std::thread(&CameraNode::cameraDesktopThread,this);
      //th=std::thread(&CameraNode::cameraThread,this);
      th.detach();
 }
 CameraNode:: ~CameraNode()
 {
    th.join();
    cameraInitStatus=false;
 }
bool CameraNode::claimGoodsTask(std::string goodsShelf,std::string goodsName)
{
    //首先抓手右转90度
    std::cout<<"抓手右转90度"<<std::endl;
    serialPortSent(10,10,0x66,10);
    sleep(5);
    std::cout<<"旋转结束"<<std::endl;

    //进行货架识别
    LayerControl(image,goodsShelf);

    std::cout<<"抓手平台向上移动固定距离，准备进行货物识别与矫正"<<std::endl;
    serialPortSent(0xbb,0x01,0x44,10);
    sleep(4);

    //进行货物识别
    goodsControl(image,goodsName);

    std::cout<<"开始下降"<<std::endl;
    serialPortSent(0xaa,0x01,0x44,10);
    sleep(4);

    //通知抓取
    serialPortSent(10,10,0x22,10);
    std::cout<<"抓取任务结束"<<std::endl;
    return true;
}
bool CameraNode::deliverGoodsTask(std::string carShelf,std::string goodsShelf)
{
    //首先告知送货模式
    serialPortSent(10,0x02,0x55,10);

    //进行料箱车架识别
    LayerControl(image,carShelf);
    std::cout<<"料箱车层数矫正完成"<<endl;

    //发送取货后旋转命令
    serialPortSent(99,0x99,0x99,10);   //----具体值待定
    sleep(8);

    //进行货架识别
    LayerControl(image,goodsShelf);

    std::cout<<"货架识别成功，发送送货指令";
    serialPortSent(10,10,0x22,10);
    std::cout<<"送货任务结束"<<std::endl;
    return true;
}
void CameraNode::goodsControl(cv::Mat& image,std::string goodsName)
{
    bool goodStatus=true;
    while(goodStatus)
    {
        //首先判断货物是否在范围内
        QRCode(image,informations,points);
        for (const auto& info : informations)
        {
            if(info==goodsName)
            {
                std::cout<<"开始货物矫正"<<std::endl;
                positionControl(image,goodsName);
                goodStatus=false;
            }
        }
    }
    std::cout<<"货物矫正完成"<<std::endl;

    

}

bool CameraNode::positionControl(cv::Mat& image,std::string str)
{
    Point2d pt(0, 0);
    bool isPosition=false;

    while(!isPosition)
    {
      QRCode(image,informations,points);
      if(points.size()>0)
      {
        int p=0;
        for (int i = 0; i < informations.size(); i++)
        {
        
            std::string infor=informations[i];
            if(infor==str)
            {
                
                //计算中心点坐标
                double x= (points[p].x + points[p+2].x) / 2.0;
                double y =(points[p].y + points[p+2].y) / 2.0;

                cv::circle(image, pt, 3, Scalar(255, 0, 0, 255), 2, 8, 0);
                if((200<x)&&(x<300)&&(200<y)&&(y<300))
                {
                    isPosition=true;
                }
                else
                {
                    //位置矫正
                    serialPortSent(x-1224,y-1024,0x11,10);
                    putText(image, str.c_str(), Point(20, 30), 2, 1, Scalar(0, 0, 255, 255), 4);
                    putText(image, "u:"+ to_string(x)+"  v:"+ to_string(y), Point(20, 60), 2, 1, Scalar(0, 0, 255, 255), 4);
                    cv::imshow("image",image);
                }
            }
            p+=4;
        }        
      }

    }
    return true;
}
void CameraNode::LayerControl(Mat& image,string goodsShelf)
{
    bool LayerControlStatus=true;
    std::cout<<"开始货架矫正识别"<<std::endl;
    while(LayerControlStatus)
    {
         
        QRCode(image,informations,points);
        for(const auto& info : informations)
        {
            
            if(strCheack(info,goodsShelf))//检查首字母是否相等
            {
            
            positionControl(image,info); //位置矫正
            layersUpOrDowm(info,goodsShelf);//进行货架上升或下降处理
            positionControl(image,goodsShelf);//位置矫正

            
            LayerControlStatus=false;
            }
        }
    }
    std::cout<<"相机已达到目标货架位置"<<std::endl;

}
 


bool CameraNode::strCheack(std::string str1,std::string str2)
{
    if(extractUppercaseLetters(str1)!=extractUppercaseLetters(str2))
    {
        return false;
    }
    else
    {
        return true;
    }
}
std::string CameraNode::extractUppercaseLetters(const std::string& input) 
{
    std::string result;
    for (char ch : input) {
        if (std::isupper(ch)) {
            result += ch;
        }
    }
    return result;
}


void CameraNode::layersUpOrDowm(std::string goodsShelf,std::string goalString)
{
    int shelfNum=extractNumber(goodsShelf);  //当前识别出来的货架编号
    int goalNum= extractNumber(goalString); //目标货架编号
    if(shelfNum!=-1)
    {
        int currentNumber=0;
        int shelfLayering=5;  //货架层数
        currentNumber=shelfNum%shelfLayering;      //shelfNum即识别出来的货架二维码编号不为0
        if(currentNumber==0)
        {
            currentNumber=shelfLayering;
        }
        int upOrDown=goalNum-currentNumber;

        switch (upOrDown)
        {
            case -4:
            {
                cout<<"向下移动四层"<<endl;
                serialPortSent(0xaa,0x04,0x33,10);
                sleep(8);
                break;
            }
            case -3:
            {
                cout<<"向下移动三层"<<endl;
                serialPortSent(0xaa,0x03,0x33,10);
                sleep(6);
                break;
            }
            case -2:
            {
                cout<<"向下移动二层"<<endl;
                serialPortSent(0xaa,0x02,0x33,10);
                sleep(4);
                break;
            }
            case -1:
            {
                cout<<"向下移动一层"<<endl;
                serialPortSent(0xaa,0x01,0x33,10);
                sleep(2);
                break;
            }
            case 0 :
            {
                cout<<"当前层数为目标层数，无需校准"<<endl;
                break;
            }
            case 1:
            {
                cout<<"向上移动一层"<<endl;
                serialPortSent(0xbb,0x01,0x33,10);
                sleep(2);
                break;
            }
            case 2:
            {
                cout<<"向上移动两层"<<endl;
                serialPortSent(0xbb,0x02,0x33,10);
                sleep(4);
                break;
            }
            case 3:
            {
                cout<<"向上移动三层"<<endl;
                serialPortSent(0xbb,0x03,0x33,10);
                sleep(6);
                break;
            }
            case 4:
            {
                cout<<"向上移动四层"<<endl;
                serialPortSent(0xbb,0x04,0x33,10);
                sleep(8);
                break;
            }
            default:
            {
                cout<<"识别错误"<<endl;
            }
        }

    }
        
}




int CameraNode::extractNumber(std::string input)
{
    std::string result;
    for (char ch : input) {
        if (std::isdigit(ch)) {
            result += ch;
        }
    }
    if (result.empty()) {
        return -1; 
    }
    try {
        int extractedNumber = std::stoi(result);
        return extractedNumber;
    } catch (const std::invalid_argument& ia) {
        std::cerr << "Invalid argument: " << ia.what() << std::endl;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of range error: " << oor.what() << std::endl;
    }
    return -1; 
}

void CameraNode::QRCode(cv::Mat& image,std::vector<std::string>&informations,std::vector<Point2d>&points)
{
    
    cv::Mat gray;
    bool isQRcode=false;
    informations.clear();
    points.clear();
    cvtColor(image, gray, COLOR_BGR2GRAY);
    cv::QRCodeDetector qrCodeDetector;
    isQRcode = qrCodeDetector.detectAndDecodeMulti(gray, informations, points);  
    if (isQRcode==true) 
    {
        int p = 0;
        int m =3;
        for(int K=0;K< informations.size();K++)
        {
            for (int i = p; i < (p+4); i++)
            {
                if (i == m) 
                {
                    line(image, points[p], points[p + 3], Scalar(0, 0, 255, 255), 2, 8);
                }
                else
                {
                    line(image, points[i], points[i + 1], Scalar(0, 0, 255, 255), 2, 8);
                }
                putText(image, to_string(i + 1), points[i], 2, 1, Scalar(0, 0, 255, 255), 4);
                cv::imshow("image",image);
            }
            p = p + 4;
            m = m + 4;      
        }
    }

}


void CameraNode::cameraDesktopThread()
 {
     
	VideoCapture cap;	// 创建视频采集对象;
	cap.open(0);    // 打开默认相机;

    // 设置采集图片大小; 具体大小根据相机设置，分辨率设置为680*480;
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);
    if (!cap.isOpened())
    {
        printf("open camera failed!\n");
        return;
    }

    cv::Mat frame;
    while(cameraInitStatus)
    {
        cap >> frame;
        if(!frame.empty())
        {
            image=frame;
            cv::imshow("frame",frame);
            int keyValue=cv::waitKey(5);
            if(keyValue==27)
            {
                cameraInitStatus=false;
            }
        }
    }
 }

 void CameraNode:: cameraThread()
 {
    unsigned char           * g_pRgbBuffer;     //处理后数据缓存区
    int                     iCameraCounts = 1;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList;
    int                     hCamera;
    tSdkCameraCapbility     tCapability;      //设备描述信息
    tSdkFrameHead           sFrameInfo;
    BYTE*			        pbyBuffer;
    int                     iDisplayFrames = 10000;
    IplImage *iplImage = NULL;
    int                     channel=3;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList,&iCameraCounts);
	printf("state = %d\n", iStatus);

	printf("count = %d\n", iCameraCounts);

    if(iCameraCounts==0){
    }

    iStatus = CameraInit(&tCameraEnumList,-1,-1,&hCamera);

    //初始化失败
	printf("state = %d\n", iStatus);
    if(iStatus!=CAMERA_STATUS_SUCCESS){
    }


    CameraGetCapability(hCamera,&tCapability);

  
    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    CameraPlay(hCamera);


    if(tCapability.sIspCapacity.bMonoSensor){
        channel=1;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        channel=3;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_BGR8);
    }


    //循环显示图像
    while(cameraInitStatus)
    {
        if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
		{
		    CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
		    
		    cv::Mat matImage(
					cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight), 
					sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
					g_pRgbBuffer
					);


           int key= waitKey(5);
           	if (key == 'Q' || key == 'q' || key == 27)
			{
				cameraInitStatus=false;  // 退出循环
			}
            cv::namedWindow("QRCodeDetection",WINDOW_NORMAL);
			imshow("QRCodeDetection", matImage);


            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
			//否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
			CameraReleaseImageBuffer(hCamera,pbyBuffer);

		}
    }

    CameraUnInit(hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);

 }

 void CameraNode::closeCamera()
 {
    cameraInitStatus=false;
 }

void CameraNode:: serialPortSent(short data_a1,short data_a2,short data_a3,int times) 
{
    short data_A1=(static_cast<short>(data_a1));
    short data_A2=(static_cast<short>(data_a2));
    short data_A3=(static_cast<short>(data_a3));
    for(int i=0;i<times;i++)
    {
         writedata(data_A1,data_A2,data_A3);
    }
}
void CameraNode::writedata(short camerapix_u,short camerapix_v, short cameramode)
{
    unsigned char buf[10];//2(帧头、次帧头)+1(数据长度)+6(数据)+1(校验位)
    int i, length = 0;
    for(i = 0; i < 2; i++)//将预定义的帧头和次帧头数据复制到缓冲区的前两个字节。
    {
        buf[i] = header[i];        
    }
    length = 6; //设置数据长度：设置数据长度为6字节，并将其写入缓冲区的第三个字节。
    buf[2] =  length;  
    for(i = 0; i < 2; i++)
    {
        buf[i + 3] = camerapix_u; //buf[3],buf[4]
        buf[i + 5] = camerapix_v; //buf[5],buf[6]
        buf[i + 7] = cameramode; //buf[7],buf[8]
    }
    // 设置校验值
    buf[3 + length] =getCrc8(buf, 3 + length);  //buf[9]
    // 通过串口下发数据
    //boost::asio::write(sp, boost::asio::buffer(buf)); 
    usleep(5);   
}
unsigned char CameraNode::getCrc8(unsigned char *ptr, unsigned short len)
{
    unsigned char crc;
    unsigned char i;
    crc = 0;
    while(len--)
    {
        crc ^= *ptr++;
        for(i = 0; i < 8; i++)
        {
            if(crc&0x01)
                crc=(crc>>1)^0x8C;
            else 
                crc >>= 1;
        }
    }
    return crc;
}
void CameraNode::serialInit()
{
    sp.open("/dev/ttyUSB0", err);
    if(err){
        std::cout << "Error: " << err << std::endl;
        std::cout << "请检查您的串口/dev/ttyUSB0，是否已经准备好：\n 1.读写权限是否打开（默认不打开) \n 2.串口名称是否正确" << std::endl;
        return ;
    }
    sp.set_option(serial_port::baud_rate(115200));
    sp.set_option(serial_port::flow_control(serial_port::flow_control::none));
    sp.set_option(serial_port::parity(serial_port::parity::none));
    sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
    sp.set_option(serial_port::character_size(8));    

    iosev.run();
}