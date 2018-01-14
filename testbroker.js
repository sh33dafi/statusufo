var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://server.home.codeandglory.be')

var express = require('express');
var app = express();

client.on('connect', function () {
  client.subscribe('/feeds/test')
  app.listen(3000);
})

client.on('message', function (topic, message) {
  console.log(message.toString())
})


app.get('/led/:status', function(req, res){
  let status = 1;
  if (req.params.status === "off") {
    status = 0;
  }
  console.log("sending", ""+ status)
  client.publish('/feeds/test', ""+ status);
  res.status(200).end();
});
