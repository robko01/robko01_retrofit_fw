#include "EnvDump.h"
#include "EnvDumpConfig.h"

void EnvDump::print(Stream &out)
{
  out.println("==== ENV DUMP (device) ====");

  out.print("WIFI_SSID=");
#ifdef WIFI_SSID
  out.println(WIFI_SSID);
#else
  out.println("<unset>");
#endif

  out.print("WIFI_PASS=");
#ifdef WIFI_PASS
  out.println(WIFI_PASS);
#else
  out.println("<unset>");
#endif

  out.print("OTA_PORT=");
#ifdef OTA_PORT
  out.println(OTA_PORT);
#else
  out.println("<unset>");
#endif

  out.print("OTA_HOST_NAME=");
#ifdef OTA_HOST_NAME
  out.println(OTA_HOST_NAME);
#else
  out.println("<unset>");
#endif

  out.print("OTA_PASS_HASH=");
#ifdef OTA_PASS_HASH
  out.println(OTA_PASS_HASH);
#else
  out.println("<unset>");
#endif

  out.print("WG_ENDPOINT=");
#ifdef WG_ENDPOINT
  out.println(WG_ENDPOINT);
#else
  out.println("<unset>");
#endif

  out.print("WG_LOCAL_IP=");
#ifdef WG_LOCAL_IP
  out.println(WG_LOCAL_IP);
#else
  out.println("<unset>");
#endif

  out.print("WG_PRIVATE_KEY=");
#ifdef WG_PRIVATE_KEY
  out.println(WG_PRIVATE_KEY);
#else
  out.println("<unset>");
#endif

  out.print("WG_PUBLIC_KEY=");
#ifdef WG_PUBLIC_KEY
  out.println(WG_PUBLIC_KEY);
#else
  out.println("<unset>");
#endif

  out.print("PS4_MAC=");
#ifdef PS4_MAC
  out.println(PS4_MAC);
#else
  out.println("<unset>");
#endif

  out.println("================================");
}
