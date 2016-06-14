 	//
//  RtspManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include <foundation/foundation.hpp>
#include "StreamManager.hpp"
#include "RtspManager.hpp"
#include "Common.hpp"
#include "CamParamsEncryption.hpp"
#include "CamNotificationDefs.hpp"
#include <memory>

using namespace Jetpack::Foundation;

int RtspManager::callCount    = 0;


RtspManager::RtspManager(std::string cameraTitle)
:  IPStreamManager()
{
    logdbg("***************************************");
    logdbg("Entering RtspManager constructor.......");
    std::unique_lock<std::mutex> guard(mutex, std::defer_lock);
    guard.lock();
    callCount++;
    dataRef = std::make_shared<RtspData>();
    dataRef->errorHandlerRef = std::make_shared<StreamErrorHandler>();
    dataRef->cameraTitle = cameraTitle;
    logdbg("Creating a RtspManager Object to hold the videopipeline for  " << cameraTitle);
    gst_init (0, nullptr);
    guard.unlock();
  
    logdbg("Leaving RtspManager constructor.......");
    logdbg("***************************************");
}

RtspManager::~RtspManager()
{
    logdbg("***************************************");
    logdbg("Entering RtspManager destructor.......");
    std::unique_lock<std::mutex> guard(mutex, std::defer_lock);
    guard.lock();
    if (dataRef)
    {    
        
        dataRef->errorHandlerRef.reset();
        dataRef->errorHandlerRef = nullptr;
        dataRef.reset();
        dataRef = nullptr;
    }
    guard.unlock();
    logdbg("Leaving RtspManager destructor.......");
    logdbg("***************************************");
}

bool RtspManager::isStream(CamParamsEncryptionRef authCamRef)
{
    // look at trhe cameraGuids
   std::string tempGuid =  authCamRef->base64_decode( authCamRef->getCameraGuid() );
   if (std::strcmp(tempGuid.c_str(), cameraGuid.c_str()) == 0)
        return true;
    else
        return false;
}

ApiStatus RtspManager::connectToIPCam( CamParamsEncryptionRef camAuthRef)
{
    logdbg("***************************************");
    logdbg("Entering connectToIPCam.......");
    std::unique_lock<std::mutex> guard(mutex, std::defer_lock);
    guard.lock();

    dataRef->instance = this;

    GstRTSPResult result;
    GstRTSPAuthMethod method = GST_RTSP_AUTH_DIGEST;
    assignAuth(camAuthRef);
    cameraGuid =   authCamRef->base64_decode(crypto_cameraGuid ).c_str();
    cakeStreamingUrl =  "rtp://127.0.0.1";
    cameraStatus = "disconnected";

    connection_info.user        = strdup(camAuthRef->base64_decode(crypto_userName).c_str());
    connection_info.passwd     = strdup(camAuthRef->base64_decode(crypto_password).c_str());
    connection_info.host        = strdup(camAuthRef->base64_decode(crypto_host).c_str());
    connection_info.port        = atoi(camAuthRef->base64_decode(crypto_port).c_str());
    connection_info.abspath    = strdup(camAuthRef->base64_decode(crypto_absPath).c_str());
    connection_info.query       = strdup(camAuthRef->base64_decode(crypto_queryParms).c_str());
    
    logdbg("Connection to IPCamera	");
    logdbg("Host is: " <<   connection_info.host );
    logdbg("Port is: " <<   std::to_string(connection_info.port) );
    logdbg("Additional Path: " << connection_info.abspath );
    logdbg("User name:  "      <<  connection_info.user );
    logdbg("Camera Guid: "     <<  cameraGuid);

    dataRef->cameraGuid            = cameraGuid;
    dataRef->cakeStreamingUrl      = cakeStreamingUrl;
    dataRef->cameraStatus          = cameraStatus;

    result = gst_rtsp_connection_create(&connection_info, &dataRef->connection);
    if (result != GST_RTSP_OK)
    {
         dataRef->cameraStatus          = cameraStatus;
        return fatalApiState("RtspManager::gst_rtsp_connection_create failed!");
    }
    result = gst_rtsp_connection_set_auth(dataRef->connection,
                                          method,
                                          connection_info.user,
                                          connection_info.passwd  );
    if (result != GST_RTSP_OK)
    {
        return errorApiState("RtspManager::gst_rtsp_connection_set_auth failed!");
    }

    GTimeVal time;
    time.tv_sec = 30;
    time.tv_usec = 3000000;
    
    result = gst_rtsp_connection_connect(dataRef->connection, &time);
    if (result != GST_RTSP_OK)
   {
        return fatalApiState("RtspManager::gst_rtsp_connection_connect failed!");
    }
    dataRef->connected = true;
     // setting the connection_url
     connection_url = "rtsp://";
     if (connection_info.user != nullptr)
         connection_url.append(connection_info.user);
     else
        logdbg("Warning UserName is NULL in RtspManager");

     connection_url.append(":");
     if (connection_info.passwd != nullptr)
      connection_url.append(connection_info.passwd);
     else
         logdbg("Warning passwd is NULL in RtspManager");
     connection_url.append("@");

     if (connection_info.host != nullptr)
       connection_url.append(connection_info.host);
     else
         logdbg("Warning host is NULL in RtspManager");
     connection_url.append(":");

     if (connection_info.port != 0)
        connection_url.append( std::to_string(connection_info.port) );
     else
       logdbg("Warning port is ZERO in RtspManager");

     if (connection_info.abspath != nullptr)   
         connection_url.append(connection_info.abspath);
     else
         logdbg("Warning abspath is NULL in RtspManager");

     if (connection_info.query != nullptr)
         connection_url.append(connection_info.query);
     else
       logdbg("Warning query is NULL in RtspManager");

    logdbg("Setting the connection url");
  
    dataRef->connectionUrl =  new  gchar[connection_url.length() + 1];
    std::strncpy( dataRef->connectionUrl ,  connection_url.c_str(), connection_url.length());
    dataRef->connectionUrl[connection_url.length()] = '\0';

    logdbg("----------------------------------------------------------------");
    logdbg("Setting the camera stuff into a data structure to pass around");
    logdbg("camera Title: "        << dataRef->cameraTitle);
    logdbg("cameraGuid is: "       << dataRef->cameraGuid);
    logdbg("cakeStreamingUrl is: " << dataRef->cakeStreamingUrl);
    logdbg("cameraStatus is: "     << dataRef->cameraStatus);
    logdbg("Connection url is: "   << dataRef->connectionUrl);
    logdbg("----------------------------------------------------------------");

    logdbg("Connection url set!");
    ApiState = testConnection();
    if (ApiState == ApiStatus::OK)
    {
        logdbg("----------------------------------------------------------------");
        logdbg("Test TCP/IP connection to IP Camera successful!");
        logdbg("----------------------------------------------------------------");
    }
    cameraStatus = "connected";
    dataRef->cameraStatus          = cameraStatus;
    guard.unlock();

    logdbg("Exiting connectToIPCam.......");
    logdbg("***************************************");
    return ApiState;
}

ApiStatus RtspManager::testConnection()
{
    /*
    dataRef->readSocket = gst_rtsp_connection_get_read_socket(dataRef->connection);
    if (!dataRef->readSocket)
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_read_socket failed!");
    }
    dataRef->writeSocket =  gst_rtsp_connection_get_write_socket(dataRef->connection);
    if (!dataRef->writeSocket)
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_write_socket failed!");
    }
    */
  
    dataRef->url =  gst_rtsp_connection_get_url(dataRef->connection);
    if (dataRef->url == NULL)
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_url failed!");
    }
    // compare to ensure we dealing with the same thing
    if (std::strcmp(dataRef->url->user, connection_info.user) != 0 )
    {
        return errorApiState(" RtspManager::gst_rtsp_connection_get_url failed for username!");
    }
    if (std::strcmp(dataRef->url->passwd , connection_info.passwd) != 0 )
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_url failed for password!");
    }
    if (std::strcmp(dataRef->url->host,  connection_info.host) != 0 )
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_url failed for host!");
    }
    if (dataRef->url->port !=   connection_info.port)
    {
        return errorApiState("RtspManager::gst_rtsp_connection_get_url failed for host!");
    }
  
    logdbg("Freeing the test connection........");
    // free the test client connection objects
    gst_rtsp_connection_close(dataRef->connection);
    gst_rtsp_connection_free(dataRef->connection);
    return ApiState;
}

void   RtspManager::reportFailedConnection()
{
    dataRef->errorHandlerRef->errorMsg = "Cam Unfound and unable to connect";
    dataRef->errorHandlerRef->category = ErrorCategoryDetected::UNKNOWN;
    dataRef->errorHandlerRef->reported  = ErrorCategoryReported::CAM_DISCOVERY_FAILED;
    logdbg("--------------------------------------------------");
    logdbg("Sending onError notification: ");
    logdbg("camera Title: "        << dataRef->cameraTitle);
    logdbg("cameraGuid is: "       << dataRef->cameraGuid);
    logdbg("cakeStreamingUrl is: " << dataRef->cakeStreamingUrl);
    logdbg("cameraStatus is: "     << dataRef->cameraStatus);
    logdbg("Connection url is: "   << dataRef->connectionUrl);
    logdbg("----------------------------------------------------------------");
    Notification::UserInfo info;
    info[IPCamUnrecoverableError] = Value::Create(dataRef->cameraStatus);
    info[IPCamGUID]               = Value::Create(dataRef->cameraGuid);
    info[IPCamCakeStreamingURL]   = Value::Create(dataRef->cakeStreamingUrl);
    info[IPCamStatus]             = Value::Create(dataRef->cameraStatus);
    info[IPCamSeq]                = Value::Create(dataRef->cameraTitle);   
    info[IPCamErrorCategory]      = Value::Create("Cam Auth failed");
    NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));

    logdbg("-----------------------------------------");
    logdbg("Calling the onError callback");
    logdbg("Camera Title is: "     << dataRef->cameraTitle);
    logdbg("Camara Guid is: "      << dataRef->cameraGuid.c_str());
    logdbg("Camara Error Msg is: " << dataRef->errorHandlerRef->errorMsg.c_str());
    dataRef->streamErrorCB(strdup(dataRef->cameraTitle.c_str()));
    logdbg("-----------------------------------------");
}

ApiStatus RtspManager::makeElements()
{
    if (!dataRef->connected || ApiState == ApiStatus::FATAL_ERROR)
        return ApiState;
    logdbg("***************************************");
    logdbg("Entering makeElements.......");
    if (createElements() == ApiStatus::OK)
    {
        setElementsProperties();
    }
    logdbg("Leaving makeElements.......");
    logdbg("***********************************");
    return ApiState;
}

ApiStatus RtspManager::setupPipeLine()
{
    if (!dataRef->connected || ApiState == ApiStatus::FATAL_ERROR)
        return ApiState;
    
    logdbg("***************************************");
    logdbg("Entering setupPipeLine.......");
  
     logdbg("Adding items to the video pipline.......");
    if (addElementsToBin() == ApiStatus::OK)
    {
        linkElements();
        addCallbacks();
    }
   logdbg("Exiting setupPipeLine.......");
   logdbg("***************************************");
    return ApiState;
}

ApiStatus RtspManager::startLoop()
{
    if (!dataRef->connected || ApiState == ApiStatus::FATAL_ERROR)
        return ApiState;
    
    logdbg("***************************************");
    logdbg("Entering startLoop.......");
    logdbg("Creating the main loop...");
    std::lock_guard<std::mutex> lock(data_mutex);

    // add a signal on the bus for messages
    dataRef->bus = gst_element_get_bus( dataRef->pipeline);
    dataRef->msg = gst_bus_pop_filtered(dataRef->bus, GST_MESSAGE_ANY);
    //g_signal_connect (dataRef->bus, "message",  G_CALLBACK (RtspManager::bus_call), &dataRef);
    gst_bus_set_sync_handler(dataRef->bus,
                              &RtspManager::busSyncHandler,
                              &dataRef,  
                              &RtspManager::destroyNotify);

    //gst_bus_add_signal_watch_full (dataRef->bus, G_PRIORITY_DEFAULT);
    // Start playing
    GstStateChangeReturn statechange = gst_element_set_state (dataRef->pipeline, GST_STATE_PLAYING);
    if (statechange == GST_STATE_CHANGE_FAILURE)
    {
        return errorApiState("RtspManager::Error calling gst_element_set_state()");
    }
    // this might be too sounds ad the videopipiline is not setup yet
    // but teh time the message is posted and the other app has read it the stream should be running
    if (dataRef)
    {    
        logdbg("---------------------------------------------");
        logdbg("Calling the connected() callback!!!!");
        logdbg("camera Guid is:      "  <<  dataRef->cameraGuid);
        logdbg("cakeStreamingURL is: "  <<  dataRef->cakeStreamingUrl);
        logdbg("IPCam status is :    "  <<  dataRef->cameraStatus);
                    int portNum = 0;
       if (dataRef->instance->getActiveCamNum() == 1)
          portNum = 8000;
       else if (dataRef->instance->getActiveCamNum() == 2)
           portNum = 8250;
        else if (dataRef->instance->getActiveCamNum() == 3)
            portNum = 8500;
        else if (dataRef->instance->getActiveCamNum() == 4)
            portNum = 8750;
        // now add it to the streaming url
        dataRef->cakeStreamingUrl.append(":");  
        dataRef->cakeStreamingUrl.append( std::to_string(portNum));
        
        // send out notifications
        logdbg("--------------------------------------------------");
        logdbg("Sending onConnected notification: ");
        logdbg("camera Title: "        << dataRef->cameraTitle);
        logdbg("cameraGuid is: "       << dataRef->cameraGuid);
        logdbg("cakeStreamingUrl is: " << dataRef->cakeStreamingUrl);
        logdbg("cameraStatus is: "     << dataRef->cameraStatus);
        logdbg("Connection url is: "   << dataRef->connectionUrl);
        logdbg("----------------------------------------------------------------");

        Notification::UserInfo info;
        info[IPCamConnectionSuccess] = Value::Create(dataRef->cameraStatus);
        info[IPCamGUID]              = Value::Create(dataRef->cameraGuid);
        info[IPCamCakeStreamingURL]  = Value::Create(dataRef->cakeStreamingUrl);
        info[IPCamStatus]            = Value::Create(dataRef->cameraStatus);
        NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));
        logdbg("Connected Notification for " << dataRef->cameraTitle  << " sent!");
        dataRef->streamConnectionCB(strdup(dataRef->cameraTitle.c_str()));
        logdbg("connected() callback finished?");  
        logdbg("----------------------------------------------");
    } else {
        logdbg("No access to the data structure cannot call the connected() callback!");
    }

    logdbg("Exiting startLoop.......");
    logdbg("***************************************");
    return ApiState;
}

GstBusSyncReply RtspManager::busSyncHandler(GstBus *bus,
                            GstMessage *msg,
                            gpointer user_data)
{
    RtspData*    ptr = (RtspData*)user_data;
    if (ptr == NULL)
    {
       logdbg("Raw Pointer is NULL for gpointer in  RtspManager::busSyncHandler!");
        return GST_BUS_ASYNC;
    }
    if (ptr)
    {
        ptr->getptr();
    }
    auto derived = std::make_shared<RtspData>();
    auto sameDerived = std::dynamic_pointer_cast<RtspData>(
        derived->shared_from_this()
    );
    logdbg("For this camera instance data: " <<  sameDerived->cameraTitle);      
    processMsgType(bus, msg, sameDerived);         
    return GST_BUS_DROP;
}

void RtspManager::destroyNotify(gpointer data)
{
   //cleanUp();
}

ApiStatus RtspManager::cleanUp()
{
 
    //gst_bus_remove_signal_watch(dataRef->bus);
  
  gst_element_set_state (dataRef->pipeline, GST_STATE_NULL);

    if (dataRef->msg)
        gst_message_unref (dataRef->msg);
    if (dataRef->bus)
        gst_object_unref (dataRef->bus);
    if (dataRef->queue1)
        gst_object_unref (dataRef->queue1);
    if (dataRef->queue2)
        gst_object_unref (dataRef->queue2);
    // free the pipeline
    if (dataRef->pipeline)
        gst_object_unref (dataRef->pipeline);
    if (dataRef->main_loop)
        gst_object_unref(dataRef->main_loop);
    if (dataRef->context)
        gst_object_unref(dataRef->context);
    if (dataRef->rtspsrc)
        gst_object_unref(dataRef->rtspsrc);
    if (dataRef->rtph264depay)
        gst_object_unref(dataRef->rtph264depay);
    if (dataRef->mpegtsmux)
        gst_object_unref(dataRef->mpegtsmux);
    if (dataRef->rtpmp2tpay)
        gst_object_unref(dataRef->rtpmp2tpay);
    if (dataRef->udpsink)
        gst_object_unref(dataRef->udpsink);

     if (dataRef->connectionUrl)
         g_free(dataRef->connectionUrl);

    // now remove signals
     ApiState =  removeCallbacks();
    free(connection_info.user);
    free(connection_info.passwd);
    free(connection_info.host);
    free(connection_info.abspath);
    free(connection_info.query);
    
    return ApiState;
}

ApiStatus RtspManager::createElements()
{
    logdbg("***************************************");
    logdbg("Enter createElements");
 
    dataRef->pipeline     = gst_pipeline_new("pipeline");
    dataRef->rtpbin       =  gst_element_factory_make ( "rtpbin", "rtpbin" );
    dataRef->rtspsrc      = gst_element_factory_make ("rtspsrc", "source");
    dataRef->queue1          = gst_element_factory_make("queue", "queue1");
    dataRef->rtph264depay = gst_element_factory_make("rtph264depay", "decoder");
    dataRef->queue2          = gst_element_factory_make("queue", "queue2");
    dataRef->mpegtsmux    = gst_element_factory_make("mpegtsmux", "mpegtsmux");
    dataRef->rtpmp2tpay   = gst_element_factory_make ("rtpmp2tpay", "rtpmp2tpay");
   
    dataRef->udpsink      = gst_element_factory_make ("udpsink", "sink");
    
    if (dataRef->pipeline)
    {
        logdbg("pipeline element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("PipeLine could not be created!");
    }
    
    if( dataRef->rtpbin)
    {
        logdbg("rtpbin element created for " <<  dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("Rtpbin could not be created");
    }
 
    if (dataRef->rtspsrc)
    {
        logdbg("rtspsrc element created for " <<  dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("rtspsrc element could not be created!");
    }

    if (dataRef->queue1)
    {
        logdbg("queue1 element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("queue1 element could not be created!");
    }
    
    if (dataRef->rtph264depay)
    {
        logdbg("rtph264depay element created for " <<  dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("rtph264depay element could not be created!");
    }
    
    if (dataRef->queue2)
    {
        logdbg("queue2 element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("queue2 element could not be created!");
    }
    
    if (dataRef->mpegtsmux)
    {
        logdbg("mpegtsmux element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("mpegtsmux element could not be created!");
    }
    
    if (dataRef->rtpmp2tpay)
    {
         logdbg("rtpmp2tpay element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("rtpmp2tpay element could not be created!");
    }
   
    if (dataRef->udpsink )
    {
        logdbg("udpsink element created for " << dataRef->cameraTitle);
    }
    else
    {
        return fatalApiState("udpsink element could not be created!");
    }

    logdbg("Leaving createElements");
    logdbg("***************************************");
    return ApiState;
}

ApiStatus  RtspManager::setElementsProperties()
{
    logdbg("***************************************");
    
    logdbg("Entering setElementsProperties");
    g_object_set( G_OBJECT (dataRef->rtpbin),
                 "name",         "rtpbin",
                NULL);
    // setting properties on rtspsrc
    logdbg("Setting url connection:");
    logdbg(  connection_url.c_str() );
    g_object_set( G_OBJECT (dataRef->rtspsrc),
                 "location",            connection_url.c_str(),
                 "ntp-sync",            FALSE,
                 "async-handling",    FALSE,
                 "do-retransmission", FALSE,
                 "user-agent",         USER_AGENT,
                 "latency",             RTSP_LATENCY,
                 "buffer-mode",        RTSP_BUFFER_MODE,
                 "rtp-blocksize",       RTSP_RTP_BLOCKSIZE,
                 "protocols",           GST_RTSP_LOWER_TRANS_TCP,
                 "debug",              TRUE,
                 "retry",                 30,
                 "do-rtcp",              TRUE,
                 "do-rtsp-keep-alive", FALSE,
                 "max-rtcp-rtp-time-diff", 1000,
                 "ntp-time-source",    0,
                 "short-header",       FALSE,
                 NULL);

    
    g_object_set( G_OBJECT(dataRef->queue1),
                 "max-size-buffers", 2,
                 "max-size-bytes", 20000,
                 "leaky",    1,
                 NULL);
                 
    g_object_set( G_OBJECT(dataRef->queue2),
                 "max-size-buffers", 2,
                  "max-size-bytes", 20000,
                 "leaky",    1,
                 NULL);

    int portNum;
    logdbg("portNum before camera calculation");
    if (getActiveCamNum() == 1)
        portNum = 8000;
    else if (getActiveCamNum() == 2)
        portNum = 8250;
    else if (getActiveCamNum() == 3)
        portNum = 8500;
    else if (getActiveCamNum() == 4)
        portNum = 8750;
    else {
        return fatalApiState("activeCamNum must be between 1 and 4");
    }
    logdbg(" UDP streaming port based on Cameras connected is: " << std::to_string(portNum));

    g_object_set( G_OBJECT (dataRef->udpsink),
                 "host",        "127.0.0.1",
                 "port",        portNum,
                 "sync",        FALSE,
                 "async",       FALSE,
                 NULL);

    logdbg("Cake box streaming url is : " << "127.0.0.1:" <<  std::to_string(portNum) << " for " <<  dataRef->cameraTitle);
    logdbg("Leaving setElementsProperties");
    logdbg("***************************************");
    return ApiState;
}

ApiStatus RtspManager::addElementsToBin()
{
    logdbg("***************************************");
    
    logdbg("Entering addElementsToBin for " << dataRef->cameraTitle);
   
     if (gst_bin_add(GST_BIN(dataRef->rtpbin),   dataRef->rtspsrc ))
    {
        logdbg("rtspsrc added to rtpbin " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add rtspsrc to rtpbin ");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin), dataRef->queue1))
    {
        logdbg("queue1 added to rtpbin " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add queue1 to rtpbin");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin),   dataRef->rtph264depay ))
    {
        logdbg("rtph264depay added to rtspbin " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add rtph264depay to rtpbin");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin), dataRef->queue2))
    {
        logdbg("queue2 added to tpbin for " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add queue2 to rtpbin ");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin), dataRef->mpegtsmux ))
    {
        logdbg("rmpegtsmux added to rtspbin for " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add rmpegtsmux to rtpbin!");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin),   dataRef->rtpmp2tpay ))
    {
        logdbg("rtpmp2tpay added to rtspbin for " << dataRef->cameraTitle);
    }
   else
    {
        return errorApiState( "Unable to add rtpmp2tpay to rtpbin");
    }
    
    if (gst_bin_add(GST_BIN(dataRef->rtpbin),   dataRef->udpsink ))
    {
        logdbg("udpsink added to rtspbin for " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add udpsink to rtpbin");
    }

    
    if (gst_bin_add(GST_BIN(dataRef->pipeline),   dataRef->rtpbin ))
    {
        logdbg("rtpbin added to pipeline for " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Unable to add rtpbin to the pipeline!");
    }

    logdbg("Leaving addElementsToBin for " <<  dataRef->cameraTitle);
    logdbg("***************************************");
    return ApiState;
}

ApiStatus  RtspManager::linkElements()
{
    logdbg("***************************************");

    logdbg("Enter linkElements");

    if (gst_element_link(dataRef->queue1, dataRef->rtph264depay))
    {
        logdbg("queue1 linked to rtph264depay...... for " <<  dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Error Linking elements queue1 to rtph264depay!");
    }
    
    if (gst_element_link(dataRef->rtph264depay, dataRef->queue2))
    {
           logdbg("rtph264depay linked to queue2 ..... for " << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Error Linking elements rtph264depay to queue2!");
    }

    
    if (gst_element_link(dataRef->queue2, dataRef->mpegtsmux))
    {
        logdbg("queue2 linked to mpegtsmux ..... for " <<  dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Error Linking elements queue2 to mpegtsmux!");
    }
    
    if (gst_element_link(dataRef->mpegtsmux, dataRef->rtpmp2tpay))
    {
        logdbg("mpegtsmux linked to rtpmp2tpay ..... for " <<  dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Error Linking elements mpegtsmux to rtpmp2tpay!");
    }
    
    if (gst_element_link(dataRef->rtpmp2tpay, dataRef->udpsink))
    {
        logdbg("rtpmp2tpay linked to udpsink ..... for "  << dataRef->cameraTitle);
    }
    else
    {
        return errorApiState("Error Linking elements rtpmp2tpay to udpsink!");
    }
    logdbg("Leaving  linkElements");
    logdbg("***************************************");
    return ApiState;
}

ApiStatus RtspManager::addCallbacks()
{
    logdbg("***************************************");
  

    logdbg("Adding  callbacks  for " << dataRef->cameraTitle);
    g_signal_connect (dataRef->rtpbin, "pad-added", G_CALLBACK (RtspManager::rtpbin_pad_added_cb), &dataRef);
    // add dynamic pads to connect them when added
    logdbg("Adding Rtspsrc  callbacks......");
    g_signal_connect (dataRef->rtspsrc,"pad-added",
                      G_CALLBACK(RtspManager::rtspsrc_pad_added_cb),  &dataRef);
    g_signal_connect (dataRef->rtspsrc, "pad-removed",
                      G_CALLBACK(RtspManager::rtspsrc_pad_removed_cb), &dataRef);
    g_signal_connect (dataRef->rtspsrc, "no-more-pads",
                      G_CALLBACK(RtspManager::rtspsrc_no_more_pads_cb), &dataRef);
 
    logdbg("Leaving addCallbacks().....");
    logdbg("***************************************");
    return ApiState;
}

ApiStatus RtspManager::removeCallbacks()
{
    //g_signal_handlers_disconnect_by_data(dataRef->rtpbin, &dataRef);
    g_signal_handlers_disconnect_by_data(dataRef->rtspsrc, &dataRef);
    return ApiState;
}

void RtspManager::on_pad_added_cb (GstElement *element, GstPad *pad, RtspDataRef  appRef)
{
    GstPad *sinkpad;
    GstElement *decoder = appRef->rtph264depay;
    /* We can now link this pad with the rtsp-decoder sink pad */
    logdbg ("Dynamic pad created, linking source/demuxer");
    sinkpad = gst_element_get_static_pad (decoder, "sink");
    gst_pad_link (pad, sinkpad);
    gst_object_unref (sinkpad);
}

void RtspManager::rtpbin_pad_added_cb(GstElement *rtpbin, GstPad  *pad, RtspDataRef appRef)
{
    logdbg("---------------------------------------------------------------------------------");
    logdbg("Entering  RtspManager::rtpbin_pad_added_cb ................");
    
    gchar *dynamic_pad_name;
    dynamic_pad_name = gst_pad_get_name (pad);
    logdbg("New payload on pad: " << GST_PAD_NAME(pad); );
    logdbg("Getting send_rtp_sink_0 element");
    GstPad* srcPad1     = gst_element_get_static_pad(appRef->rtpbin, "send_rtp_sink_0");
    if (srcPad1)
    {
      logdbg("Pad from send_rtp_sink_0 element obtained from rtpbin");
    }    
    GstPad* srcPad2    = gst_element_get_static_pad(appRef->rtpbin, "send_rtp_src_0");
    if (srcPad2)    
    {
      logdbg("Pad from send_rtp_sink_0 element obtained from rtpbin");
    }

    GstElement* linkElement = (GstElement *) appRef->udpsink;
    if ( !linkElement )
    {
        logerr() << "udpsink element is null in rtpbin_pad_added_cb";
        return;
    }
        
    GstPad *sinkpad = gst_element_get_static_pad (linkElement, "sink");
    if (!sinkpad)
    {
        logerr() << "unable to get sink element in rtpbin_pad_added_cb, returning" ;
        return;
    } else {
        gst_pad_unlink (pad, sinkpad);
    }
    g_free (dynamic_pad_name);
    gst_object_unref (srcPad1);
    gst_object_unref (srcPad2);
  
    logdbg("Leaving  RtspManager::rtpbin_pad_added_cb ................");
    logdbg("---------------------------------------------------------------------------------");
}

// must connect dynamically because of how rtspsrc works!
void RtspManager::rtspsrc_pad_added_cb (GstElement* rtspsrc, GstPad* new_pad, RtspDataRef appRef)
{
    logdbg("--------------------------------------------------------------------------");
    logdbg(" Entering rtspsrc_pad_added_cb");

    GstCaps *new_pad_caps = NULL;
    GstStructure *new_pad_struct = NULL;
    const gchar *new_pad_type = NULL;
    gchar* description = NULL;
    gboolean videoRtp = false;
    gboolean audioRtp = false;
    /* Check the new pad's type */
    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
    new_pad_type   = gst_structure_get_name (new_pad_struct);
    description         = gst_caps_to_string(new_pad_caps) ;
    
    GstPadDirection direction  = gst_pad_get_direction(new_pad);
    if (direction == GST_PAD_SRC)
        logdbg("New pad direction is a SRC");
    if (direction == GST_PAD_SINK)
        logdbg("New pad direction is a SINK");
    if (new_pad_type)
        logdbg("New pad type: " <<  new_pad_type );
    if (gst_pad_is_active(new_pad))
        logdbg("New pad is active");
    else
        logdbg("New pad is NOT active");
    
    logdbg ("Received new pad " <<  GST_PAD_NAME (new_pad) << " to "  <<  GST_ELEMENT_NAME (rtspsrc));
    logdbg( "New caps of dynamic Pad is  " << description  << "/n");
    
    gchar *dynamic_pad_name;
    dynamic_pad_name = gst_pad_get_name (new_pad);
    logdbg("New payload on pad: " << GST_PAD_NAME(new_pad); );
    std::string rtpType = description;
    std::size_t found;
    found = rtpType.find("video");
    if (found != std::string::npos)
    {
        logdbg("Video rtp stream identified!");
        videoRtp = true;
    }
    found = rtpType.find("audio");
    if (found != std::string::npos)
    {
        logdbg("Audio rtp stream identified!");
        audioRtp = true;
    }
    
    if (gst_pad_is_linked(new_pad) == FALSE)
    {
        GstElement* linkElement  = NULL;
        if (videoRtp)
             linkElement = (GstElement *) appRef->queue1;
        else if (audioRtp)
            linkElement = (GstElement *) appRef->queue2;
        
        if ( !linkElement )
        {
            logerr() << "Link element is null in rtpbin_pad_added_cb";
            return;
        }
        GstPad* sinkpad = gst_element_get_compatible_pad(linkElement, new_pad, NULL);
        if (!sinkpad)
        {
            logerr() << "unable to get compatible sink element (queue1 or 2) in rtpbin_pad_added_cb, returning" ;
            return;
        }
        if (gst_pad_can_link(new_pad , sinkpad))
        {
            logdbg("linkage is possible between this new pad and  the queue1 or queue2");
        }
        else
        {
            logdbg("linkage is NOT possible between this new pad and  queue1 or queue2!");
            g_free (dynamic_pad_name);
             g_free (description);
            logdbg("--------------------------------------------------------------------------");
        }
       logdbg("Setting the sync state of  linkElement");
        if (gst_element_sync_state_with_parent(linkElement))
        {
            logdbg("Making sure tee is in Playing State of its parent!");
            gst_element_set_state(linkElement, GST_STATE_PLAYING);
        }
       logdbg("Now linking dynamic pad to compatable pad in queue1");
        GstPadLinkReturn linkreturn = gst_pad_link (new_pad, sinkpad);
        gst_object_unref(GST_OBJECT (sinkpad));
        if (linkreturn == GST_PAD_LINK_OK)
        {
            logdbg("Dynamic Pad now linked, now link the rest of the architecture");
           if(gst_element_link_pads(appRef->rtpbin, dynamic_pad_name, appRef->rtspsrc, "sink"))
            {
                logdbg("--------------------------------------------------------------------------");
                logdbg("rtpbin linked to rtspsrc linked");
                logdbg(" Leaving rtspsrc_pad_added_cb");
                g_free (dynamic_pad_name);
                g_free (description);
                logdbg("--------------------------------------------------------------------------");
                return;
            }
            else if(gst_element_link_pads(appRef->rtspsrc, dynamic_pad_name, appRef->queue1, "sink"))
            {
                logdbg("--------------------------------------------------------------------------");
                logdbg("rtspsrc linked to queue1 linked");
                logdbg(" Leaving rtspsrc_pad_added_cb");
                g_free (dynamic_pad_name);
                g_free (description);
                logdbg("--------------------------------------------------------------------------");
                return;
            }
            else if(gst_element_link_pads(appRef->rtpbin, dynamic_pad_name, appRef->udpsink, "sink"))
            {
                logdbg("--------------------------------------------------------------------------");
                logdbg("Pad for udpsink linked");
                logdbg(" Leaving rtspsrc_pad_added_cb");
                g_free (dynamic_pad_name);
                g_free (description);
                logdbg("--------------------------------------------------------------------------");
                return;
            }
        }
        else
        {
            logerr() << "unable to link dynamic pad to  compatible sink element (queue1) ";
        }
    }
    else
    {
        logerr() << "Dynamic Pad already linked?";
    }
    logdbg(" Leaving rtspsrc_pad_added_cb");
    logdbg("--------------------------------------------------------------------------");
}

void RtspManager::rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad* pad, RtspDataRef appRef)
{
    logdbg("New pad in rtspsrc removed callback !");
}

void RtspManager::rtspsrc_no_more_pads_cb(GstElement *rtspsrc, RtspDataRef appRef)
{
     logdbg("No_more_pads in rtspsrc callback !");
}

void RtspManager::processMsgType(GstBus *bus, GstMessage* msg, RtspDataRef appRef)
{
    std::string url;
    // gstreamer is a C lib hence the C pointers
    if (msg == NULL)
    {
       logerr() << "Msg is NULL in RtspManagerprocessMsgTyped fakesink to the pipeline!";
       logdbg("Somethign wrong Msg is NULL!");
       return;
    } 

    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_UNKNOWN:
            printMsg(msg,"GST_MESSAGE_UNKNOWN");
            break;
            
        case GST_MESSAGE_EOS: 
            printMsg(msg, " GST_MESSAGE_EOS");
            logdbg ("End of stream\n");
            //g_main_loop_quit ( appRef->main_loop );
            break;
        case GST_MESSAGE_ERROR: 
            printMsg(msg, " GST_MESSAGE_ERROR");
            if (appRef)
            {
                // set to defaults, reset by the error handler
                appRef->errorHandlerRef->errorMsg = "";
                appRef->errorHandlerRef->category = ErrorCategoryDetected::UNKNOWN;
                appRef->errorHandlerRef->reported = ErrorCategoryReported::CLEAR;
                // process the error handler
                appRef->errorHandlerRef->processErrorState(msg);

                logdbg("-----------------------------------------");
                logdbg("Sending the onError Notification");
                logdbg("Camara Guid is: "      << appRef->cameraGuid);
                logdbg("Camara Error Msg is: " << appRef->errorHandlerRef->errorMsg);
          
                Notification::UserInfo info;
                info[IPCamUnrecoverableError]   = Value::Create(appRef->errorHandlerRef->errorMsg);    
                info[IPCamGUID]                 = Value::Create(appRef->cameraGuid);
                info[IPCamSeq]                  = Value::Create(appRef->cameraTitle);
                if ( appRef->errorHandlerRef->category == ErrorCategoryDetected::UNKNOWN)
                {
                        info[IPCamErrorCategory] = Value::Create(IPCamDisoveryFailed);
                        logdbg("Camara ErrorCategory is: " << IPCamDisoveryFailed);            
                        NotificationCenter::DefaultCenter()->postNotification(  Notification::Make( IPCamNotification , NULL, info));
                }
                 else if( appRef->errorHandlerRef->category == ErrorCategoryDetected::CORE)
                {
                        info[IPCamErrorCategory] = Value::Create(IPCamMiscError);
                        logdbg("Camara ErrorCategory is: " << IPCamMiscError);            
                        NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));
                } 
                else if(appRef->errorHandlerRef->category   == ErrorCategoryDetected::LIBRARY)
                {
                        info[IPCamErrorCategory]             = Value::Create(IPCamMiscError);
                        logdbg("Camara ErrorCategory is: " << IPCamMiscError);            
                        NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));
                }
                else if (appRef->errorHandlerRef->category   == ErrorCategoryDetected::RESOURCE)
                {
                        info[IPCamErrorCategory]             = Value::Create(IPCamAuthFailed);
                        logdbg("Camara ErrorCategory is: " << IPCamAuthFailed);            
                        NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));
                } 
                else if (appRef->errorHandlerRef->category   == ErrorCategoryDetected::STREAM)
                {
                        info[IPCamErrorCategory]             = Value::Create(IPCamStreamStopped);
                        logdbg("Camara ErrorCategory is: " << IPCamStreamStopped);            
                        NotificationCenter::DefaultCenter()->postNotification(Notification::Make( IPCamNotification , NULL, info));
                }
                 else
                {
                    logdbg("Serious Error, Error category canot be NULL!");
                }
                logdbg("-----------------------------------------");
                logdbg("-----------------------------------------");
                logdbg("Calling the OnError callback");

                if (appRef->streamErrorCB)
                {    
                    appRef->streamErrorCB( strdup(appRef->cameraTitle.c_str()) );
                } 
                else 
                {
                    logdbg("Unable to call onError callback!");
                }   
                logdbg("-----------------------------------------");
            }
            else 
            {
                 logdbg("No access to apRef in GST_MESSAGE_ERROR handler");
            }
    
            break;
        case GST_MESSAGE_WARNING:
            printMsg(msg, "GST_MESSAGE_WARNING");
            break;
        case   GST_MESSAGE_INFO:
            printMsg(msg, "GST_MESSAGE_INFO");
            break;
        case GST_MESSAGE_TAG:
            printMsg(msg, "GST_MESSAGE_TAG");
            break;
        case GST_MESSAGE_BUFFERING:
            printMsg(msg, "GST_MESSAGE_BUFFERING");
            break;
        case GST_MESSAGE_STATE_CHANGED:
            {
            /* We are only interested in state-changed messages from the pipeline */
             const gchar* srcObj    = GST_MESSAGE_SRC_NAME(msg);
             GstState old_state, new_state, pending_state;
         
            if (std::strcmp(srcObj, "pipeline") == 0)
            {
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                logdbg ("Pipeline state changed from "
                            << gst_element_state_get_name (old_state)
                            <<  " to "
                            << gst_element_state_get_name (new_state));
            } 
            else if (std::strcmp(srcObj, "sink") == 0) 
            {
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                logdbg("Sink changed from "
                            << gst_element_state_get_name (old_state)
                            <<  " to "
                            << gst_element_state_get_name (new_state));
            } 
            else
            {
                logdbg(srcObj << " GST_MESSAGE_STATE_CHANGE");
            }
        }
            break;
        case GST_MESSAGE_STATE_DIRTY:
            printMsg(msg, "GST_MESSAGE_STATE_DIRTY");
            break;
        case GST_MESSAGE_STEP_DONE:
            printMsg(msg, "GST_MESSAGE_STEP_DONE");
            break;
        case GST_MESSAGE_CLOCK_PROVIDE:
            printMsg(msg, "GST_MESSAGE_CLOCK_PROVIDE");
            break;
        case GST_MESSAGE_CLOCK_LOST:
            printMsg(msg, "GST_MESSAGE_CLOCK_LOST");
            break;
        case GST_MESSAGE_NEW_CLOCK:
            printMsg(msg, "GST_MESSAGE_NEW_CLOCK");
            break;
        case GST_MESSAGE_STRUCTURE_CHANGE:
            printMsg(msg, "GST_MESSAGE_STRUCTURE_CHANGE");
            break;
        case GST_MESSAGE_STREAM_STATUS:
            printMsg(msg, "GST_MESSAGE_STREAM_STATUS");
            break;
        case GST_MESSAGE_APPLICATION:
            printMsg(msg, "GST_MESSAGE_APPLICATION");
            break;
        case GST_MESSAGE_ELEMENT:
            printMsg(msg, "GST_MESSAGE_ELEMENT");
            break;
        case GST_MESSAGE_SEGMENT_START:
            printMsg(msg, "GST_MESSAGE_SEGMENT_START");
            break;
        case GST_MESSAGE_SEGMENT_DONE:
            printMsg(msg, "GST_MESSAGE_SEGMENT_DONE");
            break;
        case GST_MESSAGE_DURATION_CHANGED:
            printMsg(msg, "GST_MESSAGE_DURATION_CHANGED");
            break;
        case GST_MESSAGE_LATENCY:
            printMsg(msg, "GST_MESSAGE_LATENCY");
            break;
        case GST_MESSAGE_ASYNC_START:
            printMsg(msg, "GST_MESSAGE_ASYNC_START");
            break;
        case GST_MESSAGE_ASYNC_DONE:
            printMsg(msg, "GST_MESSAGE_ASYNC_DONE");
            break;
        case GST_MESSAGE_REQUEST_STATE:
            printMsg(msg, "GST_MESSAGE_REQUEST_STATE");
            break;
        case GST_MESSAGE_STEP_START:
            printMsg(msg, "GST_MESSAGE_STEP_START");
            break;
        case GST_MESSAGE_QOS:
            printMsg(msg, "GST_MESSAGE_QOS");
            break;
        case GST_MESSAGE_PROGRESS:
            //printMsg(msg, "GST_MESSAGE_PROGRESS");
            break;
        case GST_MESSAGE_TOC:
            printMsg(msg, "GST_MESSAGE_TOC");
            break;
        case GST_MESSAGE_RESET_TIME:
            printMsg(msg, "GST_MESSAGE_RESET_TIME");
            break;
        case GST_MESSAGE_STREAM_START:
            printMsg(msg, "GST_MESSAGE_STREAM_START");
            break;
        case GST_MESSAGE_NEED_CONTEXT:
            printMsg(msg, "GST_MESSAGE_NEED_CONTEXT");
            break;
        case GST_MESSAGE_HAVE_CONTEXT:
              printMsg(msg, "GST_MESSAGE_HAVE_CONTEXT");
            break;
        case GST_MESSAGE_EXTENDED:
            printMsg(msg, "GST_MESSAGE_EXTENDED");
            break;
        case GST_MESSAGE_DEVICE_ADDED:
            printMsg(msg, "GST_MESSAGE_DEVICE_ADDED");
            break;
        case GST_MESSAGE_DEVICE_REMOVED:
            printMsg(msg, "GST_MESSAGE_DEVICE_REMOVED");
            break;
        case GST_MESSAGE_ANY:
            printMsg(msg, "GST_MESSAGE_ANY");
            break;
    }   
}

// Called for every message on the bus, passing the customer data as the last parm
gboolean RtspManager::bus_call (GstBus *bus, GstMessage *msg, RtspDataRef appRef)
{
    logdbg("***************************************");
    logdbg("bus call!");
    // This is frequently NULL because messages are called by thew gstreamer framework
    processMsgType( bus, msg, appRef);
    RtspManager::messageCount++;
    logdbg("***************************************");
    return TRUE;
}









