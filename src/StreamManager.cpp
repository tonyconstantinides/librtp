//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "StreamManager.hpp"


StreamManagerRef StreamManager::createStreamManager()
{
    logdbg("***************************************");
    logdbg("Entering createStreamManager.......");
    refCounts++;
    instance = std::shared_ptr<StreamManager>(new StreamManager);
    logdbg("Leaving createStreamManager.......");
    logdbg("***************************************");
    return instance;
}

StreamManager::StraemManager()
: ApiState(ApiState::OK)
{
    logdbg("Entering StreamManager constructor.......");
    logdbg("Exiting StreaManager constructor.......");
}

std::string StreamManager::connectToH264Stream(CamParmsEncription& camAuth)
{
    logdbg("Entering StreamManager::connectToH264Stream.......");
    std:string encodedStr;
 
    // decode it
    encodedStr =  camAuth.getCameraGuid();
    std::string cameraGuid = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getUserName();
    std::string userName    = camAuth.base64_decode(encodedStr);
  
    encodedStr = camAuth.getHost();
    std::string  host  = camAuth.base64_decode(encodedStr);
  
    encodedStr = camAuth.getPort();
    std::string  port   = camAuth.base64_decode(encodedStr);
  
    encodedStr = camAuth.getAbsPath();
    std::string  absPath       = camAuth.base64_decode(encodedStr);
    
    encodedStr =  camAuth.getQueryParms();
    std::string  queryParms = camAuth.base64_decode(encodedStr);
    
    logdbg("Leaving StreamManager::connectToH264Stream.......");
}

std::string StreamManager::connectToMJPEGStream(CamParmsEncription& camAuth);
{
    logdbg("Entering StreamManager::connectToMJPEGStream.......");
    
    logdbg("Leaving StreamManager::connectToMJPEGStream.......");
}

ApiStatus StreamManager::disconnectStream(std::string streamID)
{
    logdbg("Entering StreamManager::disconnectStream.......");
    logdbg("Leaving StreamManager::disconnectStream.......");
}