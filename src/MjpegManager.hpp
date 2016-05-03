//
//  MjpegManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef MjpegManager_hpp
#define MjpegManager_hpp
#include "Common.hpp"
#include "IPStreamManager.hpp"
#include "CamParmsEncription.hpp"

typedef struct _MjpegData{
    GMainLoop*              main_loop = nullptr;
    GstContext*           context = nullptr ;
    GstElement*             pipeline  = nullptr;
    GstElement*             souphttpsrc = nullptr;
    GstElement*             tcpserver = nullptr;
    GstElement*             multipartdemux = nullptr;
    GstElement*             jpegdec = nullptr;
    GstElement*             ffenc_mpeg4 = nullptr;
    GstBus*                    bus = nullptr;
    GstMessage*             msg = nullptr;
    gchar*                     connectionUr = nullptr;
    CallBackFunc           streamConnectionCB;
    CallBackFunc           streamErrorCB;
    StreamErrorHandler errorHandler;
}MjpegData;

typedef std::shared_ptr<MjpegData> MjpegDataRef;

class MjpegManager : public IPStreamManager
{
public:
    static  MjpegManagerRef  createNewMjpegManager();
    virtual ~MjpegManager() = default;
    MjpegManager(MjpegManager const&)                  = delete;    // Copy construct
    MjpegManager(MjpegManager&&)                       = delete;   // Move construct
    MjpegManager& operator=(MjpegManager const&)       = delete;  // Copy assign
    MjpegManager& operator=(MjpegManager&&)            = default;  // Move assign
    ApiStatus connectToIPCam(CamParmsEncription& value) override;
    virtual ApiStatus testConnection()  override;
    virtual ApiStatus makeElements() override;
    virtual ApiStatus setupPipeLine() override;
    virtual ApiStatus startLoop() override;
protected:
    MjpegManager();
    static   MjpegManagerRef instance;
    MjpegData  data;
    virtual ApiStatus  createElements() override;
    virtual ApiStatus  addElementsToBin() override;
    virtual ApiStatus  setElementsProperties() override;
    virtual ApiStatus  addCallbacks() override;
    virtual ApiStatus  removeCallbacks() override;
    virtual ApiStatus  cleanUp() override;
    static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data);
    static void processMsgType(GstBus *bus, GstMessage* msg, MjpegData* pdata);
    static void souphttpsrc_pad_added_cb (GstElement*       souphttsrc, GstPad* pad, MjpegData* data);
    static void souphttpsrc_pad_removed_cb (GstElement*   souphttpsrc, GstPad* pad, MjpegData *data);
    static void souphttpsrc_no_more_pads_cb(GstElement*  souphttpsrc, gpointer data);
};

#endif /* MjpegManager_hpp */