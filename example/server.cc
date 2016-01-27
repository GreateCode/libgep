// simple server

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
#define __STDC_FORMAT_MACROS
#endif


#include "sgp_server.h"

#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <unistd.h>

#include "sgp_protocol.h"  // for SGPProtocol, etc
#ifndef GEP_LITE
#include "sgp.pb.h"  // for Command1, etc
#else
#include "sgp_lite.pb.h"  // for Command1, etc
#endif

class MyServer: public SGPServer {
 public:
  MyServer(int port)
    : SGPServer(16, port) {
    cnt1_ = 0;
    cnt2_ = 0;
    cnt3_ = 0;
    cnt4_ = 0;
    seen_client_ = false;
  }
  virtual ~MyServer() {};

  // protocol callbacks
  bool Recv(const Command1 &msg, int id) override;
  bool Recv(const Command2 &msg, int id) override;
  bool Recv(const Command3 &msg, int id) override;
  bool Recv(const Command4 &msg, int id) override;

  int cnt1_;
  int cnt2_;
  int cnt3_;
  int cnt4_;

  bool seen_client_;
};

bool MyServer::Recv(const Command1 &msg, int id) {
  cnt1_++;
  seen_client_ = true;
  return true;
}

bool MyServer::Recv(const Command2 &msg, int id) {
  cnt2_++;
  seen_client_ = true;
  return true;
}

bool MyServer::Recv(const Command3 &msg, int id) {
  cnt3_++;
  seen_client_ = true;
  return true;
}

bool MyServer::Recv(const Command4 &msg, int id) {
  cnt4_++;
  seen_client_ = true;
  return true;
}


typedef struct arg_values
{
  char *fifo;
  int cnt1;
  int cnt2;
  int cnt3;
  int cnt4;
  int nrem;
  char** rem;
} arg_values;


// default values
#define DEFAULT_CNT1 1
#define DEFAULT_CNT2 2
#define DEFAULT_CNT3 3
#define DEFAULT_CNT4 4


void usage(char *name)
{
  fprintf(stderr, "usage: %s [options]\n", name);
  fprintf(stderr, "where options are:\n");
  fprintf(stderr, "\t--fifo <fifo>:\tUse <fifo> for client-server "
          "communications\n");
  fprintf(stderr, "\t--cnt1 <cnt>:\tSend <cnt> Command1 messages [%i]\n",
      DEFAULT_CNT1);
  fprintf(stderr, "\t--cnt2 <cnt>:\tSend <cnt> Command2 messages [%i]\n",
      DEFAULT_CNT2);
  fprintf(stderr, "\t--cnt3 <cnt>:\tSend <cnt> Command3 messages [%i]\n",
      DEFAULT_CNT3);
  fprintf(stderr, "\t--cnt4 <cnt>:\tSend <cnt> Command4 messages [%i]\n",
      DEFAULT_CNT4);
  fprintf(stderr, "\t-h:\t\tHelp\n");
}

// For long options that have no equivalent short option, use a
// non-character as a pseudo short option, starting with CHAR_MAX + 1.
enum {
  CNT1OPTION = CHAR_MAX + 1,
  CNT2OPTION,
  CNT3OPTION,
  CNT4OPTION,
};


arg_values *parse_args(int argc, char** argv) {
  int c;
  // getopt_long stores the option index here
  int optindex = 0;

  static arg_values values;
  // set default values
  values.cnt1 = DEFAULT_CNT1;
  values.cnt2 = DEFAULT_CNT2;
  values.cnt3 = DEFAULT_CNT3;
  values.cnt4 = DEFAULT_CNT4;

  // long options
  static struct option longopts[] = {
    // matching options to short options
    {"help", no_argument, NULL, 'h'},
    {"fifo", required_argument, NULL, 'f'},
    // options without a short option match
    {"cnt1", required_argument, NULL, CNT1OPTION},
    {"cnt2", required_argument, NULL, CNT2OPTION},
    {"cnt3", required_argument, NULL, CNT3OPTION},
    {"cnt4", required_argument, NULL, CNT4OPTION},
    {NULL, 0, NULL, 0}
  };

  char *endptr;
  while ((c = getopt_long(argc, argv, "", longopts, &optindex)) != -1) {
    switch (c) {
      case 'f':
        values.fifo = optarg;
        break;

      case CNT1OPTION:
        values.cnt1 = strtol(optarg, &endptr, 0);
        if (*endptr != '\0')
          {
          usage(argv[0]);
          exit(-1);
          }
        break;

      case CNT2OPTION:
        values.cnt2 = strtol(optarg, &endptr, 0);
        if (*endptr != '\0')
          {
          usage(argv[0]);
          exit(-1);
          }
        break;

      case CNT3OPTION:
        values.cnt3 = strtol(optarg, &endptr, 0);
        if (*endptr != '\0')
          {
          usage(argv[0]);
          exit(-1);
          }
        break;

      case CNT4OPTION:
        values.cnt4 = strtol(optarg, &endptr, 0);
        if (*endptr != '\0')
          {
          usage(argv[0]);
          exit(-1);
          }
        break;

      case '?':
      case 'h':
        // getopt_long already printed an error message
        usage(argv[0]);
        exit(0);
        break;

      default:
        printf("Unsupported option: %c\n", c);
        usage(argv[0]);
    }
  }

  // store remaining arguments
  values.nrem = argc - optind;
  values.rem = argv + optind;

  return &values;
}

#define MAX_BUF 1024

int main(int argc, char **argv) {
  arg_values *values;

  // parse args
  values = parse_args(argc, argv);

  // create the server
  std::unique_ptr<MyServer> my_server;
  my_server.reset(new MyServer(0));

  // start the server
  if (my_server->Start() < 0) {
    fprintf(stderr, "error: cannot start server\n");
    return -1;
  }

  // get the port number
  int port = my_server->GetProto()->GetPort();
  fprintf(stdout, "server listening in port %i\n", port);

  // tell the client about the port
  char wbuf[MAX_BUF];
  sprintf(wbuf, "%i", port);
  int fd = open(values->fifo, O_WRONLY);
  write(fd, wbuf, strlen(wbuf));
  close(fd);

  // wait until we see a client message
  while (!my_server->seen_client_)
    std::this_thread::yield();

  // send some messages
  Command1 cmd1;
  for (int i = 0; i < values->cnt1; ++i)
    my_server->Send(cmd1);
  Command2 cmd2;
  for (int i = 0; i < values->cnt2; ++i)
    my_server->Send(cmd2);
  Command3 cmd3;
  for (int i = 0; i < values->cnt3; ++i)
    my_server->Send(cmd3);
  Command4 cmd4;
  for (int i = 0; i < values->cnt4; ++i)
    my_server->Send(cmd4);

  // give some time for all both sides to finish
  usleep(3000000);

  printf("results: %i %i %i %i\n", my_server->cnt1_, my_server->cnt2_,
      my_server->cnt3_, my_server->cnt4_);

  my_server->Stop();
  return 0;
}
