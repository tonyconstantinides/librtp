//
//  main.cpp
//  runnable
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#include <iostream>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gst/rtsp/gstrtsp.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <gst/base/gstpushsrc.h>
#include <unistd.h>
#include <string.h>

#include <gst/rtsp-server/rtsp-client.h>
#include "RtspManager.hpp"
static GstDebugCategory* my_category;

//define GST_CAT_DEFAULT my_category

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    std::shared_ptr<RtspManager> manager =  RtspManager::createNewRtspManager();
    GST_DEBUG_CATEGORY_INIT (my_category, "my_code", 0, "This is the debug category for my code.");
    manager->connectToIPCam();
    manager->setupCustomData();
    manager->setupPipeLine();
    manager->setupRTSPClient();
    manager->startLoop();
    
    /*
    GstRTSPResult result;
    
    GstRTSPConnection* conn;
    GstRTSPAuthMethod method = GST_RTSP_AUTH_DIGEST;
    result = gst_rtsp_connection_create(&info, &conn);
    if (result != GST_RTSP_OK)
    {
        GST_ERROR ("gst_rtsp_connection_create failed!");
    }
    result = gst_rtsp_connection_set_auth(conn,
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
    
    result = gst_rtsp_connection_connect(conn, &time);
    
    if (result != GST_RTSP_OK)
    {
        GST_ERROR("gst_rtsp_connection_connect failed!");
    }
    
    GSocket* readSocket = gst_rtsp_connection_get_read_socket(conn);
    if (readSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_read_socket failed!");
    }
    GSocket* writeSocket =  gst_rtsp_connection_get_write_socket(conn);
    if (writeSocket == NULL)
    {
        GST_ERROR("gst_rtsp_connection_get_write_socket failed!");
    }
    */
    /* Build the pipeline */
    /*
    data.pipeline     = gst_pipeline_new("IP Cam");
    data.rtspsrc      = gst_element_factory_make ("rtspsrc", "source");
    data.rtph264depay = gst_element_factory_make ("rtph264depay", "deplay");
    data.rtph264pay   = gst_element_factory_make ("rtph264pay", "play");
    data.fakesink     = gst_element_factory_make ("fakesink", "sink");
    
    if (!data.pipeline || !data.rtspsrc || !data.rtph264depay || !data.rtph264pay || !data.fakesink)
    {
        g_printerr ("One element could not be created. Exiting.\n");
        return -1;
  
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
 
    
    gst_bin_add_many(GST_BIN(data.pipeline), data.rtspsrc, data.rtph264depay, data.rtph264pay, data.fakesink, NULL);
    
    g_signal_connect (data.rtspsrc, "pad-added",     G_CALLBACK(RtspManager::rtspsrc_pad_added_cb),   &data);
    g_signal_connect (data.rtspsrc, "pad-removed",   G_CALLBACK(RtspManager::rtspsrc_pad_removed_cb), &data);

    GstRTSPClient* client = gst_rtsp_client_new();
    if (!client)
    {
        g_printerr ("gst_rtsp_client_new() failed! Exiting.\n");
        return -1;
    }
    gboolean status = gst_rtsp_client_set_connection(client, conn);
    if (!status)
    {
        g_printerr ("gst_rtsp_client_set_connection() return false. Exiting.\n");
        return -1;
    }
     data.main_loop = g_main_loop_new (NULL, FALSE);
    GMainContext*  context = g_main_loop_get_context(data.main_loop);
    guint  attachResult =  gst_rtsp_client_attach(client, context);
    if (!attachResult)
    {
        
    }
    g_signal_connect (client, "setup-request",      G_CALLBACK( RtspManager::setup_request_cb), NULL);
    g_signal_connect (client, "new-session",           G_CALLBACK ( RtspManager::new_session_cb),   NULL);
    g_signal_connect (client, "describe-request",   G_CALLBACK (RtspManager::describe_request_cb), NULL);
    g_signal_connect (client, "announce-request",   G_CALLBACK (RtspManager::announce_request_cb), NULL);
    g_signal_connect (client, "play-request",          G_CALLBACK (RtspManager::play_request_cb),     NULL);
    g_signal_connect (client, "options-request",     G_CALLBACK (RtspManager::options_request_cb),  NULL);
  
    
    bus = gst_element_get_bus (data.pipeline);
    // add a signal onthe bus for messages
    //g_signal_connect (bus, "message",  G_CALLBACK (bus_call), NULL);
    //gst_bus_add_signal_watch_full (bus, G_PRIORITY_DEFAULT);
    guint watch_id = gst_bus_add_watch (bus, RtspManager::bus_call, &data);
    gst_object_unref (bus);
    msg = gst_bus_pop_filtered (bus, GST_MESSAGE_ANY);


    gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
    g_main_loop_run (data.main_loop);
    
 
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_bus_remove_signal_watch(bus);
    
    if (msg != NULL)
        gst_message_unref (msg);
    gst_rtsp_client_close(client);
    gst_rtsp_connection_close(conn);
    gst_rtsp_connection_free(conn);
    
    gst_object_unref (data.pipeline);
    
*/
    manager.reset();
    return 0;
}
