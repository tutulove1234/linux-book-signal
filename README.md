### README ABOUT SIGNAL

### 引发内核为进程传递信号的事件
 1. 硬件发生异常. e.g. 除零异常 , 访问无效内存等.
 2. 用户键入能够产生信号的终端特殊字符. e.g. Ctrl-c(SIGTINT) , Ctrl-\\(SIGQUIT) , Ctrl-z
 3. 发生软件事件.如终端大小变化,进程在CPU上执行时间到期.

### Linux下信号分两种.
* 第一种: 用于内核向进程通知事件.即传统或者标准信号.1~31
* 第二种: 实时信号

### 收到信号时进程采取的动作:
 1. 忽略信号.
 2. 终止(杀死)进程.
 3. 产生核心转储文件,同时进程终止.
 4. 停止进程.
 5. 于之前暂停后再度恢复进程的执行.



### 编程可以做到的
1. 设置信号到来时采取默认的行为.
2. 忽略掉该信号.
3. 执行信号处理器程序.

### 常用信号类型即默认行为
0.  SIGINT: Ctrl-C终端下产生，默认行为：终止当前进程。
1.  SIGABRT:
2.  SIGALRM:
3.  SIGCHLD:
4.  SIGHUP:
5.  SIGKILL:
6.  SIGPIPE:
7.  SIGIO:
8.  SIGQUIT:
9.  SIGSEGV:
10. SIGSTOP:
11. SIGTERM:

### 信号是什么？

* 信号是一种很古老的IPC(进程间通信)方式。在早期的类Unix操作系统中，信号只能传递很简单的信息，即一个int值。
* 信号是一种异步的消息传递方式或者说通知机制。举一个不恰当的例子，你正在很兴奋地解决一个BUG，突然手机响了，女朋友说要分手。你立马放下手上的活，去处理女朋友那件事。手机随时都会响，随时都会中断你当下的事情。所以称之为异步事件。这个例子中的信号就体现在手机铃声响了。后面会再引用这个例子。
* 虽然信号是一种低级的IPC方式，但同时它保持了很简单的特性。在一些大型服务端程序中，很多时候也要考虑信号造成的影响。还是值得一学的。

### 如何设计信号处理函数？
* 一般而言，信号处理函数设计的越简单越好，因为当前代码的执行逻辑被打断，最好尽快恢复到刚才被打断之前的状态。从而避免竞争条件的产生。
* 在信号处理函数中，建议不要调用printf等与I/O相关的函数。以及一些慢设备操作。这样会使得信号处理函数的执行时间变长，可能，操作系统就会切换其它程序去在CPU上执行。但如果有特殊需要，则也可以使用。
* 在信号处理函数中，不要使用任何不可重入的函数(后面)会详细说。保证信号处理函数可以安全地执行完。并不会影响主逻辑执行。

### 几种常见的信号处理函数的设计方式：
1. 信号处理函数设置全局性标志变量并退出。主程序对此标志进行周期性检查，一旦标志被置位，则进行相应的处理动作（如果主程序监听一个或者多个文件描述符的I/O状态而无法进行自旋检测标志是否被设置，则可以创建一个管道，通过对管道的文件描述符进行监听，就可以在信号处理函数中向管道fd写入一字节内容，通过主程序中对管道fd事件的处理从而达到检测全局标志改变的事件。）。
2. 信号处理函数值某种类型的清理操作。接着终止进程或者使用非本地跳转。将控制返回到主程序的预定位置。

### 信号的发送与处理
来简单看一个实例，看看信号最简单的使用方式(当键入Ctrl-C时候即程序收到SIGINT信号时进行处理。)：
```C
//  signal 的函数原型
//  void (*signal(int sig , void (*func)(int)))(int);
//  至于这个怎么理解，这里就不再赘述了，请参考 《C Traps and Pitfalls》2.1节即理解函数声明。
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

void sig_handler (int signuum ) {
	// 本程序只是为了来进行演示，
	// 在信号处理程序中，尽量不要调用与标准IO相关的，不可重入的函数。
	printf ("Catch SIGINT!\nExit!\n") ;
	exit (1) ;
}

int main() {

	// 注册信号处理函数
	signal ( SIGINT , sig_handler ) ;

	// 让主程序不退出，挂起，等待信号产生
	while (1) {
		pause () ;
	}

	return EXIT_SUCCESS ;
}
```
程序会一直停着等待用户行为。当我们键入Ctrl-C时程序打印相关信息，之后程序自己退出。那么程序的执行流程就类似这样：
![result](/home/tutu/Pictures/sigint.png)
这是一种古老的注册信号并设置信号处理函数的方式。当然现在已经不用了，现在我们使用新的信号注册函数即sigaction函数。它提供了更多的控制字段。包括屏蔽信号集，设置高级信号处理函数等(稍后会详细讲述，别担心).


### 设置信号处理函数
*__应该了解到，每一个线程只有一个errno值，所以信号处理程序可能会修改errno的值。因此，作为一个通用的规则，应当在调用那些可能改变errno的函数之前保存errno值，之后再进行恢复。__*

*__在信号处理函数中调用一个非可重入函数，其结果是不可预测的.__*
