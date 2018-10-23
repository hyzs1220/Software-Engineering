// exp3.cpp : �������̨Ӧ�ó������ڵ㡣
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
	GDALDataset* poSrcDS_bg;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	char* srcPath = "superman.jpg";
	char* srcPath_bg = "space.jpg";
	char* dstPath = "res.tif";
	GByte* buffTmp, *buffTmp_r, *buffTmp_g, *buffTmp_b;
	GByte* buffTmp_bg;
	int bandNum;

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	poSrcDS_bg = (GDALDataset*)GDALOpenShared(srcPath_bg, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();
	
	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	buffTmp_r = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	buffTmp_g = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	buffTmp_b = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	buffTmp_bg = (GByte*)CPLMalloc(imgXlen*imgYlen * sizeof(GByte));
	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,
		0, 0, imgXlen, imgYlen, buffTmp_r, imgXlen, imgYlen, GDT_Byte, 0, 0);
	poSrcDS->GetRasterBand(2)->RasterIO(GF_Read,
		0, 0, imgXlen, imgYlen, buffTmp_g, imgXlen, imgYlen, GDT_Byte, 0, 0);
	poSrcDS->GetRasterBand(3)->RasterIO(GF_Read,
		0, 0, imgXlen, imgYlen, buffTmp_b, imgXlen, imgYlen, GDT_Byte, 0, 0);


	vector<vector<int> > flag(imgYlen, vector<int>(imgXlen));

	for (int i = 0; i < imgYlen; i++)
	{
		for (int j = 0; j < imgXlen; j++)
		{
			if (buffTmp_r[i*imgXlen + j] <= 10 || buffTmp_r[i*imgXlen + j] >= 160)
				continue;
			if (buffTmp_g[i*imgXlen + j] <= 100 || buffTmp_g[i*imgXlen + j] >= 220)
				continue;
			if (buffTmp_b[i*imgXlen + j] <= 10 || buffTmp_b[i*imgXlen + j] >= 110)
				continue;
			flag[i][j] = 1;
				
		}
	}

	//һ�������ε����룬Ȼ��һ�������ε����
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poSrcDS_bg->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp_bg, imgXlen, imgYlen, GDT_Byte, 0, 0);


		for (int j = 0; j < imgYlen; j++)
		{
			for (int k = 0; k < imgXlen; k++)
			{
				
				if (flag[j][k] == 1) {
					//cout << j << " " << k << endl;
					buffTmp[j*imgXlen + k] = buffTmp_bg[j*imgXlen + k];
				}
			}
		}


		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}




	//����ڴ�
	CPLFree(buffTmp);
	CPLFree(buffTmp_r);
	CPLFree(buffTmp_g);
	CPLFree(buffTmp_b);
	CPLFree(buffTmp_bg);
	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
	GDALClose(poSrcDS_bg);

	system("PAUSE");
	return 0;
}