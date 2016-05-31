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
    static StreamManagerRef createStreamManager();
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

    void connectBlock(CamParamsEncryptionRef camAuthRef,
                       ConnectedCallBackFunc  streamStarted,
                       ErrorCallBackFunc      streamError,
                       StreamType             streamType);
    ApiStatus disconnectStream(CamParamsEncryptionRef camAUthRef);
    static std::string getLastCameraGuid() { return cameraGuid; }
    static std::string getLastCameraStatus() { return cameraStatus; }
    static std::string getLastCameraErrorMsg() { return  cameraErrorMsg; }
    static std::string getLastCakeboxStreamingUrl() { return cakeboxStreamingUrl; }
    static ErrorCategoryDetected getLastErrorCategoryDetected()  { return category; }
    static ErrorCategoryReported getLastErrorCategoryuReported() { return reported; }

    static void setLastCameraGuid(std::string lastCameraGuid) { cameraGuid = lastCameraGuid; }
    static void setLastCameraErrorMsg(std::string  lastCameraErrorMsg) { cameraErrorMsg = lastCameraErrorMsg; }
    static void setLastCameraStatus(std::string lastCameraStatus ) { cameraStatus = lastCameraStatus; }
    static void setLastCakeboxStreamingUrl( std::string lastCakeboxStreamingUrl  ) { cakeboxStreamingUrl = lastCakeboxStreamingUrl;}
    static void setLastErrorCategoryDetected( ErrorCategoryDetected last) { category = last; }
    static void setLastErrorCategoryReported( ErrorCategoryReported last) { reported = last; }
    dispatch_queue_t  queue1, queue2, queue3, queue4;
protected:
    StreamManager();    
    std::mutex instance_mutex;
    static StreamManagerRef instance;
    std::mutex streamList_mutex;
    static VideoDataList    streamList;
    std::mutex activeCamNum_mutex;
    static int activeCamNum;
    static std::string cameraGuid;
    static std::string cameraStatus;
    static std::string cameraErrorMsg;
    static std::string cakeboxStreamingUrl;
    static ErrorCategoryDetected category;
    static ErrorCategoryReported reported;
    int callCount = 0;
    bool activeStream; // the one that streaming
    bool validStreamingMethod;  // may be possible but not streaming
    ApiStatus ApiState;
};

#endif /* StreamManager_hpp */
