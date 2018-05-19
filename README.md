#  基于TB65603V2的步进电机手脉控制器
## -----------MPG stepper motor for STM32     

## 功能说明
通过手摇手脉控制器，控制步进电机运动；手脉摇动过程中保持电机的启、停平滑，不抖动不失步。
## 测试数据
 起始速度：200 rev/min         
 最大速度 15000 rev/min    
 250ms内分50个梯度从启动加速到最大速度  即(△t)5ms一次速度梯度  
 ![enter description here][1]
 ![enter description here][2]
 ![enter description here][3]
 ![enter description here][4]
 ![enter description here][5]
 ## 操作说明
 ![enter description here][6]
 ![enter description here][7]
 ## 视频演示
 %[点击进入视频演示][8]


  [1]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/1.jpg
  [2]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/2.jpg
  [3]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/3.jpg
  [4]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/4.jpg
  [5]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/5.jpg
  [6]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/6.jpg
  [7]: https://github.com/sevenchau/STM32-TB65603V2/blob/master/__images/7.jpg
  [8]: http://v.youku.com/v_show/id_XMTI5NzUzMDYzMg==.html?spm=a2hzp.8244740.0.0