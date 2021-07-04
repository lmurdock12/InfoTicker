Initial Commit for the open source information ticker


Running demo:

sudo ./demo --led-gpio-mapping="adafruit-hat" --led-cols=64 -f ./matrix/fonts/8x13.bdf -s1 hello

#Running image demo:

sudo ./imageDemo --led-gpio-mapping="adafruit-hat" --led-cols=64 -D 1 ford-32-2.ppm

Copy file local to remote:

scp uparrow.png pi@raspberrypi:/home/pi/InfoTicker

scp Green-Up-Arrow-raw.ppm pi@<ip addr>:/home/pi/InfoTicker



Features I want:

Hookup to alexa..hey alexa...add this or that to the feed.
