#ifndef MATRIX_H
#define MATRIX_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

//#define N 12
#define SPACER  1
#define CHAR_WIDTH  (5 + SPACER)

String form =
        "<p>"
        "<center>"
        "<h1>ESP8266 Web Server</h1>"
        "<form action='msg'><p>Type your message <input type='text' name='msg' size=100 autofocus> <input type='submit' value='Submit'></form>"
        "<form action='delay'><p>Scroll speed (delay) <input type='text' name='delay' size=10> <input type='submit' value='Set'></form>"
        "</center>";

static inline unsigned get_ccount(void)
{
    unsigned r;
    asm volatile ("rsr %0, ccount" : "=r"(r));
    return r;
}

void handle_msg();
void handle_delay();
void handle_root();

class DotMatrix
{
    friend void handle_msg();
    friend void handle_delay();
    friend void handle_root();
public:
    DotMatrix(int pinCs, int numberOfHorizontalDisplays, int numberOfVerticalDisplays):
        m_delay(10000000),
        m_bRefresh(false),
        m_server(80),
        m_msgLen(0),
        m_matrix(pinCs, numberOfHorizontalDisplays, numberOfVerticalDisplays)
    {
        m_pInstance = this;
        m_lastTick = 0;

        m_matrix.setIntensity(1); // Use a value between 0 and 15 for brightness
        for (int i = 0; i < m_matrix.width()/8; i++)
        {
            m_matrix.setRotation(i,1);
            m_matrix.setPosition(i, i, 0); // The first display is at <0, 7>
        }


        // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
        m_server.on("/", handle_root);
        m_server.on("/msg", handle_msg);                  // And as regular external functions:
        m_server.on("/delay", handle_delay);                  // And as regular external functions:
        m_server.begin();                                 // Start the server


        char ip[16];
        sprintf(ip, "%3d.%3d.%1d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
        m_decodedMsg = ip;
        Serial.println(WiFi.localIP());
    }

    void loop(void)
    {
        int tick = get_ccount() & 0x7FFFFFFF;
        if (abs(tick - m_lastTick) < m_delay)
        {
            return;
        }
        //Serial.printf("%d - %d = %d < %d\n", tick, m_lastTick, abs(tick-m_lastTick), m_delay);
        m_lastTick = tick;

        m_server.handleClient();
        if (m_bRefresh)
        {
            // start over
            m_bRefresh = false;
            m_msgPos = 0;
        }

        if (m_msgPos == 0)
        {
            //Serial.printf("Matrix loop...T=%u\n", get_ccount());
            m_msgLen = CHAR_WIDTH * m_decodedMsg.length() + m_matrix.width() - 1 - SPACER;
        }

        m_matrix.fillScreen(LOW);

        int letter = m_msgPos / CHAR_WIDTH;
        int x = (m_matrix.width() - 1) - m_msgPos % CHAR_WIDTH;
        int y = 0;//(m_matrix.height() - 8) / 2; // center the text vertically

        while ( x + CHAR_WIDTH - SPACER >= 0 && letter >= 0 )
        {
            if ( letter < m_decodedMsg.length() ) {
                m_matrix.drawChar(x, y, m_decodedMsg[letter], HIGH, LOW, 1);
            }

            letter--;
            x -= CHAR_WIDTH;
        }

        m_matrix.write(); // Send bitmap to display
        if (m_msgPos++ >= m_msgLen)
        {
            m_msgPos = 0;
        }

    }


    void setDelay(int ms)
    {
        m_delay = ms;
    }

    static DotMatrix *getInstance()
    {
        return m_pInstance;
    }

    void setMessage(const char *msg)
    {
        m_decodedMsg = msg;
        // Restore special characters that are misformed to %char by the client browser
        //m_decodedMsg.replace("+", "+");
        m_decodedMsg.replace("%21", "!");
        m_decodedMsg.replace("%22", "");
        m_decodedMsg.replace("%23", "#");
        m_decodedMsg.replace("%24", "$");
        m_decodedMsg.replace("%25", "%");
        m_decodedMsg.replace("%26", "&");
        m_decodedMsg.replace("%27", "'");
        m_decodedMsg.replace("%28", "(");
        m_decodedMsg.replace("%29", ")");
        m_decodedMsg.replace("%2A", "*");
        m_decodedMsg.replace("%2B", "+");
        m_decodedMsg.replace("%2C", ",");
        m_decodedMsg.replace("%2F", "/");
        m_decodedMsg.replace("%3A", ":");
        m_decodedMsg.replace("%3B", ";");
        m_decodedMsg.replace("%3C", "<");
        m_decodedMsg.replace("%3D", "=");
        m_decodedMsg.replace("%3E", ">");
        m_decodedMsg.replace("%3F", "?");
        m_decodedMsg.replace("%40", "@");

        m_bRefresh = true;
    }

private:
    static DotMatrix *m_pInstance;

    void handleRoot()
    {
        m_server.send(200, "text/html", form);
    }

    void handleMsg()
    {
        m_matrix.fillScreen(LOW);
        m_server.send(200, "text/html", form);    // Send same page so they can send another msg
        setMessage(m_server.arg("msg").c_str());
    }

    void handleDelay()
    {
        int delay;
        m_matrix.fillScreen(LOW);
        m_server.send(200, "text/html", form);

        if ((delay = m_server.arg("delay").toInt()) > 0)
        {
            m_delay = delay * 1000000;
        }
    }

    int m_lastTick;
    int m_msgPos;
    int m_msgLen;
    unsigned int m_delay;
    int m_bRefresh;
    String m_decodedMsg;
    ESP8266WebServer m_server;
    Max72xxPanel m_matrix;
};



// ******************* String form to sent to the client-browser ************************************


//long period;
//int offset=1,refresh=0;
//int pinCS = 0; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
//int numberOfHorizontalDisplays = N;
//int numberOfVerticalDisplays = 1;

void handle_msg()
{
    DotMatrix::getInstance()->handleMsg();
}

void handle_delay()
{
    DotMatrix::getInstance()->handleDelay();
}

void handle_root()
{
    DotMatrix::getInstance()->handleRoot();
}

DotMatrix *DotMatrix::m_pInstance = NULL;

#endif // MATRIX_H
