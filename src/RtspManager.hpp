//
//  RtspManager.hpp
//  libevartp
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#ifndef RtspManager_hpp
#define RtspManager_hpp
#include <memory>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-client.h>

#define RTSP_URI "rtsp://tony:Cyprus2016@192.168.1.220:88/videoMain"
#define RTSP_LATENCY 0
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 65536
/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData{
    GMainLoop *main_loop;
    GstElement *pipeline, *rtspsrc, *rtph264depay, *rtph264pay, *fakesink;
    gboolean playing;             /* Are we in the PLAYING state? */
    gboolean terminate;        /* Should we terminate execution? */
    gboolean seek_enabled; /* Is seeking enabled for this media? */
    gboolean seek_done;      /* Have we performed the seek already? */
    gint64      duration;        /* How long does this media last, in nanoseconds */
}CustomData;

class RtspManager
{
public:
   static std::shared_ptr<RtspManager>   createNewRtspManager();
    virtual ~RtspManager();
    RtspManager(RtspManager const&)                       = default;    // Copy construct
    RtspManager(RtspManager&&)                              = default;   // Move construct
    RtspManager& operator=(RtspManager const&)       = default;  // Copy assign
    RtspManager& operator=(RtspManager&&)              = default;  // Move assign
    
    static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data);
    static void rtspsrc_pad_added_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data);
    static void rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad* pad, CustomData *data);
    // callbacks
    static void setup_request_cb(GstRTSPClient  *gstrtspclient,GstRTSPContext *arg1, gpointer user_data);
    static void announce_request_cb(GstRTSPClient *gstrtspclient, GstRTSPContext *arg1, gpointer user_data);
    static void new_session_cb (GstRTSPClient  *gstrtspclient, GstRTSPSession *arg1, gpointer user_data);
    static void describe_request_cb(GstRTSPClient  *gstrtspclient,GstRTSPContext *arg1, gpointer user_data);
    static void play_request_cb(GstRTSPClient  *gstrtspclient, GstRTSPContext *arg1, gpointer  user_data);
    static void options_request_cb(GstRTSPClient  *gstrtspclient, GstRTSPContext *arg1, gpointer  user_data);
    // utility funcs
    static void printMsg(GstMessage* msg);
    static void processMsgType(GstMessage* msg, CustomData* data);
    // actual api
    static void connectToIPCam();
    static void setupCustomData();
    static void setupPipeLine();
    static void setupRTSPClient();
    static void startLoop();
protected:
    RtspManager();
    static std::shared_ptr<RtspManager> instance;
    static GstBus *bus;
    static GstMessage *msg;
    static GstRTSPConnection* connection;
    /* Initialize our data structure */
    static CustomData data;
    static  GstRTSPUrl connection_info;
    static   GstRTSPClient* rtsp_client;

};



#endif /* RtspManager_hpp */
