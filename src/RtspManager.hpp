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
#include "Common.hpp"
#include "IPStreamManager.hpp"
#include "CamParmsEncription.hpp"

// Structure to contain all our information, so we can pass it around
// must raw pointers as the underlying API is C-based
typedef struct _Data{
    GMainLoop*    main_loop = nullptr;
    GstContext*   context   = nullptr;;
    GstElement*   pipeline  = nullptr;
    GstElement*   rtpbin    = nullptr;
    GstElement*   rtspsrc   = nullptr;
    GstElement*   rtph264depay = nullptr;
    GstElement*   mpegtsmux    = nullptr;
    GstElement*   rtpmp2tpay   = nullptr;
    GstElement*   identity     = nullptr;
    GstElement*   udpsink      = nullptr;
    GstElement*   udpsrc       = nullptr;
    GstBus*         bus                 = nullptr;
    GstMessage*   msg                   = nullptr;
    GstRTSPConnection* connection       = nullptr;
    GstRTSPUrl*        url              = nullptr;
    GSocket*           writeSocket      = nullptr;
    GSocket*           readSocket       = nullptr;
    GstRTSPWatch*      rtspWatch        = nullptr;
    gchar*             connectionUrl    = nullptr;
    std::function<void(char*)> connectionCB;
}CustomData;		

typedef std::shared_ptr<CustomData> CustomDataRef;



class RtspManager : public IPStreamManager
{
public:
    static  RtspManagerRef  createNewRtspManager();
    virtual ~RtspManager()                      = default;
    RtspManager(RtspManager const&)             = delete;    // Copy construct
    RtspManager(RtspManager&&)                  = delete;   // Move construct
    RtspManager& operator=(RtspManager const&)  = delete;  // Copy assign
    RtspManager& operator=(RtspManager&&)       = default;  // Move assign
    static  short messageCount;
    // the structure containes base64 encoded parms
    void addConnectionCallback(std::function<void(char*)>  newCallBack) { data.connectionCB = newCallBack; }

    virtual ApiStatus connectToIPCam(CamParmsEncription& camAuth) override;
    virtual ApiStatus  testConnection()  override;
    virtual ApiStatus makeElements() 	 override;
    virtual ApiStatus setupPipeLine()    override;
    virtual ApiStatus startLoop()        override;
    // test the ip connection before we try to use it
    virtual ApiStatus  createElements()        override;
    virtual ApiStatus  addElementsToBin()      override;
    virtual ApiStatus  setElementsProperties() override;
    virtual ApiStatus  addCallbacks()          override;
    virtual ApiStatus  removeCallbacks()       override;
    virtual ApiStatus   cleanUp()              override;
protected:
    RtspManager();
    static RtspManagerRef instance;
    CallbacksRef callbacksRef;
    CustomData data;
    GstRTSPUrl connection_info = {
        GST_RTSP_LOWER_TRANS_TCP,
        GST_RTSP_FAM_INET,
        nullptr,
        nullptr,
        nullptr,
        88,
        nullptr,
        nullptr
    };
};

class RtspManagerCallbacks
{
public:
    RtspManagerCallbacks()          = default;
    virtual ~RtspManagerCallbacks() = default;
    RtspManagerCallbacks(RtspManagerCallbacks const&)             = delete;    // Copy construct
    RtspManagerCallbacks(RtspManagerCallbacks&&)                  = delete;   // Move construct
    RtspManagerCallbacks& operator=(RtspManagerCallbacks const&)  = delete;  // Copy assign
    RtspManagerCallbacks& operator=(RtspManagerCallbacks&&)       = default;  // Move assign

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
