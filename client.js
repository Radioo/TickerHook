let socket = new WebSocket("ws://10.0.0.31:10573");

socket.onmessage = function(event) {
        let message = event.data;
        document.getElementById("tickertext").innerHTML = message;
}