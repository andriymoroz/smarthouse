#ifndef ESPOTAUPDATE_H
#define ESPOTAUPDATE_H
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "credentials.h"

#define FW_UPD_URL "http://"FW_UPD_HOST"/iot/iotupdate.php"

class ESPOTAUpdate: public ESP8266HTTPUpdate
{
public:
    ESPOTAUpdate(HardwareSerial *pSerial = NULL):
        m_pSerial(pSerial),
        m_sURL(FW_UPD_URL)
    {
    }

    HTTPUpdateResult fwUpdate(const char *name, const char *version, int attempts = 1)
    {
        m_sURL = m_sURL + "?img=" + name + "&ver=" + version;

        if (m_pSerial)
        {
            m_pSerial->printf("Fetching  %s\n", m_sURL.c_str());
            m_pSerial->flush();
        }

        HTTPUpdateResult res = update(m_sURL, version);

        return res;
    }

private:
    String m_sURL;
    HardwareSerial *m_pSerial;


};

#endif // ESPOTAUPDATE_H
