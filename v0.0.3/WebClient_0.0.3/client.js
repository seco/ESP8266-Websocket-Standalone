var server;
var clientID = -1;

$(document).ready(function(){
	// connect to server
	server = new WebSocket("ws://192.168.178.47:81");
	
	server.onopen = function (event) {
		var msg = {
			type: "message",
			text: "This is Text!",
			id:   clientID,
			timestamp: new Date().getTime()/1000
		};
		server.send( JSON.stringify( msg ) ); 
	};
	
	server.onmessage = function (event) {
		console.log(event.data);
	}
	
	$('#test1').on('change', function(){
		var msg = {
			type: "LED1",
			text: $(this).is(':checked'),
			id:   clientID,
			timestamp: new Date().getTime()/1000
		};
		server.send( JSON.stringify( msg ) ); 
	});
});