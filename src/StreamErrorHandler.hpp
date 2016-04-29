//
//  StreamCallbacksBase.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/28/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef StreamErrorHandler_hpp
#define StreamErrorHandler_hpp

#include "Common.hpp"

class StreamErrorHandler
{
public:
    StreamErrorHandler()          = default;
    virtual ~StreamErrorHandler() = default;
    StreamErrorHandler(StreamErrorHandler const&)             = delete;    // Copy construct
    StreamErrorHandler(StreamErrorHandler&&)                  = delete;   // Move construct
    StreamErrorHandler& operator=(StreamErrorHandler const&)  = delete;  // Copy assign
    StreamErrorHandler& operator=(StreamErrorHandler&&)       = default;  // Move assign
    static ApiStatus processErrorState(GstMessage* msg );
    
};


#endif /* StreamErrorHandler_hpp */
