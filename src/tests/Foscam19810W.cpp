//
//  Foscam19810W.cpp
//  ipcams
//
//  Created by Tony Constantinides on 2/3/16.
//  Copyright © 2016 Eva Automation. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <foundation/data.hpp>
#include <ctime>
#include <chrono>
#include <iostream>
#include <ostream>
#include <fstream>
#include <curl/curl.h>
#include "Foscam19810W.hpp"
#include "Base64.hpp"

#include "catch.hpp"
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

int videoStreamType = -1;

namespace IPCams {
    namespace FoscamMJPEG {
        static long bytesDownloaded;
        static long TotalBytes = 10000000;
size_t write_MPEGVideoFile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    if (bytesDownloaded >= TotalBytes )
        return 0L;
    size_t written = fwrite(ptr, size, nmemb, stream);
    bytesDownloaded += written;
    return written;
}
        
size_t write_SnapShot(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
 
void Foscam19810W::buildCompoundKeys()
{
    CAMERA_IP_PORT += Base64::secretMap[Base64::Foscam19810W_IPADDRESS_KEY] + ':';
    CAMERA_IP_PORT += Base64::secretMap[Base64::Foscam19810W_PORT_KEY] + '/';
    AUTH += "&user=";
    AUTH += Base64::secretMap[Base64::Foscam19810W_USERNAME_KEY];
    AUTH += "&pwd=";
    AUTH += Base64::secretMap[Base64::Foscam19810W_PASSWORD_KEY];
}
        

using namespace IPCams::FoscamMJPEG;
TEST_CASE( "Testing the Foscam 19810W cam", "[run]" )
{
    using namespace std::chrono;
    using namespace Jetpack::Foundation;
    using namespace std;
    using namespace IPCams::FoscamMJPEG;
    
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    SECTION("Loading the secret keys")
    {
        WARN("Executing checking the secret keys for Foscam19810W is already loaded");
        Base64::readSecrets();
        REQUIRE(Base64::secretMap.size() > 0);
        Foscam19810W::buildCompoundKeys();
    }
    SECTION("Testing Getting the Video Stream type")
    {
        WARN("Executing Getting the Video Stream type!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_GET_VIDEO_STREAM_TYPE;
        cout << "url is: " << url << endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
     
        res = curl_easy_perform(curl);
        CHECK(res != CURLE_WRITE_ERROR);
        REQUIRE(res == CURLE_OK);
        REQUIRE(videoStreamType >= 0);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Getting the video stream at Max Frame Rate to Max")
    {
        WARN("Executing Setting the Frame Rate to Max!");
        REQUIRE( curl != nullptr );
        std::string path = "/tmp/video_Max.mjpeg";
        FILE* file = fopen(  path.c_str(), "wb");
        REQUIRE( file != nullptr );
        std::string url = GET_VIDEOSTREAM_MAX_FPS;
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 0);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IPCams::FoscamMJPEG::write_MPEGVideoFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) ;
        res = curl_easy_perform(curl);
        // becaus ethe dowbloaded bytes are cut off at 100 mb it shoudl return a write error
        REQUIRE(res == CURLE_WRITE_ERROR);
        CHECK( res != CURLE_COULDNT_CONNECT);
        CHECK(res != CURLE_OK);
        double bytesDownLoaded;
        res = curl_easy_getinfo(curl,  CURLINFO_SPEED_DOWNLOAD, &bytesDownLoaded );
        REQUIRE(res == CURLE_OK);
        CHECK(bytesDownLoaded > 0);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(file);
    }
    SECTION("Testing Setting the Cam to Center on Startup")
    {
        WARN("Executing Setting the Cam to Center on Startup Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_CENTER_ON_START.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Cam to Stop Patrol")
    {
        WARN("Executing Setting the Cam to Stop Patrol Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_PATROL_TYPE_STOP.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Cam to Patrol Horizontally")
    {
        WARN("Executing Setting the Cam to Patrol Horizontally Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_PATROL_TYPE_HORIZONTAL.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Cam to Patrol Vertically")
    {
        WARN("Executing Setting the Cam to Patrol Vertically Test Case!");
        REQUIRE( curl != nullptr );
        std::string url =  CAMERA_PATROL_TYPE_VERTICAL.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Cam to Patrol Both Vertically and Horizontally")
    {
        WARN("Executing Setting the Cam to Patrol Both Vertically and Horizontally Test Case!");
        REQUIRE( curl != nullptr );
        std::string url =  CAMERA_PATROL_TYPE_BOTH.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Cam to Stop Patrol")
    {
        WARN("Executing Setting the Cam to Stop Patrol Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_PATROL_TYPE_STOP;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the IR off")
    {
        WARN("Executing Setting the IR OFF Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_IR_OFF;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the IR ON")
    {
        WARN("Executing Setting the IR ON Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_IR_ON;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setiing the Green Led Always Off")
    {
        WARN("Executing Setiing the Green Led Always Off Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERIA_GREEN_LED_ALWAYS_OFF;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Green Led to Blink when Connected")
    {
        WARN("Executing Setting the Green Led to Blink when Connected Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERIA_GREEN_LED_BLINKONLY_WHEN_CONNECTED;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Setting the Green Led to Blink when searching and when connected")
    {
        WARN("Executing etting the Green Led to Blink when searching and when connected Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERIA_GREEN_LED_BLINKONLY_WHEN_SEARCHING;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Cam SnapShot 320x240")
    {
        WARN("Executing Cam SnapShot 320x240 Case!");
        REQUIRE( curl != nullptr );
        std::string path = "/tmp/ShanpShot_320x240.jpg";
        FILE* file = fopen(  path.c_str(), "wb");
        std::string url = CAMERA_TAKE_SNAPSHOT_320x240;
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_SnapShot);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) ;
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(file);
    }
    SECTION("Testing Cam SnapShot 640x480")
    {
        WARN("Executing Cam SnapShot 640x480 Case!");
        char buf [512] = "pwd";
        REQUIRE( curl != nullptr );
        std::string path = "/tmp/SnapShot_640x480.jpg";
        FILE* file = fopen( path.c_str(), "wb");
        std::string url = CAMERA_TAKE_SNAPSHOT_640x480;
        /* ask libcurl to show us the verbose output */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_SnapShot);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file) ;
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(file);
    } // PTZ
    SECTION("Testing Cam PTZ - Center Cam")
    {
        WARN("Executing Cam PTZ - Center Cam Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_CENTER;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Cam PTZ - PAN LEFT")
    {
        WARN("Executing Cam PTZ - PAN LEFT Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_PAN_LEFT;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Cam PTZ - PAN RIGHT")
    {
        WARN("Executing Cam PTZ - PAN RIGHT Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_PAN_RIGHT;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Cam PTZ - PAN DOWN")
    {
        WARN("Executing Cam PTZ - PAN DOWN Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_PAN_DOWN;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing Cam PTZ - PAN UP")
    {
        WARN("Executing Cam PTZ - PAN UP Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_PAN_UP;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Setting Camera to default resolution")
    {
        WARN("Executing Setting Camera to default resolution Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_DEFAULT_RESOLUTION;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Setting Camera to VGA resolution")
    {
        WARN("Executing Setting Camera to VGA resolution Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_VGA_RESOLUTION;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Setting Camera to QVGA resolution")
    {
        WARN("Executing Setting Camera to QVGA resolution Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_QVGA_RESOLUTION;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Min Brightness")
    {
        WARN("Executing Cam Min Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MIN_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Max Brightness")
    {
        WARN("Executing Cam Max Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MAX_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 1 Brightness")
    {
        WARN("Executing Cam to 1 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_1_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 2 Brightness")
    {
        WARN("Executing Cam to 2 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_2_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 3 Brightness")
    {
        WARN("Executing Cam to 3 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_3_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 4 Brightness")
    {
        WARN("Executing Cam to 4 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_4_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 5 Brightness")
    {
        WARN("Executing Cam to 5 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_5_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 6 Brightness")
    {
        WARN("Executing Cam to 6 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_6_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 7 Brightness")
    {
        WARN("Executing Cam to 7 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_7_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 8 Brightness")
    {
        WARN("Executing Cam to 8 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_8_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 9 Brightness")
    {
        WARN("Executing Cam to 9 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_9_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 10 Brightness")
    {
        WARN("Executing Cam to 10 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_10_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 11 Brightness")
    {
        WARN("Executing Cam to 11 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_11_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 12 Brightness")
    {
        WARN("Executing Cam to 12 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_12_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 13 Brightness")
    {
        WARN("Executing Cam to 13 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_13_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 14 Brightness")
    {
        WARN("Executing Cam to 14 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_14_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - 15 Brightness")
    {
        WARN("Executing Cam to 15 Brightness Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_15_BRIGHTNESS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Min Constrat to Cam")
    {
        WARN("Executing Cam Min Constrast Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MIN_CONTRAST;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Mid Contrast to Cam")
    {
        WARN("Executing Cam Mid Constrast Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MID_CONTRAST;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Max Constrast to Cam")
    {
        WARN("Executing Cam Max Constrast Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MAX_CONTRAST;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Setting Camera Frequenzy to 50HZ")
    {
        WARN("Executing Cam frequenzy to 50HZ Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MODE_50HZ;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Setting Camera Frequenzy to 60HZ")
    {
        WARN("Executing Cam frequenzy to 60HZ Test Case!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MODE_60HZ;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Cam control to Outdoors")
    {
        WARN("Executing Cam control to Outdoors Test Case !");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MODE_OUTDOORS;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Cam Flip Mirror Normal")
    {
        WARN("Executing Cam Flip Mirror Normal Test!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_FLIP_MIRROR_NORMAL;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Cam Flip")
    {
        WARN("Executing Cam Flip Test!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_FLIP;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Cam Mirror")
    {
        WARN("Executing Cam Mirror Test!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_MIRROR;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION("Testing - Cam Flip and Mirror")
    {
        WARN("Executing Cam Flip and Mirror Test!");
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_CONTROL_FLIP_AND_MIRROR;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    SECTION( "testing Camera REBOOT" )
    {
        REQUIRE( curl != nullptr );
        std::string url = CAMERA_IP_PORT;
        url += "reboot.cgi?";
        url +=  AUTH;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        res = curl_easy_perform(curl);
        REQUIRE(res == CURLE_OK);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
}
}
}

