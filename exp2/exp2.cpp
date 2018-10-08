// exp2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	int StartX, StartY, tmpXlen, tmpYlen;
	int imgXlen, imgYlen, bandNum;
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	GByte* buffTmp;
	char* srcPath = "build.jpg";
	char* dstPath = "res.tif";

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	
	//分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));

	//一个个波段的输入，然后一个个波段的输出
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}


	// ······一个红色色块

	//起始位置坐标
	StartX = 100;
	StartY = 100;
	//区域宽度和高度
	tmpXlen = 200;
	tmpYlen = 150;
	//读取红色通道缓存在buffTmp中
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//遍历像素，逐像素置为255
	for (int j = 0; j < tmpYlen; j++)
	{
		for (int i = 0; i < tmpXlen; i++)
		{
			buffTmp[j*tmpXlen + i] = (GByte)255;
		}
	}
	//数据写入poDstDS
	poDstDS->GetRasterBand(1)->RasterIO(GF_Write,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	cout << " ... ... red block processing ... ..." << endl;

	// ······一个白块
	//起始位置坐标
	StartX = 300;
	StartY = 300;
	//区域宽度和高度
	tmpXlen = 100;
	tmpYlen = 50;
	//读取各通道缓存在buffTmp中，遍历像素，逐像素处理
	for (int k = 0; k < bandNum; k++)
	{
		poSrcDS->GetRasterBand(k + 1)->RasterIO(GF_Read,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		for (int j = 0; j < tmpYlen; j++)
		{
			for (int i = 0; i < tmpXlen; i++)
			{
				buffTmp[j*tmpXlen + i] = (GByte)255;
			}
		}
		poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);

	}
	cout << " ... ... white block processing ... ..." << endl;


	// ······一个黑块
	//起始位置坐标
	StartX = 500;
	StartY = 500;
	//区域宽度和高度
	tmpXlen = 50;
	tmpYlen = 100;
	//读取各通道缓存在buffTmp中，遍历像素，逐像素处理
	for (int k = 0; k < bandNum; k++)
	{
		poSrcDS->GetRasterBand(k + 1)->RasterIO(GF_Read,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
		for (int j = 0; j < tmpYlen; j++)
		{
			for (int i = 0; i < tmpXlen; i++)
			{
				buffTmp[j*tmpXlen + i] = (GByte)0;
			}
		}
		poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
			StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);

	}
	cout << " ... ... black block processing ... ..." << endl;
	

	//清除内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
    return 0;
}

