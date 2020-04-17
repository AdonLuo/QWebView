#pragma once
#include <QString>

#define  TILT_UP 21 // ��̨����
#define  TILT_DOWN 22 // ��̨�¸�
#define  PAN_LEFT 23 // ��̨��ת
#define  PAN_RIGHT 24 // ��̨��ת
#define  UP_LEFT 25 // ��̨��������ת
#define  UP_RIGHT 26 // ��̨��������ת
#define  DOWN_LEFT 27 // ��̨�¸�����ת
#define  DOWN_RIGHT 28 // ��̨�¸�����ת
#define  ZOOM_IN = 11			//������(���ʱ��) 
#define  ZOOM_OUT = 12		//�����С(���ʱ�С) 


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
	MOD_NORMAL = 301,	//��ͨģʽ
	MOD_SMP = 302,		// ��ͼģʽ
	MOD_UNKNOW			// δ֪ 	

}; // enum LABEL_MODEL

const QMap<LABEL_MODEL, QString> GLOBAL_LAB_MOD = {

	{ MOD_NORMAL ,u8"��ͨģʽ"},//���
	{ MOD_SMP ,u8"���ģʽ"},//�����豸
	{ MOD_UNKNOW ,u8"δ֪ģʽ"}//δ֪

};


