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

typedef std::pair<RtspManagerRef, MjpegManagerRef> VideoStreamPair;
typedef std::vector<VideoStreamPair>   VideoDataList;


class StreamManager
{
public:
    StreamManager();    
    virtual ~StreamManager();
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
    std::mutex data_mutex;
   
 protected:
    StreamManagerRef instance;
    VideoDataList    streamList;
    std::mutex activeCamNum_mutex;
    int activeCamNum;
    int callCount = 0;
    bool activeStream; // the one that streaming
    bool validStreamingMethod;  // may be possible but not streaming
    ApiStatus ApiState;
 
};

#endif /* StreamManager_hpp */
