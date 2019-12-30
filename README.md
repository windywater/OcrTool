# OCR工具
基于Qt5的客户端工具，可进行图片文件OCR，截屏OCR。工具使用[搜狗OCR引擎](https://deepi.sogou.com/registered/textcognitive "搜狗OCR引擎")，使用前需申请帐号，申请后有100元的免费额度。

帐号申请后，得到PID和KEY，在SogouOcr.cpp里填写即可使用：
```cpp
QByteArray pid = "";
QByteArray key = "";
```

![工具截图](screenshot.png "工具截图")