//
//  RtspManager.hpp
//  libevartp
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#ifndef RtspManager_hpp
#define RtspManager_hpp
#include "Common.hpp"
#include "IPStreamManager.hpp"
#include "CamParamsEncryption.hpp"
#include "StreamErrorHandler.hpp"

// Structure to contain all our information, so we can pass it around
// must raw pointers as the underlying API is C-based
typedef struct _Data{
    gboolean                  connected = false;
    GMainLoop*               main_loop = nullptr;
    GstContext*             context   = nullptr;
    GstElement*             pipeline  = nullptr;
    GstElement*             rtpbin   = nullptr;
    GstElement*             rtspsrc   = nullptr;
    GstElement*             queue1     = nullptr;
    GstElement*             rtph264depay = nullptr;
    GstElement*             queue2     = nullptr;
    GstElement*             mpegtsmux    = nullptr;
    GstElement*             rtpmp2tpay       = nullptr;
    GstElement*             udpsink             = nullptr;
    GstBus*                      bus                    = nullptr;
    GstMessage*              msg                  = nullptr;
    GstRTSPConnection*  connection       = nullptr;
    GstRTSPUrl*                 url                     = nullptr;
    GSocket*                        writeSocket      = nullptr;
    GSocket*                        readSocket       = nullptr;
    GstRTSPWatch*             rtspWatch        = nullptr;
    gchar*                             connectionUrl      = nullptr;
    ConnectedCallBackFunc    streamConnectionCB;
    ErrorCallBackFunc             streamErrorCB;
    StreamErrorHandlerRef     errorHandlerRef;
}RtspData;

typedef std::shared_ptr<RtspData> RtspDataRef;

class RtspManager : public IPStreamManager
{
public:
    static  RtspManagerRef  createNewRtspManager();
    virtual ~RtspManager();
    RtspManager(RtspManager const&)                        = delete;    // Copy construct
    RtspManager(RtspManager&&)                                = delete;   // Move construct
    RtspManager& operator=(RtspManager const&)  = delete;  // Copy assign
    RtspManager& operator=(RtspManager&&)                 = default;  // Move assign

    // the structure containes base64 encoded parms
    void addConnectionCallback(ConnectedCallBackFunc   connectedCallBack) { dataRef->streamConnectionCB = connectedCallBack; }
    void addErrorCallback(ErrorCallBackFunc streamErrorCallback)  {     dataRef->streamErrorCB = streamErrorCallback; }
    virtual ApiStatus connectToIPCam(CamParamsEncryptionRef camAuthRef) override;
    virtual ApiStatus  testConnection()  override;
    virtual ApiStatus makeElements() 	 override;
    virtual ApiStatus setupPipeLine()    override;
    virtual ApiStatus startLoop()        override;
    // test the ip connection before we try to use it
    virtual ApiStatus  createElements()        override;
    virtual ApiStatus  addElementsToBin()      override;
    virtual ApiStatus  linkElements()               override;
    virtual ApiStatus  setElementsProperties() override;
    virtual ApiStatus  addCallbacks()          override;
    virtual ApiStatus  removeCallbacks()       override;
    virtual ApiStatus   cleanUp()              override;
// need to be static for gstreamer
    // utility funcs
    static void processMsgType(GstBus *bus, GstMessage* msg, RtspDataRef appRef);
    static gboolean bus_call (GstBus *bus, GstMessage *msg, RtspDataRef appRef);
    static void on_pad_added_cb (GstElement *element, GstPad *pad, RtspDataRef  appRef);
    static void rtpbin_pad_added_cb(GstElement *rtpbin, GstPad  *pad,  RtspDataRef data);
    static void rtspsrc_pad_added_cb (GstElement *rtspsrc, GstPad  *pad,  RtspDataRef appRef);
    static void rtspsrc_pad_removed_cb (GstElement *rtspsrc, GstPad  *pad,  RtspDataRef appRef);
    static void rtspsrc_no_more_pads_cb(GstElement *rtspsrc, RtspDataRef appRef);

protected:
    RtspManager();
   
    static RtspManagerRef instance;
    RtspDataRef dataRef;
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



#endif /* RtspManager_hpp */
