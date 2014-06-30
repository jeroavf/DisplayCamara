var dado  = {
  percentual: '70' ,
  pl: 'PLM 12345',
  tema: 'Saude'
};
var express = require('express') ;
var app = express() ;

app.get('/', function(req , res) {
  res.type('text/plain') ;
  res.send('Hello express & node !') ;
}) ;

app.get('/dado/1', function(req , res) {
  res.json(dado) ;
}) ;

app.listen(process.env.PORT || 4567 ) ;
console.log("Running on port 4567") ;
