
var chatApp = angular.module('chatClient', []);

chatApp.controller('chatController', ['$window', '$scope', 
  function($window, $scope) {
    $scope.is_connected = false;

    $scope.messages = [];

    $scope.send_message = function() {
      var message = {
        user: $scope.username,
        url: $scope.url,
        text: $scope.msg_text
      };
      $scope.socket.send("test");
    };

    $scope.connect = function() {
      $scope.socket = new WebSocket("ws://localhost:12345");

      $scope.socket.onopen = function(event) {
        $scope.is_connected = true;
        $scope.$apply();
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



