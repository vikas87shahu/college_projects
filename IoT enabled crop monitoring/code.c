#include <ESP8266WiFi.h> //to connect with nodemcu esp8266 we need this
#include <ArduinoJson.h> //making data in key value pair buffer
#include <PubSubClient.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include<DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClient  client;
#define SECRET_CH_ID 1709861           // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "GG44CW38MOKBMVCY"
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const char* ssid = "funwithiot";
const char* password = "air55208";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "a2paeyfooqggtv-ats.iot.ap-south-1.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key:
#define DHTTYPE DHT11   // DHT 11


// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUddQI3PsP/Sew1KolEfb3IITBsqswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMDMwMzE4NTUy
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANeWLI7C1FXggQeFG7xN
GCpjogd9ZA+4isOOx6+L2AXz0CtovFdelc2Dc2Xf5N0yBP9S3YObliq7RZuqDxDy
BSpt+bTxXLen9YTsVlfzXEKwUuugy5OR66+1x1bJUea5heVF+HDHb6Q3WnSAEFXE
UNUmTZfyHUtlQL7dBx4iZPaizUGV0sYl2EaV8zgkWVcl6ZyH/4BAktcXfHA0N5rE
VgyNyGor9uv5gJgKKcid1QKo2gg5/Jh5OHi4ddfnoxipI8C9wvxW9nNkBAsqEYgX
FSklqn/1fUJ/zZNGXdX+43OYc8CepE+pdRhkb/l6ydi4YEegwoTAJJ0UsyWz+88J
IPMCAwEAAaNgMF4wHwYDVR0jBBgwFoAUZTLhStsgmvPZvWWA10+CU24f8GYwHQYD
VR0OBBYEFGwRpqt6WGgu1rr2KMwnufiH9l5eMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAQLugnX43Sisax/XR0icQa1Pi8
VP6qrWwiDMpSbqRDyFZ+7MGd03DxZKYiTCJFQfFP0vsH+5hCwdJ98AxK9zNYthK5
DJ66a3SncyD2jHezK+nZkWMTNAFlLCiDkcL2GOfCzZNvuUupYhLwhds7o0LhIYsW
OnTuJQPQn8FjhLD0RJN7M6lrabDDzqvgiNWKXxwASGzNrPCgH2ZxfUpAzZCSt0T2
ONm5xgRszoNXihPfJOVWBgk487Y2Zs7TctayGSGmcfMLVODnyu8h63IPMosJg3gz
JZO1QUnQP9K7ebHTe8HEdroe5fjIlYMrV5C9TBGxrhkmT2ky0hqzS3zfsc6r
-----END CERTIFICATE-----
)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA15YsjsLUVeCBB4UbvE0YKmOiB31kD7iKw47Hr4vYBfPQK2i8
V16VzYNzZd/k3TIE/1Ldg5uWKrtFm6oPEPIFKm35tPFct6f1hOxWV/NcQrBS66DL
k5Hrr7XHVslR5rmF5UX4cMdvpDdadIAQVcRQ1SZNl/IdS2VAvt0HHiJk9qLNQZXS
xiXYRpXzOCRZVyXpnIf/gECS1xd8cDQ3msRWDI3Iaiv26/mAmAopyJ3VAqjaCDn8
mHk4eLh11+ejGKkjwL3C/Fb2c2QECyoRiBcVKSWqf/V9Qn/Nk0Zd1f7jc5hzwJ6k
T6l1GGRv+XrJ2LhgR6DChMAknRSzJbP7zwkg8wIDAQABAoIBAGMjCsBndW88Ovlz
S2rO5PLxOBxLnAzipuUArnDaVO9/y9V6Z0miFC8aaKGLUNoq20NyHq0bifWCgqLv
k9gOAzQcuHxtlV5bHTsP1zDAl0ePf8IdAiCNNJ1rcQxGraP04k6pW3K5M2yHi9AQ
5/IYwbcS0xGxY6k9GYXrbA9+b4E+SflfJ/3is3nJtIpsTj2oQi1X5+Y12xfAbuVE
ZzIuLFYeC5GY9Ff/YbQ4O/YJT9WaIBoujpzuB4syZXLxq5Ddp9Ryw4ljmSeYqF3P
mNwWJlbHzKeu2lf8KQ6pkYpRlNCJlRVFGHwyKcvwEka9OaUlvBXh+xUdH1HAbcrg
NY7OGKECgYEA9KTSX6MgVecl2S5TyzVkeRIEDaRsa+lg4M/NbzP3sAn+EbjDxNxS
lJdeJFkDhxFmvZkLn+FtStz4jGXbv2tmqv7MZxB57NLEbVVPkXKlQv57OesTTT8I
DBORuthCH2y2GNtC/1RqtZNQp3EtD5aBHMwwmxbG8mk/qYV4PvIq4tECgYEA4ZgO
b70W8gvgJFUncLLWkSNU3a7lCWEfylLYAtj4rgbBIza7nvYpiVeHd7pK/fVjDi+0
TB79JMUgiCVoXXeL+kGIxj8IR1jc9B7wlsRSWFaqlJpTANptJ4C4OmhGXiAXtmZ/
vWaX92M4Gqry11lrc7wDVthmWOeYhr93v3zKEIMCgYEA5G9/DNRaXjwfCEOM9MOf
VLmw1ThCESVzU5hWIE8rKdK6zbtz+b8s0RjiTBheBeIGyGU/CIRLNEtPPnHiFkRo
MqUHIfmtj6hiyCCP0qV1kS/kynkppciEcBpZylkirQsqod0jGx2tIm0zM5anV9XW
K5iyji5fiH0kd9tsI4Km7vECgYB8xVPeuy9wqd7LWYmZzF/eIDN09KFaoawqg81T
LdvErJmvtzrn/XAVtSKItrF5annhw8Ktg0oxRxHqGv5xC4iQ3iQeuYgDpgN2xZH9
YRHGKBDqZbxYsXZDre625ytcLdXPnTszbwXH3smN/lSXhK5ocWlLZB6fN2BlzooV
j5hWrwKBgH0Ru4dNCGS1AQ7j2oq20Zy9rXMr2kAa0wu1YOxa0wQRofpRvc0U6vd1
NRQDjmcMdT2Az3Vkp5ueTU8tV1Mspcu3LudupepDqrIW97vkkEDk8vS6WAuITDzH
loCxWFMWQ+DInGzJL2khSbBoZfT02Xb6DOhGNmYWHO4IAlk64F1c
-----END RSA PRIVATE KEY-----
)EOF";

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

#define dht_dpin 0
int relayInput = D5; // the input to the relay pin
DHT dht(dht_dpin, DHTTYPE); 

void setup() {
  Serial.begin(9600); Serial.println();
  dht.begin();
  timeClient.begin();
  timeClient.setTimeOffset(19800);
  
  Serial.println("ESP8266 AWS IoT Example");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayInput, OUTPUT); // initialize pin as OUTPUT
  digitalWrite(relayInput, LOW);
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());
  
  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
  setCurrentTime();
  ThingSpeak.begin(client); 
  // get current time, otherwise certificates are flagged as expired

}

unsigned long lastPublish;
int msgCount;

void loop() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  String formattedTime = timeClient.getFormattedTime();
  int group_id = 1;
  int device_id =1;
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String date =   String(monthDay) + "-" + String(currentMonth) + "-" +String(currentYear);
  String time_ =formattedTime ;
  pubSubCheckConnect();
  StaticJsonDocument<300> testDocument;
  float soil_moisture = analogRead(A0);
  soil_moisture = 100 - (soil_moisture/1024)*100;
  if(soil_moisture < 5)
  {
    digitalWrite(relayInput, HIGH); // turn relay on
    delay(5000);
    
  }
  else{
    digitalWrite(relayInput, LOW); // turn relay on
  }
  
 float humidityval = dht.readHumidity();
 float temperatureval = dht.readTemperature();
 ThingSpeak.setField(1,temperatureval );
  ThingSpeak.setField(2,humidityval);
  ThingSpeak.setField(3, soil_moisture);
  testDocument["group_id"]=group_id ;
  testDocument["device_id"]=device_id ; 
 testDocument["date"] = date;
 testDocument["time"] = time_ ;
  testDocument["temperature"] = temperatureval;
  testDocument["humidity"] = humidityval;
  testDocument["soil_moisture"] = soil_moisture;
String myStatus;
  if(temperatureval > 50){
    myStatus = String("It is very hot"); 
  }
  else if(humidityval > 30){
    myStatus = String("very humid");
  }
  else if(soil_moisture <10){
    myStatus = String("please water the plant");
  }
  else
  {
    myStatus = String("cool");
  }
  ThingSpeak.setStatus(myStatus);
  char buffer[300];
  
  serializeJsonPretty(testDocument, buffer);
  
   
  if (millis() - lastPublish > 15000) {
//    String msg = String("soil_moisture : ") + soil_moisture + String("%\nHumidity : ")+ humidityval + String("%\n  Temperature: " ) + temperatureval;
    pubSubClient.publish("outTopic", buffer);
    Serial.print("Published: "); 
    Serial.println(buffer);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
      Serial.println("Channel update successful.");
       }
     else{
       Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
//    Serial.println(timestamp);
    lastPublish = millis();
  }
}


void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(":");

  digitalWrite(relayInput, HIGH);
  delay(5000);


}
  


void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); 
Serial.print(asctime(&timeinfo));
}
