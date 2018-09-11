/*
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/
#ifndef SHA1_H
#define SHA1_H

#if _WIN32
typedef unsigned long long u_int64_t;
#endif

#include <stdlib.h>
#include <string.h>

unsigned char *
SHA1(const unsigned char *d, size_t n, unsigned char *md);

#endif /* SHA1_H */
