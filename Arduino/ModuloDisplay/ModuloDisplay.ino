/*
  ModuloDisplay - 05/07/2014
  Prototipo de display com exibicao de informacao por um servo-motor simulando um display analogico
  e display lcd de 2 linhas por 16 caracteres
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
#include <aJSON.h>

// MAC definition 
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//char server[] = "192.168.25.4"; 
byte server[] = { 
  192, 168, 25, 5 };
boolean startRead = false;
String jsonString = "";
String jsonStringCurr = "";
//server port
const int port = 4567;
EthernetClient client;
IPAddress ipArduino ;

// valores recebidos do servidor
int percentual_int  ;
String pl_string   ;
String tema_string ;


void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando ...") ;

  // Iniciando a conexao ethernet e recebendo o IP dinamicamente
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Erro na rede");
    Serial.println("Retry apos 5 segs");
    // Problemas na conexao ethernet, verifique a sua rede
    delay(5000);
    setup();
  } 
  else {
    ipArduino = Ethernet.localIP() ;
    Serial.print("IP obtido por DHCP:") ;
    Serial.println(ipArduino) ;
  }  

}

void loop()
{
  Serial.println("Aguarde 5 segs");
  delay(5000);

  //Consulta a URL / do servidor e recebe uma String com notacao JSON
  getMessageFromServer() ;
  displayMessage() ;
  moveServo() ;
}

void getMessageFromServer() {
  //tenta conectar ao servidor
  int ret = client.connect(server, port) ;
  
  //Serial.println(ret,DEC)  ;
  //verifica o retorno se nao conectou encerra
  //if( !client.connect(server, port)) {
  if( ret != 1) {
    Serial.print("Erro de conexao: ") ;
    Serial.println(ret,DEC)  ;
    return ;
  }
  
  Serial.println("Conectado...") ;
  // faz requisicao ao servidor
  client.println("GET /dado/1 HTTP/1.1");
  client.println("HOST: 192.168.25.5");
  client.println();

    //Aguardando conexao
  while(!client.available()){
    delay(1);
  }  
  // limpa a string json 
  jsonString ="" ;
  
  //verifica se tem caracter de resposta
  int startJson = false ;
  
  while( client.available()) {
    char c = client.read() ;
    if(startJson == true) {
      jsonString += c ;
      if(c == '}' ) {
        startJson = false ;
      }  
        
    } else {
      if(c == '{' ) {
        startJson = true ;
        jsonString += c ;
      }  
      
    }

  }
  
  //Serial.println(jsonString) ;
  char jsonChar[jsonString.length()];
  jsonString.toCharArray(jsonChar, jsonString.length() + 1);
  parseJson(jsonChar);  
  
  while(client.connected()){
    delay(1);
  }
  
  // encerra o cliente se encerrou a conexao
  if(!client.connected()) {
    client.stop() ;
    Serial.println("Disconectado" ) ;
  }

}

void displayMessage() {
  Serial.println(percentual_int);
  Serial.println(pl_string );
  Serial.println(tema_string );
}

void moveServo() {

}

void parseJson(char *jsonString) 
{
  //Inicializa o objeto Pai
  //Serial.println("Iniciando parsing ...")  ;
  
  aJsonObject* root = aJson.parse(jsonString);

  if (root != NULL) {
    //Caputura o objeto percentual
    aJsonObject* percentual = aJson.getObjectItem(root, "percentual"); 

    //Caputura o objeto pl
    aJsonObject* pl = aJson.getObjectItem(root, "pl");

    //Caputura o objeto totalSolicitacoes
    aJsonObject* tema = aJson.getObjectItem(root, "tema");

    percentual_int = percentual->valueint;
    pl_string =  pl->valuestring ;
    tema_string  =  tema->valuestring ;



  }

  //Deleta o objeto apos a utilizacao para liberacao de memoria
  aJson.deleteItem(root);
  //Serial.println("Fim parsing ...")  ;

}





