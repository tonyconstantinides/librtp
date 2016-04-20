//
//  StreamManager.cpp
//  evartp
//
//  Created by Tony Constantinides on 4/14/16.
//  Copyright © 2016 EVA Automation, Inc. All rights reserved.
//

#include "StreamManager.hpp"


StreamManagerRef StreamManager::createStreamManager()
{
    return new StreamManager();
}