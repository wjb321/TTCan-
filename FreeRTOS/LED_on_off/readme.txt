将stm32f103ze 移植到C8T6中：


1，更改startup_hd.s to startup_md.s. 注意：驱动文件需要在最小系统的cmsis-> arm下面获取。
2，通过魔法棒更改芯片，c/c++下的HD -> MD， ultilities下更改flash size.
3,   由于freeRTOS 中RAM分配大小有问题，第一次通过将其变大，发现没有效果。然后变小后才体现功能。
   其问题分析如此：https://blog.csdn.net/qq_33161479/article/details/90064215
   