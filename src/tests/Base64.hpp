//
//  Base64.hpp
//  ipcams
//
//  Created by Tony Constantinides on 2/22/16.
//  Copyright © 2016 Tony Constantinides. All rights reserved.
//

#ifndef Base64_hpp
#define Base64_hpp
#include <cstddef>
#include <string>
#include <vector>
#include <map>
typedef unsigned char BYTE;

class Base64
{
public:
    static std::string base64_encode(BYTE const* buf, unsigned int bufLen);
    static std::vector<BYTE> base64_decode(std::string& encoded_string);
    static void readSecrets();
    static void loadSecretMap();
    static bool is_base64(unsigned char c);
    static std::string encriptedData;
    typedef std::map<std::string, std::string>  MapKeyValues;
    static MapKeyValues secretMap;
    
    // all these funcs are used to load the data into the secretMap
    static bool detectFoscam19810WCam( std::string possibleKey);
    static bool detectFoscam19821WCam( std::string possibleKey );
    static bool detectIPAddress(std::string possibleKey );
    static bool detectPort(std::string possibleKey);
    static bool detectUserName(std::string possibleKey);
    static bool detectPassword(std::string possibleKey);
    static bool isKeyAlreadyAdded(std::string possibleKey);
    static void addIPAddressKey(std::string possibleKey , std::string actualKey);
    static void addPortKey(std::string possibleKey, std::string actualKey);
    static void addUserName(std::string possibleKey, std::string actualKey);
    static void addPassword(std::string possibleKey, std::string actualKey);
    static void buildCompoundKeys();
    static bool exists_test(const std::string& name);
    static bool alreadyLoaded();
    
    static std::string IP_ADDRESS;
    static std::string Foscam19810W;
    static std::string Foscam19821W;
    static std::string PASSWORD;
    static std::string USERNAME;
    static std::string PORT;
    static unsigned char EQUALS;
    static unsigned char BETWEEN_SEP;
    static unsigned char END_SEP;
    static std::string Foscam19810W_IPADDRESS_KEY;
    static std::string Foscam19810W_PORT_KEY;
    static std::string Foscam19810W_USERNAME_KEY;
    static std::string Foscam19810W_PASSWORD_KEY;
    static std::string Foscam19821W_IPADDRESS_KEY;
    static std::string Foscam19821W_PORT_KEY;
    static std::string Foscam19821W_USERNAME_KEY;
    static std::string Foscam19821W_PASSWORD_KEY;
    
    static bool openSecrets();
    static bool   keysAlreadyCreated;
    
};

#endif /* Base64_hpp */
