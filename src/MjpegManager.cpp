//
//  MjpegManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/21/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//
#include "MjpegManager.hpp"
#include "Common.hpp"
#include "CamParmsEncription.hpp"

MjpegManagerRef MjpegManager::instance = nullptr;

MjpegManagerRef  MjpegManager::createNewMjpegManager()
{
    logdbg("***************************************");
    logdbg("Entering createNewMjpegManager.......");
    instance = std::shared_ptr<MjpegManager>(new MjpegManager);
    
    logdbg("Leaving createNewMjpegManager.......");
    logdbg("***************************************");
    return instance;
}

MjpegManager::MjpegManager()
{
    data.main_loop      = nullptr;
    data.context           = nullptr;
    data.pipeline          = nullptr;
    data.souphttpsrc    = nullptr;
    data.tcpserver        = nullptr;
    data.multipartdemux = nullptr;
    data.jpegdec          = nullptr;
    data.ffenc_mpeg4  = nullptr;
    data.bus                 = nullptr;
    data.msg                = nullptr;
    data.connectionUrl = nullptr;
}

ApiStatus MjpegManager::connectToIPCam(CamParmsEncription& camAuth)
{
    // decode parms
    std::string encodedStr;
    // decode it
    encodedStr =  camAuth.getCameraGuid();
    std::string cameraGuid = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getUserName();
    std::string userName = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getPassword();
    std::string password = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getHost();
    std::string  host  = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getPort();
    std::string  port   = camAuth.base64_decode(encodedStr);
    
    encodedStr = camAuth.getAbsPath();
    std::string  absPath = camAuth.base64_decode(encodedStr);
    
    encodedStr =  camAuth.getQueryParms();
    std::string  queryParms = camAuth.base64_decode(encodedStr);

    connection_url.append(host);
    connection_url = "http://";
    connection_url.append(host.c_str());
    connection_url.append(":");
    connection_url.append(port.c_str());
    connection_url.append("/");
    connection_url.append("videostream.asf?");
    connection_url.append("user=");
    connection_url.append(userName.c_str());
    connection_url.append("&pwd=");
    connection_url.append(password.c_str());
    ApiState = ApiStatus::OK;
    return ApiState;
}

ApiStatus MjpegManager::testConnection()
{
      return ApiState;
}


ApiStatus MjpegManager::makeElements()
{
    logdbg("***************************************");
    logdbg("Entering makeElements.......");
    if (createElements() == ApiStatus::OK)
    {
        ApiState = setElementsProperties();
    }
    logdbg("Leaving makeElements.......");
    logdbg("***********************************");
    return ApiState;
}

ApiStatus MjpegManager::setupPipeLine()
{
    logdbg("***************************************");
    logdbg("Entering setupPipeLine.......");
        
    logdbg("Leaving setupPipeLine.......");
    logdbg("***********************************");
     return ApiState;
}

ApiStatus MjpegManager::startLoop()
{
    logdbg("***************************************");
    logdbg("Entering startLoop.......");
    
    logdbg("Leaving startLoop.......");
    logdbg("***********************************");
     return ApiState;
}

ApiStatus MjpegManager::setElementsProperties()
{
    logdbg("***************************************");
    logdbg("Entering setElementsProperties.......");
    
    logdbg("Leaving setElementsProperties.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::addCallbacks()
{
    logdbg("***************************************");
    logdbg("Entering addCallbacks.......");
    
    logdbg("Leaving addCallbacks.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::removeCallbacks()
{
    logdbg("***************************************");
    logdbg("Entering removeCallbacks.......");
    
    logdbg("Leaving removeCallbacks.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::cleanUp()
{
    logdbg("***************************************");
    logdbg("Entering cleanUp.......");
    
    logdbg("Leaving cleanUp.......");
    logdbg("****************************************");
     return ApiState;
}

ApiStatus  MjpegManager::createElements()
{
    logdbg("***************************************");
    logdbg("Entering createElements.......");
    
    logdbg("Leaving createElements.......");
    logdbg("****************************************");
    return ApiState;
}

ApiStatus  MjpegManager::addElementsToBin()
{
    logdbg("***************************************");
    logdbg("Entering addElementsToBin.......");
    data.pipeline             = gst_pipeline_new("pipeline");
    data.souphttpsrc        = gst_element_factory_make("souphttpsrc", "souphttpsrc");
    data.tcpserver           = gst_element_factory_make("tcpserver", "tcpserver");
    data.multipartdemux   = gst_element_factory_make("multipartdemux", "multipartdemux");
    data.jpegdec               = gst_element_factory_make("jpegdec", "jpegdec");
    data.ffenc_mpeg4        = gst_element_factory_make("ffenc_mpeg4", "ffenc_mpeg4");
    
    if (!data.pipeline)
    {
        return fatalApiState("PipeLine could not be created!");
    }
    if (!data.souphttpsrc)
    {
        return fatalApiState("souphttpsrc  element could not be created!");
    }
    if (!data.tcpserver)
    {
        return fatalApiState("tcpserver element could not be created!");
    }
    if (!data.multipartdemux)
    {
        return fatalApiState("multipartdemux element could not be created!");
    }
    if (!data.jpegdec)
    {
        return fatalApiState("jpegdec element could not be created!");
    }
    if (!data.ffenc_mpeg4)
    {
        return fatalApiState("ffenc_mpeg4 element could not be created!");
    }

    logdbg("Leaving addElementsToBin.......");
    logdbg("****************************************");
    return ApiState;
}





