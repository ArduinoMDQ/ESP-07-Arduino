#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

#define MQTT_SERVER_LAN "192.168.1.106"
//#define MQTT_SERVER_WAN "giovanazzi.ddns.net"
#define MQTT_SERVER_WAN "idirect.dlinkddns.com"
#define PORT "1883"
#define HOME "casa"
#define ID "esp-00"
#define SITE "living"
#define FLOOR "piso-0"
#define DEPARTMENT "dto-0"

//*********  prieba  *******************

String cadenaS="";

//****************



String id_=ID;
String home_=HOME;
String site_=SITE;
String floor_=FLOOR;
String department_=DEPARTMENT;

SimpleDHT11 dht11;
//**************** PINES     ****************

const int Btn_Config=0;// boton configuracion
const int Led_Verde=16;// boton para la señalizacion del estado del modulo
const int Sw_1= 12;
const int Sw_2= 13;
const int Relay_1= 4;
const int Relay_2= 5 ;
const int pinDHT11 = 2;
const int pinPIR = 14;
const int Adc_Analog=A0;

//********** fin pines **********************

char* SERVER_LAN = " ";
char* SERVER_WAN =" ";
char* sw1= " ";

String temp_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String hum_str;
String adc_str;

char temp[50];
char hum[50];
char adc[10];

//////////////////////// AGREGADO NUEVO ANTIREBOTE

const int tiempoAntirebote=20;
int cuentaNSw_1= 0;
int cuentaNSw_2= 0;
int cuentaNpinPIR= 0;

boolean estadoSw_2= true;
boolean estadoSw_2Anterior= true;
boolean stateSw_2= true;

boolean estadoSw_1= true;
boolean estadoSw_1Anterior= true;
boolean stateSw_1= true;

boolean estadoBtn_Config= false;
boolean estadoBtn_ConfigAnterior= false;
boolean stateBtn_Config= false;

boolean estadopinPIR= false;
boolean estadopinPIRAnterior= false;
boolean statepinPIR= false;


int address = 0;
byte value;
byte modo=0;
/* Set these to your desired credentials. */
const char* ssid_AP = "ESP8266";
const char* password_AP = "PASSWORD";
const int channel=11;
const int hidden=0;

int cont_mqtt=0;
String Wan=" ";

char ssid[20];
char pass[20];
char configuracion[30];
char Topic1[30];
char Topic2[30];
char TopicSensor[30];
char TopicPir[30];
char ServerWan[30];
char ServerLan[30];
char Home[20];
char Floor[20];
char Site[20];
char Id[20];
char Department[20];
char Port[5];
char UserMqtt[20];
char PassMqtt[20];

String conf_leido;
String ssid_leido;
String pass_leido;
String Home_leido;
String Floor_leido;
String Site_leido;
String Id_leido;
String Department_leido;
String Topic1_leido;
String Topic2_leido;
String TopicSensor_leido;
String TopicPir_leido;
String ServerWan_leido;
String ServerLan_leido;
String Port_leido;
String UserMqtt_leido;
String PassMqtt_leido;

//listado de topicos


String TopicoSw1;
char* TopicoSw1Char;

String TopicoSw2;
char* TopicoSw2Char;

String TopicoPir;
char* TopicoPirChar;

String TopicoSensor;
char* TopicoSensorChar;

String NTopicoSw1;
String NTopicoSw2;
String NTopicoPir;
String NTopicoSensor;
String NTopicoTempSensor;
String NTopicoHumSensor;

String NTopicoSw1Confirm;
String NTopicoSw2Confirm;
String NTopicoPirConfirm;
String NTopicoSensorTempConfirm;
String NTopicoSensorHumConfirm;

String scanWifi;

int ssid_tamano = 0;
int pass_tamano = 0;
int Home_tamano = 0;
int Floor_tamano = 0;
int Site_tamano = 0;
int Id_tamano = 0;
int Department_tamano;
int Topic1_tamano = 0;
int Topic2_tamano = 0;
int TopicSensor_tamano = 0;
int TopicPir_tamano = 0;
int ServerWan_tamano = 0;
int ServerLan_tamano = 0;
int Port_tamano = 0;
int UserMqtt_tamano = 0;
int PassMqtt_tamano = 0;

////// ADDRESS EEPROM
int dir_modo= 0; //0=normal 1 configuracion
int dir_ssid = 1;
int dir_pass = 30;
int dir_conf = 70;
int dir_topic1 = 100;
int dir_topic2 = 130;
int dir_serverwan = 160;
int dir_serverlan = 190;
int dir_puerto = 220;
int dir_topic_sensor = 230;
int dir_topic_pir = 260;
int dir_department= 280;
int dir_home =300;
int dir_floor =320;
int dir_site =340;
int dir_id=360;
int dir_userMqtt=380;
int dir_passMqtt=400;

ESP8266WebServer server(80);    //creo el servidor en el puerto 80
WiFiClient wifiClient;          //creo el cliente


// defino pagina de inicio
String pral = "<html>"
              "<meta http-equiv='Content-Type' content='text/html  ; charset=utf-8'/>"
              "<title>ESP8266</title> <style type='text/css'> body,td,th { color: #036; } body { background-color: #999; } </style> </head>"
              "<body> "
              "<title>CONFIGURACION</title><br>"              
              "<form action='config' method='get' target='pantalla'>"
              "<fieldset align='center' style='border-style:solid; border-color:#336666; width:300px; height:600px; padding:20px; margin: 20px;'>"
              "<legend><strong>CONFIGURATION</strong></legend>"              
              "<div align=center>WIRELESS</div><br>"
              "<div align=left> Wifi........<input name='ssid' type='text' size='15'/></div>"
              "<div align=left> Pass........<input name='pass' type='password' size='15'/></div>"
              "<div align=left> Wan........<input name='serverwan' type='text' size='15'/> </div>"
              "<div align=left> Lan.........<input name='serverlan' type='text' size='15'/> </div>"
              "<div align=left> Port.........<input name='port' type='TEXT' size='5'/> </div><br>"
              "<div align=center>TOPICS</div><br>"             
              "<div align=left>Home......<input name='home' type='text' size='15'/> </div>"
              "<div align=left>Floor........<input name='floor' type='text' size='15'/> </div>"
              "<div align=left>Depart.......<input name='department' type='text' size='15'/> </div>"
              "<div align=left>Place.......<input name='site' type='text' size='15'/> </div><br>"              
              "<div align=center>DEVICE</div><br>"
              "<div align=left>ID............<input name='id' type='text' size='15'/> </div>"
              "<div align=left>Switch 1..<input name='topic1' type='text' size='15'/> </div>"
              "<div align=left>Switch 2..<input name='topic2' type='text' size='15'/> </div>"
              "<div align=left>Sensor.....<input name='topicsensor' type='text' size='15'/> 3.3 vcc</div>"
              "<div align=left>Pir...........<input name='topicpir' type='text' size='15'/> 5 vcc</div><br>"
              "<div align=center>MQTT</div><br>"           
              "<div align=left>User.......<input name='userMqtt' type='text' size='15'/> </div>"
              "<div align=left>Password..<input name='passMqtt' type='password' size='15'/> </div><br>"             
              "<input type='submit' value='APPLY CONFIGURATION' />"
              "</fieldset>"
              "</form>"
              "<iframe id='pantalla' name='pantalla' src='' width=1200px height=700px frameborder='0' scrolling='yes'></iframe>"
              "</body>"
              "</html>";

void setup() {

Serial.begin(115200);
Serial.println();
EEPROM.begin(512);

//pinMode(A0,INPUT);
pinMode(pinPIR,INPUT);
pinMode(Btn_Config, INPUT);
pinMode(Led_Verde,OUTPUT);
pinMode(Sw_1, INPUT);
pinMode(Sw_2, INPUT);//tiene hardware antirebote
pinMode(Relay_1,OUTPUT);
pinMode(Relay_2,OUTPUT);
digitalWrite(Led_Verde,true);
//digitalWrite(Relay_2,false);
  
value = EEPROM.read(0);//carga el valor 1 si no esta configurado o 0 si esta configurado
delay(10);

ReadDataEprom();


if(lee(dir_conf)!="configurado"){
    value=1;
   }
    
if(value){
          
            Serial.println("**********MODO CONFIGURACION************");
            scanWIFIS();
            Serial.print("Configuring access point...");
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid_AP, password_AP,channel,hidden);// (*char SSID,*char PASS,int CHANNEL,int HIDDEN=1 NO_HIDDEN=0)
            IPAddress myIP = WiFi.softAPIP();
            Serial.print("AP IP address: ");
            Serial.println(myIP);
            
            server.on("/", []() {server.send(200, "text/html", pral);});
            server.on("/config", wifi_conf);
            server.begin();
            
            Serial.println("**********  Webserver iniciado ***************");
            Serial.print("ssid: "); Serial.println(ssid_AP);
            Serial.print("password: "); Serial.println(password_AP);
            Serial.print("channel: "); Serial.println(channel);
            Serial.print("hidden: "); Serial.println(hidden);
            Serial.println();

      }else{
           Serial.println("**********MODO NORMAL************");  
           ServerLan_leido= lee(dir_serverlan);
           ServerWan_leido= lee(dir_serverwan);
           ServerLan_tamano = ServerLan_leido.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
           ServerWan_tamano = ServerWan_leido.length() + 1;
           ServerLan_leido.toCharArray(SERVER_LAN, ServerLan_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
           ServerWan_leido.toCharArray(SERVER_WAN, ServerWan_tamano);
            
           ConcatenarTopicos();// esta fncion crea los topicas para   que se subscriban
                                // con los strings correctos quitando los espacion que molestan y dan error
           WiFi.mode(WIFI_STA);
           intento_conexion();
    }
  
  modo=0;//normal
  EEPROM.write(dir_modo,modo);
  EEPROM.commit();
}

PubSubClient client(SERVER_WAN, 1883, callback, wifiClient);

void loop() {
   Botones();
      if(value){
        server.handleClient();
        delay(500);
        digitalWrite(Led_Verde,!digitalRead(Led_Verde)); 
         }
      else{ 
       //maintain MQTT connection
       client.loop();
       if (WiFi.status() == WL_CONNECTED) { 
            digitalWrite(Led_Verde,true);
            reconexionMQTT();
         }else{
           digitalWrite(Led_Verde,false);
           intento_conexion();
         }
         Botones();
       }
       
   BotonConfiguracion();
}

void callback(char* topic, byte* payload, unsigned int length) {

  //convert topic to string to make it easier to work with
  String topicStr = topic; 
  Serial.print("Topic Subscript: ");Serial.print(topicStr);

  if(topicStr == NTopicoSw1){
       if(payload[0] == '1'){
          digitalWrite(Relay_1, HIGH);
          client.publish((char*)NTopicoSw1Confirm.c_str(), "On");
            Serial.println(" = 1");
          }
       else {
          digitalWrite(Relay_1, LOW);
          client.publish((char*)NTopicoSw1Confirm.c_str(), "Off");
           Serial.println(" = 0");
       }

      
  }
  
  if(topicStr == NTopicoSw2){
      if(payload[0] == '1'){
          digitalWrite(Relay_2, HIGH);
          client.publish((char*)NTopicoSw2Confirm.c_str(), "On");
           Serial.println(" = 1");
          }
      else{
          digitalWrite(Relay_2, LOW);
         client.publish((char*)NTopicoSw2Confirm.c_str(), "Off");
          Serial.println(" = 0");
      }
    }

  if(topicStr == NTopicoSensor){
       if(payload[0] == '1'){
           SensorHumTemp();
          }
     }
 if(topicStr == NTopicoPir){
       if(payload[0] == '1'){
       
          client.publish((char*)NTopicoPirConfirm.c_str(), "On");
           Serial.println("pir = 1");
          }
      else{
          
         client.publish((char*)NTopicoPirConfirm.c_str(), "Off");
          Serial.println("pir = 0");
      }
     }
   
}
// ***************     Funciones      ****************//

String arregla_simbolos(String a) {
  
  a.replace("%C3%A1", "Ã¡");
  a.replace("%C3%A9", "Ã©");
  a.replace("%C3%A", "i");
  a.replace("%C3%B3", "Ã³");
  a.replace("%C3%BA", "Ãº");
  a.replace("%21", "!");
  a.replace("%23", "#");
  a.replace("%24", "$");
  a.replace("%25", "%");
  a.replace("%26", "&");
  a.replace("%27", "/");
  a.replace("%28", "(");
  a.replace("%29", ")");
  a.replace("%3D", "=");
  a.replace("%3F", "?");
  a.replace("%27", "'");
  a.replace("%C2%BF", "Â¿");
  a.replace("%C2%A1", "Â¡");
  a.replace("%C3%B1", "Ã±");
  a.replace("%C3%91", "Ã‘");
  a.replace("+", " ");
  a.replace("%2B", "+");
  a.replace("%22", "\"");
  return a;
}

void scanWIFIS(){
  Serial.println("scan start");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
     
    }
  }

}

String macToStr(const uint8_t* mac){

  String result;

  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);

    if (i < 5){
      result += ':';
    }
  }

  return result;
}

void blink50(){
    digitalWrite(Led_Verde,true);
    delay(30);
    digitalWrite(Led_Verde,false);
    delay(30);
    digitalWrite(Led_Verde,true);
    delay(30);
    digitalWrite(Led_Verde,false);
    delay(30);
     }

void blink_100(){
    int pin=digitalRead(pinPIR);
    digitalWrite(Led_Verde,!pin);
    delay(50);
    digitalWrite(Led_Verde,pin);
    delay(50);
    digitalWrite(Led_Verde,!pin);
    delay(50);
    digitalWrite(Led_Verde,pin);
  
  }

void blinkLento(){
    digitalWrite(Led_Verde,false);
    delay(500);
    digitalWrite(Led_Verde,true);
    delay(500);
    digitalWrite(Led_Verde,false);
    delay(500);
    digitalWrite(Led_Verde,true);
    delay(500);
    
  }

//*******  G R A B A R  EN LA  E E P R O M  ************
void graba(int addr, String a) {
  int tamano = (a.length() + 1);
  Serial.print(tamano);
  char inchar[30];    //'30' TamaÃ±o maximo del string
  a.toCharArray(inchar, tamano);
  EEPROM.write(addr, tamano);
  for (int i = 0; i < tamano; i++) {
    addr++;
    EEPROM.write(addr, inchar[i]);
  }
  EEPROM.commit();
}

//*******  L E E R   EN LA  E E P R O M    *************
String lee(int addr) {
  String nuevoString;
  int valor;
  int tamano = EEPROM.read(addr);
  for (int i = 0; i < tamano; i++) {
    addr++;
    valor = EEPROM.read(addr);
    nuevoString += (char)valor;
  }
  return nuevoString;}

//*******  L E E R    C O N F I G U R A C I O N    *****
void ReadDataEprom(){

  Serial.println("Lectura de EEPROM...");
  Serial.print("Configuracion: ");

  conf_leido   = lee(dir_conf);
  ssid_leido      = lee(dir_ssid);
  pass_leido      = lee(dir_pass);
  Home_leido      = lee(dir_home);
  Floor_leido     = lee(dir_floor);
  Site_leido      = lee(dir_site);
  Id_leido        = lee(dir_id);
  Department_leido= lee(dir_department);
  Topic1_leido    = lee(dir_topic1);
  Topic2_leido    = lee(dir_topic2);
  TopicSensor_leido = lee(dir_topic_sensor);
  TopicPir_leido    = lee(dir_topic_pir);
  ServerWan_leido = lee(dir_serverwan);
  ServerLan_leido = lee(dir_serverlan);
  Port_leido      = lee(dir_puerto);
  UserMqtt_leido  = lee(dir_userMqtt);
  PassMqtt_leido = lee(dir_passMqtt);
  
  conf_leido = arregla_simbolos(conf_leido);Serial.print("Eprom ssid_leido:");Serial.println(conf_leido);
  ssid_leido      = arregla_simbolos(ssid_leido);Serial.print("Eprom ssid_leido:");Serial.println(ssid_leido);
  pass_leido      = arregla_simbolos(pass_leido);Serial.print("Eprom pass_leido:");Serial.println(pass_leido);
  ServerWan_leido = arregla_simbolos(ServerWan_leido);Serial.print("Eprom ServerWan_leido:");Serial.println(ServerWan_leido);
  ServerLan_leido = arregla_simbolos(ServerLan_leido);Serial.print("Eprom ServerLan_leido:");Serial.println(ServerLan_leido);
  Port_leido      = arregla_simbolos(Port_leido);Serial.print("Eprom Port_leido:");Serial.println(Port_leido);
 
  Home_leido      = arregla_simbolos(Home_leido);Serial.print("Eprom Home_leido:");Serial.println(Home_leido);
  Floor_leido     = arregla_simbolos(Floor_leido);Serial.print("Eprom Floor_leido:");Serial.println(Floor_leido);
  Department_leido= arregla_simbolos(Department_leido);Serial.print("Eprom Department_leido:");Serial.println(Department_leido);
  Site_leido      = arregla_simbolos(Site_leido);Serial.print("Eprom Site_leido:");Serial.println(Site_leido);
  
  Id_leido        = arregla_simbolos(Id_leido);Serial.print("Eprom Id_leido:");Serial.println(Id_leido);
  Topic1_leido    = arregla_simbolos(Topic1_leido);Serial.print("Eprom Topic1_leido:");Serial.println(Topic1_leido);
  Topic2_leido    = arregla_simbolos(Topic2_leido);Serial.print("Eprom Topic2_leido:");Serial.println(Topic2_leido);
  TopicSensor_leido = arregla_simbolos(TopicSensor_leido);Serial.print("Eprom TopicSensor_leido:");Serial.println(TopicSensor_leido);
  TopicPir_leido    = arregla_simbolos(TopicPir_leido);Serial.print("Eprom TopicPir_leido:");Serial.println(TopicPir_leido);
  UserMqtt_leido      = arregla_simbolos(UserMqtt_leido);Serial.print("Eprom UserMqtt_leido:");Serial.println(UserMqtt_leido);
  PassMqtt_leido   = arregla_simbolos(PassMqtt_leido);Serial.print("Eprom PassMqtt_leido:");Serial.println(PassMqtt_leido);

  

  ServerWan_tamano    = ServerWan_leido.length() ;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  ServerLan_tamano    = ServerLan_leido.length() ;
  ssid_tamano         = ssid_leido.length() ;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  pass_tamano         = pass_leido.length();
  Topic1_tamano       = Topic1_leido.length();  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  Topic2_tamano       = Topic2_leido.length();
  Home_tamano         = Home_leido.length();
  Floor_tamano        = Floor_leido.length();
  Site_tamano         = Site_leido.length();
  Id_tamano           = Id_leido.length();
  Department_tamano   = Department_leido.length();
  TopicSensor_tamano  = TopicSensor_leido.length();
  TopicPir_tamano     = TopicPir_leido.length();
  Port_tamano         = Port_leido.length();
  UserMqtt_tamano     = UserMqtt_leido.length();
  PassMqtt_tamano     = PassMqtt_leido.length();
  
  ServerWan_leido.toCharArray(ServerWan, ServerWan_tamano); //Transformamos el string en un char array ya que es lo que nos pide WIFI.begin()
  ServerLan_leido.toCharArray(ServerLan, ServerLan_tamano);
  ssid_leido.toCharArray(ssid, ssid_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
  pass_leido.toCharArray(pass, pass_tamano);
  Port_leido.toCharArray(Port,Port_tamano);
 
  Home_leido.toCharArray(Home,Home_tamano);
  Floor_leido.toCharArray(Floor,Floor_tamano);
  Department_leido.toCharArray(Department,Department_tamano);
  Site_leido.toCharArray(Site,Site_tamano);
  Id_leido.toCharArray(Id,Id_tamano);
  
  Topic1_leido.toCharArray(Topic1, Topic1_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
  Topic2_leido.toCharArray(Topic2, Topic2_tamano);
  TopicSensor_leido.toCharArray(TopicSensor,TopicSensor_tamano);
  TopicPir_leido.toCharArray(TopicPir,TopicPir_tamano);
 
  
  UserMqtt_leido.toCharArray(UserMqtt,UserMqtt_tamano);
  PassMqtt_leido.toCharArray(PassMqtt,PassMqtt_tamano);

  
  Serial.print("Eprom Topico Sw1:");Serial.println(Home_leido+"/"+Floor_leido+"/"+Department_leido+"/"+Site_leido+"/"+Id_leido+"/"+Topic1_leido);
  Serial.print("Eprom Topico Sw2:");Serial.println(Home_leido+"/"+Floor_leido+"/"+Department_leido+"/"+Site_leido+"/"+Id_leido+"/"+Topic2_leido);
  Serial.print("Eprom Topico Sensor:");Serial.println(Home_leido+"/"+Floor_leido+"/"+Department_leido+"/"+Site_leido+"/"+Id_leido+"/"+TopicSensor_leido);
  Serial.print("Eprom Topico Pir:");Serial.println(Home_leido+"/"+Floor_leido+"/"+Department_leido+"/"+Site_leido+"/"+Id_leido+"/"+TopicPir_leido);
  
  }

//**** CONFIGURACION WIFI  *******
void wifi_conf() {
  int cuenta = 0;

  String getssid = server.arg("ssid"); //Recibimos los valores que envia por GET el formulario web
  String getpass = server.arg("pass");
  String getServerWan = server.arg("serverwan"); //Recibimos los valores que envia por GET el formulario web
  String getServerLan = server.arg("serverlan");
  String getPort = server.arg("port");

  String getHome = server.arg("home");
  String getFloor = server.arg("floor");
  String getSite = server.arg("site");
  String getDepartment = server.arg("department");
  
  String getId = server.arg("id");
  String getTopic1 = server.arg("topic1"); //Recibimos los valores que envia por GET el formulario web
  String getTopic2 = server.arg("topic2");
  String getTopicSensor = server.arg("topicsensor");
  String getTopicPir = server.arg("topicpir");

  String getUserMqtt = server.arg("userMqtt");
  String getPassMqtt = server.arg("passMqtt");

  
   Serial.print(" INFO CARGADA EN LA WEB");
  

   Serial.print("NTopicoSw1 : ");Serial.println(NTopicoSw1);
   Serial.print("NTopicoSw2 : ");Serial.println(NTopicoSw2);
   Serial.print("NTopicoSensor : ");Serial.println(NTopicoSensor);
   Serial.print("NTopicoPir : ");Serial.println(NTopicoPir);
   
   Serial.print("getUserMqtt : ");Serial.println(getUserMqtt);
   Serial.print("getPassMqtt : ");Serial.println(getPassMqtt);
   
  //////////////////

  getssid = arregla_simbolos(getssid); //Reemplazamos los simbolos que aparecen cun UTF8 por el simbolo correcto
  getpass = arregla_simbolos(getpass);
  getServerWan = arregla_simbolos(getServerWan); //Reemplazamos los simbolos que aparecen cun UTF8 por el simbolo correcto
  getServerLan = arregla_simbolos(getServerLan);
  getPort = arregla_simbolos(getPort);

  getHome = arregla_simbolos(getHome);
  getFloor = arregla_simbolos(getFloor);
  getSite = arregla_simbolos(getSite);
  getDepartment = arregla_simbolos(getDepartment);

  getId =arregla_simbolos(getId); 
  getTopic1 = arregla_simbolos(getTopic1); //Reemplazamos los simbolos que aparecen cun UTF8 por el simbolo correcto
  getTopic2 = arregla_simbolos(getTopic2);
  getTopicSensor = arregla_simbolos(getTopicSensor);
  getTopicPir = arregla_simbolos(getTopicPir); 

  getUserMqtt = arregla_simbolos(getUserMqtt);
  getPassMqtt = arregla_simbolos(getPassMqtt);

  
  ssid_tamano = getssid.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  pass_tamano = getpass.length() + 1;
  ServerWan_tamano = getServerWan.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  ServerLan_tamano = getServerLan.length() + 1;
  Port_tamano = getPort.length() + 1;
  
  Department_tamano= getDepartment.length() + 1;
  Home_tamano= getHome.length() + 1;
  Floor_tamano= getFloor.length() + 1;
  Site_tamano= getSite.length() + 1;
 
  Id_tamano= getId.length() + 1;
  Topic1_tamano = getTopic1.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
  Topic2_tamano = getTopic2.length() + 1;
  TopicSensor_tamano = getTopicSensor.length() +1;
  TopicPir_tamano = getTopicPir.length() +1;

  UserMqtt_tamano = getUserMqtt.length() +1;
  PassMqtt_tamano = getPassMqtt.length() +1;
  
  getssid.toCharArray(ssid, ssid_tamano); //Transformamos el string en un char array ya que es lo que nos pide WIFI.begin()
  getpass.toCharArray(pass, pass_tamano);
  getServerWan.toCharArray(ServerWan, ServerWan_tamano); //Transformamos el string en un char array ya que es lo que nos pide WIFI.begin()
  getServerLan.toCharArray(ServerLan, ServerLan_tamano);
  getPort.toCharArray(Port, Port_tamano);
  
  getDepartment.toCharArray(Department, Department_tamano);
  getHome.toCharArray(Home, Home_tamano);
  getFloor.toCharArray(Floor, Floor_tamano);
  getSite.toCharArray(Site,Site_tamano);

  
  getId.toCharArray(Id, Id_tamano);
  getTopic1.toCharArray(Topic1, Topic1_tamano); //Transformamos el string en un char array ya que es lo que nos pide WIFI.begin()
  getTopic2.toCharArray(Topic2, Topic2_tamano);
  getTopicSensor.toCharArray(TopicSensor, TopicSensor_tamano);
  getTopicPir.toCharArray(TopicPir,TopicPir_tamano);

  getUserMqtt.toCharArray(UserMqtt,UserMqtt_tamano);
  getPassMqtt.toCharArray(PassMqtt,PassMqtt_tamano);

  TopicoSw1=getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopic1;
  TopicoSw2=getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopic2;
  TopicoPir=getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopicPir;
  TopicoSensor=getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopicSensor;
 
  Serial.print("ssid: ");  Serial.println(ssid);     //para depuracion
  Serial.print("pass: ");  Serial.println(pass);
  Serial.print("Server Wan MQTT: ");Serial.println(ServerWan);
  Serial.print("Server Lan MQTT: ");Serial.println(ServerLan);
  Serial.print("Puerto: ");Serial.println(Port);
  Serial.println();
  Serial.print("Department: ");Serial.println(getDepartment);
  Serial.print("Home: ");Serial.println(getHome);
  Serial.print("Floor: ");Serial.println(getFloor);
  Serial.print("Site: ");Serial.println(getSite);
  Serial.print(" Topic:");
  Serial.println(getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite);
  Serial.print("Device(Id): ");Serial.print(Id);Serial.print(" Topic:");Serial.println(getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId);
  Serial.print("Switch 1: ");Serial.print(Topic1);Serial.print(" Topic:");Serial.println(TopicoSw1);
  Serial.print("Switch 2: "); Serial.print(Topic2);Serial.print(" Topic:");Serial.println(TopicoSw2);
  Serial.print("Sensor: ");Serial.print(TopicSensor);Serial.print(" Topic:");Serial.println(TopicoSensor);
  Serial.print("Pir: ");Serial.print(TopicPir);Serial.print(" Topic:");Serial.println(TopicoPir);
  Serial.print("User Mqtt: ");Serial.print(UserMqtt);
  Serial.print("Pass Mqtt: ");Serial.print(PassMqtt);
  WiFi.begin(ssid, pass); 
  //Intentamos conectar
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(400);
    blink50();
    Serial.print(".");
    cuenta++;
    if (cuenta > 20) {
      graba(dir_conf, "noconfigurado");
      EEPROM.write(dir_modo,1);
      EEPROM.commit();;
      server.send(200, "text/html", String("<h2>No se pudo realizar la conexion<br>no se guardaron los datos.</h2>"));
      return;
    }
  }
  
  Serial.print("Cuenta: ");
  Serial.println(cuenta);
  Serial.print(WiFi.localIP());
  graba(dir_conf, "configurado");
  EEPROM.write(dir_modo,0);
  EEPROM.commit();
  
  graba(dir_ssid, getssid);
  graba(dir_pass, getpass);
  graba(dir_serverwan, getServerWan);
  graba(dir_serverlan, getServerLan);
  graba(dir_puerto, getPort);

  graba(dir_home, getHome);
  graba(dir_floor, getFloor);
  graba(dir_site, getSite);
  graba(dir_department, getDepartment);
  
  graba(dir_id, getId);
  graba(dir_topic1, getTopic1);
  graba(dir_topic2, getTopic2);
  graba(dir_topic_sensor, getTopicSensor);
  graba(dir_topic_pir, getTopicPir);

  graba(dir_userMqtt, getUserMqtt);
  graba(dir_passMqtt, getPassMqtt);
  
  
  server.send(200, "text/html", String("<h2>Conexion exitosa a: "+ getssid + "<br> Pass: '" + getpass + "' .<br>" 
  + "<br> Topico Sw1: " + getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopic1 + ".<br>" 
  + "<br> Topico Sw2: " + getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopic2 + ".<br>" 
  + "<br> Topico Sensor: " + getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopicSensor + ".<br>" 
  + "<br> Topico Pir: " + getHome+"/"+getFloor+"/"+getDepartment+"/"+getSite+"/"+getId+"/"+getTopicPir + ".<br>" 
  + "<br> Server Wan MQTT: " + getServerWan + ".<br>" 
  + "<br> Server Lan MQTT: " + getServerLan + ".<br>" 
  + "<br> User MQTT: " + getUserMqtt + ".<br>" 
  + "<br> Pass MQTT: " + getPassMqtt + ".<br>" 
  + "<br>El equipo se reiniciara Conectandose a la red configurada."));
  delay(100);
  ESP.restart();
}

void intento_conexion() {
  
  if (lee(dir_conf).equals("configurado")) {
     
    Serial.print("SSID: ");  //Para depuracion
    Serial.println(ssid_leido);  //Para depuracion
    Serial.print("PASS: ");  //Para depuracion
    Serial.println(pass_leido);
    Serial.print("Server Lan MQTT: ");  //Para depuracion
    Serial.println(ServerLan_leido);  //Para depuracion
    Serial.print("Server Wan MQTT: ");  //Para depuracion
    Serial.println(ServerWan_leido);

    Serial.print("Id Device: ");  //Para depuracion
    Serial.println(Id_leido);  //Para depuracion
    Serial.print("Switch 1: ");  //Para depuracion
    Serial.println(Topic1_leido);  //Para depuracion
    Serial.print("Switch 2: ");  //Para depuracion
    Serial.println(Topic2_leido);
    Serial.print("Sensor: ");  //Para depuracion
    Serial.println(TopicSensor_leido);  //Para depuracion
    Serial.print("Pir: ");  //Para depuracion
    Serial.println(TopicPir_leido);
    Serial.print("User mqtt: ");  //Para depuracion
    Serial.println(UserMqtt_leido);
    Serial.print("Pass mqtt: ");  //Para depuracion
    Serial.println(PassMqtt_leido);
  

    ssid_tamano = ssid_leido.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
    pass_tamano = pass_leido.length() + 1;
    Topic1_tamano = Topic1_leido.length() + 1;  //Calculamos la cantidad de caracteres que tiene el ssid y la clave
    Topic2_tamano = Topic2_leido.length() + 1;

    ssid_leido.toCharArray(ssid, ssid_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
    pass_leido.toCharArray(pass, pass_tamano);
    Topic1_leido.toCharArray(Topic1, Topic1_tamano); //Transf. el String en un char array ya que es lo que nos pide WiFi.begin()
    Topic2_leido.toCharArray(Topic2, Topic2_tamano);

    int cuenta = 0;
    WiFi.begin(ssid, pass);      //Intentamos conectar
    while (WiFi.status() != WL_CONNECTED) {
      delay(400);
      blink50();
      cuenta++;
      Botones();
      BotonConfiguracion();
      if (cuenta > 20) {
        Serial.println("Fallo al conectar");
        return;
      }
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Conexion exitosa a WIFI: ");
    Serial.println(ssid);
    Serial.println(WiFi.localIP());
    digitalWrite(Led_Verde,true);
    reconexionMQTT();
  }
 }

void reconexionMQTT(){

    int cuenta=0;

    while (!client.connected()) {
      if (WiFi.status() != WL_CONNECTED) {
        ESP.restart();
       }
      Botones();
      BotonConfiguracion();
      Serial.println("Attempting MQTT connection...");
    // Generate client name based on MAC address and last 8 bits of microsecond counter
      String clientName;
     //clientName += getId+"esp8266-";
     clientName += "esp8266-";
      uint8_t mac[6];
      WiFi.macAddress(mac);
      clientName += macToStr(mac);
      Serial.println(clientName);

      if (client.connect((char*)clientName.c_str(),(char*)UserMqtt_leido.c_str(),(char*)PassMqtt_leido.c_str())){

       client.subscribe((char*)NTopicoSw1.c_str());
        client.subscribe((char*)NTopicoSw2.c_str());
        client.subscribe("casa/piso-0/dto-0/living/esp-00/dht11");
        client.subscribe("casa/piso-0/dto-0/living/esp-00/pir");
        digitalWrite(Led_Verde,true);// wifi + mqtt ok !!!
        Serial.println("MTQQ Connected");
      }
      //otherwise print failed for debugging
      else{
           digitalWrite(Led_Verde,true);
           Serial.println("Failed  connection mqtt.");
           Serial.print("Client state: ");
           Serial.println(client.state());
           Serial.println(" try again in 3 seconds");
           // Wait 3 seconds before retrying
         //  blinkLento();
           cuenta++;   
           Serial.print("cuenta: ");Serial.println(cuenta);
           if(cuenta>4){
            Serial.println("abortooo");
              ESP.restart();
             }
         }
    }
 }

void SensorHumTemp(){

  long tiempo =millis();
  digitalWrite(Led_Verde,false);
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
    return;
  }
  temp_str=String((int)temperature);
  hum_str=String((int)humidity);
  Serial.print("  Tem: "+temp_str);
  Serial.println("  Hum: "+hum_str);
  
  temp_str.toCharArray(temp, temp_str.length()+1); 
  hum_str.toCharArray(hum, hum_str.length()+1); 
  
  client.publish((char*)NTopicoSensorTempConfirm.c_str(),temp );
  client.publish((char*)NTopicoSensorTempConfirm.c_str(),hum );
  
  digitalWrite(Led_Verde,true);
  Serial.print("Tiempo de lectura de sensor:"); Serial.print(millis()-tiempo);Serial.println(" mseg.");
  }

boolean antirebote(int pin){
  int contadorN=0;
  boolean estado=true;
  boolean estadoAnterior=true;
  do{
    estado=digitalRead(pin);
    if(estado != estadoAnterior){
     
      contadorN=0;
      estadoAnterior=estado;      
      }
     else{
      contadorN++;
    }
      delay(1);
     }while(contadorN < tiempoAntirebote);

    return estado;
  }

void Botones(){
//////********  BOTON 1    
     estadoSw_1=digitalRead(Sw_1);
     if(estadoSw_1!=estadoSw_1Anterior){
          stateSw_1 = antirebote(Sw_1);
          if(stateSw_1){
            cuentaNSw_1++;
            digitalWrite(Relay_1,!digitalRead(Relay_1));
          }else{
            cuentaNSw_1++;
            digitalWrite(Relay_1,!digitalRead(Relay_1));
          }
        if(digitalRead(Relay_1)){
            client.publish("casa/piso-0/dto-0/living/esp-00/light1/confirm", "On");
            Serial.println("Switch 1 ON!!");
          }else{
            client.publish("casa/piso-0/dto-0/living/esp-00/light1/confirm", "Off");
            Serial.println("Switch 1 OFF!!");}     
        }
     estadoSw_1Anterior=estadoSw_1; 
//////********  BOTON 2  
     estadoSw_2=digitalRead(Sw_2);
     if(estadoSw_2!=estadoSw_2Anterior){
          stateSw_2 = antirebote(Sw_2);
          if(stateSw_2){
            cuentaNSw_2++;
            digitalWrite(Relay_2,!digitalRead(Relay_2));
            
          }else{
            cuentaNSw_2++;
            digitalWrite(Relay_2,!digitalRead(Relay_2));
          }
          if(digitalRead(Relay_2)){
            client.publish("casa/piso-0/dto-0/living/esp-00/light2/confirm", "On");
            Serial.println("Switch 2 ON !!");
          }else{
            client.publish("casa/piso-0/dto-0/living/esp-00/light2/confirm", "Off");
            Serial.println("Switch 2 OFF !!");}     
        }
     estadoSw_2Anterior=estadoSw_2; 
//////********  PIR 
     estadopinPIR=digitalRead(pinPIR);
     if(estadopinPIR!=estadopinPIRAnterior){
          statepinPIR = antirebote(pinPIR);
          if(statepinPIR){
              blink_100();
              client.publish("casa/piso-0/dto-0/living/esp-00/pir/confirm", "On");
              Serial.println("PIR ON !!");
          }else{
                client.publish("casa/piso-0/dto-0/living/esp-00/pir/confirm", "Off");         
                  Serial.println("PIR OFF !!");   
            }
         
      }
      estadopinPIRAnterior=estadopinPIR;

 } 

 void BotonConfiguracion(){
  
      estadoBtn_Config=!digitalRead(Btn_Config);
      if(estadoBtn_Config!=estadoBtn_ConfigAnterior){
          stateBtn_Config = antirebote(Btn_Config);
          if(stateBtn_Config){
            Serial.print("Solicitud para entrar en modo Configuracion...");
            blink50();
            modo=1;
            EEPROM.write(0,modo);
            EEPROM.commit();
            delay(10);
           ESP.restart();       
           
          }       
        }
      estadoBtn_ConfigAnterior=estadoBtn_Config;
  }
 
 void ConcatenarTopicos(){
  
           NTopicoSw1.concat(Home_leido);NTopicoSw1.concat('/');
           NTopicoSw1.concat(Floor_leido);NTopicoSw1.concat('/');
           NTopicoSw1.concat(Department_leido);NTopicoSw1.concat('/');
           NTopicoSw1.concat(Site_leido);NTopicoSw1.concat('/');
           NTopicoSw1.concat(Id_leido);NTopicoSw1.concat('/');
           NTopicoSw1.concat(Topic1_leido);
          

           NTopicoSw2.concat(Home_leido);NTopicoSw2.concat('/');
           NTopicoSw2.concat(Floor_leido);NTopicoSw2.concat('/');
           NTopicoSw2.concat(Department_leido);NTopicoSw2.concat('/');
           NTopicoSw2.concat(Site_leido);NTopicoSw2.concat('/');
           NTopicoSw2.concat(Id_leido);NTopicoSw2.concat('/');
           NTopicoSw2.concat(Topic2_leido);

           NTopicoSensor.concat(Home_leido);NTopicoSensor.concat('/');
           NTopicoSensor.concat(Floor_leido);NTopicoSensor.concat('/');
           NTopicoSensor.concat(Department_leido);NTopicoSensor.concat('/');
           NTopicoSensor.concat(Site_leido);NTopicoSensor.concat('/');
           NTopicoSensor.concat(Id_leido);NTopicoSensor.concat('/');
           NTopicoSensor.concat(TopicSensor_leido);

           NTopicoPir.concat(Home_leido);NTopicoPir.concat('/');
           NTopicoPir.concat(Floor_leido);NTopicoPir.concat('/');
           NTopicoPir.concat(Department_leido);NTopicoPir.concat('/');
           NTopicoPir.concat(Site_leido);NTopicoPir.concat('/');
           NTopicoPir.concat(Id_leido);NTopicoPir.concat('/');
           NTopicoPir.concat(TopicPir_leido);

                
           
          int largo=NTopicoSw1.length();
          int largo2=NTopicoSw2.length();
          int largo3=NTopicoSensor.length();
          int largo4=NTopicoPir.length();
          
           for(int i=0;i<=largo;i++)
            {
              if(NTopicoSw1.substring(i).equals("") or NTopicoSw1.substring(i).equals(" "))
              {              
                }else{ cadenaS += NTopicoSw1.charAt(i);  
                }
            }
            
          NTopicoSw1=cadenaS;
          NTopicoSw1Confirm=cadenaS+"/confirm";
          
          cadenaS="";
          for(int i=0;i<=largo2;i++)
            {
              if(NTopicoSw2.substring(i).equals("") or NTopicoSw2.substring(i).equals(" "))
              {              
                }else{ cadenaS += NTopicoSw2.charAt(i);  
                }
            }

           NTopicoSw2=cadenaS;
           NTopicoSw2Confirm=cadenaS+"/confirm";
           cadenaS="";

           for(int i=0;i<=largo3;i++)
            {
              if(NTopicoSensor.substring(i).equals("") or NTopicoSensor.substring(i).equals(" "))
              {              
                }else{ cadenaS += NTopicoSensor.charAt(i);  
                }
            }

           NTopicoSensor=cadenaS;
           NTopicoSensorTempConfirm=cadenaS+"/temp/confirm";
           NTopicoSensorHumConfirm=cadenaS+"/hum/confirm";
           cadenaS="";

           for(int i=0;i<=largo4;i++)
            {
              if(NTopicoPir.substring(i).equals("") or NTopicoPir.substring(i).equals(" "))
              {              
                }else{ cadenaS += NTopicoPir.charAt(i);  
                }
            }

           NTopicoPir=cadenaS;
           NTopicoPirConfirm=cadenaS+"/confirm";
           cadenaS="";

           Serial.print("NTopicoSw1 concatenado: ");Serial.println(NTopicoSw1);
           Serial.print("NTopicoSw1Confirm concatenado: ");Serial.println(NTopicoSw1Confirm);
           
           Serial.print("NTopicoSw2 concatenado: ");Serial.println(NTopicoSw2);
           Serial.print("NTopicoSw2Confirm concatenado: ");Serial.println(NTopicoSw2Confirm);
           
           Serial.print("NTopicoSensorTemp concatenado: ");Serial.println(NTopicoTempSensor);
           Serial.print("NTopicoSensorTempConfirm concatenado: ");Serial.println(NTopicoSensorTempConfirm);

           Serial.print("NTopicoSensorHum concatenado: ");Serial.println(NTopicoHumSensor);
           Serial.print("NTopicoSensorHumConfirm concatenado: ");Serial.println(NTopicoSensorHumConfirm);
           
           Serial.print("NTopicoPir concatenado: ");Serial.println(NTopicoPir);
           Serial.print("NTopicoPirConfirm concatenado: ");Serial.println(NTopicoPirConfirm);
           
  
  
  }
 
