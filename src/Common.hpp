//
//  Common.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/19/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef Common_h
#define Common_h

class StreamManager;
class RtspManager;
class RtspManagerCallbacks;

#define USER_AGENT "Eva Automartion Cake"
#define RTSP_LATENCY 0
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 50000
typedef std::shared_ptr<RtspManager> RtspManagerRef;
typedef std::shared_ptr<RtspManagerCallbacks> CallbacksRef;
typedef std::shared_ptr<StreamManager> StreamManagerRef;
enum   API_CALL_STATUS {OK, CLEAR, FAIL, FATAL_ERROR };
typedef API_CALL_STATUS ApiStatus;
typedef unsigned char BYTE;
typedef std::map<std::string, std::string>  MapKeyValues;

#endif /* Common_h */
