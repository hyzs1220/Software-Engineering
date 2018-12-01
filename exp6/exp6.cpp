// exp6.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

#define N 256
void printGod();
int main()
{
	//输入图像
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	char* srcPath_MUL = "./img/Mul_large.tif";
	char* srcPath_PAN = "./img/Pan_large.tif";
	char* dstPath = "./img/Ful_large.tif";
	float* buffTmp_PAN;
	float* buffTmp_rgb[3], *trans_res[3];
	int bandNum;
	// RGB ==> IHS 正变换矩阵
	float tran1[3][3] = {
		{ 1.0f / 3.0f,		1.0f / 3.0f,		1.0f / 3.0f },
		{ -sqrt(2.0f) / 6.0f,	-sqrt(2.0f) / 6.0f,	sqrt(2.0f) / 3.0f },
		{ 1.0f / sqrt(2.0f),	-1.0f / sqrt(2.0f),	0 }
	};
	// IHS ==> RGB 逆变换矩阵
	float tran2[3][3] = {
		{ 1.0f,  -1.0f / sqrt(2.0f),	1.0f / sqrt(2.0f) },
		{ 1.0f,  -1.0f / sqrt(2.0f),	-1.0f / sqrt(2.0f) },
		{ 1.0f,  sqrt(2.0f),			0 }
	};

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS_MUL = (GDALDataset*)GDALOpenShared(srcPath_MUL, GA_ReadOnly);
	poSrcDS_PAN = (GDALDataset*)GDALOpenShared(srcPath_PAN, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS_MUL->GetRasterXSize();	//15696
	imgYlen = poSrcDS_MUL->GetRasterYSize();	//12004
	bandNum = poSrcDS_PAN->GetRasterCount();

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, 3, GDT_Byte, NULL);

	int m_choose = 0;
	printGod();
	cout << "input 1 to use 256 * 256 block process image" << endl;
	cout << "input 2 to use 256 * imgXlen block process image" << endl;
	cout << "please input:";
	cin >> m_choose;
	if (m_choose == 1)
	{
		// 使用256 * 256 大小的块对图像进行处理
		//分配内存
		buffTmp_PAN = (float*)CPLMalloc(N*N * sizeof(float));
		for (int i = 0; i < 3; i++)
		{
			buffTmp_rgb[i] = (float*)CPLMalloc(N*N * sizeof(float));
			trans_res[i] = (float*)CPLMalloc(N*N * sizeof(float));
		}

		// 相关变量
		int blockX_num = (imgXlen - 1) / N + 1;	// 块数
		int blockY_num = (imgYlen - 1) / N + 1;
		int blockX_len = N, blockY_len = N;	// 当前处理块的长宽
		int img_x, img_y;	// 当前处理位置

		for (int i = 0; i < blockY_num; i++)
		{
			for (int j = 0; j < blockX_num; j++)
			{
				blockX_len = N;
				blockY_len = N;
				// 边块处理，对于正好分割的图形也试用，学一手
				if (i == blockY_num - 1) {	//最低侧的边块
					blockY_len = (imgYlen - 1) % N + 1;
				}
				if (j == blockX_num - 1) {	//最右侧的边块
					blockX_len = (imgXlen - 1) % N + 1;
				}

				img_x = j * N;
				img_y = i * N;

				// 读取颜色通道
				for (int k = 0; k < 3; k++)
				{
					poSrcDS_MUL->GetRasterBand(k + 1)->RasterIO(GF_Read,
						img_x, img_y, blockX_len, blockY_len, buffTmp_rgb[k], blockX_len, blockY_len, GDT_Float32, 0, 0);

				}
				poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read,
					img_x, img_y, blockX_len, blockY_len, buffTmp_PAN, blockX_len, blockY_len, GDT_Float32, 0, 0);

				// 进行 RGB ==> IHS 正变换
				for (int m = 1; m < 3; m++)
				{
					for (int n = 0; n < blockX_len*blockY_len; n++)
					{
						trans_res[m][n] = 0;
						for (int k = 0; k < 3; k++)
						{
							trans_res[m][n] += tran1[m][k] * buffTmp_rgb[k][n];
						}
					}
				}

				// 利用 PAN 替换 I 分量
				for (int k = 0; k < blockX_len*blockY_len; k++)
				{
					trans_res[0][k] = buffTmp_PAN[k];
				}

				// 进行 RGB ==> IHS 反变换
				for (int m = 1; m < 3; m++)
				{
					for (int n = 0; n < blockX_len*blockY_len; n++)
					{
						buffTmp_rgb[m][n] = 0;
						for (int k = 0; k < 3; k++)
						{
							buffTmp_rgb[m][n] += tran2[m][k] * trans_res[k][n];
						}
					}
				}

				for (int k = 0; k < 3; k++)
				{
					poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
						img_x, img_y, blockX_len, blockY_len, buffTmp_rgb[k], blockX_len, blockY_len, GDT_Float32, 0, 0);
				}

				cout << " ... ... block " << i * blockX_num + j + 1 << " processing ... ..." << endl;
			}
		}
		//清除内存
		CPLFree(buffTmp_PAN);
		for (int i = 0; i < 3; i++)
		{
			CPLFree(buffTmp_rgb[i]);
			CPLFree(trans_res[i]);
		}
	}
	else if( m_choose == 2){
		// 使用256 * imgXlen 大小的块对图像进行处理
		//分配内存
		buffTmp_PAN = (float*)CPLMalloc(N*imgXlen * sizeof(float));
		for (int i = 0; i < 3; i++)
		{
			buffTmp_rgb[i] = (float*)CPLMalloc(N*imgXlen * sizeof(float));
			trans_res[i] = (float*)CPLMalloc(N*imgXlen * sizeof(float));
		}

		// 相关变量
		int blockY_num = (imgYlen - 1) / N + 1;
		int blockY_len = N;	// 当前处理块的宽
		int img_y;	// 当前处理位置

		for (int i = 0; i < blockY_num; i++)
		{

			blockY_len = N;
			// 边块处理，对于正好分割的图形也试用，学一手
			if (i == blockY_num - 1) {	//最低侧的边块
				blockY_len = (imgYlen - 1) % N + 1;
			}

			img_y = i * N;

			// 读取颜色通道
			for (int k = 0; k < 3; k++)
			{
				poSrcDS_MUL->GetRasterBand(k + 1)->RasterIO(GF_Read,
					0, img_y, imgXlen, blockY_len, buffTmp_rgb[k], imgXlen, blockY_len, GDT_Float32, 0, 0);
			}
			poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 
				0, img_y, imgXlen, blockY_len, buffTmp_PAN, imgXlen, blockY_len, GDT_Float32, 0, 0);

			// 进行 RGB ==> IHS 正变换
			for (int m = 1; m < 3; m++)
			{
				for (int n = 0; n < imgXlen*blockY_len; n++)
				{
					trans_res[m][n] = 0;
					for (int k = 0; k < 3; k++)
					{
						trans_res[m][n] += tran1[m][k] * buffTmp_rgb[k][n];
					}
				}
			}

			// 利用 PAN 替换 I 分量
			for (int k = 0; k < imgXlen*blockY_len; k++)
			{
				trans_res[0][k] = buffTmp_PAN[k];
			}

			// 进行 RGB ==> IHS 反变换
			for (int m = 1; m < 3; m++)
			{
				for (int n = 0; n < imgXlen*blockY_len; n++)
				{
					buffTmp_rgb[m][n] = 0;
					for (int k = 0; k < 3; k++)
					{
						buffTmp_rgb[m][n] += tran2[m][k] * trans_res[k][n];
					}
				}
			}

			for (int k = 0; k < 3; k++)
			{
				poDstDS->GetRasterBand(k + 1)->RasterIO(GF_Write,
					0, img_y, imgXlen, blockY_len, buffTmp_rgb[k], imgXlen, blockY_len, GDT_Float32, 0, 0);
			}

			cout << " ... ... block " << i + 1 << " processing ... ..." << endl;
		}
		//清除内存
		CPLFree(buffTmp_PAN);
		for (int i = 0; i < 3; i++)
		{
			CPLFree(buffTmp_rgb[i]);
			CPLFree(trans_res[i]);
		}
	}

	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);

	system("PAUSE");

	return 0;
}


void printGod()
{
	cout << " ......................实验6......................" << endl;
	cout << "                       _oo0oo_                      " << endl;
	cout << "                      o8888888o                     " << endl;
	cout << "                      88\" . \"88                     " << endl;
	cout << "                      (| -_- |)                     " << endl;
	cout << "                      0\\  =  /0                     " << endl;
	cout << "                    ___/‘---’\\___                   " << endl;
	cout << "                  .' \\|       |/ '.                 " << endl;
	cout << "                 / \\\\|||  :  |||// \\                " << endl;
	cout << "                / _||||| -卍-|||||_ \\               " << endl;
	cout << "               |   | \\\\\\  -  /// |   |              " << endl;
	cout << "               | \\_|  ''\\---/''  |_/ |              " << endl;
	cout << "               \\  .-\\__  '-'  ___/-. /              " << endl;
	cout << "             ___'. .'  /--.--\\  '. .'___            " << endl;
	cout << "          .\"\" ‘<  ‘.___\\_<|>_/___.’ >’ \"\".           "<< endl;
	cout << "         | | :  ‘- \\‘.;‘\\ _ /’;.’/ - ’ : | |        " << endl;
	cout << "         \\  \\ ‘_.   \\_ __\\ /__ _/   .-’ /  /        " << endl;
	cout << "     =====‘-.____‘.___ \\_____/___.-’___.-’=====     " << endl;
	cout << "                       ‘=---=’                      " << endl;
	cout << "                                                    " << endl;
}