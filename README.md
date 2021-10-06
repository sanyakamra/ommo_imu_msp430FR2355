# ommo_imu_msp430FR2355
msp430FR2355 interfaced with IMU (LSM6DSM) on SPI

SPI mode 3 as only one spi device is connected
Big Endian Data selection

## SPI Write and configure Registers for IMU:
bit 0: WRITE bit. The value is 0.
bit 1 -7: address AD(6:0). This is the address field of the indexed register.
bit 8-15: data DI(7:0) (write mode). This is the data that is written inside the device (MSb
first).
bit 16-... : data DI(...-8). Further data in multiple byte writes.

FIFO_CTRL5 (0Ah)
CTRL3_C (12h) 
CTRL1_XL (10h) ;Linear acceleration sensor control register 
CTRL2_G (11h)  ;Angular rate sensor control register

Configuration sent to IMU over SPI

Read P4.1 for Button press
if button pressed then the line gets short and no signal at microcontroller input.

Once the button is pressed, then the data is collected from gyro and acc.
by sending the read command of SPI:
## SPI Read
bit 0: READ bit. The value is 1.
bit 1-7: address AD(6:0). This is the address field of the indexed register.
bit 8-15: data DO(7:0) (read mode). This is the data that will be read from the device (MSb
first).

Read the data of specific sensor(Acc/gyro)

 OUTX_L_G (22h) & OUTX_H_G (23h)
          Angular rate sensor pitch axis (X) angular rate output register (r)  -CR CTRL2_G (11h))
          
   OUTY_L_G (24h) & OUTY_H_G (25h)
          Angular rate sensor roll axis (Y) angular rate output register (r)   -CR CTRL2_G (11h))
          
   OUTZ_L_G (26h) & OUTZ_H_G (27h)
          Angular rate sensor yaw axis (Z) angular rate output register (r)   -CR CTRL2_G (11h))
          
   OUTX_L_XL (28h) & OUTX_H_XL (29h)
          Linear acceleration sensor X-axis output register (r)
          
   OUTY_L_XL (2Ah) & OUTY_H_XL (2Bh)
          Linear acceleration sensor Y-axis output register (r)
          
   OUTZ_L_XL (2Ch) & OUTZ_H_XL (2Dh)
          Linear acceleration sensor Z-axis output register (r)
          
          
 ## Enable interrupt
 
 Then the SPI interrupt is on and the IMU will send the data to microcontroller.
 So it's SPI Tx
 store the data of sensors.
 
 
 ## Main loop
 
 The while loop is in main loop that check the status of P4.1 pin  
-flag=0 and button is pressed
  The SPI interrupts are enabled
  
-flag=1 and button is pressed for 500ms.
the delay is given and rechecking of pin is done to ensure button is pressed for 500 ms
if less than 500ms the IMU will be ON state
if around 500ms the SPI interrupt is disabled.
          
