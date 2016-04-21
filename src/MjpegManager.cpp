//
//  MjpegManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//
#include "MjpegManager.hpp"
#include "Common.hpp"
#include "CamParmsEncription.hpp"

MjpegManagerRef MjpegManager::instance = nullptr;

MjpegManagerRef  MjpegManager::createNewMjpegManager()
{
    logdbg("***************************************");
    logdbg("Entering createNewMjpegManager.......");
    instance = std::shared_ptr<MjpegManager>(new MjpegManager);
    
    logdbg("Leaving createNewMjpegManager.......");
    logdbg("***************************************");
    return instance;
}

MjpegManager::MjpegManager()
: ApiState(ApiStatus::OK)
{
    ApiState = ApiStatus::OK;
}

ApiStatus MjpegManager::connectToIPCam(CamParmsEncription& value)
{
    return ApiStatus::OK;
}
