# dwmhook
兼容且隐蔽的DWM HOOK
## 来自几年前的代码,代码质量堪忧但是能跑。
## 实现原理
### 使用数字签名且有自解压行为的外壳模块注入DWM进程,然后通过类的虚表HOOK DX函数 ，达到可以在DWM进程里面使用imgui的目的。
## 优点
### 没有模块
### 使用数字签名且有自解压行为的外壳模块注入DWM进程这样子不会被内存扫描到
### HOOK单个类实体的虚表而不是常量区的虚表不会被检查
### 使用内存通讯可以在外部进程改变imgui需要绘制的内容
### 通过获取当前系统DWM组件版本在线下载符号文件解析实现多系统兼容
## 缺点
### 古老且没有质量的代码
## 其他
### 也许有时间我会重构这个项目,但不是现在
