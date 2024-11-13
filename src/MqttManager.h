#include <PubSubClient.h>
#include <WiFiClient.h>


class MqttManager : public PubSubClient
{
    public:
        MqttManager(WiFiClient &wifi) : PubSubClient(wifi)
        {}

        PubSubClient& setServer(String serverAddress, int port)
        {
            serverAddress_ = serverAddress;
            return PubSubClient::setServer(serverAddress_.c_str(), port);
        }
    
    private:
        String serverAddress_;
 
};