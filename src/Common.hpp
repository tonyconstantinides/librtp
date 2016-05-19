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
#include <gst/rtsp/gstrtspconnection.h>
#include <curl/curl.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <dispatch/dispatch.h>

enum  class ApiStatus {OK, CLEAR, FAIL, FATAL_ERROR };
enum  class StreamType {MJPEG_ONLY, H264_ONLY, H264_AND_MJPEG};
enum  class ErrorCategoryDetected {  UNKNOWN, CORE,  STREAM,  LIBRARY,  RESOURCE};
enum  class ErrorCategoryReported { CLEAR, CAM_DISCOVERY_FAILED, AUTH_FAILED, STREAM_STOPPED, MISC_ERROR};

#define USER_AGENT "Eva Automartion Cake"
#define RTSP_LATENCY 2
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 50000

class StreamManager;
class RtspManager;
class MjpegManager;
class CamParamsEncryption;
class IPStreamManager;
class StreamErrorHandler;

typedef std::shared_ptr<StreamManager> StreamManagerRef;
typedef std::shared_ptr<RtspManager> RtspManagerRef;
typedef std::shared_ptr<MjpegManager>  MjpegManagerRef;
typedef std::shared_ptr<CamParamsEncryption> CamParamsEncryptionRef;
typedef std::shared_ptr<StreamErrorHandler> StreamErrorHandlerRef;
typedef std::function<void(char*)> ConnectedCallBackFunc;
typedef std::function<void(ErrorCategoryDetected, ErrorCategoryReported,  std::string  )>      ErrorCallBackFunc;

typedef unsigned char             BYTE;
typedef std::map<std::string, std::string>  MapKeyValues;
extern ApiStatus ApiState;

#endif /* Common_h */
