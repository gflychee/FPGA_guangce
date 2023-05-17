/////////////////////////////////////////////////////////////////////////
/////@system      TacFeed行情服务
/////@company     上海广策信息技术有限公司
/////@file        tacfeedmd.h
/////@brief       定义了客户端行情接口
/////@version     4.0
///////////////////////////////////////////////////////////////////////////

#ifndef __TACFEED_MD_H__
#define __TACFEED_MD_H__

#include <functional>

#ifdef  __cplusplus
extern "C"  {
#endif

enum TacFeedApiSelectMode
{
    ApiSelectAuto       = 0, /// Tacfeed api will automaticlly choose the fastest api to receive data
    ApiSelectLinux      = 1, /// Tacfeed api will use posix linux socket to receive data
    ApiSelectTcpDirect  = 2, /// Tacfeed api will use Tcpdirect on Solarflare to receive data
    ApiSelectTacnic     = 3, /// Tacfeed api will use Tacnic interface to receive data
};

/// data structure for level 1 quote data
typedef struct Level1QuoteDataT
{
    char    InstrumentID[31];
    char    ActionDay[9];
    char    UpdateTime[12];
    int     UpdateMillisec;
    int     Volume;
    double  LastPrice;
    double  Turnover;
    double  OpenInterest;
    double  BidPrice1;
    int     BidVolume1;
    double  AskPrice1;
    int     AskVolume1;
} Level1QuoteDataT;

/// data structure for level 2 quote data
typedef struct Level2QuoteDataT
{
    char    InstrumentID[31];
    char    ActionDay[9];
    char    UpdateTime[12];
    int     UpdateMillisec;
    int     Volume;
    double  LastPrice;
    double  Turnover;
    double  OpenInterest;
    double  BidPrice1;
    int     BidVolume1;
    double  AskPrice1;
    int     AskVolume1;
    double  BidPrice2;
    int     BidVolume2;
    double  AskPrice2;
    int     AskVolume2;
    double  BidPrice3;
    int     BidVolume3;
    double  AskPrice3;
    int     AskVolume3;
    double  BidPrice4;
    int     BidVolume4;
    double  AskPrice4;
    int     AskVolume4;
    double  BidPrice5;
    int     BidVolume5;
    double  AskPrice5;
    int     AskVolume5;
} Level2QuoteDataT;


/// data structure for mbl quote data
typedef struct MBLQuoteDataT
{
    char    InstrumentID[8];
    double  AskPrice1;
    int     AskVolume1;
    double  AskPrice2;
    int     AskVolume2;
    double  AskPrice3;
    int     AskVolume3;
    double  AskPrice4;
    int     AskVolume4;
    double  AskPrice5;
    int     AskVolume5;
    double  AskPrice6;
    int     AskVolume6;
    double  AskPrice7;
    int     AskVolume7;
    double  AskPrice8;
    int     AskVolume8;
    double  AskPrice9;
    int     AskVolume9;
    double  AskPrice10;
    int     AskVolume10;
    double  BidPrice1;
    int     BidVolume1;
    double  BidPrice2;
    int     BidVolume2;
    double  BidPrice3;
    int     BidVolume3;
    double  BidPrice4;
    int     BidVolume4;
    double  BidPrice5;
    int     BidVolume5;
    double  BidPrice6;
    int     BidVolume6;
    double  BidPrice7;
    int     BidVolume7;
    double  BidPrice8;
    int     BidVolume8;
    double  BidPrice9;
    int     BidVolume9;
    double  BidPrice10;
    int     BidVolume10;
} MBLQuoteDataT;

typedef struct TacFeedFlag
{
    unsigned int UseHwFilter : 1;    /// tacnic only, use hw filter to filter raw packet
    unsigned int UsePcieBus  : 1;    /// tacnic only, receive from tacnic pcie bus
    unsigned int UseSbe      : 1;
    unsigned int Reserve     : 29;
} TacFeedFlag;

/// user callback proto type
typedef std::function<void (Level1QuoteDataT *pData)>   Level1Callback;
typedef std::function<void (Level2QuoteDataT *pData)>   Level2Callback;
typedef std::function<void (MBLQuoteDataT *pData)>      MBLCallback;

typedef struct TacFeedInitParam
{
    const char*             LocalIP;            /// (must) local ip on which the data is received
    const char*             LoginServerIP;      /// (must) login server ip
    uint16_t                LoginServerPort;    /// (must) login server port
    const char*             UserName;           /// (must) username for login
    const char*             Password;           /// (must) password for login
    Level1Callback          L1Callback;         /// (opt)  call back to process L1 MD
    Level2Callback          L2Callback;         /// (opt)  call back to process L2 MD
    MBLCallback             MblCallback;        /// (opt)  call back to process MBL MD
    TacFeedApiSelectMode    ApiSelectMode;      /// (opt)  api select mode, default is Auto
    TacFeedFlag             Flag;               /// (opt)  tacnic only
} TacFeedInitParam;

/// Get TacFeed API version.
///     return: version string 
const char* TacFeedGetApiVersion();

/// Set TacFeed worker thread cpu affinity. Must be called before TacFeedSubscribe.
///     cpu: the cpu id on which worker thread will run
///     return: error code, 0 on success
int TacFeedSetCpuAffinity(int cpu);

/// Subscribe TacFeed market data service.
///     init_param: the parameters needed for initialization, see @TacFeedInitParam
///     return: error code, 0 on success
int TacFeedSubscribe(const TacFeedInitParam* init_param);

/// Release TacFeed
///     Stop the worker thread and release resource
///     return: error code, 0 on success
int TacFeedRelease();

#ifdef __cplusplus
}
#endif
#endif
