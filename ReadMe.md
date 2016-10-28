# Autonomous Video Panner

Panner:
   * Offers operator direct control over the (pan of) video camera in real time
   * Allows operator to define waypoints
   * Offers operator a rudimentary editor for a 'program'
   * Executes a program edited in a previous step

In the future the Panner MIGHT be able to:
   * Offer remote control facility
   * Offer remote program editor with facility to upload the program to the panner.
   * Control not only pan but also tilt and slide in the same manner.


To learn more about the project, check out this wiki:
https://github.com/asokolsky/Panner/wiki

## TODO

### Software
  * Add tilt and slide steppers
  * Replace AccelStepper with MultiStepper
  * Settings GUI to control LCD brightness (timeout?) with PWM?
  * Settings GUI to control LCD display orientation?

### Hardware
  * Add tilt and slide stepper drivers
  * test 0.9° step stepper
  * Step up DC voltage between batteries and stepper motors?
  * Add temperature-sensitive fan: LM35 sensor to PWM 12V fan or DS18B20 to SPI?
  * Control LCD brightness with PWM?

## DONE

## Docs
  * Moved schematics/PCB to EasyEDA: https://easyeda.com/asokolsky/Panner-OohztL3lD
    Fritzing is obsolete now.

### Software
  * Version 0.5 released
  * Add Settings View to change e.g. max pan speed
  * Add widget to change numerical values for settings - see KeyValueListWidget

### Hardware
  * Connect steppers to control board through RJ45
  * Connect camera to pan stepper via a 3/18-16 bolt - drill a 5mm center in the middle of the bolt
