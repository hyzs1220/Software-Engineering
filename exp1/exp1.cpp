// exp1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>

using namespace std;

#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


int main()
{
	//����ͼ��
	GDALDataset* poSrcDS;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	char* srcPath = "trees.jpg";
	char* dstPath = "res.tif";
	GByte* buffTmp;
	int i, bandNum;

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	//�����ȡ���
	cout << "image X Lenth: " << imgXlen << endl;
	cout << "image Y Lenth: " << imgYlen << endl;
	cout << "Band number: " << bandNum << endl;
	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//һ�������ε����룬Ȼ��һ�������ε����
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}
	//����ڴ�
	CPLFree(buffTmp);
	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	system("PAUSE");
    return 0;
}

