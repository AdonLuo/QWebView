#include "stdafx.h"
#include "windowcontainer.h"

WindowContainer::WindowContainer(QThreadPool* thread, QWidget *parent)
    : QFrame(parent)
    , thread_(thread)
    , process_(nullptr)
    , hwnd_(nullptr)
{
    SetupUI();

    connect(this, &WindowContainer::Opened, this, [this](quint64 process, quint64 hwnd)
    {
        tips_->hide();

        process_ = (HANDLE)process;
        hwnd_ = (HWND)hwnd;

        //SetParent(hwnd_, (HWND)winId());
        auto rc = contentsRect();
		auto real_rc = QRect(mapToGlobal(rc.topLeft()), size());;
		qDebug() << "Pos: " <<rc;

        SetWindowPos(hwnd_, HWND_TOP, real_rc.x(), real_rc.y(), real_rc.width(), real_rc.height(), SWP_SHOWWINDOW);
    });
    connect(this, &WindowContainer::TipsTextChanged, this, [this](QString text)
    {
        tips_->setText(text);
    });
}

WindowContainer::~WindowContainer()
{
    Close();
}

void WindowContainer::Open(QJsonObject open)
{
    Close();
    
    tips_->show();

    QtConcurrent::run(thread_, [=]()
    {
        auto command = open["path"].toString() + " " + open["param"].toString();
        STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
        startupInfo.dwFlags = STARTF_USESHOWWINDOW;
        startupInfo.wShowWindow = SW_HIDE;
        PROCESS_INFORMATION processInfo = { 0 };
        if (!CreateProcess(nullptr, (LPSTR)command.toStdString().c_str(), nullptr,
            nullptr, FALSE, 0, nullptr, nullptr, &startupInfo, &processInfo))
        {
			qDebug() << "CreateProcess Err " << command;
            emit TipsTextChanged(u8"打开失败!");
            return;
        }
		qDebug() << "CreateProcess Succed " << command <<"process : "<< processInfo.hProcess <<"processid: "<< processInfo.dwProcessId;
        auto className = open["class"].toString().toStdString();
        auto hwnd = GetProcessHwnd(processInfo.hProcess, processInfo.dwProcessId, className, 20000);
        if (!hwnd)
        {
			qDebug() << "GetProcessHwnd Err " << open["class"].toString();


			WId wid = (WId)FindWindow("MozillaWindowClass", NULL);
			auto window = QWindow::fromWinId(wid);
			if (window)
			{
				hwnd = (HWND)wid;				
			}

			if (!hwnd)
			{
				emit TipsTextChanged(u8"打开失败!");
				TerminateProcess(processInfo.hProcess, 0);
				return;
			}
        }

        auto style = GetWindowLong(hwnd, GWL_STYLE);
        style = style | WS_CHILD;
        style = style & ~WS_CAPTION & ~WS_THICKFRAME;
        SetWindowLong(hwnd, GWL_STYLE, style);
        SetWindowLong(hwnd, GWL_EXSTYLE, 0);

        emit Opened((quint64)processInfo.hProcess, (quint64)hwnd);
    });

    tips_->setText(u8"正在打开,请稍后...");
}

void WindowContainer::Close()
{
    if (process_)
    {
        TerminateProcess(process_, 0);
        process_ = nullptr;
        hwnd_ = nullptr;
    }
}

void WindowContainer::SetupUI()
{
    setObjectName("windowcontainer");

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    tips_ = new QLabel(this);
    tips_->setObjectName("tips");
    tips_->setAlignment(Qt::AlignCenter);
    tips_->setText(u8"正在打开,请稍后...");
    tips_->hide();
    layout->addWidget(tips_);
}

struct EnumWindowsStru
{
    DWORD processId;
    std::string className;
    HWND hwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto processIdHwnd = (EnumWindowsStru*)lParam;
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processIdHwnd->processId == processId)
    {
        TCHAR className[MAX_PATH] = { 0 };
        GetClassName(hwnd, className, MAX_PATH);
        if (processIdHwnd->className == className)
        {
            processIdHwnd->hwnd = hwnd;
            return FALSE;
        }
		else
		{
			qDebug() << "processIdHwnd->className != className" << QString::fromStdString(processIdHwnd->className) << " -  " << QString(className);
		}
    }
	else
	{
		qDebug() <<u8"进程id:"<<processIdHwnd->processId << " !=  " << processId;
	}
    return TRUE;
}

HWND WindowContainer::GetProcessHwnd(HANDLE process, DWORD processId, std::string className, int timeout)
{
    DWORD exitCode = 0;
    GetExitCodeProcess(process, &exitCode);
    auto alwaysLoop = -1 == timeout ? true : false;
    while (STILL_ACTIVE == exitCode)
    {
        EnumWindowsStru processIdHwnd = { processId, className, nullptr };
        EnumWindows(EnumWindowsProc, (LPARAM)&processIdHwnd);
        if (processIdHwnd.hwnd)
        {
            auto rc = contentsRect();
            SetWindowPos(processIdHwnd.hwnd, HWND_TOP, rc.x(), rc.y(), rc.width(), rc.height(), SWP_HIDEWINDOW);
            return processIdHwnd.hwnd;
        }
        if (!alwaysLoop && 0 >= timeout)
        {
			qDebug() << u8"没找到";
            break;
        }
        QThread::msleep(50);
        timeout -= 50;
        GetExitCodeProcess(process, &exitCode);
    }
    return nullptr;
}

void WindowContainer::resizeEvent(QResizeEvent *event)
{
    if (hwnd_)
    {
        auto rc = contentsRect();
		auto real_rc = QRect(mapToGlobal(rc.topLeft()), size());;
        SetWindowPos(hwnd_, HWND_TOP, real_rc.x(), real_rc.y(), real_rc.width(), real_rc.height(), SWP_SHOWWINDOW);
    }
}
