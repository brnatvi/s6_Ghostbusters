#pragma once

#define USER_ID_LEN      8           // length of gamer's id (preudo)
#define LEN_PORT         4           // length of port in string
//#define SERVER_PORT      4242        // length of port in string

#define MSG_START_LEN    5           // length of keywords
#define MSG_END_LEN      3           // length of keywords
#define MSG_MAX_LEN      220         // example: [MALL? idxxxxxx mess200max***]+buffer
#define MSG_MAX_TXT_LEN  200         //
#define IP_BUFFER_MAX    8192

#define MSG_TCP_END      "***"
#define MSG_UDP_END      "+++"
#define SMG_WHITE        " "


enum eMsgId
{
    eTcpMsgGAMES,
    eTcpMsgOGAME,
    eTcpMsgNEWPL,
    eTcpMsgREGIS,
    eTcpMsgREGOK,
    eTcpMsgREGNO,
    eTcpMsgSTART,
    eTcpMsgUNREG,
    eTcpMsgUNROK,
    eTcpMsgDUNNO,
    eTcpMsgSIZEQ,
    eTcpMsgSIZEA,
    eTcpMsgLISTQ,
    eTcpMsgLISTA,
    eTcpMsgPLAYR,
    eTcpMsgGAMEQ,
    eTcpMsgWELCO,
    eTcpMsgPOSIT,
    eTcpMsgUPMOV,
    eTcpMsgDOMOV,
    eTcpMsgLEMOV,
    eTcpMsgRIMOV,
    eTcpMsgMOVEA,
    eTcpMsgMOVEF,
    eTcpMsgIQUIT,
    eTcpMsgGOBYE,
    eTcpMsgGLISQ,
    eTcpMsgGLISA,
    eTcpMsgGPLYR,
    eTcpMsgMALLQ,
    eTcpMsgMALLA,
    eTcpMsgSENDQ,
    eTcpMsgSENDA,
    eTcpMsgNSEND,

    eTcpMsgMaxValue,

    eUdpMsgGHOST,
    eUdpMsgSCORE,
    eUdpMsgMESSA,
    eUdpMsgENDGA,
    eUdpMsgMESSP,

    eUdpMsgMaxValue
};

struct stMsgDesc
{
    enum eMsgId  eMsg;
    const char  *pStart;
    const char  *pFormat;
    size_t       szMsg;    
};

struct stRawMessage
{
    enum    eMsgId eMsg;
    size_t  szMsg;
    uint8_t pMsgRaw[MSG_MAX_LEN];
};


struct stIpBuffer
{
    size_t   szWritten;
    uint8_t  pBuffer[8192];
};


const struct stMsgDesc *getMsgDescriptionByPrefix(const char* pMsgStart);
const struct stMsgDesc *getMsgDescriptionById(enum eMsgId eMsg);

bool                    commitMsg(struct stIpBuffer *pBuf, size_t szMsg);
bool                    socketReadMsg(int iSocket, struct stIpBuffer *pBuf, struct stRawMessage *pMsg);
bool                    socketReadMsgEx(int iSocket, struct stIpBuffer *pBuf, struct stRawMessage *pMsg, uint32_t timeoutMs);

bool                    sendMsg(int iSocket, enum eMsgId eMsg, ...);
size_t                  scanMsg(uint8_t *pMsg, enum eMsgId eMsg, ...);
void                    printMsg(uint8_t *pMsg, const char *pPrefix);

