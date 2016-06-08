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

typedef std::shared_ptr<StreamErrorHandler> StreamErrorHandlerRef;

class StreamErrorHandler
{
public:
    StreamErrorHandler()          = default;
    virtual ~StreamErrorHandler() = default;
    StreamErrorHandler(StreamErrorHandler const&)             = delete;    // Copy construct
    StreamErrorHandler(StreamErrorHandler&&)                  = delete;   // Move construct
    StreamErrorHandler& operator=(StreamErrorHandler const&)  = delete;  // Copy assign
    StreamErrorHandler& operator=(StreamErrorHandler&&)       = default;  // Move assign
    ApiStatus processErrorState(GstMessage* msg );
    std::string errorMsg;
    ErrorCategoryDetected category;
    ErrorCategoryReported reported;
};


#endif /* StreamErrorHandler_hpp */
