// Copyright Google Inc. Apache 2.0.

// GEP/SGP protocol implementation.

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
#define __STDC_FORMAT_MACROS
#endif


#include "sgp_protocol.h"

#include <cstddef>  // for NULL
#include <gep_common.h>  // for GepProtobufMessage

#ifndef GEP_LITE
#include "sgp.pb.h"  // for Command1, etc
#else
#include "sgp_lite.pb.h"  // for Command1, etc
#endif


SGPProtocol::SGPProtocol(int port)
    : GepProtocol(port) {
}

// TODO(chema): we must redefine these constants because the compiler is
// not smart enough to treat SGPProtocol::MSG_TAG_* as the corresponding
// literal
// \ref http://stackoverflow.com/questions/22867654/enum-vs-constexpr-for-actual-static-constants-inside-classes
constexpr uint32_t SGPProtocol::MSG_TAG_COMMAND_1;
constexpr uint32_t SGPProtocol::MSG_TAG_COMMAND_2;
constexpr uint32_t SGPProtocol::MSG_TAG_COMMAND_3;
constexpr uint32_t SGPProtocol::MSG_TAG_COMMAND_4;

uint32_t SGPProtocol::GetTag(const GepProtobufMessage *msg) {
  // TODO(chema): use send VFT map here instead of listing all the cases (?)
  if (dynamic_cast<const Command1 *>(msg) != NULL)
    return MSG_TAG_COMMAND_1;
  else if (dynamic_cast<const Command2 *>(msg) != NULL)
    return MSG_TAG_COMMAND_2;
  else if (dynamic_cast<const Command3 *>(msg) != NULL)
    return MSG_TAG_COMMAND_3;
  else if (dynamic_cast<const Command4 *>(msg) != NULL)
    return MSG_TAG_COMMAND_4;
  return 0;
}

GepProtobufMessage *SGPProtocol::GetMessage(uint32_t tag) {
  GepProtobufMessage *msg = NULL;
  switch (tag) {
    case MSG_TAG_COMMAND_1:
      msg = new Command1();
      break;
    case MSG_TAG_COMMAND_2:
      msg = new Command2();
      break;
    case MSG_TAG_COMMAND_3:
      msg = new Command3();
      break;
    case MSG_TAG_COMMAND_4:
      msg = new Command4();
      break;
  }
  return msg;
}
