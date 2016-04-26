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

typedef struct _MjpegData{
    GMainLoop *         main_loop;
    GstContext*        context;
    GstElement*        pipeline;
    GstElement*        souphttpsrc;
    GstElement*        tcpserver;
    GstElement*        multipartdemux;
    GstElement*        jpegdec;
    GstElement*        ffenc_mpeg4;
    GstBus*              bus;
    GstMessage *       msg;
    gchar*                connectionUrl;
    std::function<void(char*)> connectionCB;
}MjpegData;

typedef std::shared_ptr<MjpegData> MjpegDataRef;

class MjpegManager : public IPStreamManager
{
public:
    static  MjpegManagerRef  createNewMjpegManager();
    virtual ~MjpegManager() = default;
    MjpegManager(MjpegManager const&)                      = delete;    // Copy construct
    MjpegManager(MjpegManager&&)                             = delete;   // Move construct
    MjpegManager& operator=(MjpegManager const&)        = delete;  // Copy assign
    MjpegManager& operator=(MjpegManager&&)               = default;  // Move assign
    ApiStatus connectToIPCam(CamParmsEncription& value);
    void activateStream(bool ready)      { activeStream = ready; }
    void validStreamMethod(bool valid) { validStreamingMethod = valid; }
    virtual ApiStatus testConnection()  override;

    virtual ApiStatus makeElements() override;
    virtual ApiStatus setupPipeLine() override;
    virtual ApiStatus startLoop() override;
private:
    MjpegManager();
    virtual ApiStatus  createElements() override;
    virtual ApiStatus  addElementsToBin() override;
    virtual ApiStatus   setElementsProperties() override;
    virtual ApiStatus   addCallbacks() override;
    virtual ApiStatus   removeCallbacks() override;
    virtual ApiStatus   cleanUp() override;
    MjpegData   data;
    static         MjpegManagerRef instance;
};

#endif /* MjpegManager_hpp */