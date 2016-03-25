//
//  RtspManager.cpp
//  libevartp
//
//  Created by Tony Constantinides on 3/20/16.
//
//

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
#include <string.h>
#include <pthread.h>



GstRTSPUrl RtspManager::connection_info =
{   GST_RTSP_LOWER_TRANS_TCP,
    GST_RTSP_FAM_INET,
    strdup( "tony"),
    strdup("Cyprus2016"),
    strdup("192.168.6.49"),
    88,
    strdup("/videoMain"),
    strdup("")
};

CustomData RtspManager::data =
{
    nullptr,
    nullptr,  nullptr , nullptr, nullptr, nullptr ,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    0
};

std::shared_ptr<RtspManager> RtspManager::instance = nullptr;
GstBus* RtspManager::bus  = nullptr;
GstMessage* RtspManager::msg = nullptr;
GstRTSPConnection* RtspManager::connection = nullptr;
GstRTSPUrl* RtspManager::url = nullptr;
GSocket* RtspManager::writeSocket = nullptr;
GSocket* RtspManager::readSocket = nullptr;
GstRTSPWatch* RtspManager::rtspWatch = nullptr;

RtspManager::RtspManager()
{
    memset (&data, 0, sizeof (data));
    /* Initialize GStreamer */
    gst_init (0, nullptr);
}

RtspManager::~RtspManager()
{
    
}

std::shared_ptr<RtspManager> RtspManager::createNewRtspManager()
{
    if (instance == nullptr)
    {
        std::shared_ptr<RtspManager> instance (new RtspManager());
    }
    return instance;
}

void  destroyFunc  (gpointer  data)
{
    GST_LOG("RTSP called destroy");
}

GstRTSPResult rtsp_message_received(GstRTSPWatch *watch, GstRTSPMessage *message, gpointer user_data)
{
    return GST_RTSP_OK;
}

void RtspManager:: connectToIPCam()
{
    GstRTSPResult result;
    GstRTSPAuthMethod method = GST_RTSP_AUTH_DIGEST;
    result = gst_rtsp_connection_create(&RtspManager::connection_info, &connection);
    if (result != GST_RTSP_OK)
    {
        GST_ERROR ("gst_rtsp_connection_create failed!");
    }
    result = gst_rtsp_connection_set_auth(connection,
                                          method,
                                          "tony",
                                          "Cyprus2016"
                                          );
    if (result != GST_RTSP_OK)
    {
        GST_ERROR ("gst_rtsp_connection_set_auth failed!");
    }
    GTimeVal time;
    time.tv_sec = 30;
    time.tv_usec = 30000;
    
    result = gst_rtsp_connection_connect(connection, &time);
    
    if (result != GST_RTSP_OK)
    {
        GST_ERROR("gst_rtsp_connection_connect failed!");
    }
    
    readSocket = gst_rtsp_connection_get_read_socket(connection);
    if (readSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_read_socket failed!");
    }
    writeSocket =  gst_rtsp_connection_get_write_socket(connection);
    if (writeSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_write_socket failed!");
    }
    url =  gst_rtsp_connection_get_url((const  GstRTSPConnection *)connection);
    if (url == NULL)
    {
         GST_ERROR("gst_rtsp_connection_get_url failed!");
    }
    // compare to ensure we dealing with the same thing
    if (strcmp(url->user, connection_info.user) != 0 )
    {
        GST_ERROR("gst_rtsp_connection_get_url failed for username!");
    }
    if (strcmp(url->passwd , connection_info.passwd) != 0 )
    {
        GST_ERROR("gst_rtsp_connection_get_url failed for password!");
    }
    if (strcmp(url->host,  connection_info.host) != 0 )
    {
        GST_ERROR("gst_rtsp_connection_get_url failed for host!");
    }
    if (url->port !=   connection_info.port)
    {
        GST_ERROR("gst_rtsp_connection_get_url failed for host!");
    }
    GstRTSPWatchFuncs rtsp_funcs;
    rtsp_funcs.message_received = rtsp_message_received;
  
    rtspWatch = gst_rtsp_watch_new(connection, &rtsp_funcs, NULL,  destroyFunc);
}

void RtspManager::makeElements()
{
    data.pipeline     = gst_pipeline_new("pipeline");
    data.rtspsrc      = gst_element_factory_make ("rtspsrc", "source");
    data.rtph264depay = gst_element_factory_make ("rtph264depay", "deplay");
    data.rtph264pay   = gst_element_factory_make ("rtph264pay", "play");
    data.fakesink     = gst_element_factory_make ("fakesink", "sink");
    
    if (!data.pipeline || !data.rtspsrc || !data.rtph264depay || !data.rtph264pay || !data.fakesink)
    {
        g_printerr ("One element could not be created. Exiting.\n");
    }
    g_object_set( G_OBJECT (data.rtspsrc),
                 "location",      RTSP_URI,
                 "latency",       RTSP_LATENCY,
                 "buffer-mode", RTSP_BUFFER_MODE,
                 "rtp-blocksize", RTSP_RTP_BLOCKSIZE,
                 "protocols",     GST_RTSP_LOWER_TRANS_TCP,
                 "debug",         TRUE,
                 "retry",            30,
                 "do-rtcp",         TRUE,
                 "do-rtsp-keep-alive", FALSE,
                 "short-header", FALSE,
                 NULL);
    
}

void RtspManager::setupPipeLine()
{
    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtspsrc ))
    {
      g_printerr("Unable to add  rtspsrc to the pipeline!");
    }

    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtph264depay ))
    {
        g_printerr("Unable to add  rtph264depay to the pipeline!");
    }
  
    if (!gst_bin_add(GST_BIN(data.pipeline),   data.rtph264pay ))
    {
        g_printerr("Unable to add  rtph264pay to the pipeline!");
    }

    if (!gst_bin_add(GST_BIN(data.pipeline),   data.fakesink ))
    {
        g_printerr("Unable to add  fakesink to the pipeline!");
    }
   if (!gst_element_link_many (data.rtph264depay, data.rtph264pay, data.fakesink, NULL))
   {
        g_printerr("Error Linking elements");
    }
    
   g_signal_connect (data.rtspsrc,"pad-added",  G_CALLBACK(RtspManager::rtspsrc_pad_added_cb),   &data);
    g_signal_connect (data.rtspsrc, "pad-removed",   G_CALLBACK(RtspManager::rtspsrc_pad_removed_cb), &data);
    g_signal_connect (data.rtspsrc, "no-more-pads",   G_CALLBACK(RtspManager::rtspsrc_no_more_pads_cb), &data);
}


void RtspManager::startLoop()
{
    data.main_loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_get_context(data.main_loop);
    
    bus = gst_element_get_bus( data.pipeline);
    // add a signal onthe bus for messages
    g_signal_connect (bus, "message",  G_CALLBACK (bus_call), NULL);
    gst_bus_add_signal_watch_full (bus, G_PRIORITY_DEFAULT);
    
    msg = gst_bus_pop_filtered (bus, GST_MESSAGE_ANY);
    gst_object_unref (bus);
    /* Start playing */
    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    g_main_loop_run (data.main_loop);
    
    // free the bus
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_bus_remove_signal_watch(bus);
    gst_message_unref (msg);
    // free the client connection objects
    
    gst_rtsp_connection_close(connection);
    gst_rtsp_connection_free(connection);
    // free the pipeline
    gst_object_unref (data.pipeline);
}



void RtspManager::on_stream_status (GstBus     *bus, GstMessage *message, gpointer    user_data)
{
    GstStreamStatusType type;
    GstElement *owner;
    const GValue *val;
    GstTask *task = NULL;
    
    gst_message_parse_stream_status (message, &type, &owner);
    
    val = gst_message_get_stream_status_object (message);
    
    /* see if we know how to deal with this object */
    if (G_VALUE_TYPE (val) == GST_TYPE_TASK) {
        task = (GstTask *) g_value_get_object (val);
    }
    
    printMsg(message);

    switch (type) {
        case GST_STREAM_STATUS_TYPE_CREATE:
            g_message("GST_STREAM_STATUS_TYPE_CREATE");
            break;
        case GST_STREAM_STATUS_TYPE_ENTER:
            g_message("GST_STREAM_STATUS_TYPE_ENTER");
            break;
        case GST_STREAM_STATUS_TYPE_LEAVE:
            break;
        case GST_STREAM_STATUS_TYPE_DESTROY:
            break;
        case GST_STREAM_STATUS_TYPE_START:
            break;
        case  GST_STREAM_STATUS_TYPE_PAUSE:
            break;
        case GST_STREAM_STATUS_TYPE_STOP:
            break;
        default:
            break;
    }
}

void RtspManager::on_error (GstBus     *bus, GstMessage *message, gpointer    user_data)
{
    g_message ("received ERROR");
    g_main_loop_quit ( RtspManager::data.main_loop );
}

void RtspManager::on_eos (GstBus     *bus, GstMessage *message, gpointer    user_data)
{
    g_message ("received EOS");
    g_main_loop_quit ( RtspManager::data.main_loop );
}

void RtspManager::on_state_changed(GstBus  *bus, GstMessage *message, gpointer    user_data)
{
    printMsg(message);
}

void RtspManager::on_pad_added_cb (GstElement *element, GstPad *pad, CustomData*  data)
{
    GstPad *sinkpad;
    GstElement *decoder = data->rtph264pay;
    /* We can now link this pad with the rtsp-decoder sink pad */
    g_print ("Dynamic pad created, linking source/demuxer\n");
    sinkpad = gst_element_get_static_pad (decoder, "sink");
    gst_pad_link (pad, sinkpad);
    gst_object_unref (sinkpad);
}

void RtspManager::rtspsrc_pad_added_cb (GstElement* rtspsrc, GstPad* pad, CustomData* data)
{
    GST_INFO("New pad in rtspsrc added!");
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
            GST_INFO("Pad for audio linked");
            g_free (dynamic_pad_name);
            return;
        }
        else if(gst_element_link_pads(data->rtspsrc, dynamic_pad_name, data->rtph264pay, "sink")){
            GST_INFO("Pad for video linked");
            g_free (dynamic_pad_name);
            return;
        }
        else if(gst_element_link_pads(data->rtspsrc, dynamic_pad_name, data->fakesink, "sink")){
            GST_INFO("Pad for video linked");
            g_free (dynamic_pad_name);
            return;
        }
    }
}

void RtspManager::rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data)
{
    GST_INFO("New pad in rtspsrc removed !");
}

void RtspManager::rtspsrc_no_more_pads_cb(GstElement *rtspsrc, gpointer data)
{
     GST_INFO("No_more_pads in rtspsrc removed !");
}

void RtspManager::printMsg(GstMessage* msg)
{
    const gchar* msgType   = GST_MESSAGE_TYPE_NAME(msg);
    GstClockTime timeStamp = GST_MESSAGE_TIMESTAMP(msg);
    const gchar* srcObj        = GST_MESSAGE_SRC_NAME(msg);
    guint seqnum                = GST_MESSAGE_SEQNUM(msg);
    g_print("--------------------------------------------------------------------------\n");
    g_print("Mesage SeqNum                   :   %d\n",    seqnum);
    g_print("Messge type                     :  %s\n",   msgType);
    g_print("Time Stamp when mesage created  : %llu\n", timeStamp);
    g_print("Src Object Name                 : %s\n",   srcObj);
    g_print("---------------------------------------------------------------------------\n");
}

void RtspManager::processMsgType(GstBus *bus, GstMessage* msg, CustomData* data)
{
    GMainLoop *loop = (GMainLoop *) data;
    
    switch (GST_MESSAGE_TYPE (msg))
    {
        case GST_MESSAGE_UNKNOWN:
            printMsg(msg);
            break;
            
        case GST_MESSAGE_EOS: {
            g_print ("End of stream\n");
            if (data != nullptr)
            {
                data->terminate = TRUE;
            }
            g_main_loop_quit (loop);
            break;
        }
        case GST_MESSAGE_ERROR: {
            gchar  *debug;
            GError *error;
            
            gst_message_parse_error (msg, &error, &debug);
            g_free (debug);
            
            g_printerr ("Error: %s\n", error->message);
            g_error_free (error);
            
            g_main_loop_quit (loop);
            break;
        }
        case GST_MESSAGE_STATE_CHANGED: {
            printMsg(msg);
            /* We are only interested in state-changed messages from the pipeline */
            const gchar* srcObj    = GST_MESSAGE_SRC_NAME(msg);
            if (strcmp(srcObj, "pipeline") == 0)
            {
               GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                g_print ("Pipeline state changed from %s to %s:\n",
                         gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            }
            break;
        }
        case GST_MESSAGE_PROGRESS:
            printMsg(msg);
            break;
        case GST_MESSAGE_NEW_CLOCK:
            printMsg(msg);
            break;
        case GST_MESSAGE_STREAM_STATUS:
            printMsg(msg);
            break;
        case GST_MESSAGE_ELEMENT:
            printMsg(msg);
            break;
        case GST_MESSAGE_STREAM_START:
            printMsg(msg);
            break;
        case GST_MESSAGE_ASYNC_DONE:
            printMsg(msg);
            gst_bus_remove_signal_watch(bus);
            break;
        default:
            g_print("Error, something wrong should never processed this unknown messge!");
            break;
    }
}

gboolean RtspManager::bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    CustomData* customData = (CustomData*)data;
    processMsgType( bus, msg, customData);

    return TRUE;
}







