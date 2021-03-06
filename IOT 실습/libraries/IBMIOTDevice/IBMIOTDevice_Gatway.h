#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>
#include <iotLabUtil.h>

const char*       publishTopic;
const char*       infoTopic;
const char*       commandTopic;
const char*       responseTopic;
const char*       manageTopic;
const char*       updateTopic;
const char*       rebootTopic;
const char*       resetTopic ;

ESP8266WebServer  server(80);
WiFiClient  espClient;
PubSubClient      client(espClient);
char              iot_server[100];
char              msgBuffer[EEPROM_LENGTH];
char              maskBuffer[EEPROM_LENGTH];
char              Topic[8][200];


void subscribeTopic(const char* topic) {
    if (client.subscribe(topic)) {
        Serial.printf("Subscription to %s OK\n", topic);
    } else {
        Serial.printf("Subscription to %s Failed\n", topic);
    }
}

void iot_connect() {
    StaticJsonBuffer<512> jsonOutBuffer;
    
    sprintf(topic_buff[0],"iot-2/type/%s/id/%s/evt/status/fmt/json",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[1],"iot-2/type/%s/id/%s/evt/info/fmt/json",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[2],"iot-2/type/%s/id/%s/cmd/+/fmt/+",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[3],"iotdm-1/type/%s/id/%s/response",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[4],"iotdevice-1/type/%s/id/%s/mgmt/manage",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[5],"iotdm-1/type/%s/id/%s/device/update",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[6],"iotdm-1/type/%s/id/%s/mgmt/initiate/device/reboot",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    sprintf(topic_buff[7],"iotdm-1/type/%s/id/%s/mgmt/initiate/device/factory_reset",(const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
    
    publishTopic=Topic[0];
    infoTopic=Topic[1];
    commandTopic=Topic[2];
    responseTopic=Topic[3];
    manageTopic=Topic[4];
    updateTopic=Topic[5];
    rebootTopic=Topic[6];
    resetTopic=Topic[7];
    
    while (!client.connected()) {
        sprintf(msgBuffer,"d:%s:%s:%s", (const char*)(*cfg)["edge"], (const char*)(*cfg)["devType"], (const char*)(*cfg)["devId"]);
        if (client.connect(msgBuffer) ){
            Serial.println("MQ connected");
        } else {
            Serial.printf("MQ Connection fail RC = %d, try again in 5 seconds\n", client.state());
            delay(5000);
        }
    }

    subscribeTopic(responseTopic);
    subscribeTopic(rebootTopic);
    subscribeTopic(resetTopic);
    subscribeTopic(updateTopic);
    subscribeTopic(commandTopic);

    JsonObject& meta = (*cfg)["meta"];
    JsonObject& root = jsonOutBuffer.createObject();
    JsonObject& d = root.createNestedObject("d");
    JsonObject& metadata = d.createNestedObject("metadata");
    for (JsonObject::iterator it=meta.begin(); it!=meta.end(); ++it) {
        //metadata[String(it->key)] = it->value.asString();
        metadata[String(it->key)] = it->value;
    }
    JsonObject& supports = d.createNestedObject("supports");
    supports["deviceActions"] = true;
    root.printTo(msgBuffer, sizeof(msgBuffer));
    Serial.printf("publishing device metadata: %s\n", msgBuffer);
    if (client.publish(manageTopic, msgBuffer)) {
        d.printTo(msgBuffer, sizeof(msgBuffer));
        String info = String("{\"info\":") + String(msgBuffer) + String("}");
        client.publish(infoTopic, info.c_str());
    }
}

void publishError(char *msg) {
    String payload = "{\"info\":{\"error\":";
    payload += "\"" + String(msg) + "\"}}";
    client.publish(infoTopic, (char*) payload.c_str());
    Serial.println(payload);
}

void handleIOTCommand(char* topic, JsonObject* root) {
    JsonObject& d = (*root)["d"];
    if (!strcmp(responseTopic, topic)) {        // strcmp return 0 if both string matches
        return;                                 // just print of response for now
    } else if (!strcmp(rebootTopic, topic)) {   // rebooting
        WiFi.disconnect();
        ESP.restart();
    } else if (!strcmp(resetTopic, topic)) {    // clear the configuration and reboot
        reset_config();
        WiFi.disconnect();
        ESP.restart();
    } else if (!strcmp(updateTopic, topic)) {
        root->prettyPrintTo(Serial); Serial.println();
        JsonArray& fields = d["fields"];
        for(JsonArray::iterator it=fields.begin(); it!=fields.end(); ++it) {
            JsonObject& field = *it;
            const char* fieldName = field["field"];
            if (strcmp (fieldName, "metadata") == 0) {
                JsonObject& fieldValue = field["value"];
                cfg->remove("meta");
                JsonObject& meta = cfg->createNestedObject("meta");
                for (JsonObject::iterator fv=fieldValue.begin(); fv!=fieldValue.end(); ++fv) {
                    meta[String(fv->key)] = fv->value.asString();
                }
                save_config_json();
            }
        }
    } else if (!strncmp(commandTopic, topic, strlen(commandTopic))) {
        if (d.containsKey("upgrade")) {
            String response = "{\"OTA\":{\"status\":";
            if(d.containsKey("server") && d.containsKey("port") && d.containsKey("uri")) {
              Serial.println("firmware upgrading");
               const char *fw_server = d["server"];
               int fw_server_port = atoi(d["port"]);
               const char *fw_uri = d["uri"];
                client.publish(infoTopic,"{\"info\":{\"upgrade\":\"Device will be upgraded.\"}}" );
               t_httpUpdate_return ret = ESPhttpUpdate.update(fw_server, fw_server_port, fw_uri);
               switch(ret) {
                  case HTTP_UPDATE_FAILED:
                        response += "\"[update] Update failed. http://" + String(fw_server);
                        response += ":"+ String(fw_server_port) + String(fw_uri) +"\"}}";
                        client.publish(infoTopic, (char*) response.c_str());
                        Serial.println(response);
                      break;
                  case HTTP_UPDATE_NO_UPDATES:
                        response += "\"[update] Update no Update.\"}}";
                        client.publish(infoTopic, (char*) response.c_str());
                        Serial.println(response);
                      break;
                  case HTTP_UPDATE_OK:
                      Serial.println("[update] Update ok."); // may not called we reboot the ESP
                      break;
               }
            } else {
                response += "\"OTA Information Error\"}}";
                client.publish(infoTopic, (char*) response.c_str());
                Serial.println(response);
            }
        } else if (d.containsKey("config")) {
            maskConfig(maskBuffer);
            String info = String("{\"info\":") + String(maskBuffer) + String("}");
            client.publish(infoTopic, info.c_str());
        }
    }
}

