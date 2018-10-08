// exp1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	char* srcPath = "trees.jpg";
	char* dstPath = "res.tif";
	GByte* buffTmp;
	int i, bandNum;

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//输出获取结果
	cout << "image X Lenth: " << imgXlen << endl;
	cout << "image Y Lenth: " << imgYlen << endl;
	cout << "Band number: " << bandNum << endl;
	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//一个个波段的输入，然后一个个波段的输出
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}
	//清楚内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
    return 0;
}

