//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include "Common.hpp"
#include "StreamManager.hpp"
#include "RtspManager.hpp"
#include "MjpegManager.hpp"
#include <dispatch/dispatch.h>

StreamManagerRef StreamManager::instance   = nullptr;
VideoDataList StreamManager::streamList = {};
std::string StreamManager::cameraGuid = "";
std::string StreamManager::cameraStatus = "";
std::string StreamManager::cameraErrorMsg = "";
std::string StreamManager::cakeboxStreamingUrl = "";
ErrorCategoryDetected StreamManager::category = ErrorCategoryDetected::UNKNOWN;
ErrorCategoryReported StreamManager::reported = ErrorCategoryReported::CLEAR; 


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
:  queue1(nullptr), queue2(nullptr), queue3(nullptr), queue4(nullptr), ApiState(ApiStatus::OK)
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
    if (queue1)
        dispatch_release(queue1);
    if (queue2)
        dispatch_release(queue2);
    if (queue3)
        dispatch_release(queue3);
    if (queue4)
        dispatch_release(queue4);

    queue1 = nullptr;
    queue2 = nullptr;
    queue3 = nullptr;
    queue4 = nullptr;

    logdbg("Exiting StreaManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParamsEncryptionRef camAuthRef,
                                         ConnectedCallBackFunc  streamStarted,
                                         ErrorCallBackFunc      streamError,
                                         StreamType             streamType)
{
    logdbg("Entering StreamManager::connectToH264Stream.......");
    if (streamList.size() == 0)
    {    
        queue1 = dispatch_queue_create("com.evaautomation.StreamManager.ipcam1",      DISPATCH_QUEUE_SERIAL);
        dispatch_async(queue1, ^{
            connectBlock(camAuthRef, streamStarted, streamError, streamType);
        });
    } else if (streamList.size() == 1)
    {
        queue2 = dispatch_queue_create("com.evaautomation.StreamManager.ipcam2",      DISPATCH_QUEUE_SERIAL);
        dispatch_async(queue2, ^{
            connectBlock(camAuthRef, streamStarted, streamError, streamType);
        });

    } else if (streamList.size() == 2)
    {
        queue3 = dispatch_queue_create("com.evaautomation.StreamManager.ipcam3",      DISPATCH_QUEUE_SERIAL);
        dispatch_async(queue3, ^{
            connectBlock(camAuthRef, streamStarted, streamError, streamType);
        });
    }
    else if (streamList.size() == 3)
    {
       queue4 = dispatch_queue_create("com.evaautomation.StreamManager.ipcam4",      DISPATCH_QUEUE_SERIAL);
       dispatch_async(queue4, ^{
            connectBlock(camAuthRef, streamStarted, streamError, streamType);
        });
    }   
    else {
        logerr() << "Only Four active IP Cams are supported";
        ApiState = ApiStatus::FAIL;
    }
    
   logdbg("Leaving StreamManager::connectToH264Stream.......");
   return ApiState;
}   


void StreamManager::connectBlock(CamParamsEncryptionRef camAuthRef,
                                 ConnectedCallBackFunc  streamStarted,
                                 ErrorCallBackFunc      streamError,
                                 StreamType             streamType)
{

    // used to id the stream based on cmaera guid
    RtspManagerRef          rtspManagerRef = RtspManager::createNewRtspManager();
    std::string cameraGuid = camAuthRef->base64_decode(camAuthRef->getCameraGuid());
    rtspManagerRef->setName(cameraGuid);
    MjpegManagerRef   mjpegManagerRef = MjpegManager::createNewMjpegManager();
    switch (streamType)
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
                rtspManagerRef->reportFailedConnection();
                return;
            }
            
            ApiState = rtspManagerRef->makeElements();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::makeElements()");
                rtspManagerRef->reportFailedConnection();
                return;
            }
            
            ApiState = rtspManagerRef->setupPipeLine();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::setupPipeLine()");
                rtspManagerRef->reportFailedConnection();
                return;
            }
            
            ApiState = rtspManagerRef->startLoop();
            if (ApiState != ApiStatus::OK)
            {
                rtspManagerRef->fatalApiState("Unable to RtspManager::startLoop()");
                rtspManagerRef->reportFailedConnection();
                return;
            }
            
            VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
            streamList.emplace_back(pair);
            mjpegManagerRef->validStreamMethod(true);
            mjpegManagerRef->activateStream(false); // possible to switch to MJPEG but not by default
            break;
    }
}

ApiStatus StreamManager::disconnectStream(CamParamsEncryptionRef camAuth)
{
    logdbg("Entering StreamManager::disconnectStream.......");
    for (auto& pair : streamList)
    {
        RtspManagerRef rtsp = pair.first;
        MjpegManagerRef mjpeg = pair.second;
        if (rtsp->isStream(camAuth)) 
        {    
            rtsp.reset();
            mjpeg.reset();
        }    
    }
    logdbg("Leaving StreamManager::disconnectStream.......");
    return ApiStatus::OK;
}