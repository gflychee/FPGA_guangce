#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <winterfell/instab.h>
#include <winterfell/mdclient.h>
#include <cfloat>
#include <string>
#include "tacfeedmd.h"

#define s_per_day 86400
#define ns_per_s 1000000000
#define ns_per_ms 1000000
#define s_eight_hour 28800
#define s_eighteen_hour 64800

struct mdclient *pclient;


long get_exchtime(const char* UpdateTime, int UpdateMillisec) {
    int hours = 0;
    int minutes = 0; 
    int seconds = 0;
    sscanf(UpdateTime, "%d:%d:%d", &hours, &minutes, &seconds);

    int rt = hours * 3600 + minutes * 60 + seconds;

    if (rt > s_eighteen_hour)
        rt -= s_per_day;
    rt *= ns_per_s;
    rt += UpdateMillisec * ns_per_ms;
    return rt;
}

struct fpga_shfe_mc_client {
    struct mdclient mdclient;
    char localIP[64];
    char serverIP[64];
    char username[64];
    char password[64];
    int serverPort;
    int debug;
};

class handleClass
{
public:
    void handleL2(Level2QuoteDataT *pData)
    {
        long rece_time = currtime();
        long exchtime = get_exchtime(pData->UpdateTime, pData->UpdateMillisec);
        if (((struct fpga_shfe_mc_client *)pclient->container)->debug) {
            printf("%s.%d,%ld,%s,%f,%d,%f,%f,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d\n",
                            pData->UpdateTime, pData->UpdateMillisec, exchtime, \
                            pData->InstrumentID, pData->LastPrice, pData->Volume, pData->Turnover, pData->OpenInterest, \
                            pData->BidPrice1, pData->BidVolume1, pData->AskPrice1, pData->AskVolume1, \
                            pData->BidPrice2, pData->BidVolume2, pData->AskPrice2, pData->AskVolume2, \
                            pData->BidPrice3, pData->BidVolume3, pData->AskPrice3, pData->AskVolume3, \
                            pData->BidPrice4, pData->BidVolume4, pData->AskPrice4, pData->AskVolume4, \
                            pData->BidPrice5, pData->BidVolume5, pData->AskPrice5, pData->AskVolume5);
        }
        const int insidx = ins2idx(pclient->instab, (const char *)pData->InstrumentID);
        if (insidx == -1)
            return;

        uint32_t mdslot;
        struct md_static *ms = (struct md_static *)get_md_static(pclient->instab, insidx);
        struct md_snapshot *md = snapshottab_get_next_slot(pclient->sstab, insidx, &mdslot);
        md->type = MDT_Level5;
        md->exchange_time = exchtime;
        md->recv_time = rece_time;
        md->last_price = pData->LastPrice != DBL_MAX ? pData->LastPrice : 0.0;
        md->volume = pData->Volume;
        md->turnover = pData->Turnover;
        md->open_interest = (int)pData->OpenInterest;
        md->bid_price[0] = pData->BidPrice1 != DBL_MAX ? pData->BidPrice1 : 0.0;
        md->bid_size[0] = pData->BidVolume1;
        md->ask_price[0] = pData->AskPrice1 != DBL_MAX ? pData->AskPrice1 : 0.0;
        md->ask_size[0] = pData->AskVolume1;
        md->bid_price[1] = pData->BidPrice2 != DBL_MAX ? pData->BidPrice2 : 0.0;
        md->bid_size[1] = pData->BidVolume2;
        md->ask_price[1] = pData->AskPrice2 != DBL_MAX ? pData->AskPrice2 : 0.0;
        md->ask_size[1] = pData->AskVolume2;
        md->bid_price[2] = pData->BidPrice3 != DBL_MAX ? pData->BidPrice3 : 0.0;
        md->bid_size[2] = pData->BidVolume3;
        md->ask_price[2] = pData->AskPrice3 != DBL_MAX ? pData->AskPrice3 : 0.0;
        md->ask_size[2] = pData->AskVolume3;
        md->bid_price[3] = pData->BidPrice4 != DBL_MAX ? pData->BidPrice4 : 0.0;
        md->bid_size[3] = pData->BidVolume4;
        md->ask_price[3] = pData->AskPrice4 != DBL_MAX ? pData->AskPrice4 : 0.0;
        md->ask_size[3] = pData->AskVolume4;
        md->bid_price[4] = pData->BidPrice5 != DBL_MAX ? pData->BidPrice5 : 0.0;
        md->bid_size[4] = pData->BidVolume5;
        md->ask_price[4] = pData->AskPrice5 != DBL_MAX ? pData->AskPrice5 : 0.0;
        md->ask_size[4] = pData->AskVolume5;

        md->decode_time = currtime();
        pclient->output(pclient, ms, mdslot);
    }
};


static void run(struct mdclient *client) {
    printf("api version: %s", TacFeedGetApiVersion());
    struct fpga_shfe_mc_client *fpga_mc = (struct fpga_shfe_mc_client *)client->container;
    pclient = client;

    // TacFeedSetCpuAffinity(TACFEED_BIND_CORE_ID);
    
    handleClass *H = new handleClass();

    TacFeedInitParam param = {};
    
    param.LocalIP = fpga_mc->localIP;
    param.LoginServerIP = fpga_mc->serverIP;
    param.LoginServerPort = fpga_mc->serverPort;
    param.UserName = fpga_mc->username;
    param.Password = fpga_mc->password;
    param.L2Callback = std::bind(&handleClass::handleL2, H, std::placeholders::_1);
    param.ApiSelectMode = ApiSelectAuto;

    TacFeedSubscribe(&param);

    while (1)
    {
        usleep(500000);
    }
}


static struct mdclient *fpga_shfe_mc_create(cfg_t *cfg, struct memdb *memdb) {
    struct fpga_shfe_mc_client *fpga_mc = new struct fpga_shfe_mc_client;
    struct mdclient *client = &fpga_mc->mdclient;

    mdclient_init(client, cfg, memdb);

    const char *localIP;
    cfg_get_string(cfg, "localIP", &localIP);
    snprintf(fpga_mc->localIP, sizeof(fpga_mc->localIP), "%s", localIP);
    const char *serverIP;
    cfg_get_string(cfg, "serverIP", &serverIP);
    snprintf(fpga_mc->serverIP, sizeof(fpga_mc->serverIP), "%s", serverIP);
    const char *username;
    cfg_get_string(cfg, "username", &username);
    snprintf(fpga_mc->username, sizeof(fpga_mc->username), "%s", username);
    const char *password;
    cfg_get_string(cfg, "password", &password);
    snprintf(fpga_mc->password, sizeof(fpga_mc->password), "%s", password);

    cfg_get_int(cfg, "serverPort", &fpga_mc->serverPort);
    cfg_get_int(cfg, "debug", &fpga_mc->debug);

    printf("fpga_mc->localIP: %s\n", fpga_mc->localIP);
    printf("fpga_mc->serverIP: %s\n", fpga_mc->serverIP);
    printf("fpga_mc->username: %s\n", fpga_mc->username);
    printf("fpga_mc->password: %s\n", fpga_mc->password);
    printf("fpga_mc->serverPort: %d\n", fpga_mc->serverPort);
    printf("fpga_mc->debug: %d\n", fpga_mc->debug);
    fflush(stdout);
    client->run = run;
    client->decoder = NULL;
    client->flags = 0;
    client->container = fpga_mc;
    return client;
}

static struct mdsrc_module mdsrc_fpga_shfe_mc = {
    .create = fpga_shfe_mc_create,
    .api = "fpga-shfe-mc"
};

mdsrc_module_register(&mdsrc_fpga_shfe_mc);
