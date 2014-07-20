var dado  = {
  percentual: 70 ,
  pl: 'PLM 12345',
  tema: 'Saude'
};

var dado2 = 
{
  "nomeProposicao":"PLP 137/2004",
  "temas":["Economia, Administração Financeira e Orçamentária (utilizado até a legislatura 53)"],
  "contador":8244
}

var dado3 = 
{
  "nomeProposicao":"PL 6613/2009",
  "temas": "Administracao Publica" ,
  "contador":0.05
}

var dado4 = 
{
  "nomeProposicao":"PL 6613/2009",
  "temas": "Economia, Administracao Financeira" ,
  "contador":0.0512121425
}
var conta_envios = 0 ;

var express = require('express') ;
var moment = require("moment") ;
var app = express() ;

app.get('/', function(req , res) {
  res.type('text/plain') ;
  res.send('Hello express & node !') ;
}) ;

app.get('/dado/1', function(req , res) {
  dado.percentual = Math.floor( Math.random() * 180 ) ;
  res.json(dado) ;
  conta_envios += 1 ;
  console.log("get dado/1 ------------------------") ;
  console.log("Valor enviado: " + dado.percentual) ;
  console.log("Dado enviado") ;
  console.log("Envios: " + conta_envios) ;
  now = moment(new Date()) ;
  console.log( now.format("DD/MM/YYYY HH:mm") ) ;
}) ;

app.get('/dado2/1', function(req , res) {
  dado.percentual = Math.floor( Math.random() * 180 ) ;
  res.json(dado4) ;
  conta_envios += 1 ;
  console.log("get dado2/1 ------------------------") ;
  console.log("Valor enviado: " + dado.percentual) ;
  console.log("Dado enviado") ;
  console.log("Envios: " + conta_envios) ;
  now = moment(new Date()) ;
  console.log( now.format("DD/MM/YYYY HH:mm") ) ;
}) ;


app.listen(process.env.PORT || 4567 ) ;
console.log("Running on port 4567") ;
