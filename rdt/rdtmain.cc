#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GoBackNRdtReceiver.h"
#include "GoBackNRdtSender.h"
#include "SelectiveRepeatRdtReceiver.h"
#include "SelectiveRepeatRdtSender.h"
int main(int argc, char* argv[])
{
    if(argc != 4){
        cout << "Argument error!";
    }
	FILE* outfile = freopen("out.txt", "w", stdout);
    RdtSender* ps = nullptr;
    RdtReceiver* pr = nullptr;

//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();

	pns->setInputFile(argv[1]);
	pns->setOutputFile(argv[2]);

	string mode(argv[3]);
	if(mode == "GBN"){
	    ps = new GoBackNRdtSender(8);
	    pr = new GoBackNRdtReceiver();
	} else if(mode == "SR"){
        ps = new SelectiveRepeatRdtSender(8);
        pr = new SelectiveRepeatRdtReceiver(8);
	} else if(mode == "TCP"){

	}
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->start();

	fclose(outfile);
	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

