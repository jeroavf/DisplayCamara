/*
  ModuloDisplay - 05/07/2014
  Prototipo de display com exibicao de informacao por um servo-motor simulando um display analogico
  e display lcd de 2 linhas por 16 caracteres
  Arduino - Brasilia 
  Autor : Jeronimo Avelar Filho
  Versao 0.1
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
  Serial.println("Aguarde 2 segs");
  delay(2000);

  //Consulta a URL / do servidor e recebe uma String com notacao JSON
  jsonString = requestServer();

  if(!jsonString.equals("")){
    if(!jsonString.equals(jsonStringCurr)){

      //Atualiza a variavel que armazena o JSON atual
      jsonStringCurr = jsonString;

      char jsonChar[jsonString.length()];
      jsonString.toCharArray(jsonChar, jsonString.length() + 1);
      parseJson(jsonChar);
      jsonString = "";
    }
  }
}

String requestServer(){

  Serial.println("Conectando server node...");

  // Iniciando conexao com o servidor 
  if (client.connect(server, port)) {
    Serial.println("Conectado");
    // GET no PATH / do servidor
    client.println("GET /dado/1 HTTP/1.1");
    client.println("HOST: 192.168.25.5");
    client.println();

    //Aguardando conexao
    while(!client.available()){
      delay(1);
    }

    //Percorre os caracteres do envelope HTTP do servidor e armazena na String apenas o conteudo JSON
    while (client.available()) {
      char c = client.read();
      if( c == '{' ) { 
        startRead = true; 
        //Serial.print(c) ;
      }
      if ( startRead ) { 
        jsonString += c; 
      }
    }

    //Reseta a flag de leitura de conteudo JSON
    startRead = false;

  } 
  else {
    // Caso nao ocorra conexao
    Serial.println("Conexao falhou");
  }

  //Aguarda a desconexao com o servidor
  while(client.connected()){
    delay(1);
  }

  // Finaliza o socket de conexao
  if (!client.connected()) {
    Serial.println();
    Serial.println("Desconectando...");
    client.stop();

  }

  return jsonString;

}

void parseJson(char *jsonString) 
{
  //Inicializa o objeto Pai
  Serial.println("Iniciando parsing ...")  ;
  
  aJsonObject* root = aJson.parse(jsonString);

  if (root != NULL) {
    //Caputura o objeto percentual
    aJsonObject* percentual = aJson.getObjectItem(root, "percentual"); 

    //Caputura o objeto pl
    aJsonObject* pl = aJson.getObjectItem(root, "pl");

    //Caputura o objeto totalSolicitacoes
    aJsonObject* tema = aJson.getObjectItem(root, "tema");

    int percentual_int = percentual->valueint;
    String pl_string =  pl->valuestring ;
    String tema_string  =  tema->valuestring ;


    Serial.println(percentual_int);
    Serial.println(pl_string );
    Serial.println(tema_string );


  }

  //Deleta o objeto apos a utilizacao para liberacao de memoria
  aJson.deleteItem(root);
  Serial.println("Fim parsing ...")  ;

}





