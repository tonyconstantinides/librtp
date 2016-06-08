//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include "Common.hpp"
#include "StreamManager.hpp"
#include "RtspManager.hpp"
#include "MjpegManager.hpp"
#include <dispatch/dispatch.h>


class gst_player 
{
public:
    gst_player() = default;
    virtual ~gst_player();
    gst_player(gst_player const&)              = delete;
    gst_player(gst_player&&)                   = delete;
    gst_player& operator=(gst_player const&)   = delete;
    gst_player& operator=(gst_player&&)        = delete;
  
    static GThread*   m_camThread1;
    static GThread*   m_camThread2;
    static GThread*   m_camThread3;
    static GThread*   m_camThread4;
    static GMainContext *worker_context_cam1;
    static GMainContext *worker_context_cam2;
    static GMainContext *worker_context_cam3;
    static GMainContext *worker_context_cam4;
    static GMainLoop* m_mainLoop_cam1;
    static GMainLoop* m_mainLoop_cam2;
    static GMainLoop* m_mainLoop_cam3;
    static GMainLoop* m_mainLoop_cam4;
    dispatch_queue_t queue;
    CamParamsEncryptionRef camAuthRef;
    ConnectedCallBackFunc  streamStarted;
    ErrorCallBackFunc      streamError;
    StreamType             streamType; 
    VideoDataList          streamList;
    ApiStatus ApiState;
    std::mutex gst_player_mutex;
    static int camCount;

    void setup();
    void cleanup();
    static void thread_loop_run(gpointer data);
    void play(int callCount);
    void createVideoPipeline(std::string CameraTitle);
    void connectBlock(CamParamsEncryptionRef camAuthRef,
                             ConnectedCallBackFunc  streamStarted,
                             ErrorCallBackFunc      streamError,
                             StreamType             streamType,
                             std::string           CameraTitle);

};

std::shared_ptr<gst_player> playerRef = std::make_shared<gst_player>();

//dispatch_queue_t  gst_player::queue;

GThread*   gst_player::m_camThread1 = nullptr;
GThread*   gst_player::m_camThread2 = nullptr;
GThread*   gst_player::m_camThread3 = nullptr;
GThread*   gst_player::m_camThread4 = nullptr; 
GMainContext* gst_player::worker_context_cam1 = nullptr;
GMainContext* gst_player::worker_context_cam2 = nullptr;
GMainContext* gst_player::worker_context_cam3 = nullptr;
GMainContext* gst_player::worker_context_cam4 = nullptr;
GMainLoop* gst_player::m_mainLoop_cam1 = nullptr;
GMainLoop* gst_player::m_mainLoop_cam2 = nullptr;
GMainLoop* gst_player::m_mainLoop_cam3 = nullptr;
GMainLoop* gst_player::m_mainLoop_cam4 = nullptr;
int gst_player::camCount = 0;

/*
CamParamsEncryptionRef gst_player::camAuthRef;
ConnectedCallBackFunc  gst_player::streamStarted;
ErrorCallBackFunc      gst_player::streamError;
StreamType             gst_player::streamType; 
VideoDataList gst_player::streamList = {};
ApiStatus gst_player::ApiState = ApiStatus::OK;
std::mutex gst_player::gst_player_mutex;
int gst_player::camCount = 0;
*/

 gst_player::~gst_player()
 {
  playerRef.reset();
  playerRef = nullptr;
 }
 

void gst_player::play(int callCount)
{
    logdbg("Entering gst_player::play()....... "  << " with camCount is: " << callCount);
    camCount = callCount;
    if (camCount == 1)
    {
        m_camThread1 = g_thread_new("Cam1 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
    } else if (camCount == 2)
    {
        m_camThread2 = g_thread_new("Cam2 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
    } else if (camCount == 3)
    {
        m_camThread3 = g_thread_new("Cam3 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
    } else if (camCount == 4)
    {
        m_camThread4 = g_thread_new("Cam4 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
    }
     logdbg("Leaving gst_player::play()....... ");
}

void gst_player::thread_loop_run(gpointer data)
{
   logdbg("Entering gst_player::main_loop_thread().......");
 
   if (camCount == 1)
   {
      logdbg("----------------------------------------------------------");
      logdbg("Creating an Event loop for Camera One Thread.......");
      logdbg("----------------------------------------------------------");
      worker_context_cam1 = g_main_context_new();
      g_main_context_push_thread_default(worker_context_cam1);
      m_mainLoop_cam1 = g_main_loop_new(worker_context_cam1, FALSE);
      logdbg("----------------------------------------------------------");
      logdbg("Running Event loop for Camera One Thread.......");
      logdbg("----------------------------------------------------------");
      playerRef->createVideoPipeline("IPCam 1");
      g_main_loop_run(m_mainLoop_cam1);
      g_thread_exit(0);
   } else if (camCount == 2) 
   {
       logdbg("----------------------------------------------------------");
       logdbg("Creating an Event loop for Camera Two Thread.......");
       logdbg("----------------------------------------------------------");
       worker_context_cam2 = g_main_context_new();
       g_main_context_push_thread_default(worker_context_cam2);
       m_mainLoop_cam2 = g_main_loop_new(worker_context_cam2, FALSE);
       playerRef->createVideoPipeline("IPCam 2");
       logdbg("----------------------------------------------------------");
       logdbg("Running Event loop for Camera Two Thread.......");
       logdbg("----------------------------------------------------------");
       g_main_loop_run(m_mainLoop_cam2);
       g_thread_exit(0);
   } else if (camCount == 3)
   {
       logdbg("----------------------------------------------------------");
       logdbg("Creating an Event loop for Camera Three Thread.......");
       logdbg("----------------------------------------------------------");
       worker_context_cam3 = g_main_context_new();
       g_main_context_push_thread_default(worker_context_cam3);
       m_mainLoop_cam3 = g_main_loop_new(worker_context_cam3, FALSE);
       playerRef->createVideoPipeline("IPCam 3");
       logdbg("----------------------------------------------------------");
       logdbg("Running Event loop for Camera Three Thread.......");
       logdbg("----------------------------------------------------------");
       g_main_loop_run(m_mainLoop_cam3);
       g_thread_exit(0);
   } else if (camCount == 4)
   {
        logdbg("----------------------------------------------------------");
        logdbg("Creating an Event loop for Camera Four Thread.......");
        logdbg("----------------------------------------------------------");
        worker_context_cam4 = g_main_context_new();
        g_main_context_push_thread_default(worker_context_cam4);
        m_mainLoop_cam4 = g_main_loop_new(worker_context_cam4, FALSE);
        playerRef->createVideoPipeline("IPCam 4");
        logdbg("----------------------------------------------------------");
        logdbg("Running Event loop for Camera Four Thread.......");
        logdbg("----------------------------------------------------------");
        g_main_loop_run(m_mainLoop_cam4);
        g_thread_exit(0);
   }
   logdbg("Entering gst_player::main_loop_thread()......."); 
} 


void gst_player::cleanup()
{
    logdbg("Entering gst_player::cleanup()");
    if (worker_context_cam1) 
    {  
      logdbg("Cleanup worker_context_cam1!");    
      g_main_loop_unref(m_mainLoop_cam1);
      g_main_context_pop_thread_default(worker_context_cam1);
      g_main_context_unref(worker_context_cam1); 
    }
    if (worker_context_cam2) 
    {  
      logdbg("Cleanup worker_context_cam2!");    
      g_main_loop_unref(m_mainLoop_cam2);
      g_main_context_pop_thread_default(worker_context_cam2);
      g_main_context_unref(worker_context_cam2); 
    }
    if (worker_context_cam3) 
    {  
      logdbg("Cleanup worker_context_cam3!");    
      g_main_loop_unref(m_mainLoop_cam3);
      g_main_context_pop_thread_default(worker_context_cam3);
      g_main_context_unref(worker_context_cam3); 
    }
    if (worker_context_cam4) 
    {  
      logdbg("Cleanup worker_context_cam4!");    
      g_main_loop_unref(m_mainLoop_cam4);
      g_main_context_pop_thread_default(worker_context_cam4);
      g_main_context_unref(worker_context_cam4); 
    }  

    if (gst_player::queue)    
          dispatch_release(gst_player::queue);

   logdbg("Leaviung gst_player::cleanup()");
}

void gst_player::setup()
{
  logdbg("Entering gst_player::setup()");
  logdbg("Leaving gst_player::setup()");
}   

void gst_player::createVideoPipeline(std::string CameraTitle)
{

    logdbg("Entering gst_player::createVideoPipeline().......");
     gst_player::connectBlock(gst_player::camAuthRef,
                                 gst_player::streamStarted, 
                                 gst_player::streamError,
                                 gst_player::streamType,
                                 CameraTitle);
    logdbg("Leaving gst_player::createVideoPipeline().......");
}


void gst_player::connectBlock(CamParamsEncryptionRef    camAuthRef,
                                 ConnectedCallBackFunc  streamStarted,
                                 ErrorCallBackFunc      streamError,
                                 StreamType             streamType,
                                 std::string            CameraTitle)
{
    logdbg("Entering gst_player::connectBlock.......");

    // used to id the stream based on cmaera guid

    switch (streamType)
    {
        case StreamType::MJPEG_ONLY:
            {
              std::mutex rtpMangerRef_mutex;
              std::lock_guard<std::mutex> rtp_guard(rtpMangerRef_mutex);
              RtspManagerRef  rtspManagerRef = std::make_shared<RtspManager>(CameraTitle);
              std::string cameraGuid = camAuthRef->base64_decode(camAuthRef->getCameraGuid());
              rtspManagerRef->setName(cameraGuid);
              std::mutex mjpegManagerRef_mutex;
              std::lock_guard<std::mutex> mjpeg_guard(mjpegManagerRef_mutex);
              MjpegManagerRef   mjpegManagerRef = std::make_shared<MjpegManager>();

              rtspManagerRef->validStreamMethod(false);
              rtspManagerRef->activateStream(false);
              mjpegManagerRef->validStreamMethod(true);
              mjpegManagerRef->activateStream(true);
              ApiState = mjpegManagerRef->connectToIPCam(camAuthRef);
              if (ApiState == ApiStatus::OK)
              {
                  VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                  streamList.emplace_back(pair);
              }
            }
            break;
        case StreamType::H264_ONLY: 
            {
              RtspManagerRef  rtspManagerRef = std::make_shared<RtspManager>(CameraTitle);
              MjpegManagerRef   mjpegManagerRef = std::make_shared<MjpegManager>();
     
              rtspManagerRef->validStreamMethod(true);
              rtspManagerRef->activateStream(true);
              ApiState = rtspManagerRef->connectToIPCam(camAuthRef);
              if (ApiState == ApiStatus::OK)
              {
                  VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                  streamList.emplace_back(pair);
              }
              mjpegManagerRef->validStreamMethod(false);
              mjpegManagerRef->activateStream(false);
            }
              break;
        case StreamType::H264_AND_MJPEG: 
            {
              logdbg("Camera is H264 and MJPEG compatable!");
              RtspManagerRef  rtspManagerRef = std::make_shared<RtspManager>(CameraTitle);
              MjpegManagerRef   mjpegManagerRef = std::make_shared<MjpegManager>();
                // add to the list
              logdbg("Adding a stream to the stream list!");
              //std::lock_guard<std::mutex> lock(StreamManager::data_mutex);
              VideoStreamPair pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
              gst_player::streamList.emplace_back(pair);
              int size = streamList.size();
              logdbg("Size of StreamList is : " << std::to_string(size));
          
              //std::mutex rtpMangerRef_inner_mutex;
              //std::lock_guard<std::mutex> rtp_inner_guard(rtpMangerRef_inner_mutex);
              rtspManagerRef->validStreamMethod(true);
              rtspManagerRef->activateStream(true); // this is the prime stream
              // set the active Cam Num
              logdbg("Setting active cam num in RtspManager!");
              rtspManagerRef->setActiveCamNum(size);
              rtspManagerRef->addConnectionCallback(streamStarted);
              // old school programmiong with no exceptions, check every return
              rtspManagerRef->addErrorCallback(streamError);
              mjpegManagerRef->validStreamMethod(true);
              mjpegManagerRef->activateStream(false); // possible to switch to MJPEG but not by default
              ApiState = rtspManagerRef->connectToIPCam(camAuthRef);

               if (ApiState != ApiStatus::OK)
               {
                   rtspManagerRef->fatalApiState("Unable to Connect to RTSP/H264 Cam");
                   rtspManagerRef->reportFailedConnection();
                   return;
               }

               ApiState = rtspManagerRef->makeElements();
               if (ApiState != ApiStatus::OK)
               {
                   rtspManagerRef->fatalApiState("Unable to RtspManager::makeElements()");
                   rtspManagerRef->reportFailedConnection();
                   return;
               }

               ApiState = rtspManagerRef->setupPipeLine();
               if (ApiState != ApiStatus::OK)
               {
                   rtspManagerRef->fatalApiState("Unable to RtspManager::setupPipeLine()");
                   rtspManagerRef->reportFailedConnection();
                   return;
               }
               ApiState = rtspManagerRef->startLoop();
               if (ApiState != ApiStatus::OK)
               {
                   rtspManagerRef->fatalApiState("Unable to RtspManager::startLoop()");
                   rtspManagerRef->reportFailedConnection();
                   return;
               }
             }  
               break;
    }
    logdbg("Leaving gst_player::connectBlock.......");
}



/*
StreamManagerRef StreamManager::instance   = nullptr;

VideoDataList StreamManager::streamList = {};
std::string StreamManager::cameraGuid = "";
std::string StreamManager::cameraStatus = "";
std::string StreamManager::cameraErrorMsg = "";
std::string StreamManager::cakeboxStreamingUrl = "";
ErrorCategoryDetected StreamManager::category = ErrorCategoryDetected::UNKNOWN;
ErrorCategoryReported StreamManager::reported = ErrorCategoryReported::CLEAR; 
int StreamManager::activeCamNum = 0;
*/


StreamManager::StreamManager()
:  ApiState(ApiStatus::OK)
{
    logdbg("Entering StreamManager constructor.......");
    logdbg("Exiting StreaManager constructor.......");
}

StreamManager::~StreamManager()
{
    logdbg("Entering StreamManager destructor.......");

    for (auto it = streamList.begin(); it != streamList.end();)
    {
        it = streamList.erase(it);
    }       
    streamList.clear();
    instance.reset();
    instance   = nullptr;

    logdbg("Exiting StreaManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParamsEncryptionRef camAuthRef,
                                         ConnectedCallBackFunc  streamStarted,
                                         ErrorCallBackFunc      streamError,
                                         StreamType             streamType)
{
    callCount++;
    logdbg("Entering StreamManager::connectToH264Stream.......");
    logdbg("Placing mutext guard for streamList!");
    playerRef->camAuthRef    = camAuthRef;
    playerRef->streamStarted = streamStarted;
    playerRef->streamError   = streamError;
    playerRef->streamType    = streamType;
    playerRef->streamList    = streamList;

    if (callCount >= 1 && callCount <=4)
    {
        playerRef->play(callCount);
    }   else
    {
        logerr() << "Only Four active IP Cams are supported";
        ApiState = ApiStatus::FAIL;
    } 
 
   logdbg("Leaving StreamManager::connectToH264Stream.......");
   return ApiState;
}   


ApiStatus StreamManager::disconnectStream(CamParamsEncryptionRef camAuth)
{
    logdbg("Entering StreamManager::disconnectStream.......");
    for (auto& pair : streamList)
    {
        RtspManagerRef rtsp = pair.first;
        MjpegManagerRef mjpeg = pair.second;
        if (rtsp->isStream(camAuth)) 
        {    
            rtsp.reset();
            mjpeg.reset();
        }    
    }
    logdbg("Leaving StreamManager::disconnectStream.......");
    return ApiStatus::OK;
}




