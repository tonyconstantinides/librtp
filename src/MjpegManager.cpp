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

ApiStatus MjpegManager::connectToIPCam(CamParmsEncription& camAuth)
{
    // decode parms
    std::string encodedStr;
    // decode it
    encodedStr =  camAuth.getCameraGuid();
    std::string cameraGuid = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getUserName();
    std::string userName = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getPassword();
    std::string password = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getHost();
    std::string  host  = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getPort();
    std::string  port   = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getAbsPath();
    std::string  absPath = camAuth.base64_decode(encodedStr);
    
    encodedStr =  camAuth.getQueryParms();
    std::string  queryParms = camAuth.base64_decode(encodedStr);
    
    
    return ApiStatus::OK;
}
