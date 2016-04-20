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
#include "CamParmsEncription.hpp"

class StreamManager
{
public:
    static StreamManagerRef createStreamManager();
    virtual ~StreamManager() = default;
    // all auth stuff is encripted Base64, no cionnect info is sent in the clear
    ApiStatus connectToH264Stream(CamParmsEncription& camAuth );
    ApiStatus connectToMJPEGStream(CamParmsEncription& camAuth);
    ApiStatus disconnectStream(std::string streamID);
    StreamManager(StreamManager const&)                       = delete;
    StreamManager(StreamManager&&)                              = delete;
    StreamManager& operator=(StreamManager const&)    = delete;
    StreamManager& operator=(StreamManager&&)            = delete;
   
protected:
    StreamManager();
    static StreamManagerRef instance;
};

#endif /* StreamManager_hpp */
