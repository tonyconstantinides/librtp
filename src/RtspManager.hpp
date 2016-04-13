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

#define USER_AGENT "Eva Automartion Cake"
#define RTSP_LATENCY 0
#define RTSP_BUFFER_MODE 0
#define RTSP_RTP_BLOCKSIZE 50000
/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData{
    GMainLoop *main_loop;
    GstContext*   context;
    GstElement*   pipeline;
    GstElement*   rtpbin;
    GstElement*   rtspsrc;
    GstElement*   rtph264depay;
    GstElement*    mpegtsmux;
    GstElement*   rtph264pay;
    GstElement*     identity;
    GstElement*   udpsink;
    GstElement*   udpsrc;
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
enum   API_CALL_STATUS {OK, CLEAR, FAIL, FATAL_ERROR };
typedef API_CALL_STATUS ApiStatus;

class RtspManager
{
public:
    static  RtspManagerRef  createNewRtspManager();
    virtual ~RtspManager();
    RtspManager(RtspManager const&)                       = default;    // Copy construct
    RtspManager(RtspManager&&)                              = default;   // Move construct
    RtspManager& operator=(RtspManager const&)       = default;  // Copy assign
    RtspManager& operator=(RtspManager&&)              = default;  // Move assign
    
    static short getRefCount();
          // actual api
    ApiStatus connectToIPCam( const gchar * userName,
                                     const gchar * password,
                                     const gchar * host,
                                     guint16  port,
                                     const gchar* abspath,
                                     const gchar* queryParms);
    void addConnectionCallback(std::function<void(char*)>  newCallBack) {
        data.connectionCB = newCallBack; }
    
    ApiStatus makeElements();
    ApiStatus setupPipeLine();
    ApiStatus startLoop();
    static  short messageCount;
    // everything cool
    ApiStatus  okApiState();
    // clear api state used if had previous error
    ApiStatus  clearApiState( );
    // error but continue
    ApiStatus  errorApiState( const gchar * msg);
    // fatal error do not continue
    ApiStatus  fatalApiState( const gchar* msg);
protected:
    RtspManager();
    static RtspManagerRef instance;
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
    static void rtpbin_pad_added_cb(GstElement *rtpbin, GstPad  *pad, CustomData *data);
    static void rtspsrc_pad_added_cb (GstElement *rtspsrc, GstPad  *pad, CustomData *data);
    static void rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad  *pad, CustomData *data);
    static void rtspsrc_no_more_pads_cb(GstElement *rtspsrc, gpointer data);
};


#endif /* RtspManager_hpp */
