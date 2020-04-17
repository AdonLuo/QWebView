#include "stdafx.h"
#include "testdlg.h"

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

TestDlg::TestDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); //ȥ���߿�����������ö�
	setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint);
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	//setWindowFlags(Qt::FramelessWindowHint);
}

TestDlg::~TestDlg()
{
}

bool TestDlg::Setup(QJsonObject param)
{
// 	thread_.setMaxThreadCount(1); //ҵ�����߳�
// 	topTimer_ = startTimer(500);

	PreClose(); //�رղ����ĸ��ӽ���
	HandleMessage(param); //��ʼ������
	return true;
}

void TestDlg::Clear()
{
// 	thread_.waitForDone();
// 	killTimer(topTimer_);
}

void TestDlg::OnUrlReq(const QString& req)
{
	qDebug() << u8"TestDlg - onUrlReq: " << req;
	auto param = req;
	// 'windowholder://{"layout":"1+5"}' 
	QJsonObject object;
	if (param.startsWith("qcef://"))
	{
		param.remove("qcef://");
		if (param.endsWith('/'))
		{
			param.remove(param.length() - 1, 1);
		}
		param = QUrl::fromPercentEncoding(param.toUtf8());

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json��ʽ���������
		if (object.isEmpty())
		{
			return;
		}

		if (!object.isEmpty())
		{
			HandleMessage(object);
			return;
		}
	}
}

void TestDlg::PreClose()
{
	//LOG_TRACE("Ԥ�ر��ѿ����ĸ���Ӧ�ó���");
	auto classarr = qApp->property("config").toJsonObject()["class"].toArray();
	if (!EnumWindows([](HWND hwnd, LPARAM lParam)
	{
		auto classarr = (QJsonArray*)lParam;
		DWORD processId = 0;
		GetWindowThreadProcessId(hwnd, &processId);
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		char filename[MAX_PATH] = { 0 };
		GetModuleFileNameEx(process, nullptr, filename, MAX_PATH);
		QFileInfo fileInfo(filename);
		for (auto classref : *classarr)
		{
			auto classobj = classref.toObject();
			if (classobj["name"].toString() == fileInfo.baseName())
			{
				if (!TerminateProcess(process, 0))
				{
					//LOG_ERROR("�رս���ʧ��, process: {}, err: {}", processId, GetLastError());
				}
			}
		}
		CloseHandle(process);
		return TRUE;
	}, (LPARAM)&classarr))
	{
		//LOG_ERROR("Ԥ�ر�ʧ��, err: {}", GetLastError());
	}
}

void TestDlg::HandleMessage(QJsonObject message)
{
	if (message.contains("quit") && message["quit"].toBool())
	{
		//LOG_TRACE("�ر�Ӧ�ó���");
		close();
		return;
	}
	if (message.contains("visible") && message["visible"].isBool())
	{
		auto visible = message["visible"].toBool();
		//LOG_TRACE("������ʾ����Ӧ�ó���, visible: {}", visible);
		setVisible(visible);
	}
	if (message.contains("x") && message.contains("y") && message["x"].isDouble() && message["y"].isDouble())
	{
		auto x = message["x"].toInt(), y = message["y"].toInt();
		//LOG_TRACE("���ô���λ��, {}x{}", x, y);
		qDebug() << this->objectName() << "   " << this->parent()->objectName();
		auto pt = mapToGlobal(QPoint(x, y));
		auto mp_pt = mapToParent(QPoint(x, y));

		auto cur_pos = QCursor::pos();
		qDebug() << u8"�����ʾ����ȫ������:" << cur_pos << u8"��ǰĿǰ�ؼ��ڸ������е�λ��: " << pos();

		qDebug() << u8"��Ҫ���ô��ڵ�λ��" << x << y << u8"ת����Ը�����λ�ã�" << pt << "mptop: " << mp_pt;


		move(x, y);
		//move(mp_pt);
	}
	if (message.contains("width") && message.contains("height") && message["width"].isDouble() && message["height"].isDouble())
	{
		auto width = message["width"].toInt(), height = message["height"].toInt();
		//LOG_TRACE("���ô��ڴ�С, {}x{}", width, height);
		resize(width, height);
	}
	if (message.contains("layout") && message["layout"].isString())
	{
		auto layout = message["layout"].toString();
		//LOG_TRACE("���ô��ڲ���: {}", layout.toStdString());
		HandleLayout(layout);
	}
	if (message.contains("close") && message["close"].isArray())
	{
		auto closearr = message["close"].toArray();
		//LOG_TRACE("�رմ���: {}", QJsonDocument(closearr).toJson(QJsonDocument::Compact).toStdString());
		HandleClose(closearr);
	}
	if (message.contains("open") && message["open"].isArray())
	{
		auto openarr = message["open"].toArray();
		//LOG_TRACE("�򿪴���: {}", QJsonDocument(openarr).toJson(QJsonDocument::Compact).toStdString());
		HandleOpen(openarr);
	}
}

void TestDlg::HandleLayout(QString name)
{

}

void TestDlg::HandleClose(QJsonArray closes)
{

}

void TestDlg::HandleOpen(QJsonArray opens)
{

}

QString TestDlg::GetClass(QString name)
{
	auto classes = qApp->property("config").toJsonObject()["class"].toArray();
	for (auto classRef : classes)
	{
		auto cls = classRef.toObject();
		if (cls["name"].toString() == name)
		{
			return cls["class"].toString();
		}
	}
	return QString();
}

void TestDlg::resizeEvent(QResizeEvent *event)
{

}

void TestDlg::timerEvent(QTimerEvent *event)
{

}
