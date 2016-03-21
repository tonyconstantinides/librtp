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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <gst/base/gstpushsrc.h>
#include <unistd.h>
#include <String.h>

#include <gst/rtsp-server/rtsp-client.h>


GstRTSPUrl RtspManager::connection_info =
{   GST_RTSP_LOWER_TRANS_TCP,
    GST_RTSP_FAM_INET,
    strdup( "tony"),
    strdup("Cyprus2016"),
    strdup("192.168.1.220"),
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
GstRTSPClient* RtspManager:: rtsp_client = nullptr;

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
    
    GSocket* readSocket = gst_rtsp_connection_get_read_socket(connection);
    if (readSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_read_socket failed!");
    }
    GSocket* writeSocket =  gst_rtsp_connection_get_write_socket(connection);
    if (writeSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_write_socket failed!");
    }

}

void RtspManager::setupCustomData()
{
    data.pipeline     = gst_pipeline_new("IP Cam");
    data.rtspsrc      = gst_element_factory_make ("rtspsrc", "source");
    data.rtph264depay = gst_element_factory_make ("rtph264depay", "deplay");
    data.rtph264pay   = gst_element_factory_make ("rtph264pay", "play");
    data.fakesink     = gst_element_factory_make ("fakesink", "sink");
    
    if (!data.pipeline || !data.rtspsrc || !data.rtph264depay || !data.rtph264pay || !data.fakesink)
    {
        g_printerr ("One element could not be created. Exiting.\n");
    }
    g_object_set( G_OBJECT (data.rtspsrc), "location", RTSP_URI,
                 "latency", RTSP_LATENCY,
                 "buffer-mode", RTSP_BUFFER_MODE,
                 "rtp-blocksize", RTSP_RTP_BLOCKSIZE,
                 "protocols", GST_RTSP_LOWER_TRANS_TCP,
                 "debug", TRUE,
                 "retry", 30,
                 "do-rtcp", TRUE,
                 "do-rtsp-keep-alive", TRUE,
                 NULL);
}

void RtspManager::setupPipeLine()
{
    gst_bin_add_many(GST_BIN(data.pipeline), data.rtspsrc, data.rtph264depay, data.rtph264pay, data.fakesink, NULL);
    
    g_signal_connect (data.rtspsrc, "pad-added",     G_CALLBACK(RtspManager::rtspsrc_pad_added_cb),   &data);
    g_signal_connect (data.rtspsrc, "pad-removed",   G_CALLBACK(RtspManager::rtspsrc_pad_removed_cb), &data);
  
}

void RtspManager::setupRTSPClient()
{
    rtsp_client  = gst_rtsp_client_new();
    if (!rtsp_client)
    {
        g_printerr ("gst_rtsp_client_new() failed! Exiting.\n");

    }
    gboolean status = gst_rtsp_client_set_connection(rtsp_client, connection);
    if (!status)
    {
        g_printerr ("gst_rtsp_client_set_connection() return false. Exiting.\n");
    }
    g_signal_connect (rtsp_client, "setup-request",      G_CALLBACK( RtspManager::setup_request_cb), NULL);
    g_signal_connect (rtsp_client, "new-session",           G_CALLBACK ( RtspManager::new_session_cb),   NULL);
    g_signal_connect (rtsp_client, "describe-request",   G_CALLBACK (RtspManager::describe_request_cb), NULL);
    g_signal_connect (rtsp_client, "announce-request",   G_CALLBACK (RtspManager::announce_request_cb), NULL);
    g_signal_connect (rtsp_client, "play-request",          G_CALLBACK (RtspManager::play_request_cb),     NULL);
    g_signal_connect (rtsp_client, "options-request",     G_CALLBACK (RtspManager::options_request_cb),  NULL);
    
    data.main_loop = g_main_loop_new (NULL, FALSE);
    GMainContext*  context = g_main_loop_get_context(data.main_loop);
    guint  attachResult =  gst_rtsp_client_attach(rtsp_client, context);
    if (!attachResult)
    {
        g_printerr("Unable to attach the client!");
    }

}

void RtspManager::startLoop()
{
    bus = gst_element_get_bus (data.pipeline);
    // add a signal onthe bus for messages
    //g_signal_connect (bus, "message",  G_CALLBACK (bus_call), NULL);
    //gst_bus_add_signal_watch_full (bus, G_PRIORITY_DEFAULT);
    guint watch_id = gst_bus_add_watch (bus, RtspManager::bus_call, &data);
    gst_object_unref (bus);
    msg = gst_bus_pop_filtered (bus, GST_MESSAGE_ANY);
    
    /* Start playing */
    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    g_main_loop_run (data.main_loop);
    
    /* Free resources */
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_bus_remove_signal_watch(bus);
    
    if (msg != NULL)
        gst_message_unref (msg);
    gst_rtsp_client_close(rtsp_client);
    gst_rtsp_connection_close(connection);
    gst_rtsp_connection_free(connection);
    
    gst_object_unref (data.pipeline);
}

void RtspManager::rtspsrc_pad_added_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data)
{
    GST_INFO("New pad in rtspsrc added!");
    gchar *name;
    GstCaps * p_caps;
    gchar * description;
    GstElement *rtph264depay = GST_ELEMENT(data->rtph264depay);
    GstElement *rtph264pay   = GST_ELEMENT(data->rtph264pay);
    GstElement *fakesink     = GST_ELEMENT(data->fakesink);
    
    name = gst_pad_get_name(pad);
    g_print("A new pad %s was created\n", name);
    p_caps = gst_pad_get_pad_template_caps (pad);
    
    description = gst_caps_to_string(p_caps);
    printf("%s\n",p_caps,", ",description,"\n");
    
    // try to link the pads then ...
    if(!gst_element_link_pads(rtspsrc, name, rtph264depay, "deplay"))
    {
        printf("Failed to link elements 3\n");
    }
    if(!gst_element_link_pads(rtspsrc, name, rtph264pay, "play"))
    {
        printf("Failed to link elements 3\n");
    }
    if(!gst_element_link_pads(rtspsrc, name, fakesink, "sink"))
    {
        printf("Failed to link elements 3\n");
    }
    g_free(description);
}

void RtspManager::rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data)
{
    GST_INFO("New pad in rtspsrc removed !");
}

void RtspManager::printMsg(GstMessage* msg)
{
    const gchar* msgType   = GST_MESSAGE_TYPE_NAME(msg);
    GstClockTime timeStamp = GST_MESSAGE_TIMESTAMP(msg);
    const gchar* srcObj    = GST_MESSAGE_SRC_NAME(msg);
    guint seqnum           = GST_MESSAGE_SEQNUM(msg);
    g_print("--------------------------------------------------------------------------\n");
    g_print("Mesage SeqNum                   :   %d\n",    seqnum);
    g_print("Messge type                     :  %s\n",   msgType);
    g_print("Time Stamp when mesage created  : %lu\n", timeStamp);
    g_print("Src Object Name                 : %s\n",   srcObj);
    g_print("---------------------------------------------------------------------------\n");
}

void RtspManager::processMsgType(GstMessage* msg, CustomData* data)
{
    GMainLoop *loop = (GMainLoop *) data;
    
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_UNKNOWN:
            printMsg(msg);
            break;
            
        case GST_MESSAGE_EOS: {
            g_print ("End of stream\n");
            data->terminate = TRUE;
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
        case GST_MESSAGE_WARNING:
            printMsg(msg);
            break;
        case GST_MESSAGE_INFO:
            printMsg(msg);
            break;
        case GST_MESSAGE_TAG:
            printMsg(msg);
            break;
        case GST_MESSAGE_BUFFERING:
            printMsg(msg);
            break;
        case GST_MESSAGE_STATE_CHANGED:
            printMsg(msg);
            /* We are only interested in state-changed messages from the pipeline */
            if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->pipeline)) {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                g_print ("Pipeline state changed from %s to %s:\n",
                         gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
            }
            break;
        case GST_MESSAGE_STATE_DIRTY:
            printMsg(msg);
            break;
        case GST_MESSAGE_STEP_DONE:
            printMsg(msg);
            break;
        case GST_MESSAGE_CLOCK_PROVIDE:
            printMsg(msg);
            break;
        case GST_MESSAGE_CLOCK_LOST:
            printMsg(msg);
            break;
        case GST_MESSAGE_NEW_CLOCK:
            printMsg(msg);
            break;
        case GST_MESSAGE_STRUCTURE_CHANGE:
            printMsg(msg);
            break;
        case GST_MESSAGE_STREAM_STATUS:
            
            printMsg(msg);
            if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->rtspsrc))
            {
                g_print("Ip Cam now streaming...........");
            }
            break;
        case GST_MESSAGE_APPLICATION:
            printMsg(msg);
            break;
        case GST_MESSAGE_ELEMENT:
            printMsg(msg);
            break;
        case GST_MESSAGE_SEGMENT_START:
            printMsg(msg);
            break;
        case GST_MESSAGE_SEGMENT_DONE:
            printMsg(msg);
            break;
        case GST_MESSAGE_DURATION_CHANGED:
            printMsg(msg);
            // mark as invalid
            data->duration = GST_CLOCK_TIME_NONE;
            break;
        case GST_MESSAGE_LATENCY:
            printMsg(msg);
            break;
        case GST_MESSAGE_ASYNC_START:
            printMsg(msg);
            break;
        case GST_MESSAGE_ASYNC_DONE:
            printMsg(msg);
            break;
        case GST_MESSAGE_REQUEST_STATE:
            printMsg(msg);
            break;
        case GST_MESSAGE_STEP_START:
            printMsg(msg);
            break;
        case GST_MESSAGE_QOS:
            printMsg(msg);
            break;
        case GST_MESSAGE_PROGRESS:
            printMsg(msg);
            break;
        case GST_MESSAGE_TOC:
            printMsg(msg);
            break;
        case GST_MESSAGE_RESET_TIME:
            printMsg(msg);
            break;
        case GST_MESSAGE_STREAM_START:
            printMsg(msg);
            
            break;
        case GST_MESSAGE_NEED_CONTEXT:
            printMsg(msg);
            break;
        case GST_MESSAGE_HAVE_CONTEXT:
            printMsg(msg);
            break;
        case GST_MESSAGE_EXTENDED:
            printMsg(msg);
            break;
        case GST_MESSAGE_DEVICE_ADDED:
            printMsg(msg);
            break;
        case GST_MESSAGE_DEVICE_REMOVED:
            printMsg(msg);
            break;
        case GST_MESSAGE_ANY:
            printMsg(msg);
            break;
        default:
            g_print("Error, something wrong should never processed this unknown messge!");
            break;
    }
}

gboolean RtspManager::bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;
    CustomData* customData = (CustomData*)data;
    processMsgType( msg, customData);
    return TRUE;
}


// signals for the connection
void RtspManager::setup_request_cb(GstRTSPClient  *gstrtspclient,GstRTSPContext *arg1, gpointer user_data)
{
    GST_LOG("setup_request_cb");
}

void RtspManager::announce_request_cb(GstRTSPClient *gstrtspclient, GstRTSPContext *arg1, gpointer user_data)
{
    GST_LOG("announce_request_cb");
}

void RtspManager::new_session_cb (GstRTSPClient  *gstrtspclient, GstRTSPSession *arg1, gpointer user_data)
{
    GST_LOG("new_session_cb");
}

void RtspManager::describe_request_cb(GstRTSPClient  *gstrtspclient,GstRTSPContext *arg1, gpointer user_data)
{
    GST_LOG("describe_request_cb");
}

void RtspManager::play_request_cb(GstRTSPClient  *gstrtspclient, GstRTSPContext *arg1, gpointer  user_data)
{
    GST_LOG("play_request_cb");
}
void RtspManager::options_request_cb(GstRTSPClient  *gstrtspclient, GstRTSPContext *arg1, gpointer  user_data)
{
    GST_LOG("options_request_cb");
}





