#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qwebview.h"
#include "customcefview.h"
#include "windowholderex.h"
#include "monitordemo.h"
#include "testwid.h"
#include "testdlg.h"

class QWebView : public QMainWindow
{
	Q_OBJECT

public:
	QWebView(QWidget *parent = Q_NULLPTR);

protected:

	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;


	virtual void moveEvent(QMoveEvent *event) override;


	virtual void resizeEvent(QResizeEvent *event) override;
	void ScreenInfo();

signals:
	void UrlReqParamSignal(const QJsonObject& url);
protected slots:

	void on_btn_min_clicked();
	void on_btn_max_clicked();
	void onBtnCloseClicked();
	void onBtnGoClicked();
	void onBtnGoBackClicked();
	void onBtnGoForwardClicked();
	void onBtnReloadOrStopClicked();
	void onBtnChangeColorClicked();

	void onLoadingStateChangedSignal(bool isLoading, bool canGoBack, bool canGoForward);
	void onCurrentChanged(int index);

	void onStartExe(const QVariantList& arguments);

	void onNotifyCamPragam(QJsonObject& new_obj);

	void OnUrlReq(const QString& req);

private:
	void ReqParamParse(const QJsonObject& req_param_obj);
	QRect Show2NowRect(QRect& real_rc);
private:
	Ui::QWebViewClass ui;
	CustomCefView* curCefView_;
	WindowHolderEx* win_holder_;
	MonitorDemo* monitor_view_wid_;
	TestWid* wid_;
	TestDlg* dlg_;

	QPoint last_pos_;
	bool is_move_;
	bool is_not_max_;
	QRect rect_;
	QSize size_;

};
