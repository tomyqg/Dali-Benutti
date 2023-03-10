class DLight {
  constructor(shortAddress, name , room, minLevel, maxLevel, groups = [], sceneLevels = [], failLevel = 254, powerOnLevel = 254, phyMinLevel, fadeTime, fadeRate,level) {
    this.shortAddress = shortAddress;
    this.name = name;
    this.room = room;
    this.minLevel = minLevel;
    this.maxLevel = maxLevel;
    this.groups = groups;
    this.sceneLevels = sceneLevels;
    this.failLevel = failLevel;
    this.powerOnLevel = powerOnLevel;
    this.phyMinLevel = phyMinLevel;
    this.fadeTime = fadeTime;
    this.fadeRate = fadeRate;
    this.level = level;
  }
}
var Dlights=[];
var socket;
var sockurl ="ws://192.168.103.77:81" //debugging address
// var sockurl ="ws://" + window.location.hostname + ":81";
//class DGroup

//class DScene

function sendMessage(socket, message) {
  if (socket.readyState === WebSocket.OPEN) {
    socket.send(message);
  } else {
    socket = new WebSocket(sockurl);
    socket.onopen = function() {
    socket.send(message);};
  }
};

function getFadeRateText(fadeRate) {
  switch (fadeRate) {
    case 0:
      return "Not permitted";
    case 1:
      return "357.796 steps/s";
    case 2:
      return "253.000 steps/s";
    case 3:
      return "178.898 steps/s";
    case 4:
      return "126.500 steps/s";
    case 5:
      return "89.449 steps/s";
    case 6:
      return "63.250 steps/s";
    case 7:
      return "44.725 steps/s";
    case 8:
      return "31.625 steps/s";
    case 9:
      return "22.362 steps/s";
    case 10:
      return "15.813 steps/s";
    case 11:
      return "11.181 steps/s";
    case 12:
      return "7.906 steps/s";
    case 13:
      return "5.591 steps/s";
    case 14:
      return "3.953 steps/s";
    case 15:
      return "2.795 steps/s";
    default:
      return "Invalid fade rate value";
  }
}

function getFadeTimeText(fadeTimeValue) {
  switch (fadeTimeValue) {
    case 0:
      return "< 0.707s";
    case 1:
      return "0.707s";
    case 2:
      return "1.000s";
    case 3:
      return "1.414s";
    case 4:
      return "2.000s";
    case 5:
      return "2.828s";
    case 6:
      return "4.000s";
    case 7:
      return "5.657s";
    case 8:
      return "8.000s";
    case 9:
      return "11.314s";
    case 10:
      return "16.000s";
    case 11:
      return "22.627s";
    case 12:
      return "32.000s";
    case 13:
      return "45.255s";
    case 14:
      return "64.000s";
    case 15:
      return "90.510s";
    default:
      return "Invalid fade time value";
  }
}

function renderAccordion() {
  var rooms = [];
  Dlights.forEach(function(light) {
    if (!rooms.includes(light.room) && light.room !== "Unknown") {
      rooms.push(light.room);
    }
  });
  rooms.sort();
  rooms.unshift("Unknown");

  var accordionHTML = "";
  rooms.forEach(function(room) {
    accordionHTML += '<h3>' + room + '</h3><div><div class="light-buttons  button-grid">';
    Dlights.forEach(function(light) {
      if (light.room === room) {
        accordionHTML +=
          '<button data-light="' +
          light.shortAddress +
          '" class="ui-button ui-corner-all">' +
          "DALI A" +
          light.shortAddress +
          " " +
          light.name +
          "</button> ";
      }
    });
    accordionHTML += "</div></div>";
  });
  $("#roomsAccordion").empty();
  $("#roomsAccordion").html(accordionHTML);
  $("#roomsAccordion").accordion({
    collapsible: true,
    active: false,
    heightStyle: "content"
  });

  $(".light-buttons button").on("click", function() {
    var lightShortAddress = $(this).data("light");
    var daliLightData = Dlights.find(function(light) {
      return light.shortAddress === lightShortAddress;
    });
    
    var formHTML = '<form align="center"><input type="hidden" name="command" value="saveLightData"><input type="hidden" name="shortAddress" value="'+daliLightData.shortAddress+'">';
    formHTML += '<div class="controlgroup"><div class="form-group"><label for="lightName">Name:</label><input type="text" class="form-control" id="lightName" name="lightName" value="' + daliLightData.name + '"></div>';
    formHTML += '<div class="form-group"><label for="lightRoom">Room:</label><input type="text" class="form-control" id="lightRoom" name="lightRoom" value="' + daliLightData.room + '"></div>';
    formHTML += '<div class="form-group">';
    formHTML += '<label for="curLevelSlider">Current Level:</label>';
    formHTML += '<input type="range" min="0" max="100" class="form-control-range curLevelSlider" name="currentLevel" id="light-slidr-' + daliLightData.shortAddress + '" value="' + daliLightData.level + '">';
    formHTML += '<output id="curLevelValue">' + daliLightData.level + '</output>';
    formHTML += '</div>';
    formHTML += '<label for="maxLevelSlider">Max Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="maxLevelSlider" name="maxLevel" value="' + daliLightData.maxLevel + '">';
    formHTML += '<output id="maxLevelValue">' + daliLightData.maxLevel + '</output>';
    formHTML += '</div>';
    
    formHTML += '<div class="form-group">';
    formHTML += '<label for="minLevelSlider">Min Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="minLevelSlider" name="minLevel" value="' + daliLightData.minLevel + '">';
    formHTML += '<output id="minLevelValue">' + daliLightData.minLevel + '</output>';
    formHTML += '</div>';
    
    formHTML += '<div class="form-group">';
    formHTML += '<label for="failLevelSlider">System Fail Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="failLevelSlider" name="failLevel" value="' + daliLightData.failLevel + '">';
    formHTML += '<output id="failLevelValue">' + daliLightData.failLevel + '</output>';
    formHTML += '</div>';
    formHTML += '<div class="form-group">';
    formHTML += '<label for="powerOnLevelSlider">Power On Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="powerOnLevelSlider" name="powerOnLevel" value="' + daliLightData.powerOnLevel + '">';
    formHTML += '<output id="powerOnLevelValue">' + daliLightData.powerOnLevel + '</output>';
    formHTML += '</div>';
    formHTML += '<div class="form-group">\
    <label for="fadeTimeSlider">Fade Time:</label>\
    <input type="range" min="0" max="15" value="' + daliLightData.fadeTime + '" class="form-control-range" name="fadeTime" id="fadeTimeSlider">\
    <div id="fadeTimeValue">' + getFadeTimeText(daliLightData.fadeTime) + '</div>\
  </div>';  
    formHTML += '<div class="form-group">\
    <label for="fadeRate">Fade Rate:</label>\
    <input type="range" min="1" max="15" step="1" class="form-control" id="fadeRateSlider" name="fadeRate" value="' + daliLightData.fadeRate + '">\
    <div id="fadeRateValue">' + getFadeRateText(daliLightData.fadeRate) + '</div>\
  </div>';
    formHTML += '<table><tr>'
    // console.log(daliLightData);
    for (var i = 0; i <= 15; i++) {
      formHTML += '<td><div class="form-group"><label for="groupMembership' + i + '">Group ' + i + ':</label><input type="checkbox" class="form-check-input" name="groupMembership' + i + '" id="groupMembership' + i + '" value="1" ' + (daliLightData.groups[i] ? 'checked="checked"' : '') + '></div></td>';
      formHTML += '<td><div class="form-group"><label for="sceneLevel' + i + '">Scene ' + i + ':</label><input type="number" class="form-control" name="sceneLevel' + i + '" id="sceneLevel' + i + '" min="0" max="255" value="' + daliLightData.sceneLevels[i] + '"></div></td></tr><tr>';
    }
    formHTML.slice(0,-5);

    formHTML += '</tr></div><input type="submit" tabindex="-1" style="position:absolute; top:-1000px">';
    formHTML += '</form>';

    // open the dialog with the form
    $(formHTML).dialog({
      modal: true,
      resizable: false,
      width: 'auto',
      height: 'auto',
      close: function() {
        $(this).remove();
      },
      buttons: {
        "Cancel": function() {
          $(this).dialog('close');
        },
        "Save": function() {
          var form = $(this).find("form").on("submit", function(event) {
            event.preventDefault();
          });
        
          var formData = form.prevObject.serializeArray().reduce(function(obj, item) {
            obj[item.name] = item.value;
            return obj;
          }, {});
        
          // Find the DLight instance with the matching shortAddress
          var daliLightData = Dlights.find(function(light) {
            return light.shortAddress === parseInt(formData.shortAddress);
          });
        
          // Update the DLight instance properties with the form data
          daliLightData.name = formData.lightName;
          daliLightData.room = formData.lightRoom;
          daliLightData.level = parseInt(formData.currentLevel);
          daliLightData.maxLevel = parseInt(formData.maxLevel);
          daliLightData.minLevel = parseInt(formData.minLevel);
          daliLightData.failLevel = parseInt(formData.failLevel);
          daliLightData.powerOnLevel = parseInt(formData.powerOnLevel);
          daliLightData.fadeTime = parseInt(formData.fadeTime);
          daliLightData.fadeRate = parseInt(formData.fadeRate);
        
          // Update the group memberships and scene levels array
          for (var i = 0; i <= 15; i++) {
            var groupMembership = parseInt(formData["groupMembership" + i]);
            daliLightData.groups[i] = (groupMembership === "1");
            daliLightData.sceneLevels[i] = parseInt(formData["sceneLevel" + i]);
          }

            // Create a JSON object with the updated DLight data
        var updatedData = {
          shortAddress: daliLightData.shortAddress,
          name: daliLightData.name,
          room: daliLightData.room,
          level: daliLightData.level,
          maxLevel: daliLightData.maxLevel,
          minLevel: daliLightData.minLevel,
          failLevel: daliLightData.failLevel,
          powerOnLevel: daliLightData.powerOnLevel,
          fadeTime: daliLightData.fadeTime,
          fadeRate: daliLightData.fadeRate,
          groups: daliLightData.groups,
          sceneLevels: daliLightData.sceneLevels
        };

        // Send the updated data to the server via the WebSocket
        var socketData = {
          command: "updateLight",
          lightData: updatedData
        };
        socket.send(JSON.stringify(socketData));
          $(this).dialog('close');
          CreateHtmlLights();

        }
        // "Save": function() {
        //   var form = $(this).find( "form" ).on( "submit", function( event ) {
        //     event.preventDefault();
        //   });

        //   var formData = form.prevObject.serializeArray().reduce(function(obj, item) {
        //     obj[item.name] = item.value;
        //     return obj;
        //   }, {});

        
        //   var jsonFormData = JSON.stringify(formData);
        //   console.log(formData);
        //   // socket.send(jsonFormData);
        //   $(this).dialog('close');
        // }
      }
    });

    $(".curLevelSlider").on("input", function() {
      var curLevelValue = parseInt($(this).val());
      $("#curLevelValue").val(curLevelValue+"%");
      var idValue = $(this).attr('id');
      var shortAddress = idValue.split("-")[2];
      var daliLight = Dlights.find(function(light) {
        return light.shortAddress == shortAddress;
        });
      if (curLevelValue == 0) {
        var daliValue = 0;
        }
        else if (curLevelValue == 1){
        var daliValue = daliLight.minLevel;
        }
        else {
        var percentage = curLevelValue / 100;
        var daliValue = daliLight.minLevel + (daliLight.maxLevel - daliLight.minLevel) * percentage;
        daliValue = Math.round(daliValue);
        }

        // console.log("Short Address:" + shortAddress);
        // console.log("  Slider value: " + curLevelValue + "%");
        // console.log("  DALI value: " + daliValue);
        setTimeout(function() {
          socket.send(JSON.stringify({
            "command": "setLevel",
            "shortAddress": shortAddress,
            "level":daliValue
          }));
        }, 200); // delay of 500 milliseconds
    }); 

    $("#maxLevelSlider").on("input", function() {
      var maxLevelValue = parseInt($(this).val());
      if (maxLevelValue < $("#minLevelSlider").val()) {
        $("#minLevelSlider").val(maxLevelValue);
        $("#minLevelValue").val(maxLevelValue);
      }
      $("#maxLevelValue").val(maxLevelValue);
    });
    
    $("#minLevelSlider").on("input", function() {
      var minLevelValue = parseInt($(this).val());
      if (minLevelValue > $("#maxLevelSlider").val()) {
        $("#maxLevelSlider").val(minLevelValue);
        $("#maxLevelValue").val(minLevelValue);
      }
      $("#minLevelValue").val(minLevelValue);
    });
    
    $("#failLevelSlider").on("input", function() {
      var failLevelValue = parseInt($(this).val());
      $("#failLevelValue").val(failLevelValue);
    });    
    $("#curLevelSlider").on("input", function() {
      var curLevelValue = parseInt($(this).val());
      $("#curLevelValue").val(curLevelValue);
    });
    
    $("#powerOnLevelSlider").on("input", function() {
      var powerOnLevelValue = parseInt($(this).val());
      $("#powerOnLevelValue").val(powerOnLevelValue);
    });
  
    $("#fadeRateSlider").on("input", function() {
      var fadeRateValue = parseInt($(this).val());
      $("#fadeRateValue").text(getFadeRateText(fadeRateValue));
    });
    $("#fadeTimeSlider").on("input", function() {
      var fadeTimeValue = parseInt($(this).val());
      $("#fadeTimeValue").text(getFadeTimeText(fadeTimeValue));
    });
   $( ".controlgroup" ).controlgroup()
  
});
};
    

 


function CreateHtmlLights(){
  var mainhtml = `
  <div id="Tabs">
    <ul id="navlist">
    </ul>
    <!-- dynamically generated sections for each room -->
    <div id="config">
      <h2>Configuration Page</h2>
      <ul>
        <li>
          <button id="init-all" class="ui-button ui-corner-all">INIT ALL</button>
          <button id="init-new" class="ui-button ui-corner-all">INIT NEW</button>
          <button id="search" class="ui-button ui-corner-all">SEARCH</button>
          <button id="save" class="ui-button ui-corner-all">SAVE</button>
        </li>
        <li>
          <h3>Lights</h3>
          <div id="roomsAccordion">
            <!-- dynamically generated accordion for each room and its lights -->
          </div>
        </li>
      </ul>
    </div>
  </div>
`;

  // console.log(Dlights);
  $("main").empty();
  // Select the elements you want to keep inside the div
  $("main").html(mainhtml);

  var rooms = [];
  Dlights.forEach(function(light) {
    if (!rooms.includes(light.room) && light.room !== "Unknown") {
      rooms.push(light.room);
    }
  });

  rooms.forEach(function(room) {
    // console.log("room value: ", room);
      // Create a navigation link for each room
      var navLink = $("<li>").append(
        $("<a>", {
          href: "#" + room.toLowerCase(),
          text: room
        })
      );
    $("#navlist").append(navLink);
      // Create a room section for each room
      var roomDiv = $("<div>", {
        id: room.toLowerCase()
      }).append(
        $("<h2>", {
          text: room
        }),
        $("<ul>")
      );
      $("#Tabs").append(roomDiv);
    });
      var navLink2 =$("<li>").append(
        $("<a>", {
          href: "#config",
          text: "Configuration"
        })
      );
      $("#navlist").append(navLink2);
//append the lights to the rooms
  Dlights.forEach(function(light) {
    if (light.room !== "Unknown") {
      var room = light.room.toLowerCase();
      var lightName = light.name;
      var shortAddress = light.shortAddress;
      var lightItem = 
      '<div class="lightControl" id="light-number-' + shortAddress + '">' +
        '<h3>' + lightName + '</h3>' +
        '<div class="slider-container">' +
          '<input type="range" min="0" max="100" class="light-slider" id="light-slider-' + shortAddress + '">' +
        '</div>' +
      '</div>';
      $("#" + room + " ul").append(lightItem);
    }
  });
  renderAccordion();
  $( "#Tabs" ).tabs();
  $(".ui-widget-content ul").css("list-style", "none");

  $(".light-slider").on("input", function() {
    var sliderValue = $(this).val();
    var lightControl = $(this).closest(".lightControl");
    var shortAddress = lightControl.attr("id").split("-")[2];

    var daliLight = Dlights.find(function(light) {
    return light.shortAddress == shortAddress;
    });
    
    if (sliderValue == 0) {
    var color = "hsl(50, 100%, 0%, 0)";
    var daliValue = 0;
    }
    else if (sliderValue == 1){
    var color = "hsl(50, 100%, 1%, 0)";
    var daliValue = daliLight.minLevel;
    }
    else {
    var percentage = sliderValue / 100;
    var color = "hsl(50, 100%, " + (percentage * 40) + "%, " + percentage  + ")";
    var daliValue = daliLight.minLevel + (daliLight.maxLevel - daliLight.minLevel) * percentage;
    daliValue = Math.round(daliValue);
    }
    lightControl.css("background-color", color);
    //  console.log("Short Address:" + shortAddress);
    //  console.log("  Slider value: " + sliderValue + "%");
    //  console.log("  DALI value: " + daliValue);
    setTimeout(function() {
      socket.send(JSON.stringify({
        "command": "setLevel",
        "shortAddress": shortAddress,
        "level":daliValue
      }));
    }, 200); 

    });
    socket.send(JSON.stringify({
      command: "levels"
    }));
}


$(document).ready(function() {

  socket = new WebSocket(sockurl);


  
socket.onopen = function() {
  console.log("WebSocket connection established");
  socket.send(JSON.stringify({
    command: "getLights"
  }));
};

socket.onmessage = function(event) {
  console.log("Message received: " + event.data);
  var data = JSON.parse(event.data);
  
  if(data.hasOwnProperty("command")) {
    switch (data.command) {
      case "lights":
        // register lights command
        var lights = data.lights;
        Dlights = [];
        lights.forEach(function(light) {
          var dlight = new DLight(light.shortAddress,light.name,light.room,light.minLevel,light.maxLevel,light.groups,light.sceneLevels,light.failLevel,light.powerOnLevel,light.physmin,light.fadeTime,light.fadeRate,0);
          Dlights.push(dlight);
        });
        CreateHtmlLights();

        break;
        case "levels":
          data.lights.forEach(function(light) {
            var shortAddress = light.shortAddress;

            var level = light.level;
            var dlight = Dlights.find(function(dlight) {
              return dlight.shortAddress == shortAddress;
            });

            var range = dlight.maxLevel - dlight.minLevel;
            var percentage = (level - dlight.minLevel) / range *100;
            var color = "hsl(50, 100%, " + (percentage * 40/100) + "%, " + percentage/100 + ")";
            // console.log(color+" loaded");
            $("#light-number-" + shortAddress).css("background-color", color);
            $("#light-slider-" + shortAddress).val(percentage);
            dlight.level=level;
          });
          break;
      default:
        console.log("Unknown command: " + data.command);
    }
  }
  else{console.log("received unknown data")}
};

socket.onclose = function() {
  console.log("WebSocket connection closed");
};




      $("#init-all").click(function() {
        console.log("INIT ALL button clicked");
      });
    
      $("#init-new").click(function() {
        console.log("INIT NEW button clicked");
      });
    
      $("#search").click(function() {
        console.log("SEARCH button clicked");
      });

      $("#save").click(function() {
        console.log("SAVE button clicked");
      });


    $( "#rooms-accordion" ).accordion({
        collapsible: true,
        active: false,
        heightStyle: "content"
    });
    


  });
  