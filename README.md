## A DIY, Internet Aware, Smartphone Controlled Wake-up Light - ESP8266

This is the Arduino code for an ESP8266 used to create a wake-up light with the following features:

* Sunrise simulation (the main feature)
* Smartphone controlled via WiFi
* Can be used as a (digitally) dimmable bedside lamp
* Sound activated, turns on and off by clapping (yes, really)
* Automatic network time synchronization
* Automatic address resolution (mDNS)
* Easy WiFi set up (captive portal)
* Beeping as a failsafe wake-up alarm and audio feedback
* A hidden morse-code message as an easter egg 
	
Please check out the full project at https://www.edusteinhorst.com/building-a-smartphone-controlled-wake-up-light/

## Disclaimer

This is a rough initial version of the code. Things should work as expected, but let me know if you find bugs. Also, keep in mind this project involves AC mains current which can be dangerous, potentially lethal. I have exercised safety to the best of my hability and nothing bad has happened while using it for over an year, but be aware very bad things could potentially happen, such as a fire while you sleep. That would suck. This is for educational purposes, I'm not suggesting you build this project, and I can't be held responsible should you attempt to build it and something bad happens. 

## Motivation

This was a Christmas gift to my lovely girlfriend. You can read about it on the blog post!

## Installation

Please check the details at https://www.edusteinhorst.com/a-diy-internet-aware-smartphone-controlled-wake-up-light/

## Usage

Modify the constants in the code to reflect your timezonedb.com API key.

## Contributors

Feel free to fork this and have at it. You can reach me at edusteinhorst.com

## License

This project is licensed under the terms of the MIT license.
