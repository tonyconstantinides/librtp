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
    // all auth stuff is encripted Base64, no cionnect info is sent in the clear
    ApiStatus connectToStream(CamParmsEncription& camAuth, StreamType type);
    ApiStatus disconnectStreams(std::string streamID);
    StreamManager(StreamManager const&)              = delete;
    StreamManager(StreamManager&&)                   = delete;
    StreamManager& operator=(StreamManager const&)   = delete;
    StreamManager& operator=(StreamManager&&)        = delete;
    void activateStream(bool ready)      { activeStream = ready; }
    void validStreamMethod(bool valid) { validStreamingMethod = valid; }
protected:
    StreamManager();    
    static StreamManagerRef instance;
    static VideoDataList    streamList;
    bool activeStream; // the one that streaming
    bool validStreamingMethod;  // may be possible but not streaming
    ApiStatus ApiState;
};

#endif /* StreamManager_hpp */
