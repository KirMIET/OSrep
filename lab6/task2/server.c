#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "common.h"

struct FactorialArgs {
uint64_t begin;
uint64_t end;
uint64_t mod;
};

uint64_t Factorial(const struct FactorialArgs *args) {
uint64_t ans = 1;
for (uint64_t i = args->begin; i <= args->end; i++) {
ans = MultModulo(ans, i, args->mod);
}
return ans;
}

void *ThreadFactorial(void *args) {
struct FactorialArgs *fargs = (struct FactorialArgs *)args;
uint64_t *res = malloc(sizeof(uint64_t));
*res = Factorial(fargs);
return (void *)res;
}

int main(int argc, char **argv) {
int tnum = -1;
int port = -1;

while (true) {
static struct option options[] = {{"port", required_argument, 0, 0},
{"tnum", required_argument, 0, 0},
{0, 0, 0, 0}};
int option_index = 0;
int c = getopt_long(argc, argv, "", options, &option_index);
if (c == -1)
break;


switch (option_index) {
case 0:
  port = atoi(optarg);
  break;
case 1:
  tnum = atoi(optarg);
  break;
default:
  printf("Unknown option index %d\n", option_index);
}


}

if (port == -1 || tnum == -1) {
fprintf(stderr, "Usage: %s --port 20001 --tnum 4\n", argv[0]);
return 1;
}

int server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd < 0) {
perror("socket");
return 1;
}

struct sockaddr_in server;
server.sin_family = AF_INET;
server.sin_port = htons((uint16_t)port);
server.sin_addr.s_addr = htonl(INADDR_ANY);

int opt_val = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
perror("bind");
return 1;
}

if (listen(server_fd, 128) < 0) {
perror("listen");
return 1;
}

printf("Server listening on port %d\n", port);

while (true) {
struct sockaddr_in client;
socklen_t client_len = sizeof(client);
int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
if (client_fd < 0) {
perror("accept");
continue;
}


while (true) {
  unsigned int buffer_size = sizeof(uint64_t) * 3;
  char from_client[buffer_size];
  int readed = recv(client_fd, from_client, buffer_size, 0);
  if (readed <= 0)
    break;

  uint64_t begin, end, mod;
  memcpy(&begin, from_client, sizeof(uint64_t));
  memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
  memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

  printf("Received range [%llu, %llu], mod %llu\n", begin, end, mod);

  pthread_t threads[tnum];
  struct FactorialArgs args[tnum];
  uint64_t range = end - begin + 1;
  uint64_t step = range / tnum;
  uint64_t cur = begin;

  for (int i = 0; i < tnum; i++) {
    args[i].begin = cur;
    args[i].end = (i == tnum - 1) ? end : cur + step - 1;
    args[i].mod = mod;
    cur = args[i].end + 1;
    pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i]);
  }

  uint64_t total = 1;
  for (int i = 0; i < tnum; i++) {
    uint64_t *res;
    pthread_join(threads[i], (void **)&res);
    total = MultModulo(total, *res, mod);
    free(res);
  }

  printf("Result: %llu\n", total);
  send(client_fd, &total, sizeof(total), 0);
}

shutdown(client_fd, SHUT_RDWR);
close(client_fd);


}

close(server_fd);
return 0;
}
