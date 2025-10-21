#pragma once
#include <stdint.h>
#include <stdbool.h>

struct Server {
char ip[255];
int port;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);
bool ConvertStringToUI64(const char *str, uint64_t *val);
