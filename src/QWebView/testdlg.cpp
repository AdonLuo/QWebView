#include "stdafx.h"
#include "testdlg.h"

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

TestDlg::TestDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint); //去除边框和任务栏、置顶
	setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint);
	//setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	//setWindowFlags(Qt::FramelessWindowHint);
}

TestDlg::~TestDlg()
{
}

bool TestDlg::Setup(QJsonObject param)
{
// 	thread_.setMaxThreadCount(1); //业务处理线程
// 	topTimer_ = startTimer(500);

	PreClose(); //关闭残留的附加进程
	HandleMessage(param); //初始化参数
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

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json格式化输入参数
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
	//LOG_TRACE("预关闭已开启的附加应用程序");
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
					//LOG_ERROR("关闭进程失败, process: {}, err: {}", processId, GetLastError());
				}
			}
		}
		CloseHandle(process);
		return TRUE;
	}, (LPARAM)&classarr))
	{
		//LOG_ERROR("预关闭失败, err: {}", GetLastError());
	}
}

void TestDlg::HandleMessage(QJsonObject message)
{
	if (message.contains("quit") && message["quit"].toBool())
	{
		//LOG_TRACE("关闭应用程序");
		close();
		return;
	}
	if (message.contains("visible") && message["visible"].isBool())
	{
		auto visible = message["visible"].toBool();
		//LOG_TRACE("设置显示隐藏应用程序, visible: {}", visible);
		setVisible(visible);
	}
	if (message.contains("x") && message.contains("y") && message["x"].isDouble() && message["y"].isDouble())
	{
		auto x = message["x"].toInt(), y = message["y"].toInt();
		//LOG_TRACE("设置窗口位置, {}x{}", x, y);
		qDebug() << this->objectName() << "   " << this->parent()->objectName();
		auto pt = mapToGlobal(QPoint(x, y));
		auto mp_pt = mapToParent(QPoint(x, y));

		auto cur_pos = QCursor::pos();
		qDebug() << u8"相对显示器的全局坐标:" << cur_pos << u8"当前目前控件在父窗口中的位置: " << pos();

		qDebug() << u8"需要设置窗口的位置" << x << y << u8"转成相对父窗口位置：" << pt << "mptop: " << mp_pt;


		move(x, y);
		//move(mp_pt);
	}
	if (message.contains("width") && message.contains("height") && message["width"].isDouble() && message["height"].isDouble())
	{
		auto width = message["width"].toInt(), height = message["height"].toInt();
		//LOG_TRACE("设置窗口大小, {}x{}", width, height);
		resize(width, height);
	}
	if (message.contains("layout") && message["layout"].isString())
	{
		auto layout = message["layout"].toString();
		//LOG_TRACE("设置窗口布局: {}", layout.toStdString());
		HandleLayout(layout);
	}
	if (message.contains("close") && message["close"].isArray())
	{
		auto closearr = message["close"].toArray();
		//LOG_TRACE("关闭窗口: {}", QJsonDocument(closearr).toJson(QJsonDocument::Compact).toStdString());
		HandleClose(closearr);
	}
	if (message.contains("open") && message["open"].isArray())
	{
		auto openarr = message["open"].toArray();
		//LOG_TRACE("打开窗口: {}", QJsonDocument(openarr).toJson(QJsonDocument::Compact).toStdString());
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
