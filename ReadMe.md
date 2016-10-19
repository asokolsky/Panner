# Autonomous Video Panner

Panner:
    * Offers operator direct control over the video camera in real time
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
  * Add temperature-sensitive fan: LM35 sensor to PWM 12V fan

### Hardware
  * Add tilt and slide stepper drivers
  * Add step up DC converter between batteries and stepper drivers
  * Add temperature-sensitive fan: LM35 sensor to PWM 12V fan

## DONE

### Software
  * Add Settings View to change e.g. max pan speed
  * Add widget to change numerical values for settings - see KeyValueListWidget

### Hardware
  * Connect steppers to control board through RJ45
  * Connect camera to pan stepper via a 3/18-16 bolt - drill a 5mm center in the middle of the bolt
