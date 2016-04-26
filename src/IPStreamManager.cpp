//
//  IPStreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "IPStreamManager.hpp"

IPStreamManager::IPStreamManager()
: name(nullptr),
activeStream(false),
validStreamingMethod(false),
connection_url(""),
ApiState(ApiStatus::OK)
{
    
}

IPStreamManager::~IPStreamManager()
{
    
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


