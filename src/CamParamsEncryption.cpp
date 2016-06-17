//
//  CamParmsEncription.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/19/16.
//  Copyright © 2016 Bowers & Wilkins. All rights reserved.
//

#include "CamParamsEncryption.hpp"
#include <string>

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


CamParamsEncryption::CamParamsEncryption()
{
} 

CamParamsEncryption::~CamParamsEncryption()
{
}    

ApiStatus CamParamsEncryption::setCameraTitle(std::string Title)
{
    if (Title == "" || Title.length() == 0)
       return ApiStatus::FAIL;
    if (Title.c_str())
    { 
        cameraTitle.assign(Title);
    }
    return ApiStatus::OK;
}


ApiStatus CamParamsEncryption::setCameraGuid(std::string Guid)
{
    if (Guid == "" || Guid.length() == 0)
       return ApiStatus::FAIL;
    if (Guid.c_str())
    { 
        cameraGuid.assign(Guid);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setUserName(std::string Username)
{
    if (Username == "" || Username.length() == 0)
        return ApiStatus::FAIL;
    if (Username.c_str())
    {  
        userName.assign(Username);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setPassword(std::string Password)
{
    if (Password == "" || Password.length() == 0)
        return ApiStatus::FAIL;
    if (Password.c_str())
    {
        password.assign(Password);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setHost(std::string Host)
{
    if (Host == "" || Host.length() == 0)
        return ApiStatus::FAIL;
    if (Host.c_str())
    {    
        host.assign(Host);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setPort(std::string Port)
{
    if (Port == "" || Port.length() == 0)
        return ApiStatus::FAIL;
    if (Port.c_str())
    {    
        port.assign(Port);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setAbsPath(std::string Path)
{
    if (Path == "" || Path.length() == 0)
        return ApiStatus::FAIL;
    if (Path.c_str())
    {    
     abspath.assign(Path);
    }
    return ApiStatus::OK;
}

ApiStatus CamParamsEncryption::setQueryParms(std::string Parms)
{
    if (Parms == "" || Parms.length() == 0)
        return ApiStatus::FAIL;
    if (Parms.c_str())
    {    
        queryParms.assign(Parms);
    }
   return ApiStatus::OK;
}


std::string  CamParamsEncryption::getCameraTitle()
{
    return cameraTitle;
 }

std::string  CamParamsEncryption::getCameraGuid()
{
    return cameraGuid;
 }

std::string  CamParamsEncryption::getUserName()
{
    return userName;
}

std::string  CamParamsEncryption::getPassword()
{
    return password;
}

std::string  CamParamsEncryption::getHost()
{
    return host;
}			

std::string   CamParamsEncryption::getPort()
{
    return port;
}

std::string  CamParamsEncryption::getAbsPath()
{
    return abspath;
}

std::string  CamParamsEncryption::getQueryParms()
{
    return queryParms;
}

bool CamParamsEncryption::is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string CamParamsEncryption::base64_encode(BYTE const* buffer, unsigned int bufLen)
{
    char* buf = strdup((const char *)buffer);
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

std::string CamParamsEncryption::base64_decode(std::string const& encoded_string)
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



