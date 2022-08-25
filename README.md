# OCR小工具
使用[有道OCR引擎](https://ai.youdao.com/product-ocr-print.s)，支持手动截屏OCR和固定区域OCR，后者一般用于抓取电影硬字幕。

运行前，请在 MainWindow.cpp 中设置有道帐户的key和secret：

`m_youdaoOcr->setKey("your key", "your secret");`

![image](screenshot.png)
