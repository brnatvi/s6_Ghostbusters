#include "common.h"

static const struct stMsgDesc g_pMsg[] = 
{
    {.eMsg = eTcpMsgGAMES,  .pStart = "GAMES", .pFormat = "GAMES %n***",                .szMsg = sizeof("GAMES n***")   - 1                           }, //+
    {.eMsg = eTcpMsgOGAME,  .pStart = "OGAME", .pFormat = "OGAME %m %s***",             .szMsg = sizeof("OGAME m s***") - 1                           }, //+
    {.eMsg = eTcpMsgNEWPL,  .pStart = "NEWPL", .pFormat = "NEWPL %G %P***",             .szMsg = sizeof("NEWPL idxxxxxx port***") - 1                 },
    {.eMsg = eTcpMsgREGIS,  .pStart = "REGIS", .pFormat = "REGIS %G %P %m***",          .szMsg = sizeof("REGIS idxxxxxx port m***") - 1               },
    {.eMsg = eTcpMsgREGOK,  .pStart = "REGOK", .pFormat = "REGOK %m***",                .szMsg = sizeof("REGOK m***") - 1                             },
    {.eMsg = eTcpMsgREGNO,  .pStart = "REGNO", .pFormat = "REGNO***",                   .szMsg = sizeof("REGNO***") - 1                               },
    {.eMsg = eTcpMsgSTART,  .pStart = "START", .pFormat = "START***",                   .szMsg = sizeof("START***") - 1                               },
    {.eMsg = eTcpMsgUNREG,  .pStart = "UNREG", .pFormat = "UNREG***",                   .szMsg = sizeof("UNREG***") - 1                               },
    {.eMsg = eTcpMsgUNROK,  .pStart = "UNROK", .pFormat = "UNROK %m***",                .szMsg = sizeof("UNROK m***") - 1                             },
    {.eMsg = eTcpMsgDUNNO,  .pStart = "DUNNO", .pFormat = "DUNNO***",                   .szMsg = sizeof("DUNNO***") - 1                               },
    {.eMsg = eTcpMsgSIZEQ,  .pStart = "SIZE?", .pFormat = "SIZE? %m***",                .szMsg = sizeof("SIZE? m***") - 1                             }, //+
    {.eMsg = eTcpMsgSIZEA,  .pStart = "SIZE!", .pFormat = "SIZE! %m %h %w***",          .szMsg = sizeof("SIZE! m hh ww***") - 1                       }, //+
    {.eMsg = eTcpMsgLISTQ,  .pStart = "LIST?", .pFormat = "LIST? %m***",                .szMsg = sizeof("LIST? m***") - 1                             },
    {.eMsg = eTcpMsgLISTA,  .pStart = "LIST!", .pFormat = "LIST! %m %s***",             .szMsg = sizeof("LIST! m s***") - 1                           },
    {.eMsg = eTcpMsgPLAYR,  .pStart = "PLAYR", .pFormat = "PLAYR %G***",                .szMsg = sizeof("PLAYR idxxxxxx***") - 1                      },
    {.eMsg = eTcpMsgGAMEQ,  .pStart = "GAME?", .pFormat = "GAME?***",                   .szMsg = sizeof("GAME?***") - 1                               }, //++
    {.eMsg = eTcpMsgWELCO,  .pStart = "WELCO", .pFormat = "WELCO %m %h %w %f %i %P***", .szMsg = sizeof("WELCO m hh ww f IPxxxxxxxxxxxxx port***") - 1},
    {.eMsg = eTcpMsgPOSIT,  .pStart = "POSIT", .pFormat = "POSIT %G %x %y***",          .szMsg = sizeof("POSIT idxxxxxx XXX YYY***") - 1              },
    {.eMsg = eTcpMsgUPMOV,  .pStart = "UPMOV", .pFormat = "UPMOV %d***",                .szMsg = sizeof("UPMOV ddd***") - 1                           },
    {.eMsg = eTcpMsgDOMOV,  .pStart = "DOMOV", .pFormat = "DOMOV %d***",                .szMsg = sizeof("DOMOV ddd***") - 1                           },
    {.eMsg = eTcpMsgLEMOV,  .pStart = "LEMOV", .pFormat = "LEMOV %d***",                .szMsg = sizeof("LEMOV ddd***") - 1                           },
    {.eMsg = eTcpMsgRIMOV,  .pStart = "RIMOV", .pFormat = "RIMOV %d***",                .szMsg = sizeof("RIMOV ddd***") - 1                           },
    {.eMsg = eTcpMsgMOVEA,  .pStart = "MOVE!", .pFormat = "MOVE! %x %y***",             .szMsg = sizeof("MOVE! XXX YYY***") - 1                       },
    {.eMsg = eTcpMsgMOVEF,  .pStart = "MOVEF", .pFormat = "MOVEF %x %y %p***",          .szMsg = sizeof("MOVEF XXX YYY pppp***") - 1                  },
    {.eMsg = eTcpMsgIQUIT,  .pStart = "IQUIT", .pFormat = "IQUIT***",                   .szMsg = sizeof("IQUIT***") - 1                               },
    {.eMsg = eTcpMsgGOBYE,  .pStart = "GOBYE", .pFormat = "GOBYE***",                   .szMsg = sizeof("GOBYE***") - 1                               },
    {.eMsg = eTcpMsgGLISQ,  .pStart = "GLIS?", .pFormat = "GLIS?***",                   .szMsg = sizeof("GLIS?***") - 1                               },
    {.eMsg = eTcpMsgGLISA,  .pStart = "GLIS!", .pFormat = "GLIS! %s***",                .szMsg = sizeof("GLIS! s***") - 1                             },
    {.eMsg = eTcpMsgGPLYR,  .pStart = "GPLYR", .pFormat = "GPLYR %G %x %y %p***",       .szMsg = sizeof("GPLYR idxxxxxx XXX YYY pppp***") - 1         },
    {.eMsg = eTcpMsgMALLQ,  .pStart = "MALL?", .pFormat = "MALL? %M***",                .szMsg = 0 /*sizeof("MALL? mess***")*/               },
    {.eMsg = eTcpMsgMALLA,  .pStart = "MALL!", .pFormat = "MALL!***",                   .szMsg = sizeof("MALL!***") - 1                               },
    {.eMsg = eTcpMsgSENDQ,  .pStart = "SEND?", .pFormat = "SEND? %G %M***",             .szMsg = 0 /*sizeof("SEND? idxxxxxx mess***")*/               },
    {.eMsg = eTcpMsgSENDA,  .pStart = "SEND!", .pFormat = "SEND!***",                   .szMsg = sizeof("SEND!***") - 1                               },
    {.eMsg = eTcpMsgNSEND,  .pStart = "NSEND", .pFormat = "NSEND***",                   .szMsg = sizeof("NSEND***") - 1                               },

    {.eMsg = eUdpMsgGHOST,  .pStart = "GHOST", .pFormat = "GHOST %x %y+++",             .szMsg = sizeof("GHOST xxx yyy+++") - 1                       }, 
    {.eMsg = eUdpMsgSCORE,  .pStart = "SCORE", .pFormat = "SCORE %G %p %x %y+++",       .szMsg = sizeof("SCORE idxxxxxx pppp XXX YYY+++") - 1         }, 
    {.eMsg = eUdpMsgMESSA,  .pStart = "MESSA", .pFormat = "MESSA %G %M+++",             .szMsg = 0 /*sizeof("MESSA idxxxxxx mess+++")*/               }, 
    {.eMsg = eUdpMsgENDGA,  .pStart = "ENDGA", .pFormat = "ENDGA %G %p+++",             .szMsg = sizeof("ENDGA idxxxxxx pppp+++") - 1                 }, 
    {.eMsg = eUdpMsgMESSP,  .pStart = "MESSP", .pFormat = "MESSP %G %M+++",             .szMsg = 0 /*sizeof("MESSP idxxxxxx mess+++")*/               }
};


const struct stMsgDesc *getMsgDescriptionByPrefix(const char* pMsgStart)
{
    if (!pMsgStart)    
    {
        return NULL;
    }

    size_t szCount = sizeof(g_pMsg) / sizeof(g_pMsg[0]);
    for (size_t szI = 0; szI < szCount; szI++)
    {
        if (0 == strncmp(pMsgStart, g_pMsg[szI].pStart, MSG_START_LEN))
        {
            return &g_pMsg[szI];
        }
    }

    return NULL;
}

const struct stMsgDesc *getMsgDescriptionById(enum eMsgId eMsg)
{
    size_t szCount = sizeof(g_pMsg) / sizeof(g_pMsg[0]);
    for (size_t szI = 0; szI < szCount; szI++)
    {
        if (g_pMsg[szI].eMsg == eMsg)
        {
            return &g_pMsg[szI];
        }
    }

    return NULL;
}

bool processIncomingData(int iSocket, struct stIpBuffer *pBuf, uint32_t timeoutMs)
{
    size_t szOffW = pBuf->szWritten;    
    if ((IP_BUFFER_MAX - pBuf->szWritten) > MSG_MAX_LEN)
    {
        if (socket_isReady(iSocket, eTcpDirectionRead, timeoutMs))
        {
            ssize_t szRec = recv(iSocket, 
                                pBuf->pBuffer + pBuf->szWritten, 
                                IP_BUFFER_MAX - pBuf->szWritten, 
                                0);
            if (szRec > 0)
            {
                pBuf->szWritten += szRec;
            }
        }
    }

    return szOffW != pBuf->szWritten;
};


bool commitMsg(struct stIpBuffer *pBuf, size_t szMsg)
{
    if ((!pBuf) || (szMsg > pBuf->szWritten)) return false;

    if (szMsg == pBuf->szWritten)
    {
        pBuf->szWritten  = 0;
        return true;
    }

    memmove(pBuf->pBuffer,
            pBuf->pBuffer + szMsg,
            pBuf->szWritten - szMsg        
           );

    pBuf->szWritten -= szMsg;
    return true;
}

void print_msg_hex(uint8_t *msg, size_t size)
{
    printf("Raw message: [");
    while (size--)
    {         
        //https://www.rapidtables.com/code/text/ascii-table.html
        if ((*msg >= 32) && (*msg < 127)) //visible characters 
        {
            printf("%c:%03u-", *msg, (uint32_t)*msg);
        }
        else //control characters
        {
            printf("\xe2\x96\x91:%03u-", (uint32_t)*msg);
        }
        msg++;        
    }   
    printf("]\n");
}


bool socketReadMsg(int iSocket, struct stIpBuffer *pBuf, struct stRawMessage *pMsg)
{
    return socketReadMsgEx(iSocket, pBuf, pMsg, 0xFFFFFFFF);
}

bool socketReadMsgEx(int iSocket, struct stIpBuffer *pBuf, struct stRawMessage *pMsg, uint32_t timeoutMs)
{
    if ((!pBuf) || (!pMsg)) return false;

    bool bRet = false;

    do
    {
        if (pBuf->szWritten > MSG_START_LEN)
        {
            const struct stMsgDesc *pDesc = getMsgDescriptionByPrefix((const char*)pBuf->pBuffer);

            if (!pDesc)
            {
                log_error("Protocol error! message not recognized!");
                print_msg_hex(pBuf->pBuffer, MSG_START_LEN);
                return false;
            }

            if ((pBuf->szWritten >= pDesc->szMsg) && (pDesc->szMsg))
            {
                pMsg->eMsg  = pDesc->eMsg;
                pMsg->szMsg = pDesc->szMsg;
                memcpy(pMsg->pMsgRaw, pBuf->pBuffer, pMsg->szMsg);
                commitMsg(pBuf, pMsg->szMsg);
                bRet = true;
            }
            else if (0 == pDesc->szMsg)
            {
                const char  *pTail  = pDesc->eMsg > eTcpMsgMaxValue ? "+++" : "***";
                const size_t szTail = 3;
                uint8_t *pIt = pBuf->pBuffer + MSG_START_LEN;
                while ((pIt + szTail) <= (pBuf->pBuffer + pBuf->szWritten))
                {
                    if (0 == strncmp(pTail, (const char*)pIt, szTail))
                    {
                        pIt += szTail; //***
                        pMsg->eMsg  = pDesc->eMsg;
                        pMsg->szMsg = pIt - pBuf->pBuffer;
                        memcpy(pMsg->pMsgRaw, pBuf->pBuffer, pMsg->szMsg);
                        commitMsg(pBuf, pMsg->szMsg);

                        bRet = true;
                        break;
                    }
                    pIt++;    
                }
            }
        }

        if (!bRet)
        {
            if (!processIncomingData(iSocket, pBuf, timeoutMs))
            {
                break;
            }
        }

    } while (false == bRet);

    if (bRet) printMsg(pMsg->pMsgRaw, "<<");

    return bRet;
}

bool sendMsg(int iSocket, enum eMsgId eMsg, ...)
{
    uint8_t  pMsg[MSG_MAX_LEN];    
    uint8_t *pBuf = &pMsg[0];
    
    const struct stMsgDesc *pDesc = getMsgDescriptionById(eMsg);
    if (!pDesc) {return false;}

    va_list  pVargs;
    va_start(pVargs, eMsg);

    const char *pFormatIt = pDesc->pFormat;
    uint8_t    *pBufStart = pBuf;

    while (*pFormatIt)
    {
        if ('%' != *pFormatIt)
        {
            *pBuf = *pFormatIt; pBuf++; 
        }
        else 
        {
            pFormatIt++;
            if (!*pFormatIt) return -1;
            if (('n' == *pFormatIt) || ('m' == *pFormatIt) || ('s' == *pFormatIt) || ('f' == *pFormatIt)) //1 byte
            {
                *pBuf = (uint8_t)va_arg(pVargs, uint32_t); pBuf += sizeof(uint8_t);
            }
            else if ('G' == *pFormatIt) //8 shars
            {
                memcpy(pBuf, va_arg(pVargs, char*), USER_ID_LEN); pBuf += USER_ID_LEN;
            }
            else if (('P' == *pFormatIt) ||  ('p' == *pFormatIt)) //4 chars
            {
                int ret = sprintf((char*)pBuf, "%04u", (uint32_t)va_arg(pVargs, uint32_t));
                if (ret) pBuf += 4; else return -1;
            }
            else if (('h' == *pFormatIt) || ('w' == *pFormatIt)) //2 bytes
            {
                *(uint16_t*)pBuf = (uint16_t)va_arg(pVargs, uint32_t); pBuf += sizeof(uint16_t);
            }
            else if ('i' == *pFormatIt) //15 chars
            {
                const size_t szIp = 15;
                char         pIp[szIp*2];
                uint32_t     uIp = va_arg(pVargs, uint32_t);

                sprintf((char*)pIp, "%u.%u.%u.%u",
                        (uint32_t)((uIp >> 24) & 0xFF),
                        (uint32_t)((uIp >> 16) & 0xFF),
                        (uint32_t)((uIp >> 8) & 0xFF),
                        (uint32_t)(uIp & 0xFF)
                       );
                for (size_t szI = strlen(pIp); szI < szIp; szI++)
                {
                    pIp[szI] = '#';
                }

                memcpy(pBuf, pIp, szIp); pBuf += szIp;
            }
            else if ( ('x' == *pFormatIt) || ('y' == *pFormatIt) || ('d' == *pFormatIt)) //3 chars
            {
                int ret = sprintf((char*)pBuf, "%03u", va_arg(pVargs, uint32_t));
                if (ret) pBuf += 3; else return -1;
            }
            else if ('M' == *pFormatIt) //string 200 chars max
            {
                const char *userMsg = (const char*)va_arg(pVargs, char*);
                strncpy((char*)pBuf, userMsg, MSG_MAX_TXT_LEN);
                size_t szMsg = strlen(userMsg);
                if (szMsg <= MSG_MAX_TXT_LEN) { pBuf += szMsg; } else { pBuf += MSG_MAX_TXT_LEN; }
            }
        }

        pFormatIt++;
    }

    *pBuf = 0;

    va_end(pVargs);

    if ((pDesc->szMsg) && ((pBuf - pBufStart) != pDesc->szMsg))
    {
        log_error("{formatMsg} buffer formatting error!, expected size %zu, calculated size %zu", 
                  pDesc->szMsg, (pBuf - pBufStart));
        printMsg(pMsg, ">!");

        return 0;
    }

    printMsg(pMsg, ">>");
    return (pBuf - pBufStart) == send(iSocket, pMsg, pBuf - pBufStart, 0);
}

size_t scanMsg(uint8_t *pMsg, enum eMsgId eMsg, ...)
{
    const struct stMsgDesc *pDesc = getMsgDescriptionById(eMsg);
    if (!pDesc) {return 0;}

    size_t   szRet = 0;
    va_list  pVargs;
    va_start(pVargs, eMsg);

    const char *pFormatIt = pDesc->pFormat;
    uint8_t    *pBuf = pMsg;

    while (*pFormatIt)
    {
        if ('%' != *pFormatIt)
        {
            pBuf++; 
        }
        else 
        {
            pFormatIt++;
            if (!*pFormatIt) return -1;
            if (('n' == *pFormatIt) || ('m' == *pFormatIt) || ('s' == *pFormatIt) || ('f' == *pFormatIt)) //1 byte
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                *pVar = *(uint8_t*)pBuf; 
                pBuf += sizeof(uint8_t);
                szRet++;
            }
            else if ('G' == *pFormatIt) //8 shars
            {
                char* pId = va_arg(pVargs, char*);
                memcpy(pId, pBuf, USER_ID_LEN); pBuf += USER_ID_LEN;
                pId[USER_ID_LEN] = 0;
                szRet++;
            }
            else if (('P' == *pFormatIt) || ('p' == *pFormatIt)) //4 chars
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (1 != sscanf((char*)pBuf, "%04u", pVar)) { return -1; }
                pBuf += 4; 
                szRet++;
            }
            else if (('h' == *pFormatIt) || ('w' == *pFormatIt)) //2 bytes
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                *pVar = *(uint16_t*)pBuf; 
                pBuf += sizeof(uint16_t);
                szRet++;
            }
            else if ('i' == *pFormatIt) //15 chars
            {
                uint32_t uIp1, uIp2, uIp3, uIp4;
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (4 != sscanf((char*)pBuf, "%u.%u.%u.%u", &uIp1, &uIp2, &uIp3, &uIp4))  { return -1; }
                *pVar = (uIp1 << 24) | (uIp2 << 16)  | (uIp3 << 8) | (uIp4 & 0xFF);
                pBuf += 15;
                szRet++;
            }
            else if ( ('x' == *pFormatIt) || ('y' == *pFormatIt) || ('d' == *pFormatIt)) //3 chars
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (1 != sscanf((char*)pBuf, "%03u", pVar)) { return -1; }
                pBuf += 3;
                szRet++;
            }
            else if ('M' == *pFormatIt) //string 200 chars max
            {
                char* pMsg = va_arg(pVargs, char*);
                size_t szMsg = 0;
                while (    (0 != memcmp(pBuf, MSG_TCP_END, MSG_END_LEN)) 
                        && (0 != memcmp(pBuf, MSG_UDP_END, MSG_END_LEN))
                      )
                {
                    if (szMsg < MSG_MAX_TXT_LEN) { *pMsg = *pBuf; pMsg++; szMsg++; }
                    pBuf++;
                }
                *pMsg = 0;
                szRet++;
            }
        }

        pFormatIt++;
    }

    va_end(pVargs);

    return szRet;
}

void printMsg(uint8_t *pMsg, const char *pPrefix)
{
    const struct stMsgDesc *pDesc = getMsgDescriptionByPrefix(pMsg);
    if (!pDesc) {return;}

#if defined(VERBOSE_TCP_PROTOCOL)
    if (pDesc->eMsg >= eTcpMsgMaxValue)
    {
        return;
    }
#endif

#if defined(VERBOSE_UDP_PROTOCOL)
    if (pDesc->eMsg < eTcpMsgMaxValue)
    {
        return;
    }
#endif
    if (0 == strcmp(pPrefix, ">>")) { setTerminalTextColor(eTcLightMagenta); }
    else { setTerminalTextColor(eTcLightGreen);}

    //setTerminalTextColor(eTcLightGreen);

    printf("%s [", pPrefix);

    const char *pFormatIt = pDesc->pFormat;
    uint8_t    *pBuf = pMsg;

    while (*pFormatIt)
    {
        if ('%' != *pFormatIt)
        {
            printf("%c", *pBuf);
            pBuf++; 
        }
        else 
        {
            pFormatIt++;
            if (!*pFormatIt) break;

            if (('n' == *pFormatIt) || ('m' == *pFormatIt) || ('s' == *pFormatIt) || ('f' == *pFormatIt)) //1 byte
            {
                printf("%u", (uint32_t)(*(uint8_t*)pBuf));
                pBuf += sizeof(uint8_t);
            }
            else if ('G' == *pFormatIt) //8 shars
            {
                char pId[USER_ID_LEN+1];
                memcpy(pId, pBuf, USER_ID_LEN);
                pBuf += USER_ID_LEN;
                pId[USER_ID_LEN] = 0;
                printf("%s", pId);
            }
            else if (('P' == *pFormatIt) || ('p' == *pFormatIt)) //4 chars
            {
                uint32_t pVar;
                sscanf((char*)pBuf, "%04u", &pVar);
                printf("%04u", pVar);
                pBuf += 4; 
            }
            else if (('h' == *pFormatIt) || ('w' == *pFormatIt)) //2 bytes
            {
                printf("%04u", (uint32_t)(*(uint16_t*)pBuf));
                pBuf += sizeof(uint16_t);
            }
            else if ('i' == *pFormatIt) //15 chars
            {
                uint32_t uIp1, uIp2, uIp3, uIp4;
                sscanf((char*)pBuf, "%u.%u.%u.%u", &uIp1, &uIp2, &uIp3, &uIp4);
                printf("%02u.%02u.%02u.%02u", uIp1, uIp2, uIp3, uIp4);
                pBuf += 15;
            }
            else if ( ('x' == *pFormatIt) || ('y' == *pFormatIt) || ('d' == *pFormatIt)) //3 chars
            {
                uint32_t pVar;
                sscanf((char*)pBuf, "%03u", &pVar);
                printf("%03u", pVar);
                pBuf += 3;
            }
            else if ('M' == *pFormatIt) //string 200 chars max
            {
                while (    (0 != memcmp(pBuf, MSG_TCP_END, MSG_END_LEN)) 
                        && (0 != memcmp(pBuf, MSG_UDP_END, MSG_END_LEN))
                      )
                {
                    printf("%c", *pBuf);
                    pBuf++;
                }
            }
        }

        pFormatIt++;
    }

    printf("]\n");

    setTerminalTextColor(DEFAULT_TERM_COLOR);
}
