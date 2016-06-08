//
//  CamNotificationDefs.hpp
//  ipcams
//
//  Created by Tony Constantinides on 5/17/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#ifndef CamNotificationDefs_h
#define CamNotificationDefs_h

static const std::string IPCamNotification    = "ipcam_notification_catageory";
static const std::string IPCamErrorCategory = "ipcam_errorCategoryKey";

static const std::string  IPCamGUID            = "ipcam_cameraGuidKey";
static const std::string   IPCamSeq             = "ipcam_seqnum";
static const std::string  IPCamCakeStreamingURL = "ipcam_cakeStreamingURLKey";
static const std::string  IPCamStatus = "ipcam_statusKey";

static const std::string  IPCamUnrecoverableError = "ipcam_unrecoverableErrorKey";
static const std::string  IPCamConnectionSuccess  = "ipcam_connectionSuccessKey";
static const std::string  IPCamDisoveryFailed     = "CAM_DISCOVERY_FAILED";
static const std::string  IPCamAuthFailed         = "AUTH_FAILED";
static const std::string  IPCamStreamStopped      = "STREAM_STOPPED";
static const std::string  IPCamMiscError          = "MISC_ERROR";

#endif /* CamNotificationDefs_h */
