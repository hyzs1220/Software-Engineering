// exp4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

class kernel
{
public:
	float data[5][5];
	int kernelX, kernelY;
	int kCenterX, kCenterY;
	kernel(){}
	kernel(float arry[5][5], int row, int col, double a ) {	//a为参数，默认为1
		kernelX = row; kernelY = col;
		kCenterX = kernelX / 2;  kCenterY = kernelY / 2;
		for (int i = 0; i < kernelX; i++)
		{
			for (int j = 0; j < kernelY; j++)
			{
				data[i][j] = arry[i][j] * a;
			}
		}
	}
};

int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	char* srcPath = "lena.jpg";
	char* dstPath = "test.tif";
	GByte* buffTmp, *test;
	int bandNum;

	int mm,nn,rowIndex, colIndex;
	//卷积核
	float m_kernel_1[5][5] = { { 0,1,0 },{ 1,1,1 },{ 0,1,0 } };//均值模糊
	float m_kernel_2[5][5] = { { 1,0,0,0,0 },{ 0,1,0,0,0 },{ 0,0,1,0,0 },{ 0,0,0,1,0 },{ 0,0,0,0,1 } };//运动模糊
	float m_kernel_3[5][5] = { { -1,-1,-1 },{ -1,8,-1 },{ -1,-1,-1 } };//边缘检测
	float m_kernel_4[5][5] = { { -1,-1,-1 },{ -1,9,-1 },{ -1,-1,-1 } };//锐化
	float m_kernel_5[5][5] = { { -1,-1,0 },{ -1,0,1 },{ 0,1,1 } };//浮雕效果
	float m_kernel_6[5][5] = {
		0.0120, 0.1253, 0.2736, 0.1253, 0.0120,
		0.1253, 1.3054, 2.8514, 1.3054, 0.1253,
		0.2736, 2.8514, 6.2279, 2.8514, 0.2736,
		0.1253, 1.3054, 2.8514, 1.3054, 0.1253,
		0.0120, 0.1253, 0.2736, 0.1253, 0.0120
	};//高斯模糊

	kernel m_kernel(m_kernel_1, 3, 3, 0.2);
	//kernel m_kernel(m_kernel_2, 5, 5, 0.2);
	//kernel m_kernel(m_kernel_3, 3, 3, 1);
	//kernel m_kernel(m_kernel_4, 3, 3, 1);
	//kernel m_kernel(m_kernel_5, 3, 3, 1);
	//kernel m_kernel(m_kernel_6, 5, 5, 0.04);

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	test = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	for (int k = 0; k < 3; k++)	//三个通道处理
	{
		poSrcDS->GetRasterBand( k + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		for (int i = 0; i < imgXlen; i++)
		{
			for (int j = 0; j < imgYlen; j++)	//逐像素处理
			{
				if (i < m_kernel.kCenterX || i > imgXlen - m_kernel.kCenterX
						|| j < m_kernel.kCenterY || j > imgYlen - m_kernel.kCenterY) {	//边界处理，直接置0
					buffTmp[i*imgXlen + j] = 0;
					continue;
				}
				float result = 0;	//卷积核运算结果
				for (int m = 0; m < m_kernel.kernelX; m++)
				{
					rowIndex = i + m - m_kernel.kCenterX;
					for (int n = 0; n < m_kernel.kernelY; n++)
					{
						colIndex = j + n - m_kernel.kCenterY;
						if (rowIndex >= 0 && rowIndex < imgXlen && colIndex >= 0 && colIndex < imgYlen)
							result += buffTmp[imgXlen * rowIndex + colIndex] * m_kernel.data[m][n];
					}
				}
				test[i*imgXlen + j] = result;
			}
		}
		poDstDS->GetRasterBand( k + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, test, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << k+1 << " processing ... ..." << endl;
	}

	//清楚内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");

    return 0;
}

