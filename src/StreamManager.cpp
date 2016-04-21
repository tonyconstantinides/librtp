//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "StreamManager.hpp"
#include "RtspManager.hpp"
#include "MjpegManager.hpp"

StreamManagerRef StreamManager::instance   = nullptr;
VideoDataList StreamManager::streamList = {};

StreamManagerRef StreamManager::createStreamManager()
{
    logdbg("***************************************");
    logdbg("Entering createStreamManager.......");
    // only allow once instance  evan if teh client calls it multiple times
    if (instance == nullptr && streamList.empty())
    {    
      instance   =  StreamManagerRef(new StreamManager);
    }
    logdbg("Leaving createStreamManager.......");
    logdbg("***************************************");
    return instance;
}

StreamManager::StreamManager()
: ApiState(ApiStatus::OK)
{
    logdbg("Entering StreamManager constructor.......");
    logdbg("Exiting StreaManager constructor.......");
}

StreamManager::~StreamManager()
{
    logdbg("Entering StreamManager destructor.......");
    for (auto it = streamList.begin(); it != streamList.end();)
    {
        it = streamList.erase(it);
    }       
    streamList.clear();
    instance   = nullptr;
    logdbg("Exiting StreaManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParmsEncription& camAuth, StreamType type)
{
    logdbg("Entering StreamManager::connectToH264Stream.......");
    // used to id the stream based on cmaera guid
    RtspManagerRef          rtspManagerRef = RtspManager::createNewRtspManager();
    std::string cameraGuid = camAuth.base64_decode(camAuth.getCameraGuid());
    rtspManagerRef->setName(cameraGuid);
    MjpegManagerRef   mjpegManagerRef = MjpegManager::createNewMjpegManager();
    switch (type )
    {
        case MJPEG_ONLY:
            rtspManagerRef->validStreamMethod(false);
            rtspManagerRef->activateStream(false);
            mjpegManagerRef->validStreamMethod(true);
            mjpegManagerRef->activateStream(true);
            ApiState = mjpegManagerRef->connectToIPCam(camAuth);
            if (ApiState == ApiStatus::OK)
            {
                VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                streamList.emplace_back(pair);
            }
            break;
        case H264_ONLY:
            rtspManagerRef->validStreamMethod(true);
            rtspManagerRef->activateStream(true);
            ApiState = rtspManagerRef->connectToIPCam(camAuth);
            if (ApiState == ApiStatus::OK)
            {
                VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                streamList.emplace_back(pair);
            }
            mjpegManagerRef->validStreamMethod(false);
            mjpegManagerRef->activateStream(false);
         break;
        case H264_AND_MJPEG:
            rtspManagerRef->validStreamMethod(true);
            rtspManagerRef->activateStream(true); // this is the prime stream
            ApiState = rtspManagerRef->connectToIPCam(camAuth);
            if (ApiState == ApiStatus::OK)
            {
                VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                streamList.emplace_back(pair);
            }
            mjpegManagerRef->validStreamMethod(true);
            mjpegManagerRef->activateStream(false); // possible to switch to MJPEG but not by default
            break;
        default:
            break;
    }
   logdbg("Leaving StreamManager::connectToH264Stream.......");
    return ApiState;
}   

ApiStatus StreamManager::disconnectStreams(std::string streamID)
{
    logdbg("Entering StreamManager::disconnectStream.......");
    for (auto& pair : streamList)
    {
        RtspManagerRef rtsp = pair.first;
        MjpegManagerRef mjpeg = pair.second;
        rtsp.reset();
        mjpeg.reset();
    }
    logdbg("Leaving StreamManager::disconnectStream.......");
    return ApiStatus::OK;
}