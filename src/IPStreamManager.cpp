//
//  IPStreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "IPStreamManager.hpp"
#include "CamParmsEncription.hpp"

IPStreamManager::IPStreamManager()
: name(""),
activeStream(false),
validStreamingMethod(false),
connection_url(""),
ApiState(ApiStatus::OK)
{
    
}

IPStreamManager::~IPStreamManager()
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


