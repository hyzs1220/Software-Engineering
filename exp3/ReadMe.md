# 简单抠像  
在这一次的实验中，对superman.jpg图像进行操作，对其中的像素进行挑选，将其中满足条件的像素点绘制在space.jpg中，实现抠像操作。   

## 图片素材  

space.jpg  
![space](https://camo.githubusercontent.com/f60c069ce2a7244d3774b657e4fcce0fca77eb4a/687474703a2f2f7777312e73696e61696d672e636e2f6c617267652f36646562373261336c793166776738637277736c346a323068733064636a75342e6a7067)  

superman.jpg  
![superman](https://camo.githubusercontent.com/8c8842dc1627e9762bf1f4f354fed244ee1c491a/687474703a2f2f7777312e73696e61696d672e636e2f6c617267652f36646562373261336c79316677673864697238646c6a323068733064636e32332e6a7067)  

最终结果：  
![result](https://camo.githubusercontent.com/1fd1ffcc0e2965349fe85493bed8257d94ef6891/687474703a2f2f7777312e73696e61696d672e636e2f6c617267652f36646562373261336c7931667767386d34723561656a323068733064643430332e6a7067)  

## 实现思路  

 + 首先读取superman.jpg图像的三个颜色通道，然后对其进行条件判断(superman.jpg图像中的像素值用（r,g,b）表示，条件1：10 < r < 160; 条件2：100 < g < 220; 条件3：10 < b < 110；同时满足这三个条件的像素为背景区域像素)，如果为非背景区域像素，便将对应的flag二维数组的值置1。
```c
	vector<vector<int> > flag(imgYlen, vector<int>(imgXlen));	//标志像素是否符合判断条件
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
```
 + 这样便可以通过flag数组来找到满足条件的像素点。然后读取space图像和superman图像，从space图像中选取背景，从superman图像中选取超人部门，然后写入到新建图像中，完成抠像操作。  

```c++
	for (int i = 0; i < bandNum; i++)
	{
		//读取两个图像像素
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poSrcDS_bg->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp_bg, imgXlen, imgYlen, GDT_Byte, 0, 0);


		for (int j = 0; j < imgYlen; j++)
		{
			for (int k = 0; k < imgXlen; k++)
			{
				if (flag[j][k] == 1) {	//根据flag标志，来确定像素点来源
					buffTmp[j*imgXlen + k] = buffTmp_bg[j*imgXlen + k];
				}
			}
		}
		//写入新建图像
		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		cout << " ... ... band " << i << " processing ... ..." << endl;
	}
```

+ 好好学习，天天向上~  
