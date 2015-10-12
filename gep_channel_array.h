// GEP protocol: server's array of GepChannel.

#ifndef _GEP_CHANNEL_ARRAY_H_
#define _GEP_CHANNEL_ARRAY_H_

#include <google/protobuf/message.h>  // for Message
#include <memory>  // for shared_ptr
#include <mutex>  // for mutex
#include <string>  // for string
#include <sys/select.h>  // for fd_set
#include <vector>  // for vector

#include "gep_channel.h"  // for GepChannel
#include "gep_protocol.h"  // for GepProtocol (ptr only), etc


// Class used to manage an array of communication channels where protobuf
// messages can be sent back and forth.
class GepChannelArray {
 public:
  GepChannelArray(const std::string &name, GepProtocol *proto,
                  int max_channels, const GepVFT *ops, void *context);
  int OpenServerSocket();
  int AcceptConnection();
  int Stop();

  // Send a specific protobuf message to all GEP clients.
  // Returns status value (0 if all ok, -1 if any of the receivers failed).
  //
  // Note that the current GEP implementation assumes the protocol has
  // non-idempotent (zero-or-one) execution semantics. A message sent
  // may or may not arrive to the receiver. We provide an error when it
  // does not (both for the server and the client(s)).
  //
  // If the client needs to implement idempotent requests, it can use the
  // error detection and do it itself.
  //
  // The server is definitely not prepared yet to offer idempotence: That
  // would require identifying the clients, and keeping track of which
  // messages have been received and which not. We expect the protocols
  // implemented on top of GEP to not require idempotence in the
  // server-initiated operations.
  int SendMessage(const ::google::protobuf::Message &msg);

  void ClearGepChannelVector();
  // accessors
  int GetVectorSize();
  int GetVectorSocket(int i);

  // network management
  int GetServerSocket() const { return server_socket_; }
  void GetVectorReadFds(int *max_fds, fd_set *read_fds);
  void RecvData(fd_set *read_fds);

 private:
  int AddChannel(int socket);

  std::string name_;
  GepProtocol *proto_;  // not owned
  const GepVFT *ops_;  // VFT for the receiving side (not owned)
  void *context_;  // link to context (not owned)
  int max_channels_;
  int last_channel_id_;
  // GEP channel vector (one per client)
  std::vector<std::shared_ptr<GepChannel>> gep_channel_vector_;
  // mutex to protect gep_channel_vector_
  std::mutex gep_channel_vector_lock_;

  // socket accepting conns for new ctrl channels
  int server_socket_;
};

#endif  // _GEP_CHANNEL_ARRAY_H_
