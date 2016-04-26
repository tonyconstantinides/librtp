//
//  Common.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/19/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef Common_h
#define Common_h
#include <foundation/foundation.hpp>
#include <functional>
#include <string>
#include <memory>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <map>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gst/rtsp/gstrtsp.h>
#include <gst/gstutils.h>
#include <gst/base/gstpushsrc.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define USER_AGENT "Eva Automartion Cake"
#define RTSP_LATENCY 0
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 50000

class StreamManager;
class RtspManager;
class MjpegManager;
class RtspManagerCallbacks;
class CamParmsEncription;

enum   API_CALL_STATUS {OK, CLEAR, FAIL, FATAL_ERROR };
enum   STREAM_TYPE {MJPEG_ONLY, H264_ONLY, H264_AND_MJPEG};
typedef std::shared_ptr<StreamManager> StreamManagerRef;
typedef std::shared_ptr<RtspManager> RtspManagerRef;
typedef std::shared_ptr<RtspManagerCallbacks> CallbacksRef;
typedef std::shared_ptr<MjpegManager>  MjpegManagerRef;
typedef API_CALL_STATUS   ApiStatus;
typedef STREAM_TYPE         StreamType;
typedef unsigned char             BYTE;
typedef std::map<std::string, std::string>  MapKeyValues;
extern ApiStatus ApiState;

#endif /* Common_h */
