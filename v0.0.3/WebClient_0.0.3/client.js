var server;
var clientID = -1;

$(document).ready(function(){
	// connect to server
	server = new WebSocket("ws://192.168.178.21:81");
	
	server.onopen = function (event) {
		var msg = {
			type: "message",
			text: "This is Text!",
			id:   clientID,
			timestamp: Date.now()
		};
		server.send( JSON.stringify( msg ) ); 
	};
	
	server.onmessage = function (event) {
		console.log(event.data);
	}
});