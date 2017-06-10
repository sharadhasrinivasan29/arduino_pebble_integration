INTRODUCTION
The goal of the project is to create a Pebble smartwatch
application that is able to get data from and send control information to a remote sensor
and display driven by an Arduino microcontroller.



SYSTEM ARCHITECTURE
The system shall consist of three major components, described as follows:
• Sensor and Display: A Gravitech 7-Segment Shield will be attached to an
Arduino microcontroller. This component includes a temperature sensor, a sevensegment
display (like a digital clock), and an RGB light.

• Middleware: The Arduino is connected via USB to a Linux or Mac machine,
which runs a C++ program that handles all communication between the sensor
and the user interface.

• User interface: The user interface is a Pebble smartwatch application, which
communicates with the middleware over the Internet via an Android or iOS phone.


FUNCITONALITY
Our system fulfills all of the following requirements:
• The user is able to see the most recent temperature sensor reading.

• The user is able to see the average, low, and high temperature sensor
reading for the past hour (if the sensor has been running for less than an hour, the
user should be able to see the statistics for the time since the sensor started
running).

• The user is able to decide whether to see temperature readings (and
statistics) either in Fahrenheit or Celsius. Additionally, the user is able to
change the 7-segment display on the sensor to show the temperature either in
Fahrenheit or Celsius.

• The user is able to put the sensor into a stand-by mode in which it is not
reporting temperature readings (either to the user interface or to the display);
however, during this time, the readings are tracked for determining the
high, low, and average. When the sensor is in stand-by mode, the user is also 
able to tell it to resume reporting the readings.

• The user is able to change the light on the Arduino to signify whether they are hot (red),
cold (blue), or comforable (green). A menu is presented on the Pebble watch and the user can
select their preference.

• The user is able to get a drink recommendation based on their temperature preferences and the
temperature over the past hour. A linera regression is used to determine the used temperature in
the analysis and the user is also able to input what is hot, cold, and comforable for them.

• The user is able to select words from a menu and those words will be displayed on the Arduino
7-SEG display.

• If the user interface becomes disconnected from the middleware (e.g. because of a
network error or if the middleware stops running), an appropriate message is
shown to the user the next time it tries to connect.

• If the middleware cannot get a reading from the sensor (e.g. because the sensor is
disconnected), an appropriate message is shown to the user the next time
the user interface tries to get a reading.

• If the middleware becomes disconnected from the sensor but it is still running, it
is possible to reconnect the sensor and resume normal operations without
needing to restart the middleware.

In addition to the features described above, a thread is running on the
middleware that waits/blocks until a user sitting at the terminal where the middleware
was started enters the letter ‘q’. Once it sees that, the middleware program then
terminates.
