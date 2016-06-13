//
//  StreamManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//
#ifndef StreamManager_hpp
#define StreamManager_hpp
#include "Common.hpp"




class StreamManager
{ 
public:
    StreamManager() = default;    
     ~StreamManager();
    StreamManager(StreamManager const&)              = delete;
    StreamManager(StreamManager&&)                   = delete;
    StreamManager& operator=(StreamManager const&)   = delete;
    StreamManager& operator=(StreamManager&&)        = delete;
    // all auth stuff is encripted Base64, no cionnect info is sent in the clear
    ApiStatus connectToStream(CamParamsEncryptionRef camAuthRef,
                              ConnectedCallBackFunc  streamStarted,
                              ErrorCallBackFunc     streamError,
                              StreamType type );

    void connectBlock(CamParamsEncryptionRef  camAuthRef,
                       ConnectedCallBackFunc  streamStarted,
                       ErrorCallBackFunc      streamError,
                       StreamType             streamType,
                       std::string            CameraTitle);
    ApiStatus disconnectStream(CamParamsEncryptionRef camAUthRef);
    std::mutex mutex;
  protected:
    int activeCamNum = 0;
    int callCount = 0;
    bool activeStream = false; // the one that streaming
    bool validStreamingMethod = false;  // may be possible but not streaming
    ApiStatus ApiState = ApiStatus::OK;
 };

#endif /* StreamManager_hpp */
