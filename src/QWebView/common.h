#pragma once
#include <QString>

#define  TILT_UP 21 // 云台上仰
#define  TILT_DOWN 22 // 云台下俯
#define  PAN_LEFT 23 // 云台左转
#define  PAN_RIGHT 24 // 云台右转
#define  UP_LEFT 25 // 云台上仰和左转
#define  UP_RIGHT 26 // 云台上仰和右转
#define  DOWN_LEFT 27 // 云台下俯和左转
#define  DOWN_RIGHT 28 // 云台下俯和右转
#define  ZOOM_IN = 11			//焦距变大(倍率变大) 
#define  ZOOM_OUT = 12		//焦距变小(倍率变小) 


#define  MIN_INDEX	 110
#define  MAX_INDEX	 299
#define  MAIN_INDEX  300



struct MonitorInfo_T
{
	QString id_;
	QString dev_type_;
	QString name_;
	QString channel_id_;
	QString ip_;
	QString port_;
	QString user_name_;
	QString pwd_;

};


enum LABEL_MODEL
{
	MOD_NORMAL = 301,	//普通模式
	MOD_SMP = 302,		// 简图模式
	MOD_UNKNOW			// 未知 	

}; // enum LABEL_MODEL

const QMap<LABEL_MODEL, QString> GLOBAL_LAB_MOD = {

	{ MOD_NORMAL ,u8"普通模式"},//监控
	{ MOD_SMP ,u8"简洁模式"},//智能设备
	{ MOD_UNKNOW ,u8"未知模式"}//未知

};


