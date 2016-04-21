//
//  MjpegManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef MjpegManager_hpp
#define MjpegManager_hpp

#include "Common.hpp"

class MjpegManager
{
public:
    static  MjpegManagerRef  createNewMjpegManager();
    virtual ~MjpegManager() = default;
    MjpegManager(MjpegManager const&)                      = delete;    // Copy construct
    MjpegManager(MjpegManager&&)                             = delete;   // Move construct
    MjpegManager& operator=(MjpegManager const&)        = delete;  // Copy assign
    MjpegManager& operator=(MjpegManager&&)               = default;  // Move assign
    ApiStatus connectToIPCam(CamParmsEncription& value);
    void activateStream(bool ready)      { activeStream = ready; }
    void validStreamMethod(bool valid) { validStreamingMethod = valid; }
private:
    MjpegManager();
     static MjpegManagerRef instance;
     bool activeStream; // the one that streaming
     bool validStreamingMethod;  // may be possible but not streaming
    ApiStatus ApiState;
};

#endif /* MjpegManager_hpp */