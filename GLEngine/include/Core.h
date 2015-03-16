#pragma once

#define NULL 0
#define EDITOR

#define SAFE_DELETE(X) if (X) {delete X; X = NULL; }
#define SAFE_DELETE_ARRAY(X) if (X) { delete[] X; X = NULL; }
#define BEGIN_NAMESPACE(X) namespace X {
#define END_NAMESPACE(X) }
#define BEGIN_UNNAMED_NAMESPACE() namespace {
#define END_UNNAMED_NAMESPACE() }

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef long long int64;
typedef unsigned long long uint64;

void print(const char* format, ...);
void printNow(const char* format, ...);