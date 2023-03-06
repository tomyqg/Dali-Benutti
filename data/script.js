$(document).ready(function() {
  // Connect to WebSockets server
  var socket = new WebSocket('ws://' + location.hostname + ':81/');

  // Set onopen event
  socket.onopen = function() {
    console.log('Connected to WebSockets server');
  };

  // Set onmessage event
  socket.onmessage = function(event) {
    var message = JSON.parse(event.data);

    // Handle incoming message
    if (message.hasOwnProperty('lights')) {
      // Handle findLights response
      var lights = message.lights.split(',');

      for (var i = 0; i < lights.length; i++) {
        var light = lights[i];
      
        var lightControl = 
          '<div class="lightControl">' +
            '<h2>Light ' + light + '</h2>' +
            '<select class="light-number" id="light-number-' + light + '">' +
              '<option value="' + light + '">' + light + '</option>' +
            '</select>' +
            '<br><br>' +
            '<input type="text" class="light-level" id="light-level-' + light + '">' +
            '<button class="set-light-level" id="set-light-level-' + light + '">Set Level</button>' +
            '<br><br>' +
          '</div>';
      
        $('#lightsContainer').append(lightControl);
      
        // Set light level
        (function(light) {
          $('#set-light-level-' + light).click(function() {
            var lightNumber = $('#light-number-' + light).val();
            var level = $('#light-level-' + light).val();
            console.log("lightNumber: ", lightNumber);
            console.log("level: ", level);
            socket.send(JSON.stringify({
              setLightLevel: true,
              lightNumber: lightNumber,
              level: level
            }));
          });
        })(light);
      }

    } else if (message.hasOwnProperty('lightLevel')) {
      // Handle getLightLevel response
      var lightNumber = message.lightNumber;
      $('#light-level-' + lightNumber).val(message.lightLevel);

    }
  };

  // Send findLights request
  socket.onopen = function() {
  socket.send(JSON.stringify({ findLights: true }));
  };
});
