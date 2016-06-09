	//
//  CamParamsEncryption_hpp
//  evartp
//
//  Created by Tony Constantinides on 4/19/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef CamParamsEncryption_hpp
#define CamParamsEncryption_hpp
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "Common.hpp"

class CamParamsEncryption
{
public:
    CamParamsEncryption();
     ~CamParamsEncryption();
    
    std::string base64_encode(BYTE const* buf, unsigned int bufLen);
    std::string base64_decode(std::string const& encoded_string);
    
    static bool is_base64(unsigned char c);
    // always return encoded values and let the client decode
    std::string  getCameraGuid();
    std::string  getUserName();
    std::string  getPassword();
    std::string  getHost();
    std::string   getPort();
    std::string  getAbsPath();
    std::string  getQueryParms();
    // only accepted encoded values
    ApiStatus    setCameraGuid(std::string guid);
    ApiStatus    setUserName(std::string username);
    ApiStatus    setPassword(std::string password);
    ApiStatus    setHost(std::string host);
    ApiStatus    setPort(std::string port);
    ApiStatus    setAbsPath(std::string path);
    ApiStatus    setQueryParms(std::string parms);
  private:
    std::mutex mutex;
    std::unique_lock<std::mutex> guard;
    std::string encriptedData;
    std::string cameraGuid;
    std::string  userName;
    std::string  password;
    std::string  host;
    std::string   port;
    std::string  abspath;
    std::string  queryParms;
};


#endif /* CamParmsEncription_hpp */
