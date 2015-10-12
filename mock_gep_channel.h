// GEP Protocol: GEPChannel.

#ifndef _MOCK_GEP_CHANNEL_H_
#define _MOCK_GEP_CHANNEL_H_

#include <gmock/gmock.h>
#include <string>  // for string
#include <google/protobuf/message.h>  // for Message

#include "gep_channel.h"
#include "gep_protocol.h"  // for GepProtocol (ptr only), etc

// A mock version of GepChannel.
class MockGepChannel : public GepChannel {
 public:
  explicit MockGepChannel(void *context)
      : GepChannel(0, "", NULL, NULL, context) {
  }
  ~MockGepChannel() override {}
  MOCK_METHOD1(SendMessage, int(const ::google::protobuf::Message &msg));
};

#endif  // _MOCK_GEP_CHANNEL_H_
