#include "stdafx.h"
#include "qwebview.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//���������ļ�
	auto configPath = a.applicationDirPath() + "/windowholder.json";
	//LOG_TRACE("���������ļ�, config: {}", configPath.toStdString());
	QFile configFile(configPath);
	if (configFile.open(QFile::ReadOnly | QFile::Text))
	{
		auto config = QJsonDocument::fromJson(configFile.readAll()).object();
		configFile.close();
		if (config.isEmpty())
		{
			//LOG_ERROR("�����ļ��Ǳ�׼json��ʽ��Ϊ��");
			return -1;
		}
		a.setProperty("config", config); //����������Ϣ
	}
	else
	{
		//LOG_ERROR("���������ļ�ʧ��");
		return -1;
	}

	QWebView w;
	w.show();
	return a.exec();
}
