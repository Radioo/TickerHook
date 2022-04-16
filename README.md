![Screenshot](https://stn.s-ul.eu/tgRuJXCe.png)
# TickerHook
Hook dll for capturing ticker text from IIDX. Sends data via Websockets or writes to a file.

This is a successor to my STAY_COOL project. It hooks itself into the game and sends the ticker text only when it actually updates instead of running continuously on a timer.  

# Usage
Copy appropriate .dll file into your base directory, then load the dll using your tools, examples below:

inject.exe:  
`inject iidxhook1.dll TickerHook09.dll bm2dx.exe --config iidxhook-09.conf %*`  
launcher.exe:  
`launcher -H 134217728 -B iidxhook9.dll -K TickerHook28.dll bm2dx.dll --config iidxhook-28.conf %*`  
spice.exe/spice64.exe:  
`spice64 -k TickerHook28.dll`  
INFINITAS:  
Use https://github.com/emskye96/infinitas_chainload

After this, use one of the provided HTML clients to connect to your games. They should work out of the box if running on the same machine as the game. If not, change the IP address in the .js file to match the one you're running the game on.  

Optional file mode:  
Not available for INF. Copy TickerHook.ini into your game's directory and change mode along with your file path.
