//
//  CamParmsEncription.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/19/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "CamParmsEncription.hpp"

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


CamParmsEncription::CamParmsEncription()
{
}

ApiStatus CamParmsEncription::setCameraGuid(std::string guid)
{
    if (guid == "" || guid.length() == 0)
       return ApiStatus::FAIL;
    this->cameraGuid = guid;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setUserName(std::string username)
{
    if (username == "" || username.length() == 0)
        return ApiStatus::FAIL;
    this->userName = username;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setPassword(std::string password)
{
    if (password == "" || password.length() == 0)
        return ApiStatus::FAIL;
    this->password = password;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setHost(std::string host)
{
    if (host == "" || host.length() == 0)
        return ApiStatus::FAIL;
     this->host = host;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setPort(std::string port)
{
    if (port == "" || port.length() == 0)
        return ApiStatus::FAIL;
    this->port = port;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setAbsPath(std::string path)
{
    if (path == "" || path.length() == 0)
        return ApiStatus::FAIL;
    this->abspath = path;
    return ApiStatus::OK;
}

ApiStatus CamParmsEncription::setQueryParms(std::string parms)
{
    if (parms == "" || parms.length() == 0)
        return ApiStatus::FAIL;
    this->queryParms = parms;
    return ApiStatus::OK;
}

std::string  CamParmsEncription::getCameraGuid()
{
    return cameraGuid;
 }

std::string  CamParmsEncription::getUserName()
{
    return userName;
}

std::string  CamParmsEncription::getPassword()
{
    return password;
}

std::string  CamParmsEncription::getHost()
{
    return host;
}			

std::string   CamParmsEncription::getPort()
{
    return port;
}

std::string  CamParmsEncription::getAbsPath()
{
    return abspath;
}

std::string  CamParmsEncription::getQueryParms()
{
    return queryParms;
}

bool CamParmsEncription::is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CamParmsEncription::base64_encode(BYTE const* buf, unsigned int bufLen)
{
    std::string ret;
    int i = 0;
    int j = 0;
    BYTE char_array_3[3];
    BYTE char_array_4[4];
    
    while (bufLen--) {
        char_array_3[i++] = *(buf++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
        
        while((i++ < 3))
            ret += '=';
    }
    
    return ret;
}

std::string CamParmsEncription::base64_decode(std::string const& encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    
    while (in_len-- && ( encoded_string[in_] != '=') && this->is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    
    return ret;
}



