//
//  MjpegManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//
#include "StreamManager.hpp"
#include "MjpegManager.hpp"
#include "Common.hpp"
#include "CamParamsEncryption.hpp"
using namespace Jetpack::Foundation;

MjpegManager::MjpegManager()
{
    logdbg("***************************************");
    logdbg("Entering MjpegManager constructor .......");
    gst_init (0, nullptr);
    dataRef = std::make_shared<MjpegData>();
    dataRef->errorHandlerRef = std::make_shared<StreamErrorHandler>();
    logdbg("Leaving MjpegManager constructor.......");
    logdbg("***************************************");
}

MjpegManager::~MjpegManager()
{
    logdbg("***************************************");
    logdbg("Entering MjpegManager destructor .......");
    dataRef->errorHandlerRef.reset();
    dataRef->errorHandlerRef = nullptr;
    dataRef.reset();
    dataRef = nullptr;
    logdbg("Leaving MjpegManager destructor.......");
    logdbg("***************************************");
}


ApiStatus MjpegManager::connectToIPCam(CamParamsEncryptionRef camAuthRef)
{
    logdbg("*******************************************************");
    logdbg("Entering MjpegManager::connectToIPCam()");
    // this assign it to shared ptr authCamRef
    assignAuth(camAuthRef);\
    cameraGuid =   authCamRef->base64_decode(crypto_cameraGuid ).c_str();
    
    connection_url = "http://";
    connection_url.append( authCamRef->base64_decode(crypto_host).c_str() );
    connection_url.append(":");
    connection_url.append(authCamRef->base64_decode(crypto_port).c_str() );
    connection_url.append("/");
    connection_url.append("videostream.asf?");
    connection_url.append("user=");
    connection_url.append(authCamRef->base64_decode(crypto_userName).c_str() );
    connection_url.append("&pwd=");
    connection_url.append(authCamRef->base64_decode(crypto_password).c_str() );
    
    logdbg("Connection to IPCamera	");
    logdbg( "Host is            :  "  <<   authCamRef->base64_decode(crypto_host).c_str()  );
    logdbg("Port is              :   "  <<   authCamRef->base64_decode(crypto_port).c_str() );
    logdbg("Additional Path:   "  <<  authCamRef->base64_decode(crypto_absPath).c_str() );
    logdbg("User name       :   "   <<  authCamRef->base64_decode(crypto_userName).c_str() );
    logdbg("Camera Guid      :   "  << cameraGuid.c_str());
    
    
    logdbg("Setting the camera guid");
    dataRef->cameraGuid = cameraGuid;
  	ApiState = testConnection();
    logdbg("Leaving MjegManager::connectToIPCam()");
    logdbg("*******************************************************");
    return ApiState;
}

ApiStatus MjpegManager::testConnection()
{
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, connection_url.c_str() );
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    res = curl_easy_perform(curl);
    if (res  == CURLE_OK)
        ApiState = ApiStatus::OK;
    else
        ApiState = ApiStatus::FAIL;
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return ApiState;
}

ApiStatus MjpegManager::makeElements()
{
    logdbg("***************************************");
    logdbg("Entering makeElements.......");
    if (createElements() == ApiStatus::OK)
    {
        ApiState = setElementsProperties();
    }
    logdbg("Leaving makeElements.......");
    logdbg("***********************************");
    return ApiState;
}

ApiStatus MjpegManager::setupPipeLine()
{
    logdbg("***************************************");
    logdbg("Entering setupPipeLine.......");
    logdbg("Adding items to the video pipline.......");
    if (addElementsToBin() == ApiStatus::OK)
    {
        addCallbacks();
    }
    logdbg("Leaving setupPipeLine.......");
    logdbg("***********************************");
     return ApiState;
}

ApiStatus MjpegManager::startLoop()
{
    logdbg("***************************************");
    logdbg("Entering startLoop.......");
    logdbg("***************************************");
    logdbg("Creating the main loop...");
    dataRef->main_loop = g_main_loop_new (NULL, FALSE);
    // add a signal on the bus for messages
    dataRef->bus = gst_element_get_bus( dataRef->pipeline);
    dataRef->cameraGuid =  cameraGuid;
    g_signal_connect (dataRef->bus, "message",  G_CALLBACK (MjpegManager::bus_call), &dataRef);
    gst_bus_add_signal_watch_full (dataRef->bus, G_PRIORITY_DEFAULT);
    dataRef->msg = gst_bus_pop_filtered (dataRef->bus, GST_MESSAGE_ANY);
    // Start playing
    GstStateChangeReturn statechange = gst_element_set_state (dataRef->pipeline, GST_STATE_PLAYING);
    if (statechange == GST_STATE_CHANGE_FAILURE)
    {
        return errorApiState("RtspManager::Error calling gst_element_set_state()");
    }
    logdbg("Starting loop.......");
    // now start the loop
    g_main_loop_run (dataRef->main_loop);
    
    cleanUp();
    logdbg("***************************************");
    logdbg("Leaving startLoop.......");
    logdbg("***********************************");
     return ApiState;
}

ApiStatus MjpegManager::setElementsProperties()
{
    logdbg("***************************************");
    logdbg("Entering setElementsProperties.......");

    // setting properties on rtspsrc
    logdbg("Setting url connection:");
    logdbg(  connection_url.c_str() );
    CamParamsEncryption camAuth;
    
    g_object_set( G_OBJECT (dataRef->souphttpsrc),
                 "location",              connection_url.c_str(),
                 "user-agent",           USER_AGENT,
                 "automatic-redirect",  TRUE,
                 "is-live",                 TRUE,
                 "user-id",                authCamRef->base64_decode(crypto_userName).c_str() ,
                 "user=pw",              authCamRef->base64_decode(crypto_password).c_str(),
                 "timeout",                30,
                 "compress",             FALSE,
                 "keep-alive",             TRUE,
                 "retries",                  3,
                 "method" ,               "GET",
                 NULL);
    
    logdbg("Leaving setElementsProperties.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::addCallbacks()
{
    logdbg("***************************************");
    logdbg("Entering addCallbacks.......");
    g_signal_connect (dataRef->souphttpsrc, "pad-added",
                      G_CALLBACK(MjpegManager::souphttpsrc_pad_added_cb),     &dataRef);
    g_signal_connect (dataRef->souphttpsrc, "pad-removed",
                      G_CALLBACK(MjpegManager::souphttpsrc_pad_removed_cb),  &dataRef);
    g_signal_connect (dataRef->souphttpsrc, "no-more-pads",
                      G_CALLBACK(MjpegManager::souphttpsrc_no_more_pads_cb), &dataRef);
    logdbg("Leaving addCallbacks.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::removeCallbacks()
{
    logdbg("***************************************");
    logdbg("Entering removeCallbacks.......");
     g_signal_handlers_disconnect_by_data(dataRef->souphttpsrc, &dataRef);
    logdbg("Leaving removeCallbacks.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::cleanUp()
{
    logdbg("***************************************");
    logdbg("Entering cleanUp.......");
    // free the bus
    gst_element_set_state (dataRef->pipeline, GST_STATE_NULL);
    gst_bus_remove_signal_watch(dataRef->bus);
    if (dataRef->msg)
        gst_message_unref (dataRef->msg);
    if (dataRef->bus)
        gst_object_unref (dataRef->bus);
    // free the pipeline
    if (dataRef->pipeline)
        gst_object_unref (dataRef->pipeline);
    if (dataRef->main_loop)
        gst_object_unref(dataRef->main_loop);
    if (dataRef->context)
        gst_object_unref(dataRef->context);

    // now remove signals
    ApiState =  removeCallbacks();
    logdbg("Leaving cleanUp.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::createElements()
{
    logdbg("***************************************");
    logdbg("Entering createElements.......");
    dataRef->pipeline              = gst_pipeline_new("pipeline");
    dataRef->souphttpsrc          = gst_element_factory_make("souphttpsrc", "souphttpsrc");
    dataRef->tcpserver            = gst_element_factory_make("tcpserver", "tcpserver");
    dataRef->multipartdemux      = gst_element_factory_make("multipartdemux", "multipartdemux");
    dataRef->jpegdec                 = gst_element_factory_make("jpegdec", "jpegdec");
    dataRef->ffenc_mpeg4           = gst_element_factory_make("ffenc_mpeg4", "ffenc_mpeg4");
    
    if (!dataRef->pipeline)
    {
        return fatalApiState("PipeLine could not be created!");
    }
    if (!dataRef->souphttpsrc)
    {
        return fatalApiState("souphttpsrc  element could not be created!");
    }
    if (!dataRef->tcpserver)
    {
        return fatalApiState("tcpserver element could not be created!");
    }
    if (!dataRef->multipartdemux)
    {
        return fatalApiState("multipartdemux element could not be created!");
    }
    if (!dataRef->jpegdec)
    {
        return fatalApiState("jpegdec element could not be created!");
    }
    if (!dataRef->ffenc_mpeg4)
    {
        return fatalApiState("ffenc_mpeg4 element could not be created!");
    }
    logdbg("Leaving createElements.......");
    logdbg("****************************************");
    return ApiState;
}

ApiStatus  MjpegManager::addElementsToBin()
{
    logdbg("***************************************");
    logdbg("Entering addElementsToBin.......");
    // no bin
    logdbg("Leaving addElementsToBin.......");
    logdbg("****************************************");
    return ApiState;
}

// Called for every message on the bus, passing the customer data as the last parm
gboolean MjpegManager::bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    logdbg("***************************************");
    logdbg("bus call!");
    // This is frequently NULL because messages are called by thew gstreamer framework
    MjpegData* customData = (MjpegData*)data;
    processMsgType( bus, msg, customData);
    messageCount++;
    logdbg("***************************************");
    return TRUE;
}

void MjpegManager::processMsgType(GstBus *bus, GstMessage* msg, gpointer data)
{
    std::string url;
    MjpegData* pdata = (MjpegData*)data;
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
            
        case GST_MESSAGE_EOS: {
            printMsg(msg, " GST_MESSAGE_EOS");
            logdbg ("End of stream\n");
            if (pdata != NULL)
            {
                g_main_loop_quit ( pdata->main_loop );
            }
            break;
        }
        case GST_MESSAGE_ERROR: {
            printMsg(msg, " GST_MESSAGE_ERROR");
            pdata->errorHandlerRef->errorMsg = "";
            pdata->errorHandlerRef->category = ErrorCategoryDetected::UNKNOWN;
            pdata->errorHandlerRef->reported  = ErrorCategoryReported::CLEAR;
            pdata->errorHandlerRef->processErrorState(msg);
            pdata->cameraErrorMsg = pdata->errorHandlerRef->errorMsg;
            pdata->streamErrorCB();
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            printMsg(msg, " GST_MESSAGE_STATE_CHANGED");
            /* We are only interested in state-changed messages from the pipeline */
            const gchar* srcObj    = GST_MESSAGE_SRC_NAME(msg);
            if (std::strcmp(srcObj, "pipeline") == 0)
            {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                logdbg ("Pipeline state changed from "
                        << gst_element_state_get_name (old_state)
                        <<  " to "
                        << gst_element_state_get_name (new_state));
            }
            break;
        }
        case GST_MESSAGE_PROGRESS:
            printMsg(msg, "GST_MESSAGE_PROGRESS");
            break;
        case GST_MESSAGE_NEW_CLOCK:
            printMsg(msg, "GST_MESSAGE_NEW_CLOCK");
            break;
        case GST_MESSAGE_STREAM_STATUS:
            printMsg(msg, "GST_MESSAGE_STREAM_STATUS");
            break;
        case GST_MESSAGE_ELEMENT:
            printMsg(msg, "GST_MESSAGE_ELEMENT");
            break;
        case GST_MESSAGE_STREAM_START:
            printMsg(msg, "GST_MESSAGE_STREAM_START");
            logdbg ("GST_MESSAGE_ASYNC_DONE: removing the bus watch!");
            // remove the watch as you not interested in anything beyond this
            if (bus != NULL)
            {
                logdbg("Removing the bus signal watch !");
                gst_bus_remove_signal_watch(bus);
            }
            logdbg("Allocating space for the url to pass to the decoder!");
            url = "http://127.0.0.1:8000";
            if (pdata != NULL)
            {
                logdbg("Calling the connected() callback!!!!");
                pdata->cakeboxStreamingUrl = "http://127.0.0.1:8000";
                pdata->cameraStatus    = "connected";
                pdata->streamConnectionCB();
                logdbg("connected() callback finished?");
            } else {
                logdbg("No access to the data structure cannot call the connected() ca  llback!");
            }
            break;
        case GST_MESSAGE_ASYNC_DONE:
            printMsg(msg, "GST_MESSAGE_ASYNC_DONE");
            break;
        default:
            logerr() << "RtspManager::Error, something wrong should never processed this unknown messge!";
            break;
    }
}

ApiStatus  MjpegManager::linkElements()
{
    return ApiState;
}

// must connect dynamically because of how rtspsrc works!
void MjpegManager::souphttpsrc_pad_added_cb (GstElement* souphttpsrc, GstPad* pad, gpointer data)
{
    logdbg("New pad in souphttpsrc callback !");
}

void MjpegManager::souphttpsrc_pad_removed_cb (GstElement* souphttpsrc, GstPad* pad, gpointer data)
{
    logdbg("New pad in souphttpsrc removed callback !");
}

void MjpegManager::souphttpsrc_no_more_pads_cb(GstElement*  souphttpsrc, gpointer data)
{
    logdbg("No_more_pads in souphttpsrc callback !");
}

