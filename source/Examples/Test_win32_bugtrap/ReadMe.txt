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