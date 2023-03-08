class DLight {
  constructor(shortAddress, name , room, minLevel, maxLevel, groups = [], sceneLevels = [], failLevel = 254, powerOnLevel = 254, phyMinLevel, fadeTime, fadeRate) {
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
  }
}
var Dlights=[];

//class DGroup

//class DScene

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
    accordionHTML += '<h3>' + room + '</h3><div><div class="light-buttons">';
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
    
    var formHTML = '<form><input type="hidden" name="command" value="saveLightData">';
    formHTML += '<div class="controlgroup"><div class="form-group"><label for="lightName">Name:</label><input type="text" class="form-control" id="lightName" name="lightName" value="' + daliLightData.name + '"></div>';
    formHTML += '<div class="form-group"><label for="lightRoom">Room:</label><input type="text" class="form-control" id="lightRoom" name="lightRoom" value="' + daliLightData.room + '"></div>';
    // formHTML += '<div class="form-group"><label for="maxLevel">Max Level:</label><input type="text" class="form-control" id="maxLevel" name="maxLevel" value="' + daliLightData.maxLevel + '"></div>';
    // formHTML += '<div class="form-group"><label for="minLevel">Min Level:</label><input type="text" class="form-control" id="minLevel" name="minLevel" value="' + daliLightData.minLevel + '"></div>';
    // formHTML += '<div class="form-group"><label for="failLevel">System Fail Level:</label><input type="text" class="form-control" id="failLevel" name="failLevel" value="' + daliLightData.failLevel + '"></div>';
    // formHTML += '<div class="form-group"><label for="powerOnLevel">Power On Level:</label><input type="text" class="form-control" id="powerOnLevel" name="powerOnLevel" value="' + daliLightData.powerOnLevel + '"></div>';
    formHTML += '<div class="form-group">';
    formHTML += '<label for="maxLevelSlider">Max Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="maxLevelSlider" name="maxLevelSlider" value="' + daliLightData.maxLevel + '">';
    formHTML += '<output id="maxLevelValue">' + daliLightData.maxLevel + '</output>';
    formHTML += '</div>';
    
    formHTML += '<div class="form-group">';
    formHTML += '<label for="minLevelSlider">Min Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="minLevelSlider" name="minLevelSlider" value="' + daliLightData.minLevel + '">';
    formHTML += '<output id="minLevelValue">' + daliLightData.minLevel + '</output>';
    formHTML += '</div>';
    
    formHTML += '<div class="form-group">';
    formHTML += '<label for="failLevelSlider">System Fail Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="failLevelSlider" name="failLevelSlider" value="' + daliLightData.failLevel + '">';
    formHTML += '<output id="failLevelValue">' + daliLightData.failLevel + '</output>';
    formHTML += '</div>';
    
    formHTML += '<div class="form-group">';
    formHTML += '<label for="powerOnLevelSlider">Power On Level:</label>';
    formHTML += '<input type="range" min="' + daliLightData.phyMinLevel + '" max="254" class="form-control-range" id="powerOnLevelSlider" name="powerOnLevelSlider" value="' + daliLightData.powerOnLevel + '">';
    formHTML += '<output id="powerOnLevelValue">' + daliLightData.powerOnLevel + '</output>';
    formHTML += '</div>';
    
    // formHTML += '<div class="form-group"><label for="fadeTime">Fade Time:</label><input type="text" class="form-control" id="fadeTime" name="fadeTime" value="' + daliLightData.fadeTime + '"></div>';
    formHTML += '<div class="form-group">\
    <label for="fadeTimeSlider">Fade Time:</label>\
    <input type="range" min="0" max="15" value="' + daliLightData.fadeTime + '" class="form-control-range" id="fadeTimeSlider">\
    <div id="fadeTimeValue">' + getFadeTimeText(daliLightData.fadeTime) + '</div>\
  </div>';  
    // formHTML += '<div class="form-group"><label for="fadeRate">Fade Rate:</label><input type="text" class="form-control" id="fadeRate" name="fadeRate" value="' + daliLightData.fadeRate + '"></div>';
    formHTML += '<div class="form-group">\
    <label for="fadeRate">Fade Rate:</label>\
    <input type="range" min="0" max="15" step="1" class="form-control" id="fadeRateSlider" name="fadeRateSlider" value="' + daliLightData.fadeRate + '">\
    <div id="fadeRateValue">' + getFadeRateText(daliLightData.fadeRate) + '</div>\
  </div>';
    formHTML += '<table><tr>'
    console.log(daliLightData);
    for (var i = 0; i <= 15; i++) {
      formHTML += '<td><div class="form-group"><label for="groupMembership' + i + '">Group ' + i + ':</label><input type="checkbox" class="form-check-input" name="groupMembership' + i + '" id="groupMembership' + i + '" value="1" ' + (daliLightData.groups[i] ? 'checked' : '') + '></div></td>';
      formHTML += '<td><div class="form-group"><label for="sceneLevel' + i + '">Scene ' + i + ':</label><input type="number" class="form-control" name="sceneLevel' + i + '" id="sceneLevel' + i + '" min="0" max="254" value="' + daliLightData.sceneLevels[i] + '"></div></td></tr><tr>';
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
          var form = $(this).find( "form" ).on( "submit", function( event ) {
            event.preventDefault();
          });

          var formData = form.prevObject.serializeArray().reduce(function(obj, item) {
            obj[item.name] = item.value;
            return obj;
          }, {});

        
          var jsonFormData = JSON.stringify(formData);
          console.log(formData);
          socket.send(jsonFormData);
          $(this).dialog('close');
        }
      }
    });
  });
  $("#maxLevelSlider").change(function() {
    var maxLevelValue = $(this).val();
    if (maxLevelValue < $("#minLevelSlider").val()) {
      $("#minLevelSlider").val(maxLevelValue);
    }
    $("#maxLevel").val(maxLevelValue);
  });
  
  $("#minLevelSlider").change(function() {
    var minLevelValue = $(this).val();
    if (minLevelValue > $("#maxLevelSlider").val()) {
      $("#maxLevelSlider").val(minLevelValue);
    }
    $("#minLevel").val(minLevelValue);
  });
  
  $("#failLevelSlider").change(function() {
    var failLevelValue = $(this).val();
    $("#failLevel").val(failLevelValue);
  });
  
  $("#powerOnLevelSlider").change(function() {
    var powerOnLevelValue = $(this).val();
    $("#powerOnLevel").val(powerOnLevelValue);
  });

  $("#fadeRateSlider").change(function() {
    var fadeRateValue = $(this).val();
    $("#fadeRateValue").text(getFadeRateText(fadeRateValue));
  });
  $("#fadeTimeSlider").change(function() {
    var fadeTimeValue = $(this).val();
    $("#fadeTimeValue").text(getFadeTimeText(fadeTimeValue));
  });
  $( ".controlgroup" ).controlgroup()
    
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
    console.log("Short Address:" + shortAddress);
    console.log("  Slider value: " + sliderValue + "%");
    console.log("  DALI value: " + daliValue);
    });
  };


function CreateHtmlLights(){
  // todo, clear nav , rooms and accordion
  console.log(Dlights);
  // $("#navlist").empty();
  // $("#navTabs").empty();
  var rooms = [];
  Dlights.forEach(function(light) {
    if (!rooms.includes(light.room) && light.room !== "Unknown") {
      rooms.push(light.room);
    }
  });

  rooms.forEach(function(room) {
    console.log("room value: ", room);
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
      $("#navTabs").append(roomDiv);
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
  $( "#navTabs" ).tabs();
  $(".ui-widget-content ul").css("list-style", "none");
}


$(document).ready(function() {

  var socket = new WebSocket("ws://192.168.103.77:81");
  // var socket = new WebSocket("ws://" + window.location.hostname + ":81");

  
socket.onopen = function() {
  console.log("WebSocket connection established");
  socket.send(JSON.stringify({
    command: "getLights"
  }));
};

socket.onmessage = function(event) {
  console.log("Message received: " + event.data);
  var data = JSON.parse(event.data);
  
  if (data.hasOwnProperty("frlightcnt")) {
    var frlightcnt = data.frlightcnt;
    var friendlyLights = data.friendlylights;

    // Do something with the frlightcnt and friendlylights information
    // Create an array to store the unique room names
    var rooms = [];
    // Loop through the friendlyLights array to extract the unique room names
    friendlyLights[0].friendlylights.forEach(function(light) {
      if (!rooms.includes(light.room)) {
        rooms.push(light.room);
      }
    });
    // Loop through the rooms array to create the navigation links and room sections
    rooms.forEach(function(room) {
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
      $("#navTabs").append(roomDiv);
    });
      var navLink2 =$("<li>").append(
        $("<a>", {
          href: "#config",
          text: "Configuration"
        })
      );
      $("#navlist").append(navLink2);
    // Loop through the friendlyLights array again to add the lights to their respective room sections
    friendlyLights[0].friendlylights.forEach(function(light) {
      var room = light.room.toLowerCase();
      var lightName = light.name;
      var shortAddress = light.shortAddress;
      var level = light.level;
      var lightItem = 
      '<div class="lightControl" id="light-number-' + shortAddress + '">' +
        '<h3>' + lightName + '</h3>' +
        '<div class="slider-container">' +
          '<input type="range" min="0" max="100" class="light-slider" id="light-slider-' + shortAddress + '">' +
        '</div>' +
      '</div>';
      $("#" + room + " ul").append(lightItem);
    });


    var rooms = [];
    friendlyLights[0].friendlylights.forEach(function(light) {
        if (rooms.indexOf(light.room) === -1) {
            rooms.push(light.room);
        }
    });
    
    var accordionHTML = '';
    rooms.forEach(function(room) {
        accordionHTML += '<h3>' + room + '</h3><div><div class="light-buttons">';
        friendlyLights[0].friendlylights.forEach(function(light) {
            if (light.room === room) {
                accordionHTML += '<button data-light="' + light.shortAddress + '" class="ui-button ui-corner-all">' + 'DALI A' + light.shortAddress + ' ' + light.name + '</button> ';
            }
        });
        accordionHTML += '</div></div>';
    });
    
    $('#roomsAccordion').html(accordionHTML);
    $("#roomsAccordion").accordion({
        collapsible: true,
        active: false,
        heightStyle: "content"
    });
    
    $('.light-buttons button').on('click', function() {
        var lightShortAddress = $(this).data('light');
        var lightData = friendlyLights[0].friendlylights.find(function(light) {
            return light.shortAddress === lightShortAddress;
        });
        var daliLightData = daliLights[0].lights.find(function(light) {
            return light.shortAddress === lightShortAddress;
        });
    
        var formHTML = '<form><input type="hidden" name="command" value="saveLightData">';
        formHTML += '<div class="controlgroup"><div class="form-group"><label for="lightName">Name:</label><input type="text" class="form-control" id="lightName" name="lightName" value="' + lightData.name + '"></div>';
        formHTML += '<div class="form-group"><label for="lightRoom">Room:</label><input type="text" class="form-control" id="lightRoom" name="lightRoom" value="' + lightData.room + '"></div>';
        formHTML += '<div class="form-group"><label for="maxLevel">Max Level:</label><input type="text" class="form-control" id="maxLevel" name="maxLevel" value="' + daliLightData.maxLevel + '"></div>';
        formHTML += '<div class="form-group"><label for="minLevel">Min Level:</label><input type="text" class="form-control" id="minLevel" name="minLevel" value="' + daliLightData.minLevel + '"></div>';
        formHTML += '<div class="form-group"><label for="failLevel">System Fail Level:</label><input type="text" class="form-control" id="failLevel" name="failLevel" value="' + daliLightData.failLevel + '"></div>';
        formHTML += '<div class="form-group"><label for="powerOnLevel">Power On Level:</label><input type="text" class="form-control" id="powerOnLevel" name="powerOnLevel" value="' + daliLightData.powerOnLevel + '"></div>';
        formHTML += '<div class="form-group"><label for="fadeTime">Fade Time:</label><input type="text" class="form-control" id="fadeTime" name="fadeTime" value="' + daliLightData.fadeTime + '"></div>';
        formHTML += '<div class="form-group"><label for="fadeRate">Fade Rate:</label><input type="text" class="form-control" id="fadeRate" name="fadeRate" value="' + daliLightData.fadeRate + '"></div>';
        formHTML += '<table><tr>'
        for (var i = 0; i <= 15; i++) {
          formHTML += '<td><div class="form-group"><label for="groupMembership' + i + '">Group ' + i + ':</label><input type="checkbox" class="form-check-input" name="groupMembership' + i + '" id="groupMembership' + i + '" value="1" ' + (daliLightData.groupMemberships[i] ? 'checked' : '') + '></div></td>';
          formHTML += '<td><div class="form-group"><label for="sceneLevel' + i + '">Scene ' + i + ':</label><input type="number" class="form-control" name="sceneLevel' + i + '" id="sceneLevel' + i + '" min="0" max="254" value="' + daliLightData.sceneLevels[i] + '"></div></td></tr><tr>';
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
          var form = $(this).find( "form" ).on( "submit", function( event ) {
            event.preventDefault();
          });

          var formData = form.prevObject.serializeArray().reduce(function(obj, item) {
            obj[item.name] = item.value;
            return obj;
          }, {});

        
          var jsonFormData = JSON.stringify(formData);
          console.log(formData);
          socket.send(jsonFormData);
          $(this).dialog('close');
        }
      }
    });
    

    });
    
    $( ".controlgroup" ).controlgroup()
    
      $(".light-slider").on("input", function() {
        var sliderValue = $(this).val();
        var lightControl = $(this).closest(".lightControl");
        var shortAddress = lightControl.attr("id").split("-")[2];
        
        var daliLight = daliLights[0].lights.find(function(light) {
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
        console.log("Short Address:");
        console.log("  Slider value: " + sliderValue + "%");
        console.log("  DALI value: " + daliValue);
        });
    $( "#navTabs" ).tabs();
    $(".ui-widget-content ul").css("list-style", "none");
  }
  else if(data.hasOwnProperty("command")) {
    switch (data.command) {
      case "lights":
        // register lights command
        var lights = data.lights;
        Dlights = [];
        lights.forEach(function(light) {
          var dlight = new DLight(light.shortAddress,light.name,light.room,light.minLevel,light.maxLevel,light.groups,light.sceneLevels,light.failLevel,light.powerOnLevel,light.physmin,light.fadeTime,light.fadeRate);
          Dlights.push(dlight);
        });
        CreateHtmlLights();
        break;
      case "levels":
        data.lights.forEach(function(light) {
          var shortAddress = light.shortAddress;
          var level = light.level;
          var percentage = (level - daliLightMinLevel) / (daliLightMaxLevel - daliLightMinLevel) * 100;
          var color = "hsl(50, 100%, " + (percentage * 40) + "%, " + percentage + ")";
          $("#light-number-" + shortAddress).css("background-color", color);
          $("#light-slider-" + shortAddress).val(percentage);
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


  

    // $("nav a").click(function(event) {
    //     event.preventDefault();
    //     var target = $(this).attr("href");
    //     $("main section").hide();
    //     $(target).show();
    //   });

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
    
    // var newLights = [];

    // daliLights[0].lights.forEach(function(daliLight) {
    //   var shortAddressExistsInFriendlyLights = false;
    //   friendlyLights[0].friendlylights.forEach(function(friendlyLight) {
    //     if (friendlyLight.shortAddress === daliLight.shortAddress) {
    //       shortAddressExistsInFriendlyLights = true;
    //     }
    //   });
    
    //   if (!shortAddressExistsInFriendlyLights) {
    //     newLights.push({shortAddress: daliLight.shortAddress});
    //   }
    // });
    
    // // Add the lights in the "newLights" array to the "newlights-list" list in the HTML
    // newLights.forEach(function(newLight) {
    //   $("#newlights-list").append("<li>Short address: " + newLight.shortAddress + "</li>");
    // });


  });
  