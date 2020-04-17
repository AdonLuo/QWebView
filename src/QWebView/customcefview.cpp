
#include "stdafx.h"
#include "customcefview.h"

CustomCefView::CustomCefView(const QString& url, QWidget *parent /*= Q_NULLPTR*/)
	:QCefView(url, parent)
	, uri_(url)
{
	//setAttribute(Qt::WA_DeleteOnClose); //关闭时自动删除
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

}

CustomCefView::~CustomCefView()
{

}

void CustomCefView::changeColor()
{
	qsrand(::GetTickCount());	QColor color(qrand());	QCefEvent event("colorChangedEvent");	event.setStringProperty("color", color.name());	broadcastEvent("colorChange", event);

	qDebug() << u8"changeColor: " << "Event -> Set pro -> broadcast";
}


QString CustomCefView::GetUrl()
{
	return uri_;
}


void CustomCefView::onLoadingStateChanged(bool onLoadingStateChanged, bool canGoBack, bool canGoForward)
{
	qDebug() << "[ onLoadingStateChanged ]" << onLoadingStateChanged << canGoBack << canGoForward;
	emit loadingStateChangedSignal(onLoadingStateChanged, canGoBack, canGoForward);
}

void CustomCefView::onLoadStart()
{
	//qDebug() << "[ onLoadStart ]" ;
}

void CustomCefView::onLoadEnd(int httpStatusCode)
{
	//qDebug() << "[ onLoadEnd ]" << httpStatusCode;
}

void CustomCefView::onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl)
{
	qDebug() << "[ onLoadError ]" << errorCode << errorMsg << failedUrl;
}

void CustomCefView::onQCefUrlRequest(const QString& url)
{
	QString title("QCef Url Request");	QString text = QString("Current Thread: QT_UI\r\n"		"Url: %1")		.arg(url);	qDebug() << "[ onQCefUrlRequest ]" << url;	//QMessageBox::information(this->window(), title, text);

	emit UrlReqSignal(url);
}

void CustomCefView::onQCefQueryRequest(const QCefQuery& query)
{
	QString title(u8"QCef Query Request");	QString text = QString("Current Thread: QT_UI\r\n"		"Query: %1")		.arg(query.reqeust());	qDebug() << u8"[onQCefQueryRequest:] " << query.reqeust();	QMessageBox::information(this->window(), title, text);	QString response = query.reqeust().toUpper();	query.setResponseResult(true, response);	responseQCefQuery(query);
}

void CustomCefView::onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments)
{
	qDebug() << u8"[onInvokeMethodNotify: ]" << method << arguments;

	if (0 == method.compare("onDragAreaMouseDown"))
	{
		HWND hWnd = ::GetAncestor((HWND)getCefWinId(), GA_ROOT);

		// get current mouse cursor position
		POINT pt;
		::GetCursorPos(&pt);

		// in case the mouse is being captured, try to release it 鼠标被抓获,试图释放它
		::ReleaseCapture();

		// simulate that the mouse left button is down on the title area 模拟鼠标左按钮标题区域
		::SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS(pt));
		return;
	}


	if (0 == method.compare("StartExe"))
	{
		emit StartExeSignal(arguments);

		return;
	}


	QString title("QCef InvokeMethod Notify");
	QString text = QString("Current Thread: QT_UI\r\n""Method: %1\r\n""Arguments: ...").arg(method);

	qDebug() << title << text;

	QMessageBox::information(this->window(), title, text);

}


