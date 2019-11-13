#pragma once

#include <QThread>
#include <pylon/PylonIncludes.h>
#include <pylon/PylonGUI.h>
#include <genapi/Pointer.h>
#include <genapi/IInteger.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;
using namespace Pylon;
using namespace std;
//Example of an image event handler.
class CSampleImageEventHandler : public CImageEventHandler
{
public:
	virtual void OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult);
//	{
//		cout << "CSampleImageEventHandler::OnImageGrabbed called." << std::endl;
//		cout << std::endl;
//		cout << std::endl;
//		// Image grabbed successfully?
//		if (ptrGrabResult->GrabSucceeded())
//		{
//			// Access the image data.
//			cout << ":SizeX: " << ptrGrabResult->GetWidth() << endl;
//			cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
//			const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
//			cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
//			CImagePersistence::Save(ImageFileFormat_Png, "GrabbedImage.png", ptrGrabResult);
//#ifdef PYLON_WIN_BUILD
//			// Display the grabbed image.
//			Pylon::DisplayImage(1, ptrGrabResult);
//#endif
//		}
//		else
//		{
//			cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
//		}
//	}
};
class BaslerCameraThread : public QThread
{
	Q_OBJECT

public:
	/*����ģʽ*/
	static BaslerCameraThread* Instance();
	~BaslerCameraThread();
	/*��ʼ�������*/
	bool InitCamDevice();
	
public slots:
	/*�����߳̿�ʼץͼ*/
	void StartGrab();
	///**ֹͣ�߳�*/
	void StopCrab();
	/**��ɲ�ͼ*/
	void CompleteCrabImg();
	void onOneGrab();
	//�Ƿ��ʼ���ɹ�
public:
	bool IsInit = false;
signals:
	/**��ɲ�ͼ�ź�*/
	void sigCompleteCrab();
protected:
	void run()override;
private:
	BaslerCameraThread(QObject *parent = NULL);
	//CInstantCamera mCamera;
	Camera_t* camera = nullptr;
	
	bool IsStopThread = false;//�Ƿ�ֹͣ�߳�
	bool IsComplete = false; //�Ƿ������ɨ
	////����Ŀ¼��
	//QString mSaveDir;
	////��Ŀ¼��
	//QString no;
};
