#include "HAAutoDiscovery.h"
#include "PubSubClient.h"

//define stringify function
#define xstr(a) str(a)
#define str(a) #a


class SpaAutoDiscovery
{
    public:
        enum DeviceClass
        {
            Temperature,
            Voltage
        };

        SpaAutoDiscovery(PubSubClient *mqttClient, String spaName, String serialNumber, String statusTopic, String availabilityTopic, String cmd_topic, String config_url) : mqttClient_(mqttClient)
        {
            spa_.spaName = spaName;
            spa_.spaSerialNumber = serialNumber;
            spa_.stateTopic = statusTopic;
            spa_.availabilityTopic = availabilityTopic;
            spa_.manufacturer = "sn_esp32";
            spa_.model = xstr(PIOENV);
            spa_.sw_version = xstr(BUILD_INFO);
            spa_.commandTopic = cmd_topic;
            spa_.configuration_url = config_url;
        }


        bool PublishTopic(String displayName, String valueTemplate, String propertyID, DeviceClass deviceClass, String entityCategory, String stateClass, String unitOfMeasure)
        {
            AutoDiscoveryInformationTemplate ADConf;
            String output;
            String discoveryTopic;
            
            ADConf.displayName = displayName;
            ADConf.valueTemplate = valueTemplate;
            ADConf.propertyId = propertyID;
            ADConf.deviceClass = deviceClass;  // Todo: use magic enum or something to convert to string
            ADConf.entityCategory = entityCategory;
            generateSensorAdJSON(output, ADConf, spa_, discoveryTopic, stateClass, unitOfMeasure);
            
            if (mqttClient_)
                return mqttClient_->publish(discoveryTopic.c_str(), output.c_str(), true);
            else
                return false;
                
        }

    private:
        PubSubClient *mqttClient_;
        SpaADInformationTemplate spa_;
      

};