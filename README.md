LightWriter
===========
Lightwriter is a combination of an Ardunio sketch and some Python classes for controlling an Arduino
over a serial connection from python.  This code is designed for use with a RadioShack Tricolor LED
strip (http://www.radioshack.com/product/index.jsp?productId=22128606).  Much of the code is
borrowed from the RadioShack source (at http://blog.radioshack.com/2013/06/tricolor-led-strip/)
as well as some community adjustments (http://pastebin.com/eEB61uWV).

Originally, this project was written to create a Rasberry Pi and Arduino device for displaying
Nagios alerts.  However, this code could be easily re-applied for a lot of more fun use cases.

Installation
------------

The python code for this project is available on PyPi:
```
pip install LightWriter
```

Or you can simply clone the project, but be sure to install the dependencies in requirements.txt.

Setup
---------
To set up the Arduino, flash the `bin/LightWriter.ino` onto your Arduino using the Arduino IDE.
Attach the RadioShack light strip to your Arduino as instructed in its accompanying documentation.
Connect the Ardunio to a 12V power supply as the RadioShack docs suggest, and attach the Arduino over
USB.  To see if your rig is working, you can run the example script (`bin/example.py`).  
