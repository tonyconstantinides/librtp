//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include "StreamManager.hpp"
#include "RtspManager.hpp"
#include "MjpegManager.hpp"
#include <dispatch/dispatch.h>

StreamManagerRef StreamManager::instance   = nullptr;
VideoDataList StreamManager::streamList = {};

StreamManagerRef StreamManager::createStreamManager()
{
    logdbg("***************************************");
    logdbg("Entering createStreamManager.......");
    // only allow oncenstance  evan if teh client calls it multiple times
    if (instance == nullptr && streamList.empty())
    {    
     static dispatch_once_t onceToken;
     dispatch_once(&onceToken, ^{
        struct _StreamManager : StreamManager {
            _StreamManager()
            : StreamManager()  {}
        };
        instance = std::make_shared<_StreamManager>();
    });
    }
    logdbg("Leaving createStreamManager.......");
    logdbg("***************************************");
    return instance;
}

StreamManager::StreamManager()
:  queue(nullptr),  ApiState(ApiStatus::OK)
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
    instance.reset();
    instance   = nullptr;
    dispatch_release(queue);
    queue = nullptr;
    logdbg("Exiting StreaManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParamsEncryptionRef camAuthRef,
                                         ConnectedCallBackFunc streamStarted,
                                         ErrorCallBackFunc    streamError,
                                         StreamType type)
{
    logdbg("Entering StreamManager::connectToH264Stream.......");
    queue = dispatch_queue_create("com.evaautomation.StreamManager",      DISPATCH_QUEUE_SERIAL);
    dispatch_async(queue, ^{
    // used to id the stream based on cmaera guid
    RtspManagerRef          rtspManagerRef = RtspManager::createNewRtspManager();
    std::string cameraGuid = camAuthRef->base64_decode(camAuthRef->getCameraGuid());
    rtspManagerRef->setName(cameraGuid);
    MjpegManagerRef   mjpegManagerRef = MjpegManager::createNewMjpegManager();
    switch (type )
    {
        case StreamType::MJPEG_ONLY:
            rtspManagerRef->validStreamMethod(false);
            rtspManagerRef->activateStream(false);
            mjpegManagerRef->validStreamMethod(true);
            mjpegManagerRef->activateStream(true);
            ApiState = mjpegManagerRef->connectToIPCam(camAuthRef);
            if (ApiState == ApiStatus::OK)
            {
                VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                streamList.emplace_back(pair);
            }
            break;
        case StreamType::H264_ONLY:
            rtspManagerRef->validStreamMethod(true);
            rtspManagerRef->activateStream(true);
            ApiState = rtspManagerRef->connectToIPCam(camAuthRef);
            if (ApiState == ApiStatus::OK)
            {
                VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                streamList.emplace_back(pair);
            }
            mjpegManagerRef->validStreamMethod(false);
            mjpegManagerRef->activateStream(false);
         break;
        case StreamType::H264_AND_MJPEG:
       
            rtspManagerRef->validStreamMethod(true);
            rtspManagerRef->activateStream(true); // this is the prime stream
            // old school programmiong with no exceptions, check every return
            rtspManagerRef->addConnectionCallback(streamStarted);
            rtspManagerRef->addErrorCallback(streamError);
            ApiState = rtspManagerRef->connectToIPCam(camAuthRef);
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to Connect to RTSP/H264 Cam");
            }
            
            ApiState = rtspManagerRef->makeElements();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::makeElements()");
            }
            
            ApiState = rtspManagerRef->setupPipeLine();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::setupPipeLine()");
            }
            
            ApiState = rtspManagerRef->startLoop();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::startLoop()");
            }
            
            VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
            streamList.emplace_back(pair);
            mjpegManagerRef->validStreamMethod(true);
            mjpegManagerRef->activateStream(false); // possible to switch to MJPEG but not by default
    
            break;
    }
   });
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