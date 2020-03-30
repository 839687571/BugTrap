http://blog.sina.com.cn/s/blog_648d306d0100qmca.html


Windows平台下的C++程序异常通常可分为两种：
结构化异常（Structured Exception，可以理解为与操作系统相关的异常）和C++异常


对于crash错 误，一般由未被正常捕获的异常引起，Windows操作系统提供了一个API函数可以在程序crash之前有机会处理这些异常，
就是 SetUnhandleExceptionFilter函数。
（C++也有一个类似函数set_terminate可以处理未被捕获的C++异常。）


SetUnhandleExceptionFilter允许我们设置一个自己的函数作为全局SEH过滤函数，
当程序crash前会调用我们的函 数进行处理。我们可以利用的是 _EXCEPTION_POINTERS 结构类型的变量ExceptionInfo，
它包含了对异常的描述以及发生异常的线程状态，
过滤函数可以通过返回不同的值来让系统继续运行或退出应用程序

// 远程调试方案:
SetUnhandledExceptionFilter + Minidump
//确保所用的dump文件与源代码、exe、pdb文件版本是一致的，这要求我们必须维护好程序版本信息


// SHE 不能捕获的异常:

大部分异常通过SetUnhandledExceptionFilter函数能捕获，不过栈溢出、覆盖的有可能捕获不到。 
buffer overflow.不适合使用异常处理，因为在引发错误之前,buffer已经出现了问题，
很可能有很多不相干的东西都被破坏了.异常机制已经无法恢复这种错误带来的影响了

http://www.cppblog.com/woaidongmao/archive/2009/10/21/99129.html?opt=admin



很多软件通过设置自己的异常捕获函数，捕获未处理的异常，生成报告或者日志（例如生成mini-dump文件），达到Release版本下追踪Bug的目的。但是，到了VS2005（即VC8），Microsoft对CRT（C运行时库）的一些与安全相关的代码做了些改动，典型的，例如增加了对缓冲溢出的检查。新CRT版本在出现错误时强制把异常抛给默认的调试器（如果没有配置的话，默认是Dr.Watson），而不再通知应用程序设置的异常捕获函数，这种行为主要在以下三种情况出现。
（1）       调用abort函数，并且设置了_CALL_REPORTFAULT选项（这个选项在Release版本是默认设置的）。
（2）       启用了运行时安全检查选项，并且在软件运行时检查出安全性错误，例如出现缓存溢出。（安全检查选项 /GS 默认也是打开的）
（3）       遇到_invalid_parameter错误，而应用程序又没有主动调用
_set_invalid_parameter_handler设置错误捕获函数



调用DisableSetUnhandledExceptionFilter禁止CRT设置即可。
其它讨论
       上面通过设置api hook，解决了在VS2005上的异常捕获问题，这种虽然不是那么“干净”的解决方案，确是目前唯一简单有效的方式。
       虽然也可以通过_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT), signal(SIGABRT, ...), 和_set_invalid_parameter_handler(...) 解决（1）（3），但是对于（2），设置api hook是唯一的方式


可以参考 ExceptionCatcher 中的一些集成.