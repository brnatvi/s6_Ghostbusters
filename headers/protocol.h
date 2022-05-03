#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#define USER_ID_LEN      8           // length of gamer's id (preudo)
#define LEN_PORT         4           // length of port in string
#define SERVER_PORT      4242        // length of port in string

#define PREFIX_LEN       5             // length of keywords
#define END_LEN          3              // length of keywords
#define MAX_LEN          220            // example: [MALL? idxxxxxx mess200max***]+buffer
#define IP_BUFFER_MAX    8192

#define TCP_END          "***"
#define UDP_END          "+++"
#define SMG_WHITE        " "


enum msgId
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
    eTcpMsgNMEMB,
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
    enum msgId  msg;
    const char  *prefix;
    const char  *format;
    size_t       szMsg;    
};

struct stRawMessage
{
    enum    msgId msg;
    size_t  szMsg;
    uint8_t msgRaw[MAX_LEN];
};


struct stIpBuffer
{
    size_t   szWritten;
    uint8_t  buffer[8192];
};


const struct stMsgDesc *getMsgDescriptionByPrefix(const char* msgPrefix);
const struct stMsgDesc *getMsgDescriptionById(enum msgId msg);

int                     tcpGetMsg(int socket, struct stIpBuffer *buf, struct stRawMessage *msg);

bool                    sendMsg(int socket, enum msgId msg, ...);
size_t                  scanMsg(uint8_t *pMsg, enum msgId msg, ...);

