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

class StreamManager;
typedef std::shared_ptr<StreamManager> StreamManagerRef;

class StreamManager
{
public:
    static StreamManagerRef createStreamManager();
    virtual ~StreamManager() = default;
    
    StreamManager(StreamManager const&)                       = delete;
    StreamManager(StreamManager&&)                              = delete;
    StreamManager& operator=(StreamManager const&)    = delete;
    StreamManager& operator=(StreamManager&&)            = delete;
    
    enum   API_CALL_STATUS {OK, CLEAR, FAIL, FATAL_ERROR };
    typedef API_CALL_STATUS ApiStatus;
protected:
    StreamManager();
    static StreamManagerRef instance;
};


#endif /* StreamManager_hpp */
