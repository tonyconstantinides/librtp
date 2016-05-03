//
//  StreamErrorHandler.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/28/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//


#include "StreamErrorHandler.hpp"
#include "Common.hpp"

ApiStatus StreamErrorHandler::processErrorState(GstMessage* msg )
{
    gchar  *debug;
    GError *error;
    gst_message_parse_error (msg, &error, &debug);
    logerr() << "Debugging info: " << debug ;
    logerr() <<  "Error: " << error->message;
    logerr() << "From Element: " << GST_OBJECT_NAME (msg->src);
    // process the error
    // This code is a template for more sophisticated error handling
    // check the domain
    if (GST_CORE_ERROR ==  error->domain)
    {
        logerr() << "CORE Error detected!";
    
    } else if (GST_STREAM_ERROR == error->domain)
    {
        logerr() << "STREAM ERROR category detected!";
    
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

    } else if (GST_RESOURCE_ERROR == error->domain)
    {
        logerr() << "RESOURCE Error category detected!";
        if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_NOT_AUTHORIZED))
        {
         logerr() << "Authorization error to the IP Cam";
        } 
        else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_BUSY))
        {
            logerr() << "Cam is too busy to connect!";
        }
        else if (g_error_matches(error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_OPEN_READ))
        {
            logerr() << "Unable to open the cam for reading";
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