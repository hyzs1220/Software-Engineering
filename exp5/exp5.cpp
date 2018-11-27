// exp5.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	//����ͼ��
	GDALDataset* poSrcDS_MUL;
	GDALDataset* poSrcDS_PAN;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	char* srcPath_MUL = "American_MUL.bmp";
	char* srcPath_PAN = "American_PAN.bmp";
	char* dstPath = "American_FUS.tif";
	float* buffTmp_PAN;
	float* buffTmp_rgb[3], * trans_res[3];
	int bandNum;
	// RGB ==> IHS ���任����
	float tran1[3][3] = {
		{	1.0f / 3.0f,		1.0f / 3.0f,		1.0f / 3.0f },
		{	-sqrt(2.0f) / 6.0f,	-sqrt(2.0f) / 6.0f,	sqrt(2.0f) / 3.0f },
		{	1.0f / sqrt(2.0f),   -1.0f / sqrt(2.0f),	0			}
	};
	// IHS ==> RGB ��任����
	float tran2[3][3] = {
		{	1.0f,  -1.0f / sqrt(2.0f),	1.0f / sqrt(2.0f)	},
		{	1.0f,  -1.0f / sqrt(2.0f),	-1.0f / sqrt(2.0f)	},
		{	1.0f,  sqrt(2.0f),		0			}
	};

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS_MUL = (GDALDataset*)GDALOpenShared(srcPath_MUL, GA_ReadOnly);
	poSrcDS_PAN = (GDALDataset*)GDALOpenShared(srcPath_PAN, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS_MUL->GetRasterXSize();
	imgYlen = poSrcDS_MUL->GetRasterYSize();
	bandNum = poSrcDS_PAN->GetRasterCount();

	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	buffTmp_PAN = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	for (int i = 0; i < 3; i++)
	{
		buffTmp_rgb[i] = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float) );
		trans_res[i] = (float*)CPLMalloc(imgXlen*imgYlen * sizeof(float));
	}

	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, 3, GDT_Byte, NULL);

	// ��ȡ��ɫͨ��
	for (int i = 0; i < 3; i++)
	{
		poSrcDS_MUL->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp_rgb[i], imgXlen, imgYlen, GDT_Float32, 0, 0);

	}
	poSrcDS_PAN->GetRasterBand(1)->RasterIO(GF_Read,
		0, 0, imgXlen, imgYlen, buffTmp_PAN, imgXlen, imgYlen, GDT_Float32, 0, 0);

	// ���� RGB ==> IHS ���任
	for (int i = 1; i < 3; i++)
	{
		for (int j = 0; j < imgXlen*imgYlen; j++)
		{
			trans_res[i][j] = 0;
			for (int k = 0; k < 3; k++)
			{
				trans_res[i][j] += tran1[i][k] * buffTmp_rgb[k][j];
			}
		}
	}

	// ���� PAN �滻 I ����
	for (int i = 0; i < imgXlen*imgYlen; i++)
	{
		trans_res[0][i] = buffTmp_PAN[i];
	}

	// ���� IHS ==> RGB ���任
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < imgXlen*imgYlen; j++)
		{
			buffTmp_rgb[i][j] = 0;
			for (int k = 0; k < 3; k++)
			{
				buffTmp_rgb[i][j] += tran2[i][k] * trans_res[k][j];
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp_rgb[i], imgXlen, imgYlen, GDT_Float32, 0, 0);
		cout << " ... ... band " << i + 1 << " processing ... ..." << endl;
	}


	//����ڴ�
	CPLFree(buffTmp_PAN);
	for (int i = 0; i < 3; i++)
	{
		CPLFree(buffTmp_rgb[i]);
		CPLFree(trans_res[i]);
	}
	
	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS_MUL);
	GDALClose(poSrcDS_PAN);

	system("PAUSE");

    return 0;
}

