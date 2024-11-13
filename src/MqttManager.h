#include <PubSubClient.h>
#include <WiFiClient.h>
#include <TimeLib.h>
#include "SpaInterface.h"
#include "SpaUtils.h"

using namespace std::placeholders;

class MqttManager : public PubSubClient
{
    public:
        MqttManager(WiFiClient &wifi, SpaInterface &si) : PubSubClient(wifi), si_(si)
        {
            setCallback(std::bind(&MqttManager::mqttCallback, this, _1, _2, _3));  // This is ok in setup as it never changes
            setBufferSize(2048);        // This is ok in setup as it never changes
        }

        bool connect(String clientID, String username, String password, String willTopic="", int willQoS=0, bool willRetain=false, String willMessage="", bool cleanSession=true)
        {
            username_ = username;
            password_ = password;
            willTopic_= willTopic;
            willMessage_ = willMessage;

            return PubSubClient::connect(clientID.c_str(), username_.c_str(), password_.c_str(), willTopic_.c_str(), willQoS, willRetain, willMessage.c_str());
        }

        PubSubClient& setServer(String serverAddress, int port)
        {
            serverAddress_ = serverAddress;
            return PubSubClient::setServer(serverAddress_.c_str(), port);
        }

  
    
    private:

        void mqttCallback(char* topic, byte* payload, unsigned int length) {
            String t = String(topic);

            String p = "";
            for (uint x = 0; x < length; x++) {
                p += char(*payload);
                payload++;
            }

            debugD("MQTT subscribe received '%s' with payload '%s'",topic,p.c_str());

            String property = t.substring(t.lastIndexOf("/")+1);

            debugI("Received update for %s to %s",property.c_str(),p.c_str());

            if (property == "temperatures_setPoint") {
                si_.setSTMP(int(p.toFloat()*10));
            } else if (property == "heatpump_mode") {
                si_.setHPMP(p);
            } else if (property == "pump1") {
                si_.setRB_TP_Pump1(p=="OFF"?0:1);
            } else if (property == "pump2") {
                si_.setRB_TP_Pump2(p=="OFF"?0:1);
            } else if (property == "pump3") {
                si_.setRB_TP_Pump3(p=="OFF"?0:1);
            } else if (property == "pump4") {
                si_.setRB_TP_Pump4(p=="OFF"?0:1);
            } else if (property == "pump5") {
                si_.setRB_TP_Pump5(p=="OFF"?0:1);
            } else if (property == "heatpump_auxheat") {
                si_.setHELE(p=="OFF"?0:1);
            } else if (property == "status_datetime") {
                tmElements_t tm;
                tm.Year=CalendarYrToTm(p.substring(0,4).toInt());
                tm.Month=p.substring(5,7).toInt();
                tm.Day=p.substring(8,10).toInt();
                tm.Hour=p.substring(11,13).toInt();
                tm.Minute=p.substring(14,16).toInt();
                tm.Second=p.substring(17).toInt();
                si_.setSpaTime(makeTime(tm));
            } else if (property == "lights_state") {
                si_.setRB_TP_Light(p=="ON"?1:0);
            } else if (property == "lights_effect") {
                si_.setColorMode(p);
            } else if (property == "lights_brightness") {
                si_.setLBRTValue(p.toInt());
            } else if (property == "lights_color") {
                int pos = p.indexOf(',');
                if ( pos > 0) {
                int value = p.substring(0, pos).toInt();
                si_.setCurrClr(si_.colorMap[value/15]);
                }
            } else if (property == "lights_speed") {
                si_.setLSPDValue(p);
            } else if (property == "blower_state") {
                si_.setOutlet_Blower(p=="OFF"?2:0);
            } else if (property == "blower_speed") {
                if (p=="0") si_.setOutlet_Blower(2);
                else si_.setVARIValue(p.toInt());
            } else if (property == "blower_mode") {
                si_.setOutlet_Blower(p=="Variable"?0:1);
            } else if (property == "sleepTimers_1_state" || property == "sleepTimers_2_state") {
                int member=0;
                for (const auto& i : si_.sleepSelection) {
                if (i == p) {
                    if (property == "sleepTimers_1_state")
                    si_.setL_1SNZ_DAY(si_.sleepBitmap[member]);
                    else if (property == "sleepTimers_2_state")
                    si_.setL_2SNZ_DAY(si_.sleepBitmap[member]);
                    break;
                }
                member++;
                }
            } else if (property == "sleepTimers_1_begin") {
                si_.setL_1SNZ_BGN(convertToInteger(p));
            } else if (property == "sleepTimers_1_end") {
                si_.setL_1SNZ_END(convertToInteger(p));
            } else if (property == "sleepTimers_2_begin") {
                si_.setL_2SNZ_BGN(convertToInteger(p));
            } else if (property == "sleepTimers_2_end") {
                si_.setL_2SNZ_END(convertToInteger(p));
            } else if (property == "status_spaMode") {
                si_.setMode(p);
            } else {
                debugE("Unhandled property - %s",property.c_str());
            }
        }
        
        
        String serverAddress_;
        String username_;
        String password_;
        String willTopic_;
        String willMessage_;
        SpaInterface &si_;
 
};