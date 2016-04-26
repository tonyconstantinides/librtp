//
//  IPStreamManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef IPStreamManager_hpp
#define IPStreamManager_hpp

#include "Common.hpp"

class IPStreamManager
{
public:
    IPStreamManager();
    virtual ~IPStreamManager();
    IPStreamManager(IPStreamManager const&)                      = delete;    // Copy construct
    IPStreamManager(IPStreamManager&&)                             = delete;   // Move construct
    IPStreamManager& operator=(IPStreamManager const&)        = delete;  // Copy assign
    IPStreamManager& operator=(IPStreamManager&&)               = default;  // Move assign
    ApiStatus activateStream(bool ready)      { activeStream = ready; return ApiState; }
    ApiStatus validStreamMethod(bool valid) { validStreamingMethod = valid;  return ApiState;}
    std::string getName() { return name; }
    ApiStatus setName(std::string streamName) { name = streamName; return ApiState; }
    virtual ApiStatus makeElements() = 0;
    virtual ApiStatus setupPipeLine() = 0;
    virtual ApiStatus startLoop() = 0;
    ApiStatus  errorApiState( const gchar * msg);
    // fatal error do not continue
    ApiStatus  fatalApiState( const gchar* msg);
    // test the ip connection before we try to use it
    virtual ApiStatus testConnection() = 0;
protected:
    virtual ApiStatus   createElements() = 0;
    virtual ApiStatus   addElementsToBin() = 0;
    virtual ApiStatus   setElementsProperties() = 0;
    virtual ApiStatus   addCallbacks() = 0;
    virtual ApiStatus   removeCallbacks() = 0;
    virtual ApiStatus   cleanUp() = 0;
    std::string name;
    bool           activeStream;
    bool           validStreamingMethod;
    std::string connection_url;
    ApiStatus   ApiState;
};

#endif /* IPStreamManager_hpp */