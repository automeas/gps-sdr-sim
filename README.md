# GPS-SDR-SIM
realtime version based on BOOST/CPP, tested on USRP B210  
GPS-SDR-SIM generates GPS baseband signal data streams, specially for [USRP](http://www.ettus.com/).

fork from [gps-sdr-sim](https://github.com/osqzss/gps-sdr-sim) by osqzss.

### Depends on

[UHD](https://www.ettus.com/sdr-software/detail/usrp-hardware-driver)  
[BOOST](http://www.boost.org)  
[websocketd](https://github.com/joewalnes/websocketd/releases)

### Windows build instructions

1. Start Visual Studio.
2. Open the gps-sdr-sim.sln
3. Check the dependency (boost&UHD)
4. Select "Release" in Solution Configurations drop-down list.
5. Build the solution.

### How to use?
1. download the [websocketd.exe](https://github.com/joewalnes/websocketd/releases) to Release folder.
2. double click the runit.bat
3. open the googlemap.html in /www
4. click the "connect" button
5. wait a minute for the image load
6. double click the map to set the position
7. click the "go to" button to sent the signal
8. double click the map to set another position
9. click the "go to" button to change the position
10. click the "connect" button again to stop.

### screenshot

![28](https://cloud.githubusercontent.com/assets/6072743/25012185/50a6e842-20a2-11e7-8ff3-e232bf3ba7c4.png)

### License

Copyright &copy; 2017 bitdust  
Distributed under the [MIT License](http://www.opensource.org/licenses/mit-license.php).
