

if(typeof String.prototype.startsWith != 'function') {
  String.prototype.startsWith = function(str){
    return this.slice(0, str.length) == str;
  };
}



var chatApp = angular.module('chatClient', []);

chatApp.controller('chatController', ['$window', '$scope', 
  function($window, $scope) {
    $scope.my_data = {};
    $scope.my_data.cur_window = "chat";

    $scope.my_data.is_connected = false;

    $scope.my_data.messages = [];

    var createMessage = function(text) {
      return {
        user: $scope.my_data.username,
        text: text,
        type: "text"
      };
    };

    var createCommand = function(text) {
      var registerCommand = function() {
        return {
          user: $scope.my_data.username,
          type: "register"
        };
      };

      var createRoomCommand = function(args) {
        if(args.length != 1) {
          throw "Error: create room command requires one parameter only!";
        } else {
          return {
            user: $scope.my_data.username,
            type: "create",
            name: args[0]
          };
        }
      };

      var joinRoomCommand = function(args) {
        if(args.length != 1) {
          throw "Error: join room command requires one parameter only!";
        } else {
          return {
            user: $scope.my_data.username,
            type: "join",
            name: args[0]
          };
        }
      };

      var usersCommand = function(args) {
        if(args.length != 0) {
          throw "Error: users command does not take any parameters!";
        } else {
          return {
            user: $scope.my_data.username,
            type: "users"
          };
        }
      };

      var roomsCommand = function(args) {
        if(args.length != 0) {
          throw "Error: rooms-command does not take any parameters!";
        } else {
          return {
            user: $scope.my_data.username,
            type: "rooms"
          };
        }
      };

      var roomCommand = function(args) {
        if(args.length != 0) {
          throw "Error: room-command does not take any paramters!";
        } else {
          return {
            user: $scope.my_data.username,
            type: "room"
          };
        }
      };

      var splitted = text.trim().split(' ');
      var cmd = splitted[0];
      var args = splitted.slice(1);

      if(cmd == "\\join") {
        return joinRoomCommand(args);
      } else if(cmd == "\\create") {
        return createRoomCommand(args);
      } else if(cmd == "\\users") {
        return usersCommand(args);
      } else if(cmd == "\\rooms") {
        return roomsCommand(args);
      } else if(cmd == "\\room") {
        return roomCommand(args);
      } else {
        throw "Error: Invalid command: " + cmd;
      }
    };

    var parse_text = function(text) {
      if(text.trim().startsWith("\\")) {
        return createCommand(text);
      } else {
        return createMessage(text);
      }
    };

    $scope.send_message = function() {
      var text = $scope.my_data.msg_text;

      try {
        
        $scope.my_data.socket.send(JSON.stringify(parse_text(text)));
        $scope.my_data.msg_text = "";

      } catch(e) {
        $window.alert(e);
      }
    };

    $scope.my_data.connect = function() {
      $scope.my_data.socket = new WebSocket("ws://" + $scope.my_data.url);

      $scope.my_data.socket.onopen = function(event) {
        $scope.my_data.is_connected = true;
        $scope.$apply();

        var reg_cmd = {
          user: $scope.my_data.username,
          type: "register"
        };
        $scope.my_data.socket.send(JSON.stringify(reg_cmd));
      };

      $scope.my_data.socket.onclose = function(event) {
        $scope.my_data.is_connected = false;
        $scope.$apply();
      };

      $scope.my_data.socket.onerror = function(event) {
        $window.alert("Unable to connect!");
        $scope.my_data.is_connected = false;
        $scope.$apply();
      };

      $scope.my_data.socket.onmessage = function(event) {
        $scope.my_data.add_new_message(JSON.parse(event.data));
        $scope.$apply();
      };
    };

    $scope.my_data.disconnect = function() {
      $scope.my_data.socket.close();
    };

    $scope.my_data.add_new_message = function(msg) {
      var message = {
        text: msg.text
      };
      $scope.my_data.messages.push(message);
    };


    $scope.my_data.my_alert = function(txt) {
      $window.alert(txt);
    };

}]);



