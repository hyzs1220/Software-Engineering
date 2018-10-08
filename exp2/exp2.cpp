// exp2.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����ͼ��
	GDALDataset* poSrcDS;
	//���ͼ��
	GDALDataset* poDstDS;
	GByte* buffTmp;
	char* srcPath = "build.jpg";
	char* dstPath = "res.tif";

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	
	//�����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));

	//һ�������ε����룬Ȼ��һ�������ε����
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}


	// ������������һ����ɫɫ��

	//��ʼλ������
	StartX = 100;
	StartY = 100;
	//�����Ⱥ͸߶�
	tmpXlen = 200;
	tmpYlen = 150;
	//��ȡ��ɫͨ��������buffTmp��
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	//�������أ���������Ϊ255
	for (int j = 0; j < tmpYlen; j++)
	{
		for (int i = 0; i < tmpXlen; i++)
		{
			buffTmp[j*tmpXlen + i] = (GByte)255;
		}
	}
	//����д��poDstDS
	poDstDS->GetRasterBand(1)->RasterIO(GF_Write,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	cout << " ... ... red block processing ... ..." << endl;

	// ������������һ���׿�
	//��ʼλ������
	StartX = 300;
	StartY = 300;
	//�����Ⱥ͸߶�
	tmpXlen = 100;
	tmpYlen = 50;
	//��ȡ��ͨ��������buffTmp�У��������أ������ش���
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


	// ������������һ���ڿ�
	//��ʼλ������
	StartX = 500;
	StartY = 500;
	//�����Ⱥ͸߶�
	tmpXlen = 50;
	tmpYlen = 100;
	//��ȡ��ͨ��������buffTmp�У��������أ������ش���
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
	

	//����ڴ�
	CPLFree(buffTmp);
	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
    return 0;
}

