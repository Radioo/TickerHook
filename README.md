![Screenshot](https://stn.s-ul.eu/tgRuJXCe.png)
# TickerHook
Hook dll for capturing ticker text from IIDX. Sends data via Websockets.

This is a successor to my STAY_COOL project. It hooks itself into the game and sends the ticker text only when it actually updates instead of running continuously on a timer.  

# Usage
Copy appropriate .dll file into your base directory, then load the dll using your tools, examples below:

inject.exe:  
`inject iidxhook1.dll TickerHook.dll bm2dx.exe --config iidxhook-09.conf %*`  
launcher.exe:  
`launcher -H 134217728 -B iidxhook9.dll -K TickerHook.dll bm2dx.dll --config iidxhook-28.conf %*`  
spice.exe/spice64.exe:  
`spice64 -k TickerHook.dll`  

After this, use one of the provided HTML clients to connect to your games. They should work out of the box if running on the same machine as the game. If not, change the IP address in the .js file to match the one you're running the game on.  
