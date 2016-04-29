//
//  Base64.cpp
//  ipcams
//
//  Created by Tony Constantinides on 2/22/16.
//  Copyright © 2016 Tony Constantinides. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "Base64.hpp"

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";
// should start in the ipcams/build//lib/mac/debug
bool Base64::keysAlreadyCreated = false;
std::string Base64::encriptedData = "";
Base64::MapKeyValues  Base64::secretMap = {};
std::string Base64::IP_ADDRESS = "IP_ADDRESS";
std::string Base64::Foscam19810W = "Foscam19810W";
std::string Base64::Foscam19821W = "Foscam19821W";
std::string Base64::PASSWORD = "PASSWORD";
std::string Base64::USERNAME = "USERNAME";
std::string Base64::PORT = "PORT";
unsigned char Base64::EQUALS = '=';
unsigned char Base64::BETWEEN_SEP = ':';
unsigned char Base64::END_SEP = '?';
// I build this later in buildCompoundsKeys()
std::string Base64::Foscam19810W_IPADDRESS_KEY = Base64::Foscam19810W;
std::string Base64::Foscam19810W_PORT_KEY     =  Base64::Foscam19810W;
std::string Base64::Foscam19810W_USERNAME_KEY = Base64::Foscam19810W;
std::string Base64::Foscam19810W_PASSWORD_KEY = Base64::Foscam19810W;
std::string Base64::Foscam19821W_IPADDRESS_KEY = Base64::Foscam19821W;
std::string Base64::Foscam19821W_PORT_KEY      = Base64::Foscam19821W;
std::string Base64::Foscam19821W_USERNAME_KEY  = Base64::Foscam19821W;
std::string Base64::Foscam19821W_PASSWORD_KEY  = Base64::Foscam19821W;

bool Base64::detectFoscam19810WCam( std::string possibleKey)
{
    size_t pos = possibleKey.find(Foscam19810W);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::detectFoscam19821WCam( std::string possibleKey )
{
    size_t pos = possibleKey.find(Foscam19821W);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::detectIPAddress(std::string possibleKey )
{
    size_t pos = possibleKey.find(IP_ADDRESS);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::detectPort(std::string possibleKey)
{
    size_t pos = possibleKey.find(PORT);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::detectUserName(std::string possibleKey)
{
    size_t pos = possibleKey.find(USERNAME);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::detectPassword(std::string possibleKey)
{
    size_t pos = possibleKey.find(PASSWORD);
    return (pos == std::string::npos) ? false : true;
}
bool Base64::isKeyAlreadyAdded(std::string possibleKey)
{
    bool inMap = false;
    Base64::MapKeyValues::iterator it = Base64::secretMap.lower_bound(possibleKey);
    if (it != Base64::secretMap.end() && !(Base64::secretMap.key_comp()(possibleKey, it->first)))
    {
        inMap = true;
    }
    return inMap;
}
void Base64::addIPAddressKey(std::string possibleKey , std::string actualKey)
{
    try
    {
        size_t valueStart =  (actualKey.size());
        if (valueStart < possibleKey.size())
        {
            std::string value = possibleKey.substr(valueStart, possibleKey.size());
            //std::cout << std::endl<< "Value of " << actualKey <<  " is: " << value << std::endl;
            if (!isKeyAlreadyAdded(actualKey))
            {
                Base64::secretMap.emplace(actualKey, value);
            }
        }
        else
        {
            std::cout << std::endl << "Error value not set!" << std::endl;
        }
    } catch (std::exception exp)
    {
        std::cout << std::endl << exp.what() << std::endl;
    }
}
void Base64::addPortKey(std::string possibleKey, std::string actualKey)
{
    try
    {
        size_t valueStart = actualKey.size();
        if (valueStart < possibleKey.size())
        {
            std::string value = possibleKey.substr(valueStart, possibleKey.size());
            //std::cout << std::endl<< "Value of " << actualKey <<  " is: " << value << std::endl;
            if (!isKeyAlreadyAdded(actualKey))
            {
                Base64::secretMap.emplace(actualKey, value);
            }
        }
        else
        {
            std::cout << std::endl << "Error value not set!" << std::endl;
        }
    }
    catch (std::exception exp)
    {
        std::cout << std::endl << exp.what() << std::endl;
    }
}
void Base64::addUserName(std::string possibleKey, std::string actualKey)
{
    try {
        size_t valueStart = actualKey.size();
        if (valueStart < possibleKey.size())
        {
            std::string value = possibleKey.substr(valueStart, possibleKey.size());
            //std::cout << std::endl<< "Value of " << actualKey <<  " is: " << value << std::endl;
            if (!isKeyAlreadyAdded(actualKey))
            {
                Base64::secretMap.emplace(actualKey, value);
            }
        }
        else
        {
            std::cout << std::endl << "Error value not set!" << std::endl;
        }
    } catch (std::exception exp) {
        std::cout << std::endl << exp.what() << std::endl;
    }
}
void Base64::addPassword(std::string possibleKey, std::string actualKey)
{
    try {
        size_t valueStart = actualKey.size();
        if (valueStart < possibleKey.size())
        {
            std::string value = possibleKey.substr(valueStart, possibleKey.size());
            //std::cout << std::endl<< "Value of " << actualKey <<  " is: " << value << std::endl;
            if (!isKeyAlreadyAdded(actualKey))
            {
                Base64::secretMap.emplace(actualKey, value);
            }
        }
        else
        {
            std::cout << std::endl << "Error value not set!" << std::endl;
        }
    } catch (std::exception exp) {
        std::cout << std::endl << exp.what() << std::endl;
    }
}

void Base64::buildCompoundKeys()
{
    Foscam19810W_IPADDRESS_KEY += Base64::BETWEEN_SEP;
    Foscam19810W_IPADDRESS_KEY += Base64::IP_ADDRESS;
    Foscam19810W_IPADDRESS_KEY += Base64::EQUALS;
    Foscam19810W_PORT_KEY      += Base64::BETWEEN_SEP;
    Foscam19810W_PORT_KEY      += Base64::PORT;
    Foscam19810W_PORT_KEY      += Base64::EQUALS;
    Foscam19810W_USERNAME_KEY  += Base64::BETWEEN_SEP;
    Foscam19810W_USERNAME_KEY  += Base64::USERNAME;
    Foscam19810W_USERNAME_KEY  += Base64::EQUALS;
    Foscam19810W_PASSWORD_KEY  += Base64::BETWEEN_SEP;
    Foscam19810W_PASSWORD_KEY  += Base64::PASSWORD;
    Foscam19810W_PASSWORD_KEY  += Base64::EQUALS;
    Foscam19821W_IPADDRESS_KEY +=  Base64::BETWEEN_SEP;
    Foscam19821W_IPADDRESS_KEY +=  Base64::IP_ADDRESS;
    Foscam19821W_IPADDRESS_KEY +=  Base64::EQUALS;
    Foscam19821W_PORT_KEY      +=  Base64::BETWEEN_SEP;
    Foscam19821W_PORT_KEY      +=  Base64::PORT;
    Foscam19821W_PORT_KEY      +=  Base64::EQUALS;
    Foscam19821W_USERNAME_KEY  +=  Base64::BETWEEN_SEP;
    Foscam19821W_USERNAME_KEY  +=  Base64::USERNAME;
    Foscam19821W_USERNAME_KEY  +=  Base64::EQUALS;
    Foscam19821W_PASSWORD_KEY  +=  Base64::BETWEEN_SEP;
    Foscam19821W_PASSWORD_KEY  +=  Base64::PASSWORD;
    Foscam19821W_PASSWORD_KEY  +=  Base64::EQUALS;
    keysAlreadyCreated = true;
}

bool Base64::exists_test(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

bool Base64::alreadyLoaded()
{
    bool loaded = false;
    if (!keysAlreadyCreated)
        buildCompoundKeys();
    loaded = isKeyAlreadyAdded(Foscam19810W_IPADDRESS_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19810W_PORT_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19810W_USERNAME_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19810W_PASSWORD_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19821W_IPADDRESS_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19821W_PORT_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19821W_USERNAME_KEY);
    if (loaded)
        loaded = isKeyAlreadyAdded(Foscam19821W_PASSWORD_KEY);
    
    return loaded;
}

bool Base64::openSecrets()
{
    char * buffer;
    long size;
    // design to run out of the test directory
    
    std::fstream file;
    if (exists_test("secrets.dat"))
    {
        file.open("secrets.dat", std::fstream::in| std::fstream::ate | std::fstream::binary);
    }
    else if (exists_test("../../src/tests/secrets.dat"))
    {
        // running rom command line
        file.open("../../src/tests/secrets.dat", std::fstream::in| std::fstream::ate | std::fstream::binary);
    }
    else if (exists_test("../../../src/tests/secrets.dat"))
    {
        // running rom command line
        file.open("../../../src/tests/secrets.dat", std::fstream::in| std::fstream::ate | std::fstream::binary);
    }
    else if (exists_test("../../../../src/tests/secrets.dat"))
    {
        // running rom command line
        file.open("../../../../src/tests/secrets.dat", std::fstream::in| std::fstream::ate | std::fstream::binary);
    }
    if (file.is_open())
    {
        size = file.tellg();
        if (size > 0)
        {
            file.seekg (0, std::ios::beg);
            buffer = new char [size];
            file.read (buffer, size);
            file.close();
            std::cout << "the complete file is in a buffer";
            encriptedData = buffer;
            delete[] buffer;
        }
    } else {
        std::cout << "Cannot open  secret.dat file" << std::endl;
        return false;
    }
    return true;
}

void Base64::readSecrets()
{
    try {
        if (!alreadyLoaded()) {
           bool isOpened = openSecrets();
           if (isOpened)
               loadSecretMap();
        }
    } catch(std::exception exp)
    {
        std::cout << "Exception in Base64 " << exp.what() << std::endl;
    }

}

void Base64::loadSecretMap()
{
    // load up secretMap with the encoded data
    if (!Base64::encriptedData.empty())
    {
        // decode it
        std::vector<BYTE> data = Base64::base64_decode( Base64::encriptedData );
        std::string cameraData;
        size_t start = 0;
        // walk through the vector looking for the question mark which is the endline sperator
        for (auto letter : data )
        {
            size_t end = cameraData.size();
            if (letter == END_SEP)
            {
                std::string possibleKey = cameraData.substr(start, end);
                start = end + 1;
                // first detect camera type
                if (detectFoscam19810WCam(possibleKey))
                {
                    if (detectIPAddress(possibleKey))
                    {
                        addIPAddressKey(possibleKey, Foscam19810W_IPADDRESS_KEY);
                    }
                    if (detectPort(possibleKey))
                    {
                        addPortKey(possibleKey, Foscam19810W_PORT_KEY);
                    }
                    if (detectUserName(possibleKey))
                    {
                        addUserName(possibleKey, Foscam19810W_USERNAME_KEY);
                    }
                    if (detectPassword(possibleKey))
                    {
                        addPassword(possibleKey, Foscam19810W_PASSWORD_KEY);
                    }
                } else if (detectFoscam19821WCam(possibleKey))
                {
                    if (detectIPAddress(possibleKey))
                    {
                        addIPAddressKey(possibleKey, Foscam19821W_IPADDRESS_KEY);
                    }
                    if (detectPort(possibleKey))
                    {
                        addPortKey(possibleKey, Foscam19821W_PORT_KEY);
                    }
                    if (detectUserName(possibleKey))
                    {
                        addUserName(possibleKey, Foscam19821W_USERNAME_KEY);
                    }
                    if (detectPassword(possibleKey))
                    {
                        addPassword(possibleKey, Foscam19821W_PASSWORD_KEY);
                    }
                }
            }
            else
            {
                cameraData += letter;
            }
        }
    }

}


bool Base64::is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Base64::base64_encode(BYTE const* buf, unsigned int bufLen)
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


std::vector<BYTE> Base64::base64_decode(std::string& encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    BYTE char_array_4[4], char_array_3[3];
    std::vector<BYTE> ret;
    
    while (in_len-- && ( encoded_string[in_] != '=') && Base64::is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
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
        
        for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
    }
    
    return ret;
 }