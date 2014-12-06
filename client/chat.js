

if(typeof String.prototype.startsWith != 'function') {
  String.prototype.startsWith = function(str){
    return this.slice(0, str.length) == str;
  };
}



var chatApp = angular.module('chatClient', []);

chatApp.controller('chatController', ['$window', '$scope', 
  function($window, $scope) {
    $scope.is_connected = false;

    $scope.messages = [];

    var createMessage = function(text) {
      return {
        user: $scope.username,
        text: text,
        type: "text"
      };
    };

    var createCommand = function(text) {
      var registerCommand = function() {
        return {
          user: $scope.username,
          type: "register"
        };
      };

      var createRoomCommand = function(args) {
        if(args.length != 1) {
          throw "Error: create room command requires one parameter only!";
        } else {
          return {
            user: $scope.username,
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
            user: $scope.username,
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
            user: $scope.username,
            type: "users"
          };
        }
      };

      var roomsCommand = function(args) {
        if(args.length != 0) {
          throw "Error: rooms-command does not take any parameters!";
        } else {
          return {
            user: $scope.username,
            type: "rooms"
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
      var text = $scope.msg_text;

      try {
        
        $scope.socket.send(JSON.stringify(parse_text(text)));
        $scope.msg_text = "";

      } catch(e) {
        $window.alert(e);
      }
    };

    $scope.connect = function() {
      $scope.socket = new WebSocket("ws://localhost:12345");

      $scope.socket.onopen = function(event) {
        $scope.is_connected = true;
        $scope.$apply();

        var reg_cmd = {
          name: $scope.username,
          type: "register"
        };
        $scope.socket.send(JSON.stringify(reg_cmd));
      };

      $scope.socket.onclose = function(event) {
        $scope.is_connected = false;
        $scope.$apply();
      };

      $scope.socket.onerror = function(event) {
        $window.alert("error!");
        $scope.is_connected = false;
        $scope.$apply();
      };

      $scope.socket.onmessage = function(event) {
        $scope.add_new_message(event.data);
        $scope.$apply();
      };
    };

    $scope.disconnect = function() {
      $scope.socket.close();
    };

    $scope.add_new_message = function(txt) {
      var message = {
        user: $scope.username,
        url: $scope.url,
        text: txt
      };
      $scope.messages.push(message);
    };


    $scope.my_alert = function(txt) {
      $window.alert(txt);
    };

}]);



