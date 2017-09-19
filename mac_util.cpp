#include <mac_util.h>

#if defined(WIN32) || defined(UNDER_CE)
#    include <windows.h>
#    if defined(UNDER_CE)
#        include <Iphlpapi.h>
#    else
#        pragma comment(lib, "Netapi32.lib")
#  endif
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(OpenBSD) || defined(linux) || defined(__APPLE__)
#    include <string.h>
#    include <stdint.h>
#    include <sys/types.h>
#    include <sys/socket.h>
#    include <ifaddrs.h>
#    include <net/if.h>
#    ifdef __linux__
#        include <sys/ioctl.h>
#        include <netinet/in.h>
#        include <unistd.h>
//    Match Linux to FreeBSD
#        define AF_LINK AF_PACKET
#    else
#        include <net/if_dl.h>
#    endif
#endif

long MACAddressUtility::GetMACAddress(unsigned char * result)
{
 // Fill result with zeroes
 memset(result, 0, 6);
 // Call appropriate function for each platform
#if defined(WIN32) || defined(UNDER_CE)
 return GetMACAddressMSW(result);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(OpenBSD) || defined(linux) || defined(__APPLE__)
 return GetMACAddressPosix(result);
#endif
 // If platform is not supported then return error code
 return -1;
}

#if defined(WIN32) || defined(UNDER_CE)

   typedef struct _ASTAT_
   {

      ADAPTER_STATUS adapt;
      NAME_BUFFER    NameBuff [30];

   }ASTAT, * PASTAT;

   ASTAT Adapter;

inline long MACAddressUtility::GetMACAddressMSW(unsigned char * result)
{

#if defined(UNDER_CE)
 IP_ADAPTER_INFO AdapterInfo[16]; // Allocate information
 DWORD dwBufLen = sizeof(AdapterInfo); // Save memory size of buffer
 if(GetAdaptersInfo(AdapterInfo, &dwBufLen;) == ERROR_SUCCESS)
 {
  memcpy(result, AdapterInfo->Address, 6);
 }
 else return -1;
#else
 NCB Ncb;
    UCHAR uRetCode;
    UCHAR nulladdr[6] = {0};
    char NetName[50];
    LANA_ENUM lenum;
    int i;

    memset( &Ncb;, 0, sizeof(Ncb) );
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer = (UCHAR *)&lenum;
    Ncb.ncb_length = sizeof(lenum);
    Netbios( &Ncb; );

    for(i=0; i < lenum.length ;i++)
    {
        memset( &Ncb;, 0, sizeof(Ncb) );
        Ncb.ncb_command = NCBRESET;
        Ncb.ncb_lana_num = lenum.lana[i];
        Netbios( &Ncb; );

        memset( &Ncb;, 0, sizeof(Ncb) );
        Ncb.ncb_command = NCBASTAT;
        Ncb.ncb_lana_num = lenum.lana[i];
        strcpy( (char *)Ncb.ncb_callname,  "*               " );
        Ncb.ncb_buffer = (unsigned char *) &Adapter;
        Ncb.ncb_length = sizeof(Adapter);
        uRetCode = Netbios( &Ncb; );
        if(( uRetCode == 0 ) && (memcmp(Adapter.adapt.adapter_address, nulladdr, 6) != 0))
        {
   memcpy(result, Adapter.adapt.adapter_address, 6);
            break;
  }
 }
#endif
 return 0;
}

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(OpenBSD) || defined(linux) || defined(__APPLE__)

long MACAddressUtility::GetMACAddressPosix(unsigned char * result)
{
    ifaddrs *ifap = NULL, *cur = NULL;
    uint8_t *haddr = NULL;
    uint8_t nulladdr[6] = {0};
    if ((getifaddrs(&ifap) != 0) || (ifap == NULL))
        return -1;
    for (cur = ifap; cur != NULL; cur = cur->ifa_next)
    {
        if ((cur->ifa_addr != NULL) && (cur->ifa_addr->sa_family == AF_LINK))
        {
#ifdef __linux__
            // Linux
            struct ifreq ifr;
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            ifr.ifr_addr.sa_family = AF_INET;
            strcpy(ifr.ifr_name, cur->ifa_name);
            ioctl(fd, SIOCGIFHWADDR, &ifr;);
            close(fd);
            haddr = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);
#else
            // Posix/FreeBSD/Mac OS
            sockaddr_dl* sdl = (struct sockaddr_dl *)cur->ifa_addr;
            haddr = reinterpret_cast<uint8_t*>(LLADDR(sdl));
#endif
            if((cur->ifa_flags & IFF_UP) && (bcmp(haddr, nulladdr, 6) != 0))
             break;
        }
    }
    bcopy(haddr , result, 6);
    freeifaddrs(ifap);
    return 0;
}
#endif
