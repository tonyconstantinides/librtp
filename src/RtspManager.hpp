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
#include <gst/rtsp/gstrtspconnection.h>
#include <functional>
#include <string>

#define RTSP_LATENCY 0
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 65536
/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData{
    GMainLoop *main_loop;
    GstContext *context;
    GstElement *pipeline, *rtspsrc, *rtph264depay, *rtph264pay, *fakesink;
    gboolean playing;             /* Are we in the PLAYING state? */
    gboolean terminate;        /* Should we terminate execution? */
    gboolean seek_enabled; /* Is seeking enabled for this media? */
    gboolean seek_done;      /* Have we performed the seek already? */
    gint64   duration;        /* How long does this media last, in nanoseconds */
    gchar*   url;
    std::function<void(char*)> connectionCB;
}CustomData;

//typedef struct _GstRTSPStream GstRTSPStream;
class RtspManager;
class RtspManagerCallbacks;

typedef std::shared_ptr<RtspManager> RtspManagerRef;
typedef std::shared_ptr<CustomData> CustomDataRef;
typedef std::shared_ptr<RtspManagerCallbacks> CallbacksRef;


class RtspManager
{
public:
    static  std::shared_ptr<RtspManager>  createNewRtspManager();
    virtual ~RtspManager();
    RtspManager(RtspManager const&)                       = default;    // Copy construct
    RtspManager(RtspManager&&)                              = default;   // Move construct
    RtspManager& operator=(RtspManager const&)       = default;  // Copy assign
    RtspManager& operator=(RtspManager&&)              = default;  // Move assign
    
    static short getRefCount();
          // actual api
    void addConnectionCallback(std::function<void(char*)>  newCallBack) { data.connectionCB = newCallBack; }
    void connectToIPCam( const gchar * userName,
                                     const gchar * password,
                                     const gchar * host,
                                     guint16  port,
                                     const gchar* abspath,
                                     const gchar* queryParms);
    void makeElements();
    void setupPipeLine();
    void startLoop();
    static  short messageCount;
protected:
    RtspManager();
    static std::shared_ptr<RtspManager> instance;
    GstBus *bus;
    GstMessage *msg;
    GstRTSPConnection* connection;
    GstRTSPUrl* url;
    GSocket* writeSocket;
    GSocket* readSocket;
    GstRTSPWatch*  rtspWatch;
    CallbacksRef callbacksRef;
    /* Initialize our data structure */
    CustomData data;
    GstRTSPUrl connection_info;
    std::string connection_url;
    static short refCounts;
    static std::vector<RtspManagerRef>  instanceList ;
};


class RtspManagerCallbacks
{
public:
    // utility funcs
    static void printMsg(GstMessage* msg, const gchar*  msgType);
    static void processMsgType(GstBus *bus, GstMessage* msg, CustomData* data);
    static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data);
    static void on_pad_added_cb (GstElement *element, GstPad *pad, CustomData  *data);
    static void rtspsrc_pad_added_cb (GstElement *rtspsrc, GstPad  *pad, CustomData *data);
    static void rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad  *pad, CustomData *data);
    static void rtspsrc_no_more_pads_cb(GstElement *rtspsrc, gpointer data);
};


#endif /* RtspManager_hpp */
