#include "stdafx.h"
#include "qwebview.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//加载配置文件
	auto configPath = a.applicationDirPath() + "/windowholder.json";
	//LOG_TRACE("加载配置文件, config: {}", configPath.toStdString());
	QFile configFile(configPath);
	if (configFile.open(QFile::ReadOnly | QFile::Text))
	{
		auto config = QJsonDocument::fromJson(configFile.readAll()).object();
		configFile.close();
		if (config.isEmpty())
		{
			//LOG_ERROR("配置文件非标准json格式或为空");
			return -1;
		}
		a.setProperty("config", config); //缓存配置信息
	}
	else
	{
		//LOG_ERROR("加载配置文件失败");
		return -1;
	}

	QWebView w;
	w.show();
	return a.exec();
}
