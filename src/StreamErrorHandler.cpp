//
//  StreamErrorHandler.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/28/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//


#include "StreamErrorHandler.hpp"
#include "Common.hpp"

using namespace Jetpack::Foundation;

ApiStatus StreamErrorHandler::processErrorState(GstMessage* msg )
{
    gchar  *debug;
    GError *error;
    gst_message_parse_error (msg, &error, &debug);
    logerr() << "Debugging info: " << debug ;
    logerr() <<  "Error: " << error->message;
    logerr() << "From Element: " << GST_OBJECT_NAME (msg->src);
     Notification::UserInfo info;
  
    // process the error
    // This code is a template for more sophisticated error handling
    // check the domain
    if (GST_CORE_ERROR ==  error->domain)
    {
        logerr() << "CORE Error detected!";
        category  = ErrorCategoryDetected::CORE;
        reported  = ErrorCategoryReported::MISC_ERROR;
        errorMsg  = "Internal error, incorrect build issue";
    } else if (GST_STREAM_ERROR == error->domain)
    {
        logerr() << "STREAM ERROR category detected!";
        category  = ErrorCategoryDetected::STREAM;
        reported =  ErrorCategoryReported::STREAM_STOPPED;
        errorMsg =   error->message;
        
        if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_FAILED))
        {
            logerr() << "Stream Error Failed detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_TYPE_NOT_FOUND))
        {
            logerr() << "Stream Error Type Not Found detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_WRONG_TYPE))
        {
            logerr() << "Stream Error Wrong Type detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_CODEC_NOT_FOUND))
        {
            logerr() << "Stream Error Codec Not Found detected!";
        }  else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_DECODE))
        {
            logerr() << "Stream Error Decode detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_ENCODE))
        {
            logerr() << "Stream Error Encode detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_DEMUX))
        {
            logerr() << "Stream Error Demux detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_MUX))
        {
            logerr() << "Stream Error Mux detected!";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_FORMAT))
        {
            logerr() << "Stream Error Format! detected";
        } else  if (g_error_matches(error, GST_STREAM_ERROR,  GST_STREAM_ERROR_DECRYPT))
        {
            logerr() << "Stream Error Decript detected!";
        }
    } else if (GST_LIBRARY_ERROR == error->domain)
    {
        logerr() << "LIBRARY Error rcategory detected!";
        category  = ErrorCategoryDetected::LIBRARY;
        reported      = ErrorCategoryReported::MISC_ERROR;
         errorMsg =   error->message;
    } else if (GST_RESOURCE_ERROR == error->domain)
    {
         logerr() << "RESOURCE Error category detected!";
         category  = ErrorCategoryDetected::RESOURCE;
         reported      = ErrorCategoryReported::MISC_ERROR;
         errorMsg =   error->message;
        
        if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_NOT_AUTHORIZED))
        {
         logerr() << "Authorization error to the IP Cam";
           reported      = ErrorCategoryReported::AUTH_FAILED;
        }
        else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_BUSY))
        {
            logerr() << "Cam is too busy to connect!";
        }
        else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_OPEN_READ))
        {
            logerr() << "Unable to open the cam for reading";
            reported      = ErrorCategoryReported::AUTH_FAILED;
        }
        else if (g_error_matches(error,GST_RESOURCE_ERROR,  GST_RESOURCE_ERROR_OPEN_READ_WRITE))
        {
            logerr() << "Unable to open the cam for reading and writing";
        } else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_READ))
        {
            logerr() << "Unable to open the cam for reading!";
        } else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_SEEK))
        {
            logerr() << "Unable to open the cam for seeking!";
        } else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_SYNC))
        {
            logerr() << "Error Sync when openning the Cam!";
        }
    }
    g_free (debug);
    g_error_free (error);
    return ApiStatus::FATAL_ERROR;
}