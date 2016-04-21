//
//  RtspManager.hpp
//  libevartp
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#ifndef RtspManager_hpp
#define RtspManager_hpp
#include <gio/gio.h>
#include <gst/gst.h>
#include <gst/rtsp/gstrtspconnection.h>
#include <functional>
#include "Common.hpp"
#include "CamParmsEncription.hpp"


/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData{
    GMainLoop *main_loop;
    GstContext*   context;
    GstElement*   pipeline;
    GstElement*   rtpbin;
    GstElement*   rtspsrc;
    GstElement*   rtph264depay;
    GstElement*    mpegtsmux;
    GstElement*   rtpmp2tpay;
    GstElement*     identity;
    GstElement*   udpsink;
    GstElement*   udpsrc;
    GstBus*         bus;
    GstMessage *                msg;
    GstRTSPConnection* connection;
    GstRTSPUrl*             url;
    GSocket*                writeSocket;
    GSocket*                readSocket;
    GstRTSPWatch*  rtspWatch;
    gchar*   connectionUrl;
    std::function<void(char*)> connectionCB;
}CustomData;

typedef std::shared_ptr<CustomData> CustomDataRef;

class RtspManager 
{
public:
    static  RtspManagerRef  createNewRtspManager();
    virtual ~RtspManager() = default;
    RtspManager(RtspManager const&)                      = delete;    // Copy construct
    RtspManager(RtspManager&&)                             = delete;   // Move construct
    RtspManager& operator=(RtspManager const&)          = delete;  // Copy assign
    RtspManager& operator=(RtspManager&&)                  = default;  // Move assign
    
    static short getRefCount();

    // the structure containes base64 encoded parms
    ApiStatus connectToIPCam(CamParmsEncription& camAuth);
    void addConnectionCallback(std::function<void(char*)>  newCallBack)
     { data.connectionCB = newCallBack; }
    
    ApiStatus makeElements();
    ApiStatus setupPipeLine();
    ApiStatus startLoop();
    static  short messageCount;
    std::string getName() { return name; } 
    void setName(std::string streamName) { name = streamName;}   
    // error but continue
    ApiStatus  errorApiState( const gchar * msg);
    // fatal error do not continue
    ApiStatus  fatalApiState( const gchar* msg);
    // test the ip connection before we try to use it
    ApiStatus testConnection();
    void activateStream(bool ready)      { activeStream = ready; }
    void validStreamMethod(bool valid) { validStreamingMethod = valid; }
protected:
    RtspManager();
    ApiStatus  createElements();
    ApiStatus  addElementsToBin();
    void       setElementsProperties();
    void       addCallbacks();
    void       removeCallbacks();
    void       cleanUp();
    static RtspManagerRef instance;
    std::string name;
    bool activeStream;
    bool validStreamingMethod;
    CallbacksRef callbacksRef;
    /* Initialize our data structure */
    CustomData data;
    GstRTSPUrl connection_info;
    std::string connection_url;
    ApiStatus ApiState;
};


class RtspManagerCallbacks
{
public:
    RtspManagerCallbacks()              = default;
    virtual ~RtspManagerCallbacks() = default;
    RtspManagerCallbacks(RtspManagerCallbacks const&)                      = delete;    // Copy construct
    RtspManagerCallbacks(RtspManagerCallbacks&&)                             = delete;   // Move construct
    RtspManagerCallbacks& operator=(RtspManagerCallbacks const&)            = delete;  // Copy assign
    RtspManagerCallbacks& operator=(RtspManagerCallbacks&&)                   = default;  // Move assign

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
