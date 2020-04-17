
#include "stdafx.h"
#include "customcefview.h"

CustomCefView::CustomCefView(const QString& url, QWidget *parent /*= Q_NULLPTR*/)
	:QCefView(url, parent)
	, uri_(url)
{
	//setAttribute(Qt::WA_DeleteOnClose); //�ر�ʱ�Զ�ɾ��
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

}

CustomCefView::~CustomCefView()
{

}

void CustomCefView::changeColor()
{
	qsrand(::GetTickCount());

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
	QString title("QCef Url Request");

	emit UrlReqSignal(url);
}

void CustomCefView::onQCefQueryRequest(const QCefQuery& query)
{
	QString title(u8"QCef Query Request");
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

		// in case the mouse is being captured, try to release it ��걻ץ��,��ͼ�ͷ���
		::ReleaseCapture();

		// simulate that the mouse left button is down on the title area ģ�������ť��������
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

