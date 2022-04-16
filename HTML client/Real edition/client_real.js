let address = "localhost" // Change this address to the IP of your machine running TickerHook
//let socket = new WebSocket("ws://localhost:10573");

let CurrentText = "";
let LastText = "";
let TextLength;
let TickerLength = 14; // How many characters should the ticker have
let Timer;
let CurrentPos = 0;
UpdateContent("NOT CONNECTED"); // This is the string that will display at the start when waiting for connection

function connect()
{
    var ws = new WebSocket("ws://" + address + ":10573");

    ws.onmessage = function(event) {
        UpdateContent(event.data.trim().toUpperCase());
    };

    ws.onclose = function(event) {
        UpdateContent("RECONNECTING...");
        setTimeout(function() {
            connect();
        }, 1000);
    };

    ws.onerror = function(event) {
        ws.close();
    };
};

connect();

function UpdateContent(text)
{
    if(LastText != text)
    {
        LastText = text;
        CurrentText = text;
        if ([...text].length < TickerLength)
        {
            clearInterval(Timer);
            // This will center the text if its shorter than TickerLength
            let setpos = Math.round((TickerLength - text.length)/2);
            document.getElementById("tickertext").innerHTML = CurrentText.padStart(TickerLength-setpos, " ").padEnd(TickerLength, " ");
        }
        else if (text.length == TickerLength)
        {
            clearInterval(Timer);
            document.getElementById("tickertext").innerHTML = CurrentText;
        }
        else
        {
            clearInterval(Timer);
            TextLength = text.length;
            document.getElementById("tickertext").innerHTML = CurrentText.padStart(TextLength+2, " ").slice(0,TickerLength);
            CurrentText = CurrentText.padStart(TextLength+TickerLength, " ").padEnd(2*TickerLength+TextLength, " ");
            Timer = setInterval(ShiftChars, 300);
            CurrentPos = TickerLength-2;
        }
    }
};

function ShiftChars()
{
    if (CurrentPos < TextLength+TextLength)
    {
        CurrentPos++;
        document.getElementById("tickertext").innerHTML = CurrentText.slice(CurrentPos, TickerLength+CurrentPos);
        if (CurrentPos == TextLength+TickerLength)
        {
            CurrentPos = 0;
        }
    }
    else
    {
        CurrentPos = 0;
    }
};