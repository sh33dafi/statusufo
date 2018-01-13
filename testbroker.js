var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://server.home.codeandglory.be')

client.on('connect', function () {
  client.subscribe('/feeds/test')

  setInterval(() => {
      client.publish('/feeds/test', 'Hello')
  }, 1000);

})

client.on('message', function (topic, message) {
  console.log(message.toString())
})
