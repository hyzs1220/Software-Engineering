// exp6.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����ͼ��
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	char* srcPath_MUL = "./img/Mul_large.tif";
	char* srcPath_PAN = "./img/Pan_large.tif";
	char* dstPath = "./img/Ful_large.tif";
	float* buffTmp_PAN;
	float* buffTmp_rgb[3], *trans_res[3];
	int bandNum;
	// RGB ==> IHS ���任����
	float tran1[3][3] = {
		{ 1.0f / 3.0f,		1.0f / 3.0f,		1.0f / 3.0f },
		{ -sqrt(2.0f) / 6.0f,	-sqrt(2.0f) / 6.0f,	sqrt(2.0f) / 3.0f },
		{ 1.0f / sqrt(2.0f),	-1.0f / sqrt(2.0f),	0 }
	};
	// IHS ==> RGB ��任����
	float tran2[3][3] = {
		{ 1.0f,  -1.0f / sqrt(2.0f),	1.0f / sqrt(2.0f) },
		{ 1.0f,  -1.0f / sqrt(2.0f),	-1.0f / sqrt(2.0f) },
		{ 1.0f,  sqrt(2.0f),			0 }
	};

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS_MUL = (GDALDataset*)GDALOpenShared(srcPath_MUL, GA_ReadOnly);
	poSrcDS_PAN = (GDALDataset*)GDALOpenShared(srcPath_PAN, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS_MUL->GetRasterXSize();	//15696
	imgYlen = poSrcDS_MUL->GetRasterYSize();	//12004
	bandNum = poSrcDS_PAN->GetRasterCount();

	//�������ͼ��
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
		// ʹ��256 * 256 ��С�Ŀ��ͼ����д���
		//�����ڴ�
		buffTmp_PAN = (float*)CPLMalloc(N*N * sizeof(float));
		for (int i = 0; i < 3; i++)
		{
			buffTmp_rgb[i] = (float*)CPLMalloc(N*N * sizeof(float));
			trans_res[i] = (float*)CPLMalloc(N*N * sizeof(float));
		}

		// ��ر���
		int blockX_num = (imgXlen - 1) / N + 1;	// ����
		int blockY_num = (imgYlen - 1) / N + 1;
		int blockX_len = N, blockY_len = N;	// ��ǰ�����ĳ���
		int img_x, img_y;	// ��ǰ����λ��

		for (int i = 0; i < blockY_num; i++)
		{
			for (int j = 0; j < blockX_num; j++)
			{
				blockX_len = N;
				blockY_len = N;
				// �߿鴦���������÷ָ��ͼ��Ҳ���ã�ѧһ��
				if (i == blockY_num - 1) {	//��Ͳ�ı߿�
					blockY_len = (imgYlen - 1) % N + 1;
				}
				if (j == blockX_num - 1) {	//���Ҳ�ı߿�
					blockX_len = (imgXlen - 1) % N + 1;
				}

				img_x = j * N;
				img_y = i * N;

				// ��ȡ��ɫͨ��
				for (int k = 0; k < 3; k++)
				{
					poSrcDS_MUL->GetRasterBand(k + 1)->RasterIO(GF_Read,
						img_x, img_y, blockX_len, blockY_len, buffTmp_rgb[k], blockX_len, blockY_len, GDT_Float32, 0, 0);

				}
				poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read,
					img_x, img_y, blockX_len, blockY_len, buffTmp_PAN, blockX_len, blockY_len, GDT_Float32, 0, 0);

				// ���� RGB ==> IHS ���任
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

				// ���� PAN �滻 I ����
				for (int k = 0; k < blockX_len*blockY_len; k++)
				{
					trans_res[0][k] = buffTmp_PAN[k];
				}

				// ���� RGB ==> IHS ���任
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
		//����ڴ�
		CPLFree(buffTmp_PAN);
		for (int i = 0; i < 3; i++)
		{
			CPLFree(buffTmp_rgb[i]);
			CPLFree(trans_res[i]);
		}
	}
	else if( m_choose == 2){
		// ʹ��256 * imgXlen ��С�Ŀ��ͼ����д���
		//�����ڴ�
		buffTmp_PAN = (float*)CPLMalloc(N*imgXlen * sizeof(float));
		for (int i = 0; i < 3; i++)
		{
			buffTmp_rgb[i] = (float*)CPLMalloc(N*imgXlen * sizeof(float));
			trans_res[i] = (float*)CPLMalloc(N*imgXlen * sizeof(float));
		}

		// ��ر���
		int blockY_num = (imgYlen - 1) / N + 1;
		int blockY_len = N;	// ��ǰ�����Ŀ�
		int img_y;	// ��ǰ����λ��

		for (int i = 0; i < blockY_num; i++)
		{

			blockY_len = N;
			// �߿鴦���������÷ָ��ͼ��Ҳ���ã�ѧһ��
			if (i == blockY_num - 1) {	//��Ͳ�ı߿�
				blockY_len = (imgYlen - 1) % N + 1;
			}

			img_y = i * N;

			// ��ȡ��ɫͨ��
			for (int k = 0; k < 3; k++)
			{
				poSrcDS_MUL->GetRasterBand(k + 1)->RasterIO(GF_Read,
					0, img_y, imgXlen, blockY_len, buffTmp_rgb[k], imgXlen, blockY_len, GDT_Float32, 0, 0);
			}
			poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read, 
				0, img_y, imgXlen, blockY_len, buffTmp_PAN, imgXlen, blockY_len, GDT_Float32, 0, 0);

			// ���� RGB ==> IHS ���任
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

			// ���� PAN �滻 I ����
			for (int k = 0; k < imgXlen*blockY_len; k++)
			{
				trans_res[0][k] = buffTmp_PAN[k];
			}

			// ���� RGB ==> IHS ���任
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
		//����ڴ�
		CPLFree(buffTmp_PAN);
		for (int i = 0; i < 3; i++)
		{
			CPLFree(buffTmp_rgb[i]);
			CPLFree(trans_res[i]);
		}
	}

	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);

	system("PAUSE");

	return 0;
}


void printGod()
{
	cout << " ......................ʵ��6......................" << endl;
	cout << "                       _oo0oo_                      " << endl;
	cout << "                      o8888888o                     " << endl;
	cout << "                      88\" . \"88                     " << endl;
	cout << "                      (| -_- |)                     " << endl;
	cout << "                      0\\  =  /0                     " << endl;
	cout << "                    ___/��---��\\___                   " << endl;
	cout << "                  .' \\|       |/ '.                 " << endl;
	cout << "                 / \\\\|||  :  |||// \\                " << endl;
	cout << "                / _||||| -�d-|||||_ \\               " << endl;
	cout << "               |   | \\\\\\  -  /// |   |              " << endl;
	cout << "               | \\_|  ''\\---/''  |_/ |              " << endl;
	cout << "               \\  .-\\__  '-'  ___/-. /              " << endl;
	cout << "             ___'. .'  /--.--\\  '. .'___            " << endl;
	cout << "          .\"\" ��<  ��.___\\_<|>_/___.�� >�� \"\".           "<< endl;
	cout << "         | | :  ��- \\��.;��\\ _ /��;.��/ - �� : | |        " << endl;
	cout << "         \\  \\ ��_.   \\_ __\\ /__ _/   .-�� /  /        " << endl;
	cout << "     =====��-.____��.___ \\_____/___.-��___.-��=====     " << endl;
	cout << "                       ��=---=��                      " << endl;
	cout << "                                                    " << endl;
}