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
    gst_player();
    ~gst_player();
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
    CamParamsEncryptionRef camAuthRef;
    ConnectedCallBackFunc  streamStarted;
    ErrorCallBackFunc      streamError;
    StreamType             streamType; 
    VideoStreamMapRef      streamMapRef;
    ApiStatus ApiState;
    VideoStreamMapRef     getStreamList() { return streamMapRef; }

    static int camCount;
    void setup();
    void killCam(int CamNum);
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

gst_player::gst_player()
{
  streamMapRef = std::make_shared<VideoStreamMap>();
}

gst_player::~gst_player()
 {
  streamMapRef.reset();
  streamMapRef = nullptr;
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
        g_thread_ref (m_camThread1);
    } else if (camCount == 2)
    {
        m_camThread2 = g_thread_new("Cam2 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
        g_thread_ref (m_camThread2);
    } else if (camCount == 3)
    {
        m_camThread3 = g_thread_new("Cam3 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
        g_thread_ref (m_camThread3);
    } else if (camCount == 4)
    {
        m_camThread4 = g_thread_new("Cam4 Thread", (GThreadFunc)&playerRef->thread_loop_run, NULL);
        g_thread_ref (m_camThread4);
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
      playerRef->createVideoPipeline(IPCAM_ONE);
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
       playerRef->createVideoPipeline(IPCAM_TWO);
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
       playerRef->createVideoPipeline(IPCAM_THREE);
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
        playerRef->createVideoPipeline(IPCAM_FOUR);
        logdbg("----------------------------------------------------------");
        logdbg("Running Event loop for Camera Four Thread.......");
        logdbg("----------------------------------------------------------");
        g_main_loop_run(m_mainLoop_cam4);
        g_thread_exit(0);
   }
   logdbg("Entering gst_player::main_loop_thread()......."); 
} 



void gst_player::killCam(int CamNum)
{
 logdbg("Entering gst_player::killCam() for CamNum " << CamNum);
 if (CamNum == 1)
 {
      logdbg("Cleanup worker_context_cam1!");    
      g_main_loop_quit(m_mainLoop_cam1);
      g_main_loop_unref(m_mainLoop_cam1);
      g_main_context_pop_thread_default(worker_context_cam1);
      g_main_context_unref(worker_context_cam1); 
      g_thread_unref (m_camThread1);
 }
 else if (CamNum == 2)
 {
      logdbg("Cleanup worker_context_cam2!");    
      g_main_loop_quit(m_mainLoop_cam2);
      g_main_loop_unref(m_mainLoop_cam2);
      g_main_context_pop_thread_default(worker_context_cam2);
      g_main_context_unref(worker_context_cam2); 
      g_thread_unref (m_camThread2);
 } 
 else if (CamNum == 3)
 {
      logdbg("Cleanup worker_context_cam3!");    
      g_main_loop_quit(m_mainLoop_cam3);
      g_main_loop_unref(m_mainLoop_cam3);
      g_main_context_pop_thread_default(worker_context_cam3);
      g_main_context_unref(worker_context_cam3); 
      g_thread_unref (m_camThread3);
 }
 else if (CamNum == 4)
 {
      logdbg("Cleanup worker_context_cam4!");    
      g_main_loop_quit(m_mainLoop_cam4);
      g_main_loop_unref(m_mainLoop_cam4);
      g_main_context_pop_thread_default(worker_context_cam4);
      g_main_context_unref(worker_context_cam4); 
      g_thread_unref (m_camThread4);
 }

 logdbg("Leaving gst_player::killCam() for CamNum " << CamNum);
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
    std::string key = "";
    key.append( camAuthRef->base64_decode( camAuthRef->getCameraGuid() ));
    key.append( camAuthRef->base64_decode( camAuthRef->getUserName() ));
    key.append( camAuthRef->base64_decode( camAuthRef->getPassword() ));
    key.append( camAuthRef->base64_decode( camAuthRef->getHost() ));
    key.append( camAuthRef->base64_decode( camAuthRef->getPort() ));
    
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
                  std::pair<RtspManagerRef, MjpegManagerRef>  pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                  pair.first = rtspManagerRef;
                  pair.second = mjpegManagerRef;
                  // makeup the key based on all the parts of the auth
                  playerRef->getStreamList()->emplace(std::make_pair(key, pair));
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
                  std::pair<RtspManagerRef, MjpegManagerRef>  pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
                  pair.first = rtspManagerRef;
                  pair.second = mjpegManagerRef;
                  playerRef->getStreamList()->emplace(std::make_pair(key, pair));
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
              std::pair<RtspManagerRef, MjpegManagerRef>  pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
              pair.first = rtspManagerRef;
              pair.second = mjpegManagerRef;
              playerRef->getStreamList()->emplace(std::make_pair(key, pair));
            
              int size = playerRef->getStreamList()->size();
              logdbg("Size of StreamList is : " << std::to_string(size));
          
              rtspManagerRef->validStreamMethod(true);
              rtspManagerRef->activateStream(true); // this is the prime stream
              // set the active Cam Num
              logdbg("Setting active cam num in RtspManager!");
              if (std::strcmp(CameraTitle.c_str(), IPCAM_ONE) == 0)
              {
                rtspManagerRef->setActiveCamNum(1);
              }
              else if (std::strcmp(CameraTitle.c_str(), IPCAM_TWO) == 0)
              {
                  rtspManagerRef->setActiveCamNum(2);
              }
              else if (std::strcmp(CameraTitle.c_str(), IPCAM_THREE) == 0)
              {
                  rtspManagerRef->setActiveCamNum(3);
              }         
              else if (std::strcmp(CameraTitle.c_str(), IPCAM_FOUR) == 0)
              {
                  rtspManagerRef->setActiveCamNum(4);
              }   
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


StreamManager::~StreamManager()
{
    logdbg("Entering StreamManager destructor.......");
    for (auto it = playerRef->getStreamList()->begin(); it != playerRef->getStreamList()->end();)
    {
        it = playerRef->streamMapRef->erase(it);
    }       
    playerRef->getStreamList()->clear();
    playerRef.reset();
    logdbg("Exiting StreaManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParamsEncryptionRef camAuthRef,
                                         ConnectedCallBackFunc  streamStarted,
                                         ErrorCallBackFunc      streamError,
                                         StreamType             streamType)
{
    std::unique_lock<std::mutex> guard(mutex, std::defer_lock);   guard.lock();
    callCount++;
    logdbg("Entering StreamManager::connectToH264Stream.......");
    logdbg("Placing mutext guard for streamList!");
    playerRef->camAuthRef    = camAuthRef;
    playerRef->streamStarted = streamStarted;
    playerRef->streamError   = streamError;
    playerRef->streamType    = streamType;
  
    if (callCount >= 1 && callCount <= 4)
    {
        playerRef->play(callCount);
    } 
    else
    {
        logerr() << "Only Four active IP Cams are supported";
        ApiState = ApiStatus::FAIL;
    } 
   guard.unlock();
 
   logdbg("Leaving StreamManager::connectToH264Stream.......");
   return ApiState;
}   


ApiStatus StreamManager::disconnectStream(CamParamsEncryptionRef camAuthRef)
{
   logdbg("Entering StreamManager::disconnectStream.......");
   logdbg("Number of streams is : " <<  playerRef->streamMapRef->size()) ;
   std::unique_lock<std::mutex> guard(mutex, std::defer_lock); 
   guard.lock();
  
   logdbg("Removing stream item from list as thw objects and its thread is dead.......");
   std::string key = "";
   key.append( camAuthRef->base64_decode( camAuthRef->getCameraGuid() ));
   key.append( camAuthRef->base64_decode( camAuthRef->getUserName() ));
   key.append( camAuthRef->base64_decode( camAuthRef->getPassword() ));
   key.append( camAuthRef->base64_decode( camAuthRef->getHost() ));
   key.append( camAuthRef->base64_decode( camAuthRef->getPort() ));
      
   auto pair =  playerRef->getStreamList()->find(key);
   // the first item is the key the second is an std::pair 
   RtspManagerRef rtsp   = pair->second.first;
   MjpegManagerRef mjpeg = pair->second.second;
   if (rtsp->getActiveCamNum()  == 1)
   {
       logdbg("Killing Stream for Camera One.......");
       playerRef->killCam(1);
   } 
   else if  (rtsp->getActiveCamNum() == 2)
   {
      logdbg("Killing Stream for Camera Two.......");
      playerRef->killCam(2);
   } 
   else if (rtsp->getActiveCamNum() == 3) 
   {
      logdbg("Killing Stream for Camera Three.......");
      playerRef->killCam(3);
   }  
   else if (rtsp->getActiveCamNum() == 4)
   {
      logdbg("Killing Stream for Camera Four.......");
      playerRef->killCam(4);
   }
   rtsp-> cleanUp();
   rtsp.reset();
   mjpeg.reset();
   pair->second.first.reset();
   pair->second.second.reset();
   playerRef->getStreamList()->erase(key);
   callCount--;
   guard.unlock();
   ApiState = ApiStatus::OK;
   logdbg("Number of streams Now is : " <<  playerRef->getStreamList()->size());
   logdbg("Leaving StreamManager::disconnectStream.......");
   return ApiState;
}




