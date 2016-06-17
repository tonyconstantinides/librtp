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
#include <atomic>
#include <string>

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
    VideoStreamMapRef     getStreamList();
    std::mutex mutex;

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

VideoStreamMapRef streamMapRef = std::make_shared<VideoStreamMap>();
std::shared_ptr<gst_player> playerRef1 = nullptr;
std::shared_ptr<gst_player> playerRef2 = nullptr;
std::shared_ptr<gst_player> playerRef3 = nullptr;
std::shared_ptr<VideoStreamMap>    streamMapRef1 = nullptr;
std::shared_ptr<VideoStreamMap>    streamMapRef2 = nullptr;
std::shared_ptr<VideoStreamMap>    streamMapRef3 = nullptr;

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


VideoStreamMapRef  gst_player::getStreamList()
{
    return gst_player::streamMapRef;
}

gst_player::gst_player()
{
 // mutex.lock();
//  mutex.unlock();
}

gst_player::~gst_player()
 {
 // mutex.lock();
  streamMapRef.reset();
  streamMapRef = nullptr;
  if (playerRef1)
  {
     playerRef1.reset();
     playerRef1 = nullptr;
     streamMapRef1.reset();
     streamMapRef1 = nullptr;
  }
  if (playerRef2)
   {
     playerRef2.reset();
     playerRef2 = nullptr;
     streamMapRef2.reset();
     streamMapRef2 = nullptr;
   }
   if (playerRef3)
   {
     playerRef3.reset();
     playerRef3 = nullptr;
     streamMapRef3.reset();
     streamMapRef3 = nullptr;
   }
  //   mutex.unlock();
 }

void gst_player::play(int callCount)
{
    logdbg("Entering gst_player::play()....... "  << " with camCount is: " << callCount);
    camCount = callCount;
    if (camCount == 1)
    {
        m_camThread1 = g_thread_new("Cam1 Thread", (GThreadFunc)&playerRef1->thread_loop_run, NULL);
        g_thread_ref (m_camThread1);
    } else if (camCount == 2)
    {
        m_camThread2 = g_thread_new("Cam2 Thread", (GThreadFunc)&playerRef2->thread_loop_run, NULL);
        g_thread_ref (m_camThread2);
    } else if (camCount == 3)
    {
        m_camThread3 = g_thread_new("Cam3 Thread", (GThreadFunc)&playerRef3->thread_loop_run, NULL);
        g_thread_ref (m_camThread3);
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
      // one per thread otherwise we have deadlock 
      playerRef1->createVideoPipeline(IPCAM_ONE);
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
       playerRef2->createVideoPipeline(IPCAM_TWO);
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
       playerRef3->createVideoPipeline(IPCAM_THREE);
       logdbg("----------------------------------------------------------");
       logdbg("Running Event loop for Camera Three Thread.......");
       logdbg("----------------------------------------------------------");
       g_main_loop_run(m_mainLoop_cam3);
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
      mutex.lock();    
      g_main_context_unref(worker_context_cam1); 
      g_main_loop_quit(m_mainLoop_cam1);
      g_main_loop_unref(m_mainLoop_cam1);
      g_thread_unref (m_camThread1);
      m_camThread1 = nullptr;
      worker_context_cam1 = nullptr;
      m_mainLoop_cam1 = nullptr;
      mutex.unlock();    
 }
 else if (CamNum == 2)
 {
      logdbg("Cleanup worker_context_cam2!");    
      mutex.lock();    
      g_main_context_unref(worker_context_cam2); 
      g_main_loop_quit(m_mainLoop_cam2);
      g_main_loop_unref(m_mainLoop_cam2);
      g_thread_unref (m_camThread2);
      m_camThread2 = nullptr;
      worker_context_cam2 = nullptr;
      m_mainLoop_cam2 = nullptr;
      mutex.unlock();    
 } 
 else if (CamNum == 3)
 {
      logdbg("Cleanup worker_context_cam3!");    
      mutex.lock();
      g_main_context_unref(worker_context_cam3); 
      g_main_loop_quit(m_mainLoop_cam3);
      g_main_loop_unref(m_mainLoop_cam3);
      g_thread_unref (m_camThread3);
      m_camThread3 = nullptr;
      worker_context_cam3 = nullptr;
      m_mainLoop_cam3 = nullptr;
      mutex.unlock();
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
   // mutex.lock();
    if (gst_player::camCount == 1)
    {
       playerRef1->connectBlock(
                                 playerRef1->camAuthRef,
                                 playerRef1->streamStarted,
                                 playerRef1->streamError,
                                 playerRef1->streamType,
                                 CameraTitle);
    } else if (gst_player::camCount == 2)
    {
        playerRef2->connectBlock(
                                 playerRef2->camAuthRef,
                                 playerRef2->streamStarted,
                                 playerRef2->streamError,
                                 playerRef2->streamType,
                                 CameraTitle);
    }  else if (gst_player::camCount == 3)
    {
        playerRef3->connectBlock(
                                 playerRef3->camAuthRef,
                                 playerRef3->streamStarted,
                                 playerRef3->streamError,
                                 playerRef3->streamType,
                                 CameraTitle);
    } 

   //utex.unlock();
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
                  gst_player::getStreamList()->emplace(std::make_pair(key, pair));
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
                  gst_player::getStreamList()->emplace(std::make_pair(key, pair));
              }
              mjpegManagerRef->validStreamMethod(false);
              mjpegManagerRef->activateStream(false);
            }
            break;
        case StreamType::H264_AND_MJPEG: 
            {
              logdbg("Camera is H264 and MJPEG compatable!");
            //  mutex.lock();

              RtspManagerRef  rtspManagerRef = std::make_shared<RtspManager>(CameraTitle);
              MjpegManagerRef   mjpegManagerRef = std::make_shared<MjpegManager>();
              rtspManagerRef->validStreamMethod(true);
              rtspManagerRef->activateStream(true); // this is the prime stream
              rtspManagerRef->addConnectionCallback(streamStarted);
              // old school programmiong with no exceptions, check every return
              rtspManagerRef->addErrorCallback(streamError);
              mjpegManagerRef->validStreamMethod(true);
              mjpegManagerRef->activateStream(false); // possible to switch to MJPEG but not by default
  
                // add to the list
              logdbg("---------------------------------------------");
              logdbg("Adding a stream to the stream list!");
              std::pair<RtspManagerRef, MjpegManagerRef>  pair = std::make_pair(rtspManagerRef, mjpegManagerRef );
              pair.first = rtspManagerRef;
              pair.second = mjpegManagerRef;
      
              logdbg("Inserting key associated with RtspManagerRef and MjpegManagerRef pair!");
              logdbg("key is: " << key);
                    
              // set the active Cam Num
              logdbg("Setting active cam num in RtspManager!");
              if (std::strcmp(CameraTitle.c_str(), IPCAM_ONE) == 0)
              {
                  rtspManagerRef->setActiveCamNum(1);
                  logdbg("Size of the streamList before inserting another stream: " <<  streamMapRef1->size());
                  streamMapRef1->emplace(std::make_pair(key, pair));
                  logdbg("Size of the streamList after inserting another stream: " <<  streamMapRef1->size());
              }
              else if (std::strcmp(CameraTitle.c_str(), IPCAM_TWO) == 0)
              {
                  rtspManagerRef->setActiveCamNum(2);
                  logdbg("Size of the streamList before inserting another stream: " <<  streamMapRef2->size());
                  streamMapRef2->emplace(std::make_pair(key, pair));
                  logdbg("Size of the streamList after inserting another stream: " <<  streamMapRef2->size());
              }
              else if (std::strcmp(CameraTitle.c_str(), IPCAM_THREE) == 0)
              {
                  rtspManagerRef->setActiveCamNum(3);
                  logdbg("Size of the streamList before inserting another stream: " <<  streamMapRef3->size());
                  streamMapRef3->emplace(std::make_pair(key, pair));
                  logdbg("Size of the streamList after inserting another stream: " <<  streamMapRef3->size());
              }         
              logdbg("---------------------------------------------");
        
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
             //mutex.unlock();
             }  
            break;
    }
    logdbg("Leaving gst_player::connectBlock.......");
}

StreamManager::StreamManager()
{
  logdbg("Entering StreamManager constructor.......");
  logdbg("Entering StreamManager constructor.......");
}


StreamManager::~StreamManager()
{
   logdbg("Entering StreamManager destructor.......");
    mutex.lock();
    for (auto it = streamMapRef->begin(); it != streamMapRef->end();)
    {
            it = streamMapRef->erase(it);
    }
    mutex.unlock();
   logdbg("Entering StreamManager destructor.......");
}

ApiStatus StreamManager::connectToStream(CamParamsEncryptionRef camAuthRef,
                                         ConnectedCallBackFunc  streamStarted,
                                         ErrorCallBackFunc      streamError,
                                         StreamType             streamType)
{
    //mutex.lock();
    callCount++;
    logdbg("Entering StreamManager::connectToH264Stream.......");
    logdbg("Placing mutext guard for streamList!");
    if (callCount >= 1 && callCount <= 3)
    {
        if (playerRef1 == nullptr)
        {
            playerRef1 = std::make_shared<gst_player>();
            streamMapRef1 = std::make_shared<VideoStreamMap>(*streamMapRef);
            playerRef1->camAuthRef    = camAuthRef;
            playerRef1->streamStarted = streamStarted;
            playerRef1->streamError   = streamError;
            playerRef1->streamType    = streamType;
            playerRef1->play(callCount);
        } else if (playerRef2 == nullptr)
        {
            playerRef2 = std::make_shared<gst_player>();
            streamMapRef2 = std::make_shared<VideoStreamMap>(*streamMapRef);
            playerRef2->camAuthRef    = camAuthRef;
            playerRef2->streamStarted = streamStarted;
            playerRef2->streamError   = streamError;
            playerRef2->streamType    = streamType;
            playerRef2->play(callCount);
        } else if (playerRef3 == nullptr)
        {
            playerRef3 = std::make_shared<gst_player>();
            streamMapRef3 = std::make_shared<VideoStreamMap>(*streamMapRef);
            playerRef3->camAuthRef    = camAuthRef;
            playerRef3->streamStarted = streamStarted;
            playerRef3->streamError   = streamError;
            playerRef3->streamType    = streamType;
            playerRef3->play(callCount);
        } 
    }
    else
    {
        logerr() << "Only Four active IP Cams are supported";
        ApiState = ApiStatus::FAIL;
    } 
  //mutex.unlock();
   logdbg("Leaving StreamManager::connectToH264Stream.......");
   return ApiState;
}   


ApiStatus StreamManager::disconnectStream(CamParamsEncryptionRef camAuthRef)
{
   logdbg("Entering StreamManager::disconnectStream.......");
  
   logdbg("Removing stream item from list as thw objects and its thread is dead.......");
   std::string key = "";
   key.assign( camAuthRef->base64_decode( camAuthRef->getCameraGuid() ));
   logdbg("Key to lookup is CameraGuid only: " << key);
   // find active ovjects
   logdbg("Lookup up key:  " << key);
   if (std::strcmp(camAuthRef->base64_decode(camAuthRef->getCameraTitle()).c_str(),  IPCAM_ONE) == 0)
   { 
     if (streamMapRef1)
      {
      logdbg("Size of StreamMap is : " << streamMapRef1->size());
      auto search = streamMapRef1->find(key);
      if (search == streamMapRef1->end())
      { 
        logdbg("No stream found to kill for key " << key);
        ApiState = ApiStatus::FAIL;
        return ApiState;
      }
      findAndKillStream(key, search->second);
     } 
   } else if (std::strcmp(camAuthRef->base64_decode(camAuthRef->getCameraTitle()).c_str(), IPCAM_TWO) == 0)
   {
      if (streamMapRef2)
      {
        logdbg("Size of StreamMap is : " << streamMapRef2->size());
        auto search = streamMapRef2->find(key);
        if (search == streamMapRef2->end())
        { 
          logdbg("No stream found to kill for key " << key);
          ApiState = ApiStatus::FAIL;
          return ApiState;
        }
        findAndKillStream(key, search->second);
      }
    } else if (std::strcmp(camAuthRef->base64_decode(camAuthRef->getCameraTitle()).c_str(), IPCAM_THREE) == 0)
    {
      if (streamMapRef3)
      {  
        logdbg("Size of StreamMap is : " << streamMapRef3->size());
        auto search = streamMapRef3->find(key);
        if (search == streamMapRef3->end())
        { 
          logdbg("No stream found to kill for key " << key);
          ApiState = ApiStatus::FAIL;
          return ApiState;
        }
        findAndKillStream(key, search->second);
     }
    }

   logdbg("Leaving StreamManager::disconnectStream.......");
   //mutex.unlock();
   ApiState = ApiStatus::OK;
   return ApiState;
}


void StreamManager::findAndKillStream(std::string key, std::pair<RtspManagerRef, MjpegManagerRef>  search)
{
   RtspManagerRef rtsp;
   MjpegManagerRef mjpeg;
         // the first item is the key the second s an std::pair
   rtsp  = search.first;
   mjpeg = search.second;
          
         if (rtsp->getActiveCamNum()  == 1)
         {
             logdbg("Killing Stream for Camera One.......");
             rtsp->cleanUp();
             playerRef1->killCam(1);
             logdbg("----------------------------------------------------------------");
             logdbg("Cleaning up the RtspManager object associated with this thread");
             logdbg("Cleaning up the streamlist map");
             logdbg("Removing the key object");
             callCount =  streamMapRef1->size();
             logdbg("Number of streams before removal is  : " <<  callCount);
             auto map_it = streamMapRef1->begin();
             while (map_it != streamMapRef1->end())
             {  
                if (map_it->first == key)
                {
                    streamMapRef1->erase(map_it++);
                }
                else
                {
                    ++map_it;
                }  
             }  
            callCount =  streamMapRef1->size();
            logdbg("Number of streams Now is : " <<  callCount);
            playerRef1.reset();
            streamMapRef1.reset();
         }
         else if  (rtsp->getActiveCamNum() == 2)
         {
            logdbg("Killing Stream for Camera Two.......");
            rtsp->cleanUp();
            playerRef2->killCam(2);
            logdbg("----------------------------------------------------------------");
            logdbg("Cleaning up the RtspManager object associated with this thread");
            logdbg("Cleaning up the streamlist map");
            logdbg("Removing the key object");
            callCount =  streamMapRef2->size();
            logdbg("Number of streams before removal is  : " <<  callCount);
            auto map_it = streamMapRef2->begin();
            while (map_it != streamMapRef2->end())
            {  
                if (map_it->first == key)
                {
                    streamMapRef2->erase(map_it++);
                }
                else
                {
                    ++map_it;
                }  
            }  
            callCount =  streamMapRef2->size();
            logdbg("Number of streams Now is : " <<  callCount);
            playerRef2.reset();
            streamMapRef2.reset();
            mutex.unlock();
         }
         else if (rtsp->getActiveCamNum() == 3) 
         {
            logdbg("Killing Stream for Camera Three.......");
            rtsp->cleanUp();
            playerRef3->killCam(3);
            logdbg("----------------------------------------------------------------");
            logdbg("Cleaning up the RtspManager object associated with this thread");
            logdbg("Cleaning up the streamlist map");
            logdbg("Removing the key object");
            callCount =  streamMapRef3->size();
            logdbg("Number of streams before removal is  : " <<  callCount);
            auto map_it = streamMapRef3->begin();
            while (map_it != streamMapRef3->end())
            {  
              if (map_it->first == key)
              {
                streamMapRef3->erase(map_it++);
              }
              else
              {
                ++map_it;
              }
            }    
            callCount =  streamMapRef3->size();
            logdbg("Number of streams Now is : " <<  callCount);
            playerRef3.reset();
            streamMapRef3.reset();  
            mutex.unlock();
        } 
        ApiState = ApiStatus::OK;
       logdbg("----------------------------------------------------------------");
}




