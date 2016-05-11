//
//  IPStreamManager.hpp
//  evartp
//
//  Created by Tony Constantinides on 4/25/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#ifndef IPStreamManager_hpp
#define IPStreamManager_hpp

#include "Common.hpp"
#include "StreamErrorHandler.hpp"

// Just a base class that handles all errors handling but will e used as a tracing tool as well
class IPStreamManager
{
public:
    IPStreamManager();
    virtual ~IPStreamManager() = default;
    IPStreamManager(IPStreamManager const&)                      = delete;    // Copy construct
    IPStreamManager(IPStreamManager&&)                             = delete;   // Move construct
    IPStreamManager& operator=(IPStreamManager const&)             = delete;  // Copy assign
    IPStreamManager& operator=(IPStreamManager&&)                    = default;  // Move assign
    // needed by every derived class
    ApiStatus activateStream(bool ready)      { activeStream = ready; return ApiState; }
    ApiStatus validStreamMethod(bool valid) { validStreamingMethod = valid;  return ApiState;}
    std::string getName() { return name; }
    ApiStatus setName(std::string streamName) { name = streamName; return ApiState; }
    ApiStatus  errorApiState( const gchar * msg);
    ApiStatus  fatalApiState( const gchar* msg);
    ApiStatus   assignAuth( CamParmsEncriptionRef camAuthRef);
    static void printMsg(GstMessage* msg, const gchar*  msgType);
 
    virtual ApiStatus   connectToIPCam(CamParmsEncriptionRef camAuthRef) = 0;
    virtual ApiStatus   testConnection() = 0;
    // override in derived class
    virtual ApiStatus makeElements() = 0;
    virtual ApiStatus setupPipeLine() = 0;
    virtual ApiStatus startLoop() = 0;
    // test the ip connection before we try to use it
    virtual ApiStatus   createElements() = 0;
    virtual ApiStatus   addElementsToBin() = 0;
    virtual ApiStatus  linkElements()  = 0;
    virtual ApiStatus   setElementsProperties() = 0;
    virtual ApiStatus   addCallbacks() = 0;
    virtual ApiStatus   removeCallbacks() = 0;
    virtual ApiStatus   cleanUp() = 0;
    // unabmed properties used by derived classes like properties
    
protected:
    std::string  name;
    bool         activeStream;
    bool         validStreamingMethod;
    std::string  connection_url;
    ApiStatus    ApiState;
    static  short messageCount;
    CamParmsEncriptionRef authCamRef;

    // sensitive data stored as properties
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_cameraGuid;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_password;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_userName;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_host;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_port;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_absPath;
    class {
        std::string value;
    public:
        std::string & operator = (const std::string &i) { return value = i; }
        operator std::string() const { return value; }
    } crypto_queryParms;
    
};

#endif /* IPStreamManager_hpp */