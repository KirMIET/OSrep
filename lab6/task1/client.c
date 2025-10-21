#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

struct Server {
char ip[255];
int port;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
uint64_t result = 0;
a = a % mod;
while (b > 0) {
if (b % 2 == 1)
result = (result + a) % mod;
a = (a * 2) % mod;
b /= 2;
}
return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
char *end = NULL;
unsigned long long i = strtoull(str, &end, 10);
if (errno == ERANGE) {
fprintf(stderr, "Out of uint64_t range: %s\n", str);
return false;
}
if (errno != 0)
return false;
*val = i;
return true;
}

int main(int argc, char **argv) {
uint64_t k = -1;
uint64_t mod = -1;
char servers_path[255] = {'\0'};

while (true) {
int current_optind = optind ? optind : 1;
static struct option options[] = {{"k", required_argument, 0, 0},
{"mod", required_argument, 0, 0},
{"servers", required_argument, 0, 0},
{0, 0, 0, 0}};
int option_index = 0;
int c = getopt_long(argc, argv, "", options, &option_index);
if (c == -1)
break;


switch (c) {
case 0:
  switch (option_index) {
  case 0:
    ConvertStringToUI64(optarg, &k);
    break;
  case 1:
    ConvertStringToUI64(optarg, &mod);
    break;
  case 2:
    memcpy(servers_path, optarg, strlen(optarg));
    break;
  default:
    printf("Unknown option index %d\n", option_index);
  }
  break;
default:
  fprintf(stderr, "getopt returned character code 0%o?\n", c);
}


}

if (k == -1 || mod == -1 || !strlen(servers_path)) {
fprintf(stderr, "Usage: %s --k 1000 --mod 97 --servers servers.txt\n",
argv[0]);
return 1;
}

FILE *fp = fopen(servers_path, "r");
if (!fp) {
perror("fopen");
return 1;
}

unsigned int servers_num = 0;
char line[256];
while (fgets(line, sizeof(line), fp))
servers_num++;
rewind(fp);

struct Server *to = malloc(sizeof(struct Server) * servers_num);
int idx = 0;
while (fgets(line, sizeof(line), fp)) {
sscanf(line, "%[^:]:%d", to[idx].ip, &to[idx].port);
idx++;
}
fclose(fp);

uint64_t step = k / servers_num;
uint64_t cur = 1;
uint64_t total = 1;

for (unsigned int i = 0; i < servers_num; i++) {
uint64_t begin = cur;
uint64_t end = (i == servers_num - 1) ? k : cur + step - 1;
cur = end + 1;


struct hostent *hostname = gethostbyname(to[i].ip);
if (hostname == NULL) {
  fprintf(stderr, "gethostbyname failed for %s\n", to[i].ip);
  exit(1);
}

struct sockaddr_in server;
server.sin_family = AF_INET;
server.sin_port = htons(to[i].port);
memcpy(&server.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);

int sck = socket(AF_INET, SOCK_STREAM, 0);
if (sck < 0) {
  perror("socket");
  exit(1);
}

if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
  perror("connect");
  exit(1);
}

char task[sizeof(uint64_t) * 3];
memcpy(task, &begin, sizeof(uint64_t));
memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

if (send(sck, task, sizeof(task), 0) < 0) {
  perror("send");
  exit(1);
}

uint64_t answer = 0;
if (recv(sck, &answer, sizeof(answer), 0) < 0) {
  perror("recv");
  exit(1);
}

printf("Server %s:%d -> partial %llu\n", to[i].ip, to[i].port, answer);
total = MultModulo(total, answer, mod);

close(sck);


}

printf("Final result: %llu\n", total);
free(to);
return 0;
}
