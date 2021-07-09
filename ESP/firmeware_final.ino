//libs necessarias
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"
#include <PubSubClient.h>
//-----------------------------------------------------------------------------------------------------------------//
//Relé para a bomba
#define bombapararegar 33
//----------------------------------------------------------------------------------------------------------------//
// Sensor DHT11
#define dhtpin 5
#define DHTTYPE DHT11
DHT dht(dhtpin, DHTTYPE,15);
//----------------------------------------------------------------------------------------------------------------//
//Sensor de temperatura DS18b20
#define sensor 16
OneWire onewire(sensor);
DallasTemperature sensors(&onewire);
DeviceAddress sensor1;
//---------------------------------------------------------------------------------------------------------------//
//LDR para incidencia
#define LDR 34
//---------------------------------------------------------------------------------------------------------------//
// Monitor Oled SSD1306 yellow and blue
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define endereco_do_display 0x3C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//--------------------------------------------------------------------------------------------------------------//
//WIFI
const char* ssid       = "SEU_SSID";
const char* password   = "SUA_SENHA";
WiFiClient wifiClient;
//--------------------------------------------------------------------------------------------------------------//
//RTC  do esp32
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3*3600;
const int   daylightOffset_sec = 0;
//-------------------------------------------------------------------------------------------------------------//
//Sensor de umidade do solo
#define sensor_humidade_solo 32
//-------------------------------------------------------------------------------------------------------------//
//MQTT Client
#define ID_MQTT  "espcuidador"
const char* BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;
PubSubClient MQTT(wifiClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(bombapararegar,OUTPUT);
  inicia_dht();
  inicia_sensor_temp();
  start_wifi();
  set_horario();
  set_display();
  set_MQTT();
  
   
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if(get_umidade_solo() >3000){
    rega_planta();
  }
  mantemConexoes();
  monta_dashboard();
  MQTT.loop();
}
//------------------------------------------------------------------------------------------------------------//
//Funções
//função que inicia o sensor e mostra o endereço
void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
void inicia_sensor_temp(){
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
     Serial.println("Sensores nao encontrados !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
  Serial.println();
  Serial.println();
  }
//Função que retorna a temperatura do DS18b20
float get_temp(){
  sensors.requestTemperatures();
  return sensors.getTempC(sensor1);
  }
//Função que inicia o DHT11
void inicia_dht(){
  dht.begin();
  }
//Função que lê a umidade 
float get_umidade(){
  float h = dht.readHumidity();
  return h;
  }
//Função que lê o LDR
int get_ldr(){
    return analogRead(LDR);
  }
 //Função que inicia o display e faz a dashboard inicial
 void set_display(){
  display.begin(SSD1306_SWITCHCAPVCC, endereco_do_display);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("IP: ");
  display.print(WiFi.localIP());
 display.println("");
  display.print("Sensor DS18b20:");
  display.println("");
  for(int i=0 ; i<8 ;i++){
    display.print(sensor1[i],HEX);
  }
  display.println("");
  display.print("DHT 22 iniciado");
  display.println("");
  display.print("LDR iniciado");
  display.display();
  delay(3000);
  
  
  }

 void monta_dashboard(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print(get_hora());
  display.println("");
  display.print("Luminosidade: ");
  display.print(get_ldr());
  display.println(" ");
  display.print("Umidade do Solo: ");
  display.print(get_umidade_solo());
  display.println();
  display.print("Temperatura: ");
  display.print(get_temp());
  display.print(" C");
  display.println("");
  display.print("Umidade: ");
  display.print(get_umidade());
  display.print("%");
  display.display();
  }
//pega o horario local
void set_horario(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
String get_hora()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  String horario = (String)timeinfo.tm_hour + ":" +(String) timeinfo.tm_min;
  return horario;
}
//inicia Wifi
void start_wifi(){
  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(ssid);
  Serial.println("  Aguarde!");

  WiFi.begin(ssid, password); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(ssid);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}
 int get_umidade_solo(){
  return analogRead(sensor_humidade_solo);
}
void callback(char* topic, byte* payload, unsigned int length) {
  enviaValores(topic);
}
void set_MQTT(){
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(callback); 
  MQTT.connect(ID_MQTT);
  if(MQTT.connected()){
    MQTT.subscribe("esp/situacao");
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.setTextSize(1);
    display.clearDisplay();
    display.print("Connectado ao Broker");
    display.display();
    }
  }
void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    start_wifi(); //se não há conexão com o WiFI, a conexão é refeita
}
void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
            MQTT.subscribe("esp/temp");
            MQTT.subscribe("esp/hum");
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}
void enviaValores(char* topico) {
  delay(300);
  Serial.println("Enviando msg");
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.clearDisplay();
  display.print("Enviando os dados");
  display.display();
  if(strcmp(topico, "esp/situacao") ==0){
  String s = static_cast<String>(get_temp())+"/n"+static_cast<String>(get_umidade())+ "/n"+ (String)get_ldr()+ "/n"+get_hora()+"/n"+get_umidade_solo()+"/n";
  char p[s.length()];
 
    int i;
    for (i = 0; i < sizeof(p); i++) {
        p[i] = s[i];
        
    }
    MQTT.publish("retornatemp",p);
  }
}

void rega_planta(){
  digitalWrite(bombapararegar,HIGH);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.clearDisplay();
  display.print("Glub Glub Glub");
  display.display();
  MQTT.publish("regandoaplanta","Chuvendo na hortinha");
  delay(20000);
  
  digitalWrite(bombapararegar,LOW);
  }
