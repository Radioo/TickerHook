# TickerHook
Hook dll for capturing ticker text from IIDX. Sends data via Websockets or writes to a file.

This is a successor to my STAY_COOL project. It hooks itself into the game and sends the ticker text only when it actually updates instead of running continuously on a timer.  

# Usage
Copy appropriate .dll file and `TickerHook.ini` into your base directory, then load the dll using your tools, examples below:

inject.exe:  
`inject iidxhook1.dll TickerHook09.dll bm2dx.exe --config iidxhook-09.conf %*`  
launcher.exe:  
`launcher -H 134217728 -B iidxhook9.dll -K TickerHook28.dll bm2dx.dll --config iidxhook-28.conf %*`  
spice.exe/spice64.exe:  
`spice64 -k TickerHook28.dll`  

Set the mode of operation you want inside `TickerHook.ini`. There are two to choose from: server and file.  
Server mode runs a websocket server and sends the data to the client. I've included an example client application inside the `HTML client` folder, just change the IP address in `client.js` and you should be good to go.  
File mode writes the ticker text to a specified file, overwriting it each time. Set your file path inside `TickerHook.ini`.
