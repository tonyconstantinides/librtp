//
//  Foscam19821W.cpp
//  ipcams
//
//  Created by Tony Constantinides on 2/8/16.
//  Copyright © 2016 Eva Automation. All rights reserved.
//

//#include <foundation/data.hpp>
#include <map>
#include <string>
#include <ctime>
#include <chrono>
#include <iostream>
#include <ostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <curl/curl.h>

#include "Foscam19821W.hpp"
#include "../rtsp/IPCamRTSPClient.hpp"
#include "../apis/GenericIPCam.hpp"
#include "Base64.hpp"

#define RTSP_CLIENT_VERBOSITY_LEVEL 1 // by default, print verbose output from each "RTSPClient"


#include "catch.hpp"

namespace IPCams {
namespace FoscamH264 {
    
    std::string Foscam19821W::CAMERA_H264_IP_PORT = "";
    std::string Foscam19821W::H264_AUIH = "";
    std::string Foscam19821W::CGI_PROXY = "/cgi-bin/CGIProxy.fcgi?";
    std::string Foscam19821W::SET_MJPEG_STREAM = "";
    std::string Foscam19821W::SET_H264_STREAM = "";
    std::string Foscam19821W::GET_MJPEG_STREAM = "";
    std::string Foscam19821W::GET_VIDEO_STREAM_H264 = "";
    std::string Foscam19821W::GET_SUB_VIDEO_STREAM_H264 = "";
    std::string Foscam19821W::GET_AUDIO_H264 = "";
    std::string Foscam19821W::RESOLUTION_640x480 = "&resolution=32";
    std::string Foscam19821W::FPS_MAX_RATE = "&rate=0";
    std::string Foscam19821W::RTSP_STREAMING_ADDRESS = "";
    
    
    void  Foscam19821W::buildCompoundKeys()
    {
        Foscam19821W::CAMERA_H264_IP_PORT = Base64::secretMap[Base64::Foscam19821W_IPADDRESS_KEY] + ':';
        Foscam19821W::CAMERA_H264_IP_PORT += Base64::secretMap[Base64::Foscam19821W_PORT_KEY];
        
        Foscam19821W::H264_AUIH = "&user=" + Base64::secretMap[Base64::Foscam19821W_USERNAME_KEY];
        Foscam19821W::H264_AUIH += "&pwd=" + Base64::secretMap[Base64::Foscam19821W_PASSWORD_KEY];
        
        Foscam19821W::SET_MJPEG_STREAM = Foscam19821W::CAMERA_H264_IP_PORT + Foscam19821W::CGI_PROXY;
        Foscam19821W::SET_MJPEG_STREAM += "cmd=setSubStreamFormat&format=1";
        Foscam19821W::SET_MJPEG_STREAM += Foscam19821W::H264_AUIH;
        
        Foscam19821W::SET_H264_STREAM =  Foscam19821W::CAMERA_H264_IP_PORT + Foscam19821W::CGI_PROXY;
        Foscam19821W::SET_H264_STREAM += "cmd=setVideoStreamParam&streamType=1";
        
        Foscam19821W::GET_MJPEG_STREAM = Foscam19821W::CAMERA_H264_IP_PORT + Foscam19821W::CGI_PROXY;
        Foscam19821W::GET_MJPEG_STREAM += "cmd=GetMJStream" + Foscam19821W::H264_AUIH;
        
        // exclude the first 5 bytes as it is http:
        Foscam19821W::RTSP_STREAMING_ADDRESS = "rtsp://" +  Base64::secretMap[Base64::Foscam19821W_USERNAME_KEY] + ':';
        Foscam19821W::RTSP_STREAMING_ADDRESS += Base64::secretMap[Base64::Foscam19821W_PASSWORD_KEY] + '@';
        Foscam19821W::RTSP_STREAMING_ADDRESS += Foscam19821W::CAMERA_H264_IP_PORT.substr(7); // minus http://
        
        Foscam19821W::GET_VIDEO_STREAM_H264 =   Foscam19821W::RTSP_STREAMING_ADDRESS  + "//videoMain";
        Foscam19821W::GET_SUB_VIDEO_STREAM_H264 = Foscam19821W::RTSP_STREAMING_ADDRESS + "//videoSub";
        Foscam19821W::GET_AUDIO_H264 = Foscam19821W::RTSP_STREAMING_ADDRESS  + "//audio";
    }
    
   char eventLoopWatchVariable = 0;
   static long bytesWritten = 0L;
   size_t write_H264VideoFile(void *ptr, size_t size, size_t nmemb, FILE *stream)
   {
        if (bytesWritten >= 10000000)
        {
            bytesWritten = 0;
            return 0L;
        }
        size_t written = fwrite(ptr, size, nmemb, stream);
        bytesWritten += written;
        return written;
   }

}

TEST_CASE( "Testing the Foscam 191821W cam", "[run]" )
{
    using namespace std::chrono;
    using namespace Jetpack::Foundation;
    using namespace std;
    using namespace IPCams::FoscamH264;
    
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
#ifdef ENGINEERING_BUILD
    SECTION("Testing the encriptied file which holds alls the auth data")
    {
        WARN("Executing Reading the secret file!");
        Base64::readSecrets();
        // now check to see if the keys are loaded
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19810W_PASSWORD_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19810W_USERNAME_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19810W_IPADDRESS_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19810W_PORT_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19821W_PASSWORD_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19821W_USERNAME_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19821W_IPADDRESS_KEY));
        REQUIRE(true == Base64::isKeyAlreadyAdded(Base64::Foscam19821W_PORT_KEY));
        
    }
#endif
    SECTION("Testing the ;pading of the compound key")
    {
        Foscam19821W::buildCompoundKeys();
        REQUIRE( Foscam19821W::CAMERA_H264_IP_PORT != "");
        REQUIRE( Foscam19821W::H264_AUIH != "");
        REQUIRE( Foscam19821W::SET_MJPEG_STREAM  != "");
        REQUIRE( Foscam19821W::SET_H264_STREAM != "");
        REQUIRE( Foscam19821W::GET_MJPEG_STREAM != "" );
        REQUIRE( Foscam19821W::RTSP_STREAMING_ADDRESS != "");
        REQUIRE( Foscam19821W::GET_VIDEO_STREAM_H264 != "");
        REQUIRE( Foscam19821W::GET_SUB_VIDEO_STREAM_H264 != "");
        REQUIRE( Foscam19821W::GET_AUDIO_H264 != "");
    }
    SECTION("Testing Setting the MPEG Stream ")
    {
        WARN("Executing Setting the MJPEG Stream!");
        REQUIRE( curl != nullptr );
        std::string url = Foscam19821W::SET_MJPEG_STREAM.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the H264 Stream ")
    {
        WARN("Executing Setting the H264 Stream!");
        REQUIRE( curl != nullptr );
        std::string url = Foscam19821W::SET_H264_STREAM.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing getting the MJPEG Stream")
    {
        WARN("Executing Getting the MJPEG Stream!");
        std::string path = "/tmp/video_15fps.html";
        FILE* file = fopen(  path.c_str(), "wb");
        REQUIRE( file != nullptr );
        REQUIRE( curl != nullptr );
        std::string url = Foscam19821W::GET_MJPEG_STREAM.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 500L);
        curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 0);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IPCams::FoscamH264::write_H264VideoFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) ;
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(file);
    }
    SECTION("Testing Getting the video stream of the Foscam21W Cam")
    {
        WARN("Executing Getting the RTSP stream of the the Foscam1921W");
        REQUIRE( curl != nullptr );
        // Begin by setting up our usage environment:
        TaskScheduler* scheduler = BasicTaskScheduler::createNew();
        UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);
  
        // There are argc-1 URLs: argv[1] through argv[argc-1].  Open and start streaming each one:
        std::string rtspURL = Foscam19821W::GET_VIDEO_STREAM_H264.c_str();
        Boolean requestStreamingViaTCP = False;
        char const* username = Base64::secretMap[Base64::Foscam19821W_USERNAME_KEY].c_str();
        char const* password = Base64::secretMap[Base64::Foscam19821W_PASSWORD_KEY].c_str();
        char const* progName = "openIPCam";
        
        Authenticator* ourAuthenticator =  new Authenticator(username, password);
        GenericIPCam* rtspClient = IPCamRTSPClient::createNew(*env, rtspURL.c_str(), RTSP_CLIENT_VERBOSITY_LEVEL, progName);
        if (rtspClient == NULL)
        {
            *env << "Failed to create a RTSP client for URL \"" << rtspURL.c_str() << "\n";
            return;
        }
  
        unsigned int Cseq = 0;
        char volatile eventLoopWatchVariable = 0;
        Cseq = rtspClient->sendOptionsCommand(IPCamRTSPClient::continueAfterOPTIONS, ourAuthenticator);
        // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
        // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
        // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
        rtspClient->sendDescribeCommand(IPCamRTSPClient::continueAfterDESCRIBE, ourAuthenticator);
        //openURL(*env, "testOpenRTSP", url.c_str() );
        // All subsequent activity takes place within the event loop:
        env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
        // This function call does not return, unless, at some point in time, "eventLoopWatchVariable" gets set to something non-zero.
        
        // If you choose to continue the application past this point (i.e., if you comment out the "return 0;" statement above),
        // and if you don't intend to do anything more with the "TaskScheduler" and "UsageEnvironment" objects,
        // then you can also reclaim the (small) memory used by these objects by uncommenting the following code:
        env->reclaim(); env = NULL;
        delete scheduler; scheduler = NULL;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
    }
}
}