//
//  RtspManager.cpp
//  libevartp
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#include <foundation/foundation.hpp>
#include "RtspManager.hpp"
#include <gio/gio.h>

#include <gst/gst.h>

#include <gst/rtsp/gstrtsp.h>
#include <gst/gstutils.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <gst/base/gstpushsrc.h>

#include <unistd.h>
#include <pthread.h>

#include <stdlib.h>
#include <vector>

using namespace Jetpack::Foundation;

RtspManager* RtspManager::instance = nullptr;
short RtspManager::messageCount = 0;
short  RtspManager::refCounts = 0;
std::vector<RtspManagerRef> RtspManager::instanceList = {};

RtspManager::RtspManager()
:  bus(nullptr), msg(nullptr), connection(nullptr), url(nullptr), writeSocket(nullptr),
  readSocket(nullptr), rtspWatch(nullptr) 
{
    logdbg("***************************************");
    logdbg("Entering RtspManager constructor.......");
 
    data =
    {
        nullptr,
        nullptr,
        nullptr,  nullptr , nullptr, nullptr, nullptr ,
        FALSE,
        FALSE,
        FALSE,
        FALSE,
        0,
        nullptr
    };
    connection_info =
    {
        GST_RTSP_LOWER_TRANS_TCP,
        GST_RTSP_FAM_INET,
        nullptr,
        nullptr,
        nullptr,
        88,
        nullptr,
        nullptr
    };
    logdbg("Before memset the data struture.......");
    memset (&data, 0, sizeof (data));
    logdbg("After init the data struture.......");

    logdbg("Before init the shared pointers.......");
    callbacksRef = std::make_shared<RtspManagerCallbacks>();
    logdbg("After init the shared pointers.......");
 
    gst_init (0, nullptr);
 
    logdbg("Leaving RtspManager constructor.......");
    logdbg("***************************************");
}

RtspManager::~RtspManager()
{
    
}

short  RtspManager::getRefCount()
{
    return refCounts;
}

RtspManager* RtspManager::createNewRtspManager()
{
    logdbg("***************************************");
    logdbg("Entering createNewRtspManager.......");
    refCounts++;
    instance = new RtspManager();  
    logdbg("Leaving createNewRtspManager.......");
    logdbg("***************************************");
    return instance;
    //instanceList.push_back(managerRef);
}

void RtspManager::connectToIPCam( const gchar * userName,
                                  const gchar * password,
                                  const gchar * host,
                                  guint16  port,
                                  const gchar* abspath,
                                  const gchar* query)
{
    logdbg("***************************************");
    logdbg("Entering connectToIPCam.......");
    GstRTSPResult result;
    GstRTSPAuthMethod method = GST_RTSP_AUTH_DIGEST;
    connection_info.user = strdup(userName);
    connection_info.passwd = strdup(password);
    connection_info.host = strdup(host);
    connection_info.port = port;
    connection_info.abspath = strdup(abspath);
    connection_info.query = strdup(query);
    logdbg("Connection to IPCamera");
    char debug_string[100];
    sprintf(debug_string, "Host is:   %s",   connection_info.host );
    logdbg(debug_string);
    sprintf(debug_string,"Port is: %d",       connection_info.port);
    logdbg(debug_string);
    sprintf(debug_string, "Additional Path: %s", connection_info.abspath );
    logdbg(debug_string);
    sprintf(debug_string, "User name:  %s", connection_info.user );
    logdbg(debug_string);
    result = gst_rtsp_connection_create(&connection_info, &connection);
    if (result != GST_RTSP_OK)
    {
        logerr() << "RtspManager::gst_rtsp_connection_create failed!";
    }
    result = gst_rtsp_connection_set_auth(connection,
                                          method,
                                          userName,
                                          password 
                                          );
    if (result != GST_RTSP_OK)
    {
        logerr() <<  "RtspManager::gst_rtsp_connection_set_auth failed!";
    }
    GTimeVal time;
    time.tv_sec = 30;
    time.tv_usec = 30000;
    
    result = gst_rtsp_connection_connect(connection, &time);
    if (result == GST_RTSP_OK)
    {
        logdbg("----------------------------------------------------------------");
        logdbg("Test TCP/IP connection to IP Camera successful!");
        logdbg("----------------------------------------------------------------");
        // setting the connection_url
        connection_url = "rtsp://";
        connection_url.append(connection_info.user);
        connection_url.append(":");
        connection_url.append(connection_info.passwd);
        connection_url.append("@");
        connection_url.append(connection_info.host);
        connection_url.append(":");   
        char buffer[5];
        sprintf(buffer, "%u", port);
        connection_url.append(buffer);
        connection_url.append(connection_info.abspath);
        connection_url.append(connection_info.query);
        logdbg("Setting the connection url");
        data.url = new gchar(connection_url.length() + 1); 
        std::strcpy(data.url, connection_url.c_str());
        logdbg(data.url);
        logdbg("Connection url set!");
     }
    else
    {
        logerr() << "RtspManager::gst_rtsp_connection_connect failed!";
    }

    readSocket = gst_rtsp_connection_get_read_socket(connection);
    if (readSocket == NULL)
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_read_socket failed!";
    }
    writeSocket =  gst_rtsp_connection_get_write_socket(connection);
    if (writeSocket == NULL)
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_write_socket failed!";
    }
    url =  gst_rtsp_connection_get_url((const  GstRTSPConnection *)connection);
    if (url == NULL)
    {
         logerr() << "RtspManager::gst_rtsp_connection_get_url failed!";
    }
    // compare to ensure we dealing with the same thing
    if (strcmp(url->user, connection_info.user) != 0 )
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_url failed for username!";
    }
    if (strcmp(url->passwd , connection_info.passwd) != 0 )
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_url failed for password!";
    }
    if (strcmp(url->host,  connection_info.host) != 0 )
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_url failed for host!";
    }
    if (url->port !=   connection_info.port)
    {
        logerr() << "RtspManager::gst_rtsp_connection_get_url failed for host!";
    }
    logdbg("Freeing the test connection........");
      // free the test client connection objects
    gst_rtsp_connection_close(connection);
    gst_rtsp_connection_free(connection);

    logdbg("Exiting connectToIPCam.......");
    logdbg("***************************************");
}

void RtspManager::makeElements()
{
    logdbg("***************************************");
    logdbg("Entering makeElements.......");
    data.pipeline     = gst_pipeline_new("pipeline");
    data.rtspsrc      = gst_element_factory_make ("rtspsrc", "source");
    data.rtph264depay = gst_element_factory_make ("rtph264depay", "deplay");
    data.rtph264pay   = gst_element_factory_make ("rtph264pay", "play");
    data.fakesink     = gst_element_factory_make ("fakesink", "sink");
    
    if (!data.pipeline || !data.rtspsrc || !data.rtph264depay || !data.rtph264pay || !data.fakesink)
    {
        logerr() << "RtspManager::One element could not be created!";
        return;
    }
    g_object_set( G_OBJECT (data.rtspsrc),
                 "location",      connection_url.c_str(),
                 "latency",       RTSP_LATENCY,
                 "buffer-mode",   RTSP_BUFFER_MODE,
                 "rtp-blocksize", RTSP_RTP_BLOCKSIZE,
                 "protocols",     GST_RTSP_LOWER_TRANS_TCP,
                 "debug",              TRUE,
                 "retry",              30,
                 "do-rtcp",            TRUE,
                 "do-rtsp-keep-alive", FALSE,
                 "short-header",       FALSE,
                 NULL);
   
    logdbg("Leaving makeElements......."); 
    logdbg("***************************************");
}

void RtspManager::setupPipeLine()
{
    logdbg("***************************************");
    logdbg("Entering setupPipeLine.......");
    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtspsrc ))
    {
       logerr() << "Unable to add rtspsrc to the pipeline!";
        return;
    }

    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtph264depay ))
    {
        logerr() << "Unable to add  rtph264depay to the pipeline!";
        return;
    }
  
    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtph264pay ))
    {
        logerr() << "Unable to add rtph264pay to the pipeline!";
        return;
    }

    if (!gst_bin_add(GST_BIN(data.pipeline),   data.fakesink ))
    {
        logerr() << "Unable to add fakesink to the pipeline!";
        return;
    }
   if (!gst_element_link_many (data.rtph264depay, data.rtph264pay, data.fakesink, NULL))
   {
        logerr() << "Error Linking elements";
       return;
   }
    // add dynamic pads to connect them when added
   logdbg("Adding RTSPSRC plugin callbacks......");

   g_signal_connect (data.rtspsrc,"pad-added",       G_CALLBACK(RtspManagerCallbacks::rtspsrc_pad_added_cb),   &data);
   g_signal_connect (data.rtspsrc, "pad-removed",    G_CALLBACK(RtspManagerCallbacks::rtspsrc_pad_removed_cb), &data);
   g_signal_connect (data.rtspsrc, "no-more-pads",   G_CALLBACK(RtspManagerCallbacks::rtspsrc_no_more_pads_cb), &data);

   logdbg("Exiting setupPipeLine.......");
   logdbg("***************************************");
}

void RtspManager::startLoop()
{
    logdbg("***************************************");
    logdbg("Entering startLoop.......");

    logdbg("Creating the main loop...");
    data.main_loop = g_main_loop_new (NULL, FALSE);
    //g_main_loop_get_context(data.main_loop); // not needed but keep because we need the context later
    
    // add a signal on the bus for messages
    bus = gst_element_get_bus( data.pipeline);
    logdbg("Adding bus watch callback....");
    g_signal_connect (bus, "message",  G_CALLBACK (callbacksRef->bus_call), &data);
    logdbg("Adding the bus watch listening for all messages...");
    gst_bus_add_signal_watch_full (bus, G_PRIORITY_DEFAULT);
    logdbg("Listen to all bus messages based on the filter......");
    msg = gst_bus_pop_filtered (bus, GST_MESSAGE_ANY);
    // Start playing
    logdbg("Set the pipeline to playing ......");
    GstStateChangeReturn statechange = gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    if (statechange == GST_STATE_CHANGE_FAILURE)
    {
          logerr() << "RtspManager::Error calling gst_element_set_state()";
          return;
    }
    logdbg("Starting loop.......");
    // now start the loop
    g_main_loop_run (data.main_loop);
    
    // free the bus
    logdbg("Finished loop!.........");
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    logdbg("Remove the bus watch.........");
    gst_bus_remove_signal_watch(bus);
    logdbg("Unreference the gstreamer references so they can be released.........");
    gst_message_unref (msg);
    gst_object_unref (bus);
   // free the pipeline
    gst_object_unref (data.pipeline);
    logdbg("Exiting startLoop.......");
    logdbg("***************************************");
}

void RtspManagerCallbacks::on_pad_added_cb (GstElement *element, GstPad *pad, CustomData*  data)
{
    GstPad *sinkpad;
    GstElement *decoder = data->rtph264pay;
    /* We can now link this pad with the rtsp-decoder sink pad */
    logdbg ("Dynamic pad created, linking source/demuxer");
    sinkpad = gst_element_get_static_pad (decoder, "sink");
    gst_pad_link (pad, sinkpad);
    gst_object_unref (sinkpad);
}

// must connect dynamically because of ow rtspsrc works!
void RtspManagerCallbacks::rtspsrc_pad_added_cb (GstElement* rtspsrc, GstPad* pad, CustomData* data)
{
    logdbg("New pad in rtspsrc added!");
    gchar *dynamic_pad_name;
    GstPad* new_pad = nullptr;
    dynamic_pad_name = gst_pad_get_name (pad);
    GstCaps* caps = gst_pad_get_current_caps(pad);
    if (caps && pad && rtspsrc)
    {
        new_pad = gst_element_get_compatible_pad(rtspsrc, pad, caps);
    }
    
    GstElement *linkElement = (GstElement *) data->rtph264depay;
    GstPad *sinkpad = gst_element_get_static_pad (linkElement, "sink");
    if (GST_PAD_IS_LINKED(sinkpad))
    {
        gst_pad_unlink (pad, sinkpad);
    }
    GstPadLinkReturn linkreturn = gst_pad_link (pad, sinkpad);
    gst_object_unref(GST_OBJECT (sinkpad));
    
    if (linkreturn == GST_PAD_LINK_OK)
    {
        if(gst_element_link_pads(data->rtspsrc, dynamic_pad_name, data->rtph264depay, "sink")){
            logdbg("Pad for rtph264depay linked");
            g_free (dynamic_pad_name);
            return;
        }
        else if(gst_element_link_pads(data->rtspsrc, dynamic_pad_name, data->rtph264pay, "sink")){
            logdbg("Pad for rtph264pay linked");
            g_free (dynamic_pad_name);
            return;
        }
        else if(gst_element_link_pads(data->rtspsrc, dynamic_pad_name, data->fakesink, "sink")){
            logdbg("Pad for fakesink linked");
            g_free (dynamic_pad_name);
            return;
        }
    }
}

void RtspManagerCallbacks::rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data)
{
    logdbg("New pad in rtspsrc removed !");
}

void RtspManagerCallbacks::rtspsrc_no_more_pads_cb(GstElement *rtspsrc, gpointer data)
{
     logdbg("No_more_pads in rtspsrc removed !");
}

void RtspManagerCallbacks::printMsg(GstMessage* msg, const gchar*  msgType)
{
    if (msg == NULL)
    {
        return;
    }
   
    const gchar* msgTypeName    = GST_MESSAGE_TYPE_NAME(msg);
    GstClockTime timeStamp = GST_MESSAGE_TIMESTAMP(msg);
    const gchar* srcObj        = GST_MESSAGE_SRC_NAME(msg);
    guint seqnum                 = GST_MESSAGE_SEQNUM(msg);
    
    char buffer[75];
    logdbg ("--------------------------------------------------------------------------\n");
    sprintf(buffer, "Mesage SeqNum     : %d\n",    seqnum);
    logdbg (buffer);
    sprintf(buffer, "Message type           :%s", msgType);
    logdbg (buffer);
    sprintf(buffer, "Messge type  Name : %s",   msgTypeName);
    logdbg (buffer);
    sprintf(buffer, "Time Stamp when mesage created  : %llu", timeStamp);
    logdbg (buffer);
    sprintf(buffer, "Src Object Name       : %s",   srcObj);
    logdbg (buffer);
    logdbg ("---------------------------------------------------------------------------\n");
}

void RtspManagerCallbacks::processMsgType(GstBus *bus, GstMessage* msg, CustomData* pdata)
{
   
    char* url;
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
                pdata->terminate = TRUE;
            }
            if (pdata != NULL)
            {    
                g_main_loop_quit ( pdata->main_loop );
            }
            break;
        }
        case GST_MESSAGE_ERROR: {
            printMsg(msg, " GST_MESSAGE_ERROR");

            gchar  *debug;
            GError *error;
            
            gst_message_parse_error (msg, &error, &debug);
            g_free (debug);
            
            logerr() <<  "Error: " << error->message;
            g_error_free (error);
            if (pdata)
            {    
                g_main_loop_quit(pdata->main_loop);
            }
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            printMsg(msg, " GST_MESSAGE_STATE_CHANGED");
            /* We are only interested in state-changed messages from the pipeline */
            const gchar* srcObj    = GST_MESSAGE_SRC_NAME(msg);
            if (strcmp(srcObj, "pipeline") == 0)
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
            break;
        case GST_MESSAGE_ASYNC_DONE:
            printMsg(msg, "GST_MESSAGE_ASYNC_DONE");
            logdbg ("GST_MESSAGE_ASYNC_DONE: removing the bus watch!");
            // remove the watch as you not interested in anything beyond this
            if (bus != NULL)
            {    
                logdbg("Removing the bus signal watch !");
                gst_bus_remove_signal_watch(bus);
            }
            logdbg("Allocating space for the url to pass to the decoder!");
            char buffer[50];
            url = buffer;
            std::strcpy(url,"rtp://192.168.6.49:65534");
            if (pdata != NULL) 
            {    
                logdbg("Calling the connected() callback!!!!");
                pdata->connectionCB(url);    
                logdbg("connected() callback finished?");  
            } else {
                logdbg("No access to the data structure cannot call the connected() callback!");
            }
            logdbg("Ending Camera Loop.....");
            if (pdata && pdata->main_loop)
            {    
                g_main_loop_quit( pdata->main_loop ); 
            } else {
                logdbg("No access to the data structure cannot break out of Camera loop!");
            }          
            logdbg("Camera Loop finshed. .....");
            break;
        default:
            logerr() << "RtspManager::Error, something wrong should never processed this unknown messge!";
            break;
    }   
}

// Called for every message on the bus, passing the customer data as the last parm
gboolean RtspManagerCallbacks::bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    logdbg("***************************************");
    logdbg("bus call!");
    // This is frequently NULL because messages are called by thew gstreamer framework
    CustomData* customData = (CustomData*)data;
    processMsgType( bus, msg, customData);
    RtspManager::messageCount++;
    logdbg("***************************************");
    return TRUE;
}







