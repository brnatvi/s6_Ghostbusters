#include "commons.h"

const struct stMsgDesc group_msg[] = 
{
    {.msg = eTcpMsgGAMES,  .prefix = "GAMES", .format = "GAMES %n***",                .szMsg = sizeof("GAMES n***")   - 1                           }, //+
    {.msg = eTcpMsgOGAME,  .prefix = "OGAME", .format = "OGAME %m %s***",             .szMsg = sizeof("OGAME m s***") - 1                           }, //+
    {.msg = eTcpMsgNEWPL,  .prefix = "NEWPL", .format = "NEWPL %G %P***",             .szMsg = sizeof("NEWPL idxxxxxx port***") - 1                 },
    {.msg = eTcpMsgREGIS,  .prefix = "REGIS", .format = "REGIS %G %P %m***",          .szMsg = sizeof("REGIS idxxxxxx port m***") - 1               },
    {.msg = eTcpMsgREGOK,  .prefix = "REGOK", .format = "REGOK %m***",                .szMsg = sizeof("REGOK m***") - 1                             },
    {.msg = eTcpMsgREGNO,  .prefix = "REGNO", .format = "REGNO***",                   .szMsg = sizeof("REGNO***") - 1                               },
    {.msg = eTcpMsgNMEMB,  .prefix = "NMEMB", .format = "NMEMB***",                   .szMsg = sizeof("NMEMB***") - 1                               },
    {.msg = eTcpMsgSTART,  .prefix = "START", .format = "START***",                   .szMsg = sizeof("START***") - 1                               },
    {.msg = eTcpMsgUNREG,  .prefix = "UNREG", .format = "UNREG***",                   .szMsg = sizeof("UNREG***") - 1                               },
    {.msg = eTcpMsgUNROK,  .prefix = "UNROK", .format = "UNROK %m***",                .szMsg = sizeof("UNROK m***") - 1                             },
    {.msg = eTcpMsgDUNNO,  .prefix = "DUNNO", .format = "DUNNO***",                   .szMsg = sizeof("DUNNO***") - 1                               },
    {.msg = eTcpMsgSIZEQ,  .prefix = "SIZE?", .format = "SIZE? %m***",                .szMsg = sizeof("SIZE? m***") - 1                             }, //+
    {.msg = eTcpMsgSIZEA,  .prefix = "SIZE!", .format = "SIZE! %m %h %w***",          .szMsg = sizeof("SIZE! m hh ww***") - 1                       }, //+
    {.msg = eTcpMsgLISTQ,  .prefix = "LIST?", .format = "LIST? %m***",                .szMsg = sizeof("LIST? m***") - 1                             },
    {.msg = eTcpMsgLISTA,  .prefix = "LIST!", .format = "LIST! %m %s***",             .szMsg = sizeof("LIST! m s***") - 1                           },
    {.msg = eTcpMsgPLAYR,  .prefix = "PLAYR", .format = "PLAYR %G***",                .szMsg = sizeof("PLAYR idxxxxxx***") - 1                      },
    {.msg = eTcpMsgGAMEQ,  .prefix = "GAME?", .format = "GAME?***",                   .szMsg = sizeof("GAME?***") - 1                               }, //++
    {.msg = eTcpMsgWELCO,  .prefix = "WELCO", .format = "WELCO %m %h %w %f %i %P***", .szMsg = sizeof("WELCO m hh ww f IPxxxxxxxxxxxxx port***") - 1},
    {.msg = eTcpMsgPOSIT,  .prefix = "POSIT", .format = "POSIT %G %x %y***",          .szMsg = sizeof("POSIT idxxxxxx XXX YYY***") - 1              },
    {.msg = eTcpMsgUPMOV,  .prefix = "UPMOV", .format = "UPMOV %d***",                .szMsg = sizeof("UPMOV ddd***") - 1                           },
    {.msg = eTcpMsgDOMOV,  .prefix = "DOMOV", .format = "DOMOV %d***",                .szMsg = sizeof("DOMOV ddd***") - 1                           },
    {.msg = eTcpMsgLEMOV,  .prefix = "LEMOV", .format = "LEMOV %d***",                .szMsg = sizeof("LEMOV ddd***") - 1                           },
    {.msg = eTcpMsgRIMOV,  .prefix = "RIMOV", .format = "RIMOV %d***",                .szMsg = sizeof("RIMOV ddd***") - 1                           },
    {.msg = eTcpMsgMOVEA,  .prefix = "MOVE!", .format = "MOVE! %x %x***",             .szMsg = sizeof("MOVE! XXX YYY***") - 1                       },
    {.msg = eTcpMsgMOVEF,  .prefix = "MOVEF", .format = "MOVEF %x %y %p***",          .szMsg = sizeof("MOVEF XXX YYY pppp***") - 1                  },
    {.msg = eTcpMsgIQUIT,  .prefix = "IQUIT", .format = "IQUIT***",                   .szMsg = sizeof("IQUIT***") - 1                               },
    {.msg = eTcpMsgGOBYE,  .prefix = "GOBYE", .format = "GOBYE***",                   .szMsg = sizeof("GOBYE***") - 1                               },
    {.msg = eTcpMsgGLISQ,  .prefix = "GLIS?", .format = "GLIS?***",                   .szMsg = sizeof("GLIS?***") - 1                               },
    {.msg = eTcpMsgGLISA,  .prefix = "GLIS!", .format = "GLIS! %s***",                .szMsg = sizeof("GLIS! s***") - 1                             },
    {.msg = eTcpMsgGPLYR,  .prefix = "GPLYR", .format = "GPLYR %G %x %y %p***",       .szMsg = sizeof("GPLYR idxxxxxx XXX YYY pppp***") - 1         },
    {.msg = eTcpMsgMALLQ,  .prefix = "MALL?", .format = "MALL? %M***",                .szMsg = 0 /*sizeof("MALL? idxxxxxx mess***")*/               },
    {.msg = eTcpMsgMALLA,  .prefix = "MALL!", .format = "MALL!***",                   .szMsg = sizeof("MALL!***") - 1                               },
    {.msg = eTcpMsgSENDQ,  .prefix = "SEND?", .format = "SEND? %G %M***",             .szMsg = 0 /*sizeof("SEND? idxxxxxx mess***")*/               },
    {.msg = eTcpMsgSENDA,  .prefix = "SEND!", .format = "SEND!***",                   .szMsg = sizeof("SEND!***") - 1                               },
    {.msg = eTcpMsgNSEND,  .prefix = "NSEND", .format = "NSEND***",                   .szMsg = sizeof("NSEND***") - 1                               },

    {.msg = eUdpMsgGHOST,  .prefix = "GHOST", .format = "GHOST %x %y+++",             .szMsg = sizeof("GHOST xxx yyy+++") - 1                       }, 
    {.msg = eUdpMsgSCORE,  .prefix = "SCORE", .format = "SCORE %G %p %x %y+++",       .szMsg = sizeof("SCORE idxxxxxx pppp XXX YYY+++") - 1         }, 
    {.msg = eUdpMsgMESSA,  .prefix = "MESSA", .format = "MESSA %G %M+++",             .szMsg = 0 /*sizeof("MESSA idxxxxxx mess+++")*/               }, 
    {.msg = eUdpMsgENDGA,  .prefix = "ENDGA", .format = "ENDGA %G %p+++",             .szMsg = sizeof("ENDGA idxxxxxx pppp+++") - 1                 }, 
    {.msg = eUdpMsgMESSP,  .prefix = "MESSP", .format = "MESSP %G %M+++",             .szMsg = 0 /*sizeof("MESSP idxxxxxx mess+++")*/               }
};


const struct stMsgDesc *getMsgDescriptionByPrefix(const char* pMsgStart)
{
    if (!pMsgStart)    
    {
        return NULL;
    }

    size_t szCount = sizeof(group_msg) / sizeof(group_msg[0]);
    for (size_t i = 0; i < szCount; i++)
    {
        if (0 == strncmp(pMsgStart, group_msg[i].prefix, PREFIX_LEN))
        {
            return &group_msg[i];
        }
    }

    return NULL;
}

const struct stMsgDesc *getMsgDescriptionById(enum msgId msg)
{
    size_t szCount = sizeof(group_msg) / sizeof(group_msg[0]);
    for (size_t i = 0; i < szCount; i++)
    {
        if (group_msg[i].msg == msg)
        {
            return &group_msg[i];
        }
    }

    return NULL;
}

ssize_t tcpProcessIncomingData(int socket, struct stIpBuffer *buf)
{
    ssize_t szRec = 0;
    if ((IP_BUFFER_MAX - buf->szWritten) > MAX_LEN)
    {
        szRec = recv(socket, buf->buffer + buf->szWritten, IP_BUFFER_MAX - buf->szWritten, 0);
        if (szRec > 0)
        {
            buf->szWritten += szRec;
        }
        else
        {
            szRec = -1;    
        }
    }
    return szRec;
};

bool commitMsg(struct stIpBuffer *buf, size_t szMsg)
{
    if ((!buf) || (szMsg > buf->szWritten)) return false;

    if (szMsg == buf->szWritten)
    {
        buf->szWritten  = 0;
        return true;
    }

    memmove(buf->buffer,
            buf->buffer + szMsg,
            buf->szWritten - szMsg        
           );

    buf->szWritten -= szMsg;
    return true;
}

int tcpGetMsg(int socket, struct stIpBuffer *buf, struct stRawMessage *pMsg)
{
    if ((!buf) || (!pMsg)) return -1;

    int ret = 0;

    do
    {
        if (buf->szWritten > PREFIX_LEN)
        {
            const struct stMsgDesc *desc = getMsgDescriptionByPrefix((const char*)buf->buffer);

            if (!desc)
            {
                log_error("Protocol error! message not recognized!");
                return -1;
            }

            if (desc->msg >= eTcpMsgMaxValue)
            {
                log_error("Protocol error! unsupported message type");
                return -1;
            }

            if ((buf->szWritten >= desc->szMsg) && (desc->szMsg))
            {
                pMsg->msg  = desc->msg;
                pMsg->szMsg = desc->szMsg;
                memcpy(pMsg->msgRaw, buf->buffer, pMsg->szMsg);
                commitMsg(buf, pMsg->szMsg);
                ret = 1;
            }
            else if (0 == desc->szMsg)
            {
                const size_t szTail = 3;
                uint8_t *iter = buf->buffer + PREFIX_LEN;
                while ((iter + szTail) <= (buf->buffer + buf->szWritten))
                {
                    if (0 == strncmp("***", (const char*)iter, szTail))
                    {
                        iter += szTail; //***
                        pMsg->msg  = desc->msg;
                        pMsg->szMsg = iter - buf->buffer;
                        memcpy(pMsg->msgRaw, buf->buffer, pMsg->szMsg);
                        commitMsg(buf, pMsg->szMsg);

                        ret = 1;
                        break;
                    }
                    iter++;    
                }
            }
        }

        if (!ret)
        {
            if (tcpProcessIncomingData(socket, buf) < 0)
            {
                ret = -1;
                break;
            }
        }

    } while (ret <= 0);

    if (ret > 0) printMsg(pMsg->msgRaw, "<<");

    return ret;
}

size_t createMsg(uint8_t *pMsg, enum msgId msg, va_list pVargs)
{
    uint8_t *pBuf = pMsg;
    
    const struct stMsgDesc *desc = getMsgDescriptionById(msg);
    if (!desc) {return 0;}

    const char *formatIt = desc->format;
    uint8_t    *pBufStart = pBuf;

    while (*formatIt)
    {
        if ('%' != *formatIt)
        {
            *pBuf = *formatIt; pBuf++; 
        }
        else 
        {
            formatIt++;
            if (!*formatIt) return -1;
            if (('n' == *formatIt) || ('m' == *formatIt) || ('s' == *formatIt) || ('f' == *formatIt)) //1 byte
            {
                *pBuf = (uint8_t)va_arg(pVargs, uint32_t);
                pBuf += sizeof(uint8_t);
            }
            else if ('G' == *formatIt) //8 shars
            {
                memcpy(pBuf, va_arg(pVargs, char*), USER_ID_LEN);
                pBuf += USER_ID_LEN;
            }
            else if (('P' == *formatIt) ||  ('p' == *formatIt)) //4 chars
            {
                int ret = sprintf((char*)pBuf, "%04u", (uint32_t)va_arg(pVargs, uint32_t));
                if (ret) pBuf += ret; 
                else return -1;
            }
            else if (('h' == *formatIt) || ('w' == *formatIt)) //2 bytes
            {
                *(uint16_t*)pBuf = (uint16_t)va_arg(pVargs, uint32_t);
                pBuf += sizeof(uint16_t);
            }
            else if ('i' == *formatIt) //15 chars
            {
                const size_t ip = 15;
                char         pIp[ip*2];
                uint32_t     uIp = va_arg(pVargs, uint32_t);

                sprintf((char*)pIp, "%u.%u.%u.%u",
                        (uint32_t)((uIp >> 24) & 0xFF),
                        (uint32_t)((uIp >> 16) & 0xFF),
                        (uint32_t)((uIp >> 8) & 0xFF),
                        (uint32_t)(uIp & 0xFF)
                       );

                for (size_t i = strlen(pIp); i < ip; i++)
                {
                    pIp[i] = '#';
                }

                memcpy(pBuf, pIp, ip);
                pBuf += ip;
            }
            else if ( ('x' == *formatIt) || ('y' == *formatIt) || ('d' == *formatIt)) //3 chars
            {
                int ret = sprintf((char*)pBuf, "%03u", va_arg(pVargs, uint32_t));
                if (ret) pBuf += ret;
                else return -1;
            }
            else if ('M' == *formatIt) //string 200 chars max
            {
                const char *pMsg = (const char*)va_arg(pVargs, char*);
                strncpy((char*)pBuf, pMsg, MAX_MSG_LEN);
                size_t szMsg = strlen(pMsg);
                if (szMsg <= MAX_MSG_LEN) { pBuf += szMsg; } else { pBuf += MAX_MSG_LEN; }
            }
        }

        formatIt++;
    }

    *pBuf = 0;

    if ((desc->szMsg) && ((pBuf - pBufStart) != desc->szMsg))
    {
        log_error("{formatMsg} buffer formatting error!");
        return 0;
    }

    return pBuf - pBufStart;
}


bool sendMsg(int socket, enum msgId msg, ...)
{
    uint8_t  pMsg[MAX_LEN];    

    va_list  pVargs;
    va_start(pVargs, msg);
    size_t szMsg = createMsg(pMsg, msg, pVargs);
    va_end(pVargs);

    if (!szMsg)
    {
        log_error("{formatMsg} buffer formatting error!");
        return false;
    }

    printMsg(pMsg, ">>");

    return szMsg == send(socket, pMsg, szMsg, 0);
}

bool sendMsgTo(int socket, 
               const struct sockaddr *addr, 
               socklen_t addrlen, 
               enum msgId msg, 
               ...)
{
    uint8_t  pMsg[MAX_LEN];    

    va_list  pVargs;
    va_start(pVargs, msg);
    size_t szMsg = createMsg(pMsg, msg, pVargs);
    va_end(pVargs);

    if (!szMsg)
    {
        log_error("{formatMsg} buffer formatting error!");
        return false;
    }

    printMsg(pMsg, ">>");

    return szMsg == sendto(socket, pMsg, szMsg, 0, addr, addrlen);
}


size_t scanMsg(uint8_t *pMsg, enum msgId msg, ...)
{
    const struct stMsgDesc *desc = getMsgDescriptionById(msg);
    if (!desc) {return -1;}

    size_t   szRet = 0;
    va_list  pVargs;
    va_start(pVargs, msg);

    const char *formatIt = desc->format;
    uint8_t    *pBuf = pMsg;

    while (*formatIt)
    {
        if ('%' != *formatIt)
        {
            pBuf++; 
        }
        else 
        {
            formatIt++;
            if (!*formatIt) return -1;
            if (('n' == *formatIt) || ('m' == *formatIt) || ('s' == *formatIt) || ('f' == *formatIt)) //1 byte
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                *pVar = *(uint8_t*)pBuf; 
                pBuf += sizeof(uint8_t);
                szRet++;
            }
            else if ('G' == *formatIt) //8 shars
            {
                char* pId = va_arg(pVargs, char*);
                memcpy(pId, pBuf, USER_ID_LEN); pBuf += USER_ID_LEN;
                pId[USER_ID_LEN] = 0;
                szRet++;
            }
            else if (('P' == *formatIt) || ('p' == *formatIt)) //4 chars
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (1 != sscanf((char*)pBuf, "%04u", pVar)) { return -1; }
                pBuf += 4; 
                szRet++;
            }
            else if (('h' == *formatIt) || ('w' == *formatIt)) //2 bytes
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                *pVar = *(uint16_t*)pBuf; 
                pBuf += sizeof(uint16_t);
                szRet++;
            }
            else if ('i' == *formatIt) //15 chars
            {
                uint32_t uIp1, uIp2, uIp3, uIp4;
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (4 != sscanf((char*)pBuf, "%u.%u.%u.%u", &uIp1, &uIp2, &uIp3, &uIp4))  { return -1; }
                *pVar = (uIp1 << 24) | (uIp2 << 16) | (uIp3 << 8) | (uIp4 & 0xFF);
                pBuf += 15;
                szRet++;
            }
            else if ( ('x' == *formatIt) || ('y' == *formatIt) || ('d' == *formatIt)) //3 chars
            {
                uint32_t *pVar = va_arg(pVargs, uint32_t*);
                if (1 != sscanf((char*)pBuf, "%03u", pVar)) { return -1; }
                pBuf += 3;
                szRet++;
            }
            else if ('M' == *formatIt) //string 200 chars max
            {
                char* pMsg = va_arg(pVargs, char*);
                size_t szMsg = 0;
                while ((0 != memcmp(pBuf, TCP_END, END_LEN)) && (0 != memcmp(pBuf, UDP_END, END_LEN)))
                {
                    if (szMsg < MAX_MSG_LEN)
                    {
                        *pMsg = *pBuf;
                        pMsg++;
                        szMsg++;
                    }
                    pBuf++;
                }
                *pMsg = 0;
                szRet++;
            }
        }

        formatIt++;
    }

    va_end(pVargs);

    return szRet;
}


void printMsg(uint8_t *pMsg, const char *pPrefix)
{
    const struct stMsgDesc *pDesc = getMsgDescriptionByPrefix((const char*)pMsg);
    if (!pDesc) {return;}

#if defined(VERBOSE_TCP_PROTOCOL) && defined(VERBOSE_UDP_PROTOCOL)
    //nothing - allows everything
#elif defined(VERBOSE_TCP_PROTOCOL)
    if (pDesc->msg >= eTcpMsgMaxValue) //if message ID is not belongs to TCP messages range range - quit
    {
        return;
    }
#elif defined(VERBOSE_UDP_PROTOCOL)
    if (pDesc->msg <= eTcpMsgMaxValue) //if message ID is not belongs to UDP messages range range - quit
    {
        return;
    }
#endif
    
    if (0 == strcmp(pPrefix, ">>")) { SET_TERMINAL_COLOR(eTcLightGreen); }
    else { SET_TERMINAL_COLOR(eTcLightMagenta);}

    printf("%s [", pPrefix);

    const char *pFormatIt = pDesc->format;
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
                while (    (0 != memcmp(pBuf, TCP_END, END_LEN)) 
                        && (0 != memcmp(pBuf, UDP_END, END_LEN))
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

    SET_TERMINAL_COLOR(DEFAULT_TERM_COLOR);
}
