# MS51_LED_DIMMING

update @ 2019/04/18

Add LED dimming with PWM function (P1.2)

increase duty per 18 ms ,

- when reach 100 % duty , change to decrase duty per 18ms

- when reach 0 % duty , change to increase duty again

Add Button switch with GPIO interrupt function (P0.5)

- when press button , clear duty and output PWM duty to 0 %

- when release button , increause PWM duty from 0% per 18 ms
