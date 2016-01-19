// GEP/SGP protocol: server side.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
#define __STDC_FORMAT_MACROS
#endif


#include "sgp_server.h"

#include <gep_server.h>

#include "sgp_protocol.h"  // for SGPProtocol, etc
#include "sgp.pb.h"  // for Command1, etc


SGPServer::SGPServer(int max_channel_num)
    : GepServer("sgp_server",
                max_channel_num,
                reinterpret_cast<void *>(this),
                new SGPProtocol(),
                &kSGPServerOps) {
}

SGPServer::SGPServer(int max_channel_num, int port)
    : GepServer("sgp_server",
                max_channel_num,
                reinterpret_cast<void *>(this),
                new SGPProtocol(port),
                &kSGPServerOps) {
}

SGPServer::~SGPServer() {
}
