#ifndef ESPOTAUPDATE_H
#define ESPOTAUPDATE_H
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "credentials.h"

#define FW_UPD_URL "http://"FW_UPD_HOST"/iot/iotupdate.php"

class ESPOTAUpdate: public ESP8266HTTPUpdate
{
public:
    ESPOTAUpdate(const char *name, const char *version, HardwareSerial *pSerial = NULL):
        m_pSerial(pSerial),
        m_sURL(FW_UPD_URL),
        m_sVersion(version)

    {
        m_sURL = m_sURL + "?img=" + name + ".bin&ver=" + version;
    }

    HTTPUpdateResult fwUpdate(int attempts = 1)
    {
        if (m_pSerial)
        {
            m_pSerial->printf("Fetching  %s\n", m_sURL.c_str());
            m_pSerial->flush();
        }

        HTTPUpdateResult res = update(m_sURL, m_sVersion);

        return res;
    }

private:
    String m_sURL;
    String m_sVersion;
    HardwareSerial *m_pSerial;


};

#endif // ESPOTAUPDATE_H
