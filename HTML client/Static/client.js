let address = "localhost" // Change this address to the IP of your machine running TickerHook

function connect()
{
    var ws = new WebSocket("ws://" + address + ":10573");

    ws.onmessage = function(event) {
        let message = event.data.trim().toUpperCase();
        document.getElementById("tickertext").innerHTML = message;
    };

    ws.onclose = function(event) {
        document.getElementById("tickertext").innerHTML = "RECONNECTING...";
        setTimeout(function() {
            connect();
        }, 1000);
    };

    ws.onerror = function(event) {;
        ws.close();
    };
};

connect();