/*
ICS 167 snake project
Robert Filkin     67836837
Steven Tran       83611188
David Liu       76711634
Valeria Vikhliantseva 12831698

snake code adapted from tutorial code at http://snake-tutorial.zeespencer.com/
client code, web socket, and some server code adapted from the course website's chatroom file
*/
#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif /* BASE64_H */
