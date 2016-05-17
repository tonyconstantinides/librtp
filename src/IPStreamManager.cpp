//
//  IPStreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include "IPStreamManager.hpp"
#include "CamParamsEncryption.hpp"

short IPStreamManager::messageCount = 0;

IPStreamManager::IPStreamManager()
: name(""),
activeStream(false),
validStreamingMethod(false),
connection_url(""),
ApiState(ApiStatus::OK)
{
}

ApiStatus  IPStreamManager::assignAuth( CamParamsEncryptionRef authRef)
{
    authCamRef = authRef;
    crypto_cameraGuid = authCamRef->getCameraGuid();
    crypto_userName        = authCamRef->getUserName();
    crypto_password        = authCamRef->getPassword();
    crypto_host               = authCamRef->getHost();
    crypto_port               = authCamRef->getPort();
    crypto_absPath          = authCamRef->getAbsPath();
    crypto_queryParms     = authCamRef->getQueryParms();
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
