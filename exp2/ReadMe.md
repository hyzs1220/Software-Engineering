# 像素值的修改  

在这一次的实验中，主要针对RGB的颜色通道进行操作，从而实现对一个个像素值的操作。   

## 像素(Pixel)  

每个像素包括两个属性：位置和色彩(或亮度)  

## 灰度图像  

每个像素的亮度用一个整数来表示，通常数值范围在0到255之间，0表示纯黑、255表示纯白。  

在计算机中存储时，使用格式为 ```unsigned char``` 。  

GDAL中为 GByte (头文件中有这样的定义：```typedef unsigned char GByte```)  

## GDAL对彩色图像的处理  

GDAL可以用GetRasterBand()函数得到图像的通道  

 + GetRasterBand(1)得到的是红色通道;  

 + GetRasterBand(2)得到的是绿色通道;  

 + GetRasterBand(3)得到的是蓝色通道;  

## 一些学习心得  

 + 在本次实验中，仅提供了部分核心代码，所以在使用核心代码的过程中也是相应的了解了整体代码的实现。在这个过程中，对于GDAL的使用也是把握地更为熟练。  

 + 在代码编写过程中，遗漏了一些很重要的代码，例如 ```poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);``` 这个函数，用来创建输出图像。  

 + 于此同时，在申请内存时 ``` buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte)) ``` ,刚开始仅是申请了一块儿图像的内存，导致并不能够将这个图像进行处理复制。  


 + 在本次的实验中，首先是将真个图像进行复制操作，代码部分和实验一基本一致。然后对某一两个颜色通道进行单独处理，来达到预期成果。在这个过程中就是调用了 ``` poSrcDS->GetRasterBand(i) ``` 函数来对某一特定的颜色通道进行颜色处理，然后调用成员函数``` RasterIO(GF_Read, StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0); ``` 来对某一特定的区域进行读取，读取到 GByte* 类型的buffTmp中，然后对buffTmp这个一维数组进行颜色处理。然后再将该buffTmp写入到 poDstDS 中，从而实现某一特定区域块的颜色变化。  

+ 好好学习，天天向上~  