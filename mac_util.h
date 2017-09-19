//За код спасибо добрым людям отсюда:
//https://wxwidgets.info/cross-platform-way-of-obtaining-mac-address-of-your-machine
//http://www.paste.org/10239

#pragma once

class MACAddressUtility
{
public:
 static long GetMACAddress(unsigned char * result);
private:
#if defined(WIN32) || defined(UNDER_CE)
 static long GetMACAddressMSW(unsigned char * result);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(OpenBSD) || defined(linux) || defined(__APPLE__)
 static long GetMACAddressPosix(unsigned char * result);
#endif
};
