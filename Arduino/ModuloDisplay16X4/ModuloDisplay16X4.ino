/*
  ModuloDisplay - 05/07/2014
 Prototipo de display com exibicao de informacao por um servo-motor simulando um display analogico
 e display lcd de 4 linhas por 16 caracteres
 Arduino - Brasilia 
 Autor : Jeronimo Avelar Filho
 Este codigo e' baseado em exemplo existente em https://bitbucket.org/victoraldir/ajson-aviseja/src
 
 Observacoes: 
 O shield ethernet utiliza os pinos do arduino abaixo:
 D2 - Connects the INT pin of the W5100
 D3 - Contral the reset of the W5100
 D10 - Used for SPI Chip Select
 D11 - Used for SPI MOSI
 D12 - Used for SPI MISO
 D13 - Used for SPI SCK
 
 */
#include <SPI.h>
#include <Ethernet.h>
#include <JsonParser.h>
#include <Servo.h>
#include <LiquidCrystal.h>

using namespace ArduinoJson::Parser;

// MAC definition 
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//char server[] = "192.168.25.4"; 
byte server[] = { 
  192, 168, 25, 3 };
boolean startRead = false;
String jsonString = "";
String jsonStringCurr = "";

// Variaveis para a biblioteca JsonParser
JsonParser<16> parser ;

//server port
char serverUrl[] = "cddashboard.herokuapp.com" ;
//const int port = 4567;
const int port = 80;
EthernetClient client;
IPAddress ipArduino ;

// valores recebidos do servidor
double contador_double ;
char*  temas_string   ;
char*  nomeProposicao_string ;

// Variaveis LCD
//LiquidCrystal(rs, enable, d4, d5, d6, d7)  ; // rw do lcd ligado ao gnd
LiquidCrystal lcd(A0, A1,A2,A3,A4,A5 );

// Variaveis Servo
Servo servo;

void setup() {
  // Inicializa a Serial
  Serial.begin(9600);
  Serial.println("Iniciando ...") ;



  //Inicializa LCD
  lcd.begin(16, 4);
  lcd.setCursor(0,0) ;
  lcd.print("Iniciando") ;    
  lcd.setCursor(0,1) ;
  lcd.print("Linha 0x01") ;    
  lcd.setCursor(0,2) ;
  lcd.print("Linha 0x02") ;    
  lcd.setCursor(0,3) ;
  lcd.print("Linha 0x03") ;    

  //Inicializa Servo 
  servo.attach(4) ;

  // Iniciando a conexao ethernet e recebendo o IP dinamicamente
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Erro na rede");
    Serial.println("Retry em 5 segs");
    // Problemas na conexao ethernet, verifique a sua rede
    delay(5000);

  } 
  lcd.clear() ;
  ipArduino = Ethernet.localIP() ;
  Serial.print("IP obtido por DHCP:") ;
  Serial.println(ipArduino) ;
  lcd.setCursor(0,0) ;
  lcd.print("IP por DHCP") ;    
  lcd.setCursor(0,1) ;
  lcd.print(ipArduino) ;    


}

void loop()
{
  Serial.println("Aguarde 5 segs");
  delay(5000);

  //Consulta a URL / do servidor e recebe uma String com notacao JSON
  getMessageFromServer() ;
  // moveServo() ;
}

void getMessageFromServer() {
  //tenta conectar ao servidor
  //int ret = client.connect(server, port) ;
  int ret = client.connect(serverUrl, port) ;

  if( ret != 1) {
    Serial.print("Erro de conexao: ") ;
    Serial.println(ret,DEC)  ;
    return ;
  }

  Serial.println("Conectado...") ;
  // faz requisicao ao servidor
  //client.println("GET /dado2/1 HTTP/1.1");
  //client.println("HOST: localhost");

  client.println("GET /api/pl_visitas/0 HTTP/1.1");
  client.println("HOST: cddashboard.herokuapp.com");
  
  client.println();

  //Aguardando conexao
  while(!client.available()){
    delay(1);
  }  

  //verifica se tem caracter de resposta
  int startJson = false ;
  // limpa a string json
  jsonString = "";
  
  while( client.available()) {
    char c = client.read() ;
    if(startJson == true) {
      jsonString += c ;
      if(c == '}' ) {
        Serial.println("}") ;
        startJson = false ;
      }  

    } 
    else {
      if(c == '{' ) {
        Serial.println("{" );
        startJson = true ;
        jsonString += c ;
      }  

    }

  }

  Serial.println(jsonString) ;
  char jsonChar[jsonString.length()];
  jsonString.toCharArray(jsonChar, jsonString.length() + 1);
  parseMessageJson(jsonChar)  ;

  displayMessage() ;

  while(client.connected()){
    delay(1);
  }

  // encerra o cliente se encerrou a conexao
  if(!client.connected()) {
    client.stop() ;
    Serial.println("Desconectado" ) ;
  }

}

void parseMessageJson(char *jsonString) {

  JsonHashTable root = parser.parseHashTable(jsonString);
  if(!root.success())
  { 
    Serial.println("Falha no parsing JsonParser") ;  
    return ;
  } 
  contador_double  = root.getDouble("contador") ;
  temas_string = root.getString("temas") ;
  nomeProposicao_string = root.getString("nomeProposicao") ;

}  


void displayMessage() {
  Serial.println("DEBUG:") ;
  Serial.println(contador_double);
  Serial.println(nomeProposicao_string );
  Serial.println(temas_string );

  lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print(nomeProposicao_string) ; 

  
  lcd.setCursor(0,1) ;
  String temas = temas_string ;
  //lcd.print(temas_string) ; 
  lcd.print(temas.substring(0,16)) ; 
  lcd.setCursor(-4,2) ;
  lcd.print(temas.substring(15,32)) ; 
  lcd.setCursor(-4,3) ;
  lcd.print(temas.substring(31,48)) ; 

}

void moveServo() {
  int angulo = (int) contador_double * 180 ;
  servo.write(angulo) ;
}





