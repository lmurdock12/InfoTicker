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


TODO:

Fix raspberrypi timeout issue
Fix reset item flicker
Fix the tesla StockManager class
Enhance scroller past one image on screen at a time:
    //When an image reaches the end reset it and put in readyqueue
    //When the currImage is < the end of the board... add the next image from ready queue