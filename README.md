# PWM on Renesas-SKS7G2 #

## Summary:

The purpose of this tutorial is to explain "How to generate PWM (Pulse Width Modulation) signals".  As we all know PWM output is the mostly used method to control the speed of motors, the brightness of LED lights, Servo drive etc.   This tutorial will help anyone who wants to use PWM for such things:

## Requirements for this tutorial:

* Renesas Starter kit SKS7G2.

* Development IDE **E2Studio** on your PC

* Digital Multi Meter to measure Hz/Duty Cycle.

* SEGGER JLink Driver software.

* LED and connecting wires

## Step 1: Start E2Studio and choose/create a workspace:

If you already knew how to create Synergy Project on E2Studio ignore this screenshot.  Here  I've combined multiple steps into single screen shot. 

![1](https://user-images.githubusercontent.com/7789293/28495907-1c4d32ea-6f12-11e7-8c75-18fee44e4266.png)


I have chosen Blinky with Threadx so that LED blink thread will continuously ON/OFF to indicate application is running with out any issue.  

Please refer the tutorial link given below if something is not clear in this step.
http://learn.iotcommunity.io/t/sending-data-to-pc-using-usb-com-on-sk-s7g2/953


## Step 2: Adding GPT timer to your application.

![2](https://user-images.githubusercontent.com/7789293/28495911-4c4b6c8c-6f12-11e7-8931-eb94ab1ba6cb.png)

Look at label numbered in order on above screen shot.


## Step 3: GPT Configuration :

By default GPT (General Purpose Timer) will be set to periodic. Renesas SSP has an option to change this timer properties to PWM.   We need to refer SKS7 board Hardware manual to find which channels and IO Port is mapped for GPT.  SKS7 board supports 14 timers (GPT0 to GPT13) and each timer can control 2 IO ports (A/B).

Just for an example, assume we are going to choose GPT7 which is used to generate PWM on IOPORT_03_04(CLOCKA) and PORT_03_03 (CLOCKB) .  By default SSP Configurator use this IO port for some other purpose. So we need to disable that first. Otherwise we can't use this IP port for PWM.

## Step 4 Disable IO_PORT:

![3](https://user-images.githubusercontent.com/7789293/28495921-7198523e-6f12-11e7-8d1e-3a5a25c7d480.png)


## Step 5: Configuration: 

Now expand "**Peripherals**" -> Timer:GPT to configure IO Port we want to generate PWM.  Each GPT can generate PWM in two IP ports, we can configure either both or one IO PORT.

For this tutorial we are going to choose only one port which is **IOPORT_03_03**.  This is connected with **GTIOCB**. So choose **GTIOCA or GTIOCB** from **operation mode** which allow you select the IO PORT.

Refer too the below screen shot.

![4](https://user-images.githubusercontent.com/7789293/28495926-94aee210-6f12-11e7-978a-33e1c0eed4d0.png)


We are done with IO port selection, now we need to configure GPT timer **properties** to make PWM on selected channel/IO  PORT

![5](https://user-images.githubusercontent.com/7789293/28495928-aba9a590-6f12-11e7-9bfd-d73fdbef9fec.png)


## About Properties:

* We chosen GTP7 so Channel should be 7.
* Mode  :  PWM
* Period : 900Hz. Depends on motor/device specification set appropriate HZ/KHz.
* Period Unit    : Hz
* Duty Cycle     : 90 %.  You can set any % value. Anyway we are going to change this dynamically.
* Duty Cycle Unit : Percentage.
* Auto Start         :  False.  We are going to start the timer on PWM Thread.
*  GTIOCA Enabled : **True**.  In case you switch the IOPORT 03_04
*  GTIOCB Enabled : **True**. By default this False. If you don't get pulse output please make sure this is set to **True**         

This is it, we are done with GTP/PWM settings.

## Step 4 Dynamic control of Duty Cycle 

To change Duty Cycle (which controls pulse output on IO port),  we need to add Thread and write few lines of code. In our Demo we are going to change the Duty Cycle every few Milli sec(s) or Sec(s). This allows us to measure PWM Hz or Duty Cycle % and make sure our code works correctly.

Look at below screen shotto add Thread to your project/Application.

![6](https://user-images.githubusercontent.com/7789293/28495936-e7827880-6f12-11e7-8431-ac1c80686d94.png)

Once you Click (3) **Generate project content** you will see new thread source .c file added into your src/ folder just like below

Now copy below source code and replace your pwm_thread source code.

      #include "pwm_thread.h"

      #define CLOCK_A         0
      #define CLOCK_B         1

      /* Servo Motor Control entry function */
      void pwm_thread_entry(void)
      {
           //open timer object
           g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);

           //Change Duty cycle around 30% i.e. overriding properties 
           g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, (30 %100), TIMER_PWM_UNIT_PERCENT, CLOCK_B);
          
           //start the timer which will product PWM
           g_timer0.p_api->start(g_timer0.p_ctrl);

          uint8_t dutycycle = 5;
          while (1) {
                tx_thread_sleep (150);
                dutycycle += 5; // Every iteration change Duty Cycle 5% more
                g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, (dutycycle %100), TIMER_PWM_UNIT_PERCENT, CLOCK_B);
          }
    }


## NOTE:

If you choose GTIOCA  clock then change last parameter **CLOCK_B** to **CLOCK**
**_A** like below

       g_timer0.p_api->dutyCycleSet(g_timer0.p_ctrl, (dutycycle %100), TIMER_PWM_UNIT_PERCENT, CLOCK_A);

Also need to make changes in GPT properties, Peripherals settings accordingly.

Here is the screen shot to build and install the application to board.


![8](https://user-images.githubusercontent.com/7789293/28495958-524d329a-6f13-11e7-93c1-ce505dc354b5.png)


To measure PWM Duty Cycle or connect and control any device we need to wire the IO port accordingly.  For Demo purpose I connected to GND and IO PORT 03_03 to Multi meter and set Hz/% mode.

Below is screen shot of the PWM output.  If you connect servo motors (need to follow servo motors PWM specification) or DC low power motors we can see speed variation.

Green wire connected to GND
RED wire connect to IOPORT_03_03
![9](https://user-images.githubusercontent.com/7789293/28495970-73696192-6f13-11e7-8cb7-15d9a9150f29.png)


Here is output captured while running  this demo.  Duty Cycle 44% (supposed to be 45%).  Our code keep changing the Duty Cycle 5% and up to 100% then start from 0%.

![10](https://user-images.githubusercontent.com/7789293/28495973-a61a1fdc-6f13-11e7-8812-8ced026c0cfa.png)

Hz output on this port (set 900 Hz) but measured 899.9.

![11](https://user-images.githubusercontent.com/7789293/28495979-bbfdc006-6f13-11e7-9f51-9b9c367374de.png)


## WHY IOPORT 03_03 

In case if you are wondering why I chose IO PORT 03_03 for this tutorial, here is answer.
If you refer SKS7 board diagram, IOPORT 03_03 connected with Arduino interface shield Digital 03 (PWM).   This example makes the  replica of Arduino PWM usage.

![12](https://user-images.githubusercontent.com/7789293/28495980-d1feb5d6-6f13-11e7-9aba-de38515d3ec3.png)


So basically now we know how to use Arduino interface ports on SKS7G2 board.

## PWM to LED:
Instead of Multi meter connect LED between GND and IO PORT 03_03, now  you can see the brightness of the LED getting changed according to PWM Duty Cycle %.

Hope this tutorial will be easy to follow. if any questions or issue with above please drop a line here.
