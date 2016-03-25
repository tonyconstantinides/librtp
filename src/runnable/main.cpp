//
//  main.cpp
//  runnable
//
//  Created by Tony Constantinides on 3/20/16.
//
//

#include <iostream>
#include <gio/gio.h>
#include <gst/gst.h>
#include <gst/rtsp/gstrtsp.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <gst/base/gstpushsrc.h>
#include <unistd.h>
#include <string.h>

#include <gst/rtsp-server/rtsp-client.h>
#include "RtspManager.hpp"
static GstDebugCategory* my_category;

//define GST_CAT_DEFAULT my_category

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    std::shared_ptr<RtspManager> manager =  RtspManager::createNewRtspManager();
    GST_DEBUG_CATEGORY_INIT (my_category, "my_code", 0, "This is the debug category for my code.");
    manager->connectToIPCam();
    manager->setupPipeLine();
    manager->startLoop();
 
    manager.reset();
    return 0;
}
