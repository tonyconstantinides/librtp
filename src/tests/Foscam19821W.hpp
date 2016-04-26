//
//  Foscam19821W.hpp
//  ipcams
//
//  Created by Tony Constantinides on 2/8/16.
//  Copyright © 2016 Eva Autoation. All rights reserved.
//

#ifndef Foscam19821W_hpp
#define Foscam19821W_hpp

#include <string>
namespace IPCams {
    namespace FoscamH264  {
class Foscam19821W
{
public:
    static void buildCompoundKeys();
    static std::string CAMERA_H264_IP_PORT;
    static std::string H264_AUIH;
    static std::string CGI_PROXY;
    static std::string SET_MJPEG_STREAM;
    static std::string SET_H264_STREAM;
    static std::string GET_MJPEG_STREAM;
    static std::string RTSP_STREAMING_ADDRESS;
    static std::string GET_VIDEO_STREAM_H264;
    static std::string GET_SUB_VIDEO_STREAM_H264;
    static std::string GET_AUDIO_H264;
    static std::string RESOLUTION_640x480;
    static std::string FPS_MAX_RATE;
};
        
// set to H264 stream
        
        
}
}
#endif /* Foscam19821W_hpp */
