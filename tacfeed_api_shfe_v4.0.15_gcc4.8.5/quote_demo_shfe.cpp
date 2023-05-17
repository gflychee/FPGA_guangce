#include <stdio.h>
#include <unistd.h>
#include "tacfeedmd.h"

class handleClass
{
public:
    void handleL1(Level1QuoteDataT *pData)
    {
        printf("2,%15d,%s,%s,%s.%03d,%.3lf,,,,,%d,%.2lf,%.0lf,,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%d,%d,%d,%d,%d,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%d,%d,%d,%d,%d\n", 
                0, pData->InstrumentID, pData->ActionDay, pData->UpdateTime, pData->UpdateMillisec,
                pData->LastPrice, pData->Volume, pData->Turnover, pData->OpenInterest,
                pData->AskPrice1, -1.0, -1.0, -1.0, -1.0,
                pData->AskVolume1, 0, 0, 0, 0,
                pData->BidPrice1, -1.0, -1.0, -1.0, -1.0,
                pData->BidVolume1, 0, 0, 0, 0);
    }

    void handleL2(Level2QuoteDataT *pData)
    {
        printf("2,%15d,%s,%s,%s.%03d,%.3lf,,,,,%d,%.2lf,%.0lf,,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%d,%d,%d,%d,%d,%.3lf,%.3lf,%.3lf,%.3lf,%.3lf,%d,%d,%d,%d,%d\n", 
                0, pData->InstrumentID, pData->ActionDay, pData->UpdateTime, pData->UpdateMillisec,
                pData->LastPrice, pData->Volume, pData->Turnover, pData->OpenInterest,
                pData->AskPrice1, pData->AskPrice2, pData->AskPrice3, pData->AskPrice4, pData->AskPrice5,
                pData->AskVolume1, pData->AskVolume2, pData->AskVolume3, pData->AskVolume4, pData->AskVolume5,
                pData->BidPrice1, pData->BidPrice2, pData->BidPrice3, pData->BidPrice4, pData->BidPrice5,
                pData->BidVolume1, pData->BidVolume2, pData->BidVolume3, pData->BidVolume4, pData->BidVolume5);
    }
};

#define TACFEED_BIND_CORE_ID (3)

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        fprintf(stderr, "Usage: ./quote_demo_shfe LocalIP LoginServerIP LoginServerPort Username Password\n");
        exit(EXIT_FAILURE);
    }

    char *localIP = argv[1];
    char *serverIP = argv[2];
    char *serverPort = argv[3];
    char *username = argv[4];
    char *password = argv[5];
/*
    fprintf(stderr, "%s\n", TacFeedGetApiVersion());

    TacFeedSetCpuAffinity(TACFEED_BIND_CORE_ID);
*/
    handleClass *H = new handleClass();
    TacFeedInitParam param = {};
    
    param.LocalIP = localIP;
    param.LoginServerIP = serverIP;
    param.LoginServerPort = atoi(serverPort);
    param.UserName = username;
    param.Password = password;
    param.L1Callback = std::bind(&handleClass::handleL1, H, std::placeholders::_1);
    param.L2Callback = std::bind(&handleClass::handleL2, H, std::placeholders::_1);
    param.ApiSelectMode = ApiSelectAuto;

    printf("2,EpochTime,InstrumentID,Date,ExchangeTime,LastPrice,,,,,Volume,Turnover,OpenInterest,,AskPrice1,AskPrice2,AskPrice3,AskPrice4,AskPrice5,AskVolume1,AskVolume2,AskVolume3,AskVolume4,AskVolume5,BidPrice1,BidPrice2,BidPrice3,BidPrice4,BidPrice5,BidVolume1,BidVolume2,BidVolume3,BidVolume4,BidVolume5\n");

    TacFeedSubscribe(&param);

    while (1)
    {
        usleep(500000);
    }

    return 0;
}

