# **********************************************************************
# 
# PeterRichOS 0.1
# 
# 操作系统PeterRichOS的实现   杨展富
# 
# Finished data: 2016.6.1
# 
# Sun Yat-Sen University  数据科学与计算机学院  超算班 14301033
# 
# E-mail:  865031716@qq.com
# 
# 
# 
# **********************************************************************
# 代码详解
/peterRichOS
	/fs :vfs的接口

	/boot : 初始的boot代码

	/gdt_idt : 实现了分段，中断向量

	/include : 各类头文件 

	/init : main主函数 

	/kernels : 进程调度

	/lib : 键盘输入，显示输出

	/mm : 内存管理	

	/scripts : gcc链接脚本

	Makefile ： make

###功能实现
# 
# 因为比较复杂，所以本人没有实现：
# 1.shift加字母变成大写
# 2.cd
# 3.
# 4.

memory:   show memory use
pool  :   show malloc and free
run  :   show multi-process, print 'a' and 'c' at the same time
ls    :   list all file 
cat (filename)  :   read file write
vim (filename) (contents)  :   write file
clean :   show the screen after clean
rename old_file new_file:   show the screen after clean
touch (filename)  :   create a new file
cp (oldfile) (newfile):   show copy file
rm (filename)  :   delete file


###编译和启动
内核使用grub加载，运行在qemu模拟器上。 
编译：
->$ make
启动：
->$ make qemu
编译清除：
->$ make clean

###参考资料
参考书籍：《Linux内核完全剖析——基于Liunux 0.12内核》 赵炯
参考书籍：《自己动手写操作系统》于渊
参考书籍：<<Orange's 一个操作系统的实现>> 于渊
在线参考 hurlex及教程-><<一个基于x86架构的简单内核实现>>  hurlex
