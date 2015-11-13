# longboard-ctl
Arduino electronic longboard (long skateboard) controller

* Controls twin electronic speed controller and some on-board electronics.
* Takes readings of some sensors and throttle to figure out motor power.
* Connects to some android software via bluetooth to show information
  about battery etc.

## Protocol
Simple protocol on top of bluetooth serial (SPI).

* battery cell status, volts (as floats?)
  
  ```
  C <cell1>;<cell2>;<cell3>;<cell4>;<cell5>;<cell6>;\n
  ```
* debug/info/warning/error message
  
  ```
  D <msg string>\n
  ```
* speed (km/h)
  
  ```
  S <float>\n
  ```

