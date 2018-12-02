// 加速处理.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <ctime>
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")
// 普通操作
int fun_1(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS);

// 去除浮点运算
int fun_2(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS);

// 使用移位操作代替除法
int fun_3(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS);

// 使用查表数组
int fun_4(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS);

// 并行计算
int fun_5(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS);

// 还有其他方法加速，不过都比较麻烦，就不再写啦

char* srcPath = "test.jpg";
clock_t start, finish;

int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;

	char* dstPath = "result.tif";
	GByte* buffTmp_rgb[3];
	int bandNum;

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	cout << imgXlen << " " << imgYlen << " " << bandNum << endl;

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		buffTmp_rgb[i] = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	}

	// 读取颜色通道
	for (int i = 0; i < 3; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp_rgb[i], imgXlen, imgYlen, GDT_Byte, 0, 0);
	}

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	//int choose;
	//cout << "use different method to process image, please input NO:";
	//cin >> choose;
	//switch (choose)
	//{
	//case 1: fun_1(imgXlen, imgYlen, buffTmp_rgb, poDstDS); break;
	//case 2: fun_2(imgXlen, imgYlen, buffTmp_rgb, poDstDS); break;
	//case 3: fun_3(imgXlen, imgYlen, buffTmp_rgb, poDstDS); break;
	//case 4: fun_4(imgXlen, imgYlen, buffTmp_rgb, poDstDS); break;
	//case 5: fun_5(imgXlen, imgYlen, buffTmp_rgb, poDstDS); break;
	//default:
	//	break;
	//}
	
	fun_1(imgXlen, imgYlen, buffTmp_rgb, poDstDS);
	fun_2(imgXlen, imgYlen, buffTmp_rgb, poDstDS);
	fun_3(imgXlen, imgYlen, buffTmp_rgb, poDstDS);
	fun_4(imgXlen, imgYlen, buffTmp_rgb, poDstDS);
	fun_5(imgXlen, imgYlen, buffTmp_rgb, poDstDS);

	//清楚内存
	for (int i = 0; i < 3; i++)
	{
		CPLFree(buffTmp_rgb[i]);
	}

	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");

    return 0;
}

int fun_1(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS)
{
	float* resultBuff[3];

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		resultBuff[i] = (float*)CPLMalloc(imgX*imgY * sizeof(float));
	}

	start = clock();
	// RGB处理
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < imgX*imgY; i++)
		{
			resultBuff[k][i] = 0.299*buffTmp_rgb[0][i] + 0.587*buffTmp_rgb[1][i] + 0.114*buffTmp_rgb[2][i];
		}
	}
	finish = clock();
	double endtime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "fun_1 run use time:" << endtime * 1000 << "ms" << endl;	//ms为单位

	// 写入
	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i+1)->RasterIO(GF_Write,
			0, 0, imgX, imgY, resultBuff[i], imgX, imgY, GDT_Float32, 0, 0);
	}
	
	return 1;
}

int fun_2(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS)
{
	float* resultBuff[3];

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		resultBuff[i] = (float*)CPLMalloc(imgX*imgY * sizeof(float));
	}

	// RGB处理
	start = clock();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < imgX*imgY; i++)
		{
			resultBuff[k][i] = (299 * buffTmp_rgb[0][i] + 587 * buffTmp_rgb[1][i] + 114 * buffTmp_rgb[2][i]) / 1000;
		}
	}
	finish = clock();
	double endtime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "fun_2 run use time:" << endtime * 1000 << "ms" << endl;	//ms为单位

	// 写入
	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgX, imgY, resultBuff[i], imgX, imgY, GDT_Float32, 0, 0);
	}


	return 1;
}

int fun_3(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS)
{
	float* resultBuff[3];

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		resultBuff[i] = (float*)CPLMalloc(imgX*imgY * sizeof(float));
	}

	// RGB处理
	start = clock();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < imgX*imgY; i++)
		{
			resultBuff[k][i] = (1224 * buffTmp_rgb[0][i] + 2404 * buffTmp_rgb[1][i] + 467 * buffTmp_rgb[2][i]) >> 12;
		}
	}
	finish = clock();
	double endtime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "fun_3 run use time:" << endtime * 1000 << "ms" << endl;	//ms为单位

	// 写入
	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgX, imgY, resultBuff[i], imgX, imgY, GDT_Float32, 0, 0);
	}

	return 1;
}

void arry_inite(float D[256], float E[256], float F[256])
{
	for (int i = 0; i < 256; i++)
	{
		D[i] = (i * 1224) >> 12;
		E[i] = (i * 2404) >> 12;
		F[i] = (i * 467) >> 12;
	}
}

int fun_4(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS)
{
	float D[256], E[256], F[256];
	float* resultBuff[3];
	arry_inite(D, E, F);

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		resultBuff[i] = (float*)CPLMalloc(imgX*imgY * sizeof(float));
	}

	// RGB处理
	start = clock();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < imgX*imgY; i++)
		{
			resultBuff[k][i] = D[buffTmp_rgb[0][i]] + D[buffTmp_rgb[1][i]] + D[buffTmp_rgb[2][i]];
		}
	}
	finish = clock();
	double endtime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "fun_4 run use time:" << endtime * 1000 << "ms" << endl;	//ms为单位

	// 写入
	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgX, imgY, resultBuff[i], imgX, imgY, GDT_Float32, 0, 0);
	}

	return 1;
}

int fun_5(int imgX, int imgY, GByte* buffTmp_rgb[3], GDALDataset* poDstDS)
{
	float D[256], E[256], F[256];
	float* resultBuff[3];
	arry_inite(D, E, F);

	//根据图像的宽度和高度分配内存
	for (int i = 0; i < 3; i++)
	{
		resultBuff[i] = (float*)CPLMalloc(imgX*imgY * sizeof(float));
	}

	// RGB处理
	start = clock();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < imgX*imgY; i += 2)
		{
			resultBuff[k][i] = D[buffTmp_rgb[0][i]] + D[buffTmp_rgb[1][i]] + D[buffTmp_rgb[2][i]];
			resultBuff[k][i + 1] = D[buffTmp_rgb[0][i + 1]] + D[buffTmp_rgb[1][i + 1]] + D[buffTmp_rgb[2][i + 1]];
		}
	}
	finish = clock();
	double endtime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "fun_5 run use time:" << endtime * 1000 << "ms" << endl;	//ms为单位

	// 写入
	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgX, imgY, resultBuff[i], imgX, imgY, GDT_Float32, 0, 0);
	}

	return 1;
}