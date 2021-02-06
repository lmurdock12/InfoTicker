Initial Commit for the open source information ticker


Running demo:

sudo ./demo --led-gpio-mapping="adafruit-hat" --led-cols=64 -f ./matrix/fonts/8x13.bdf -s1 hello

#Running image demo:

sudo ./imageDemo --led-gpio-mapping="adafruit-hat" --led-cols=64 -D 1 ford-32-2.ppm

Copy file local to remote:

scp uparrow.png pi@raspberrypi:/home/pi/InfoTicker