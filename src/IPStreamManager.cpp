//
//  IPStreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "IPStreamManager.hpp"
#include "CamParmsEncription.hpp"

short IPStreamManager::messageCount = 0;

IPStreamManager::IPStreamManager()
: name(""),
activeStream(false),
validStreamingMethod(false),
connection_url(""),
ApiState(ApiStatus::OK)
{
}

ApiStatus  IPStreamManager::assignAuth( CamParmsEncription& camAuth)
{
    crypto_cameraGuid = camAuth.getCameraGuid();
    crypto_userName = camAuth.getUserName();
    crypto_password = camAuth.getPassword();
    crypto_host  = camAuth.getHost();
    crypto_port   = camAuth.getPort();
    crypto_absPath = camAuth.getAbsPath();
    crypto_queryParms = camAuth.getQueryParms();
    ApiState = ApiStatus::OK;
    return  ApiState;
}

ApiStatus IPStreamManager::errorApiState( const gchar * msg)
{
    logerr() << msg;
    ApiState = ApiStatus::FAIL;
    return  ApiState;
}

ApiStatus IPStreamManager::fatalApiState(const gchar * msg)
{
    logerr() << msg;
    ApiState = ApiStatus::FATAL_ERROR;
    return  ApiState;
}

void IPStreamManager::printMsg(GstMessage* msg, const gchar*  msgType)
{
    if (msg == NULL)
    {
        logerr() << "GstMessage to printMsg cannot be null";
    }
    else {
        const gchar* msgTypeName   = GST_MESSAGE_TYPE_NAME(msg);
        GstClockTime timeStamp          = GST_MESSAGE_TIMESTAMP(msg);
        const gchar* srcObj                 = GST_MESSAGE_SRC_NAME(msg);
        guint seqnum                          = GST_MESSAGE_SEQNUM(msg);
        logdbg ("--------------------------------------------------------------------------\n");
        logdbg("Mesage SeqNum     : " << std::to_string(seqnum));
        logdbg("Message type      : " << msgType);
        logdbg("Messge type  Name : " << msgTypeName);
        logdbg("Time Stamp when mesage created  : " << std::to_string(timeStamp));
        logdbg("Src Object Name   : " << srcObj);
        logdbg ("---------------------------------------------------------------------------\n");
    }
 }
