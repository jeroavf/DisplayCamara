
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
#include <Servo.h>
#include <LiquidCrystal.h>

// MAC definition 
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//char server[] = "192.168.25.4"; 
byte server[] = { 
  192, 168, 25, 3 };
boolean startRead = false;
String jsonString = "";
String jsonStringCurr = "";
//server port
const int port = 4567;
EthernetClient client;
IPAddress ipArduino ;

// valores recebidos do servidor
//int percentual_int  ;
//String pl_string   ;
//String tema_string ;

int contador_int  ;
String temas_string   ;
String nomeProposicao_string ;

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
  lcd.begin(16, 2);
  lcd.setCursor(0,0) ;
  lcd.print("Iniciando") ;    

  //Inicializa Servo 
  servo.attach(4) ;



  // Iniciando a conexao ethernet e recebendo o IP dinamicamente
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Erro na rede");
    Serial.println("Retry em 5 segs");
    // Problemas na conexao ethernet, verifique a sua rede
    delay(5000);
 
  } 

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
  //displayMessage() ;
 // moveServo() ;
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
  client.println("GET /dado2/1 HTTP/1.1");
  client.println("HOST: 192.168.25.3");
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
  parseJson(jsonChar);  
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

void displayMessage() {
  Serial.println("DEBUG:") ;
  Serial.println(contador_int);
  Serial.println(nomeProposicao_string );
  Serial.println(temas_string );

  /*lcd.clear() ;
  lcd.setCursor(0,0) ;
  lcd.print(nomeProposicao_string) ; 

  lcd.setCursor(0,1) ;
  lcd.print(temas_string) ; 
  */
}

void moveServo() {
   servo.write(contador_int) ;
}

void parseJson(char *jsonString) 
{
  //Inicializa o objeto Pai
  //Serial.println("Iniciando parsing ...")  ;

  aJsonObject* root = aJson.parse(jsonString);

  if (root != NULL) {
    //Caputura o objeto contador
    //aJsonObject* percentual = aJson.getObjectItem(root, "percentual"); 
    aJsonObject* contador = aJson.getObjectItem(root, "contador"); 

    //Caputura o objeto temas
    //aJsonObject* pl = aJson.getObjectItem(root, "pl");
    aJsonObject* temas = aJson.getObjectItem(root, "temas");

    //Caputura o objeto totalSolicitacoes
    //aJsonObject* tema = aJson.getObjectItem(root, "tema");
    aJsonObject* nomeProposicao = aJson.getObjectItem(root, "nomeProposicao");

/*    percentual_int = percentual->valueint;
    pl_string =  pl->valuestring ;
    tema_string  =  tema->valuestring ;
*/
    contador_int = contador->valueint;
    temas_string =  temas->valuestring ;
    nomeProposicao_string  =  nomeProposicao->valuestring ;


  }

  //Deleta o objeto apos a utilizacao para liberacao de memoria
  aJson.deleteItem(root);
  //Serial.println("Fim parsing ...")  ;

}






