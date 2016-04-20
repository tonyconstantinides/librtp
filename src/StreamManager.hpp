//
//  StreamManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef StreamManager_hpp
#define StreamManager_hpp
#include <functional>
#include <string>
#include "Common.hpp"


class StreamManager
{
public:
    enum   ApiStatus {OK, CLEAR, FAIL, FATAL_ERROR };
    ApiStatus ApiState;
    static StreamManagerRef createStreamManager();
    virtual ~StreamManager() = default;
    // all auth stuff is encripted Base64, no cionnect info is sent in the clear
    std::string connectToH264Stream(CamParmsEncription& camAuth );
    std::string connectToMJPEGStream(CamParmsEncription& camAuth);
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
