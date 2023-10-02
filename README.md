# Anycubic Kobra Go/Neo Raspberry Pi LCD Driver

Running Klipper on your Anycubic Kobra Go or Neo? Make the stock LCD shine again with this custom LCD driver and KlipperScreen!

This is a fork of the excellent [fbcp-ili9341](https://github.com/juj/fbcp-ili9341) project, with a driver for the ST7796 SPI TFT display used in the Anycubic Kobra Go and Neo. 

SPI init sequence is based off the Anycubic Kobra Neo Marlin fork [BSP SPI TFT driver.](https://github.com/jokubasver/Kobra_Neo/blob/d3406176308f1839130edc08825f500a72c02f64/source/board/bsp_spi_tft.cpp#L406C12-L406C12)

Since the LCD used in the Kobra Go/Neo does not have touch, you can only use the display to view print stats. KlipperScreen does not currently support keyboard or rotary encoder control, and if it ever implements these features, then it would be possible to use the built-in rotary encoder for control. As it stands now - this is only for displaying printer info.

Tested on a Raspberry Pi Zero 2W, but it should work on other Pi models, as long as they are supported by [fbcp-ili9341](https://github.com/juj/fbcp-ili9341)

### Hardware Setup
TODO

### config.txt Setup
Before we start, we need to setup config.txt

In the terminal type `sudo nano /boot/config.txt`

Find `dtoverlay=vc4-kms-v4d` and add a `#` to the beginning of the line, so it should now be: `#dtoverlay=vc4-kms-v4d`

Scroll all the way to the bottom and add:
```bash
hdmi_group=2
hdmi_mode=87
hdmi_cvt=320 240 60 1 0 0 0
hdmi_force_hotplug=1
gpu_mem=160
```

This sets the resolution to that of the LCD display. Increasing the RAM might not be needed.

If your `/boot/config.txt` file has lines that look something like `dtoverlay=pitft28r, ...`, `dtoverlay=waveshare32b, ...` or `dtoverlay=flexfb, ...`, those should be removed.

The driver neither utilizes the default SPI driver, so a line such as `dtparam=spi=on` in `/boot/config.txt` should also be removed so that it will not cause conflicts.

Press CTRL+X, then Y to save


### Driver installation
Now that the config.txt is setup, we can clone the repo, setup cmake and compile the driver. 

Run in the console of your Raspberry Pi:

```bash
sudo apt update
sudo apt install cmake
cd ~
git clone https://github.com/jokubasver/Anycubic-Kobra-Go-Neo-LCD-Driver.git
cd Anycubic-Kobra-Go-Neo-LCD-Driver
mkdir build
cd build
cmake -DST7796=ON -DGPIO_TFT_DATA_CONTROL=24 -DGPIO_TFT_RESET_PIN=25 -DSPI_BUS_CLOCK_DIVISOR=8 -DSTATISTICS=0 ..
make -j
```
Now we can test if the driver and display works:
```bash
sudo ./fbcp-ili9341
```
If you see the terminal, congrats! You can move to the next step. If you get a white screen - check your wiring. If it still doesn't work, try changing 

Now we need to make sure the driver starts at startup.
Edit the file `/etc/rc.local` by typing `sudo nano /etc/rc.local`, and add a line:

```bash
sudo /path/to/Anycubic-Kobra-Go-Neo-LCD-Driver/build/fbcp-ili9341 &
````
Make sure to change the path - for example, if your username is the default `pi`, then it should be: `sudo /home/pi/Anycubic-Kobra-Go-Neo-LCD-Driver/build/fbcp-ili9341 &`

Press CTRL+X, then Y to save, reboot by typing `sudo reboot now` and you should see the terminal after the Pi finishes booting.

At this point the driver installation is complete. Now it's time to setup KlipperScreen.

### KlipperScreen Setup

The easiest way to setup KlipperScreen is to use [KIAUH](https://github.com/dw-0/kiauh). Please follow the install instructions there to install KlipperScreen.

You will notice that after installing KlipperScreen, it will not show up on the LCD. We still need to do one more step in order for it to work.

Type `sudo nano /usr/share/X11/xorg.conf.d/99-fbturbo.conf` and you should see this:
```bash
Section "Device"
        Identifier      "Allwinner A10/A13 FBDEV"
        Driver          "fbturbo"
        Option          "fbdev" "/dev/fb0"

        Option          "SwapbuffersWait" "true"
EndSection
```

Change `Driver          "fbturbo"` to `Driver          "fbdev"`, press CTRL+X, then Y, and reboot: `sudo reboot now`

At this point you should see KlipperScreen on the LCD display.

You are done!
