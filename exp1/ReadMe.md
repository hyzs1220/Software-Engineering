# GDAL + Visual Studio 配置  

这次实验是第一次接触GDAL库，对其中的很多方法的使用并不是十分了解，所以也通过这次实验来更好的认识和学习。 在实验过程中也是学习到了一些理论知识，在这里略作归总。   

## TIFF文件格式  

标签图像文件格式(Tagged Image File Format，简写为TIFF)  

TIF保存的文件相对比较大，是许多印刷品保存的选择对象，同时它保存的图片失真度极小，并且可以保存图片的分层和透明信息。  

## .lib文件格式  

代表静态数据链接库，在windows操作系统中起到链接程序和函数(或子过程)的作用  

LIB文件是不对外公开的，可以很好的保存源代码。 在vs使用中，需要将对应的.lib文件放在工程目录下面。  

## .dll文件格式  

代表动态链接库文件，在Windows中，许多应用程序并不是一个完整的可执行文件，它们被分割成一些相对独立的动态链接库，即DLL文件，放置于系统中。当我们执行某一个程序时，相应的DLL文件就会被调用。  

使用DLL文件的好处是程序不需要在运行之初加载所有代码，只有在程序需要某个函数的时候才从DLL中取出。  

## 一些学习心得  

+ 在使用GDAL库的时候，首先需要调用``GDALAllRegister();`` 函数。打开GDAL支持的栅格数据之前需要注册驱动,而该函数将尝试注册所有已知的驱动。  


+ 例如 ``GDALDataset* poSrcDS; GDALDataset* poDstDS; GByte* buffTmp; `` 等一些图像操作参数，在运行过程中会申请大量内存资源，所以在程序运行最后要及时进行资源的释放 ``GDALClose(poDstDS); GDALClose(poSrcDS); CPLFree(buffTmp);``  

+ 好好学习，天天向上~  