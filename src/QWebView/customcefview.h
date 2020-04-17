#pragma once

#include "QCefView.h"

class CustomCefView : public QCefView
{
	Q_OBJECT

public:
	CustomCefView(const QString& url, QWidget *parent = Q_NULLPTR);
	~CustomCefView();

	void changeColor();
	QString GetUrl();

protected:


	virtual void onLoadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward) override;


	virtual void onLoadStart() override;


	virtual void onLoadEnd(int httpStatusCode) override;


	virtual void onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl) override;


	virtual void onQCefUrlRequest(const QString& url) override;


	virtual void onQCefQueryRequest(const QCefQuery& query) override;


	virtual void onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments) override;

signals:

	void loadingStateChangedSignal(bool isLoading, bool canGoBack, bool canGoForward);
	void StartExeSignal(const QVariantList& arguments);
	void UrlReqSignal(const QString& url);



private:
	QString uri_;
};
