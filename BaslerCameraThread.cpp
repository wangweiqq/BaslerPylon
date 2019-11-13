#pragma execution_character_set("utf-8")
#include "BaslerCameraThread.h"
#include <QDebug>
#include <iostream>
#include <QDir>
#include <pylon/gige/BaslerGigEInstantCamera.h>
// Namespace for using pylon objects.
#include <QMessageBox>
#include "HardwareTriggerConfiguration.h"
#include "hardwaretriggerconfiguration2.h"
int ImgCount = 0;
QString mSaveDir = "D:/IMG1101_2d_T3/";
using namespace Basler_GigECameraParams;
void CSampleImageEventHandler::OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
{
	cout << "CSampleImageEventHandler::OnImageGrabbed called." << std::endl;
	cout << std::endl;
	cout << std::endl;
	// Image grabbed successfully?
	if (ptrGrabResult->GrabSucceeded())
	{
		// Access the image data.
		cout << ":SizeX: " << ptrGrabResult->GetWidth() << endl;
		cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
		const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
		cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
		
		QDir dir(mSaveDir);
		if (!dir.exists()) {
			dir.mkpath(mSaveDir);
		}
		QString str = QString("%1%2.png").arg(mSaveDir).arg(ImgCount++);
		QByteArray qba = str.toLocal8Bit();
		char* strData = qba.data();
		std::cout << "save path = " << strData << std::endl;
		CImagePersistence::Save(ImageFileFormat_Png, strData, ptrGrabResult);
		Camera_t* p = dynamic_cast<Camera_t*>(&camera);
		/*if (ImgCount % 2 != 0) {
			
			p->ExposureTimeRaw.SetValue(2500);
		}
		else {
			p->ExposureTimeRaw.SetValue(10000);
		}*/
		cout << "Gain          : " << p->GainRaw.GetValue() << endl;
		cout << "Exposure time : " << p->ExposureTimeRaw.GetValue() << endl;
		//CImagePersistence::Save(ImageFileFormat_Png, "GrabbedImage.png", ptrGrabResult);
//#ifdef PYLON_WIN_BUILD
//		// Display the grabbed image.
//		Pylon::DisplayImage(1, ptrGrabResult);
//#endif
	}
	else
	{
		cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
	}
}
BaslerCameraThread::BaslerCameraThread(QObject *parent)
	: QThread(parent)
{
	//mSaveDir = "D:/Temp/";
	QDir dir(mSaveDir);
	if (!dir.exists()) {
		dir.mkpath(mSaveDir);
	}
	QString str = QString("%1%2.png").arg(mSaveDir).arg(ImgCount++);
	QByteArray qba = str.toLocal8Bit();
	char* strData = qba.data();
	std::cout << "save path = " << strData << std::endl;
	PylonInitialize();	
	InitCamDevice();
}

BaslerCameraThread::~BaslerCameraThread()
{
	if (isRunning()) {
		IsStopThread = true;
		wait();
	}
	delete camera;
	PylonTerminate();
	std::cout << "PylonTerminate" << std::endl;
}
BaslerCameraThread* BaslerCameraThread::Instance() {
	static BaslerCameraThread pDecod;
	/*if (pDecod == NULL) {
		pDecod = new BaslerCameraThread();
	}*/
	return &pDecod;
}
bool BaslerCameraThread::InitCamDevice() {
	IsInit = false;
	try
	{
		CDeviceInfo info;
		
		//info.SetDeviceClass(Camera_t::DeviceClass());
		info.SetSerialNumber("22658707");
		camera = new Camera_t(CTlFactory::GetInstance().CreateDevice(info));
		//camera = new Camera_t(CTlFactory::GetInstance().CreateFirstDevice(info));

		cout << "Using device " << camera->GetDeviceInfo().GetModelName() << endl;
		camera->Close();
		// Open the camera.
		camera->Open();

		UserSetDefaultSelectorEnums oldDefaultUserSet = camera->UserSetDefaultSelector.GetValue();
		// Load default settings.
		cout << "Loading default settings" << endl;
		camera->UserSetSelector.SetValue(UserSetSelector_Default);
		camera->UserSetLoad.Execute();

		cout << "Turning off Gain Auto and Exposure Auto." << endl;

		/*camera->GainAuto.SetValue(GainAuto_Off);
		camera->GainRaw.SetValue(camera->GainRaw.GetMin());*/
		camera->ExposureAuto.SetValue(ExposureAuto_Off);
		//修改曝光度
		camera->ExposureTimeRaw.SetValue(12000);

		GenApi::CIntegerPtr m_pHeartbeatTimeout = camera->GetTLNodeMap().GetNode("HeartbeatTimeout");//
		m_pHeartbeatTimeout->SetValue(1000);

		cout << "Gain          : " << camera->GainRaw.GetValue() << endl;
		cout << "Exposure time : " << camera->ExposureTimeRaw.GetValue() << endl;

		camera->TriggerMode.SetValue(TriggerMode_On);
		camera->TimerTriggerActivation.SetValue(TimerTriggerActivation_RisingEdge);
		//camera->RegisterImageEventHandler(new CSampleImageEventHandler(), RegistrationMode_Append, Cleanup_Delete);

		//camera->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);//自己触发线程
		camera->StartGrabbing(GrabStrategy_LatestImageOnly);//需自己调用RetrieveResult抓图，取最新图片


		// Print the model name of the camera.
		//cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

		//// Open the camera.
		//camera.Open();

		//StopCrab();
		//mCamera.Attach(CTlFactory::GetInstance().CreateFirstDevice());
		//GenApi::CIntegerPtr m_pHeartbeatTimeout = mCamera.GetTLNodeMap().GetNode("HeartbeatTimeout");
		//m_pHeartbeatTimeout->SetValue(1000);

		//mCamera.RegisterImageEventHandler(new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);
		////硬出发，上升沿
		//mCamera.RegisterConfiguration(new CHardwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
		////mCamera.RegisterConfiguration(new CHardwareTriggerConfiguration2, RegistrationMode_ReplaceAll, Cleanup_Delete);
		//// Camera event processing must be activated first, the default is off.
		//mCamera.GrabCameraEvents = true;
		//mCamera.Open();
		///*mCamera.ExposureAuto.SetValue(ExposureAuto_Off);
		//mCamera.ExposureTimeRaw.SetValue(10000);*/
		//// Print the model name of the camera.
		//cout << "Using device " << mCamera.GetDeviceInfo().GetModelName() << endl;	
		//mCamera.StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByInstantCamera);//自己触发线程
		////mCamera.StartGrabbing(GrabStrategy_LatestImageOnly);//需自己调用RetrieveResult抓图，取最新图片
		IsInit = true;
	}
	catch (const GenericException &e)
	{
		// Error handling.
		cerr << "An exception occurred." << endl
			<< e.GetDescription() << endl;
	}
	return IsInit;
}
/*启动线程开始抓图*/
void BaslerCameraThread::StartGrab() {
	this->start();
}
void BaslerCameraThread::StopCrab() {
	if (isRunning()) {
		IsStopThread = true;
		wait();
	}
}
void BaslerCameraThread::CompleteCrabImg() {
	IsComplete = true;
}
void BaslerCameraThread::onOneGrab() {
	/*mCamera.StartGrabbing(1);*/
	//if (mCamera.IsGrabbing())
	//{
	//	CGrabResultPtr ptrGrabResult;
	//	while (!mCamera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_Return)) {
	//		std::cout << "超时" << std::endl;
	//		//this->msleep(100);
	//	}
	//	/*while (mCamera.GrabOne(5000, ptrGrabResult, TimeoutHandling_Return)) {
	//		std::cout << "超时" << std::endl;
	//		this->msleep(100);
	//	}*/
	//	std::cout << "一张图" << std::endl;
	//	// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
	//}
	//else {
	//	QMessageBox::warning(nullptr, "错误", "请初始化");
	//}
}
void BaslerCameraThread::run() {
	IsStopThread = false;
	IsComplete = false;
	qDebug() << "进入线程";
	while (!IsStopThread) {
		CGrabResultPtr ptrGrabResult;
		bool bl = camera->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_Return);
		if (!bl) {
			cout << "bl = " << bl << endl;
			continue;
		}
		if (ptrGrabResult->GrabSucceeded())
		{
			// Access the image data.
			cout << ":SizeX: " << ptrGrabResult->GetWidth() << endl;
			cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
			const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
			cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;

			QDir dir(mSaveDir);
			if (!dir.exists()) {
				dir.mkpath(mSaveDir);
			}
			QString str = QString("%1%2.png").arg(mSaveDir).arg(ImgCount++);
			QByteArray qba = str.toLocal8Bit();
			char* strData = qba.data();
			std::cout << "save path = " << strData << std::endl;
			CImagePersistence::Save(ImageFileFormat_Png, strData, ptrGrabResult);
			/*if (ImgCount % 2 != 0) {

				camera->ExposureTimeRaw.SetValue(2500);
			}
			else {
				camera->ExposureTimeRaw.SetValue(10000);
			}*/
			cout << "Gain          : " << camera->GainRaw.GetValue() << endl;
			cout << "Exposure time : " << camera->ExposureTimeRaw.GetValue() << endl;
			//CImagePersistence::Save(ImageFileFormat_Png, "GrabbedImage.png", ptrGrabResult);
			//#ifdef PYLON_WIN_BUILD
			//		// Display the grabbed image.
			//		Pylon::DisplayImage(1, ptrGrabResult);
			//#endif
		}
		else
		{
			cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
		}
	}
	//while (mCamera.IsGrabbing())
	//{
	//	if (IsStopThread) {
	//		break;
	//	}
	//	// Execute the software trigger. Wait up to 1000 ms for the camera to be ready for trigger.
	//	if (!mCamera.WaitForFrameTriggerReady(5000, TimeoutHandling_Return))
	//	{
	//		qDebug() << "超时" ;
	//		continue;
	//	}
	//	qDebug() << "获取一张图";
	//}
	//qDebug() << "退出线程" ;
	// This smart pointer will receive the grab result data.
	
	//CGrabResultPtr ptrGrabResult;
	//// Camera.StopGrabbing() is called automatically by the RetrieveResult() method
	//// when c_countOfImagesToGrab images have been retrieved.
	//int countsum = 0;
	//try {
	//	//mCamera.StartGrabbing(GrabStrategy_LatestImageOnly);//需自己调用RetrieveResult抓图，取最新图片
	//	bool bl = mCamera.IsGrabbing();
	//	qDebug() << "IsGrabbing = " << bl;
	//	while (mCamera.IsGrabbing())
	//	{
	//		if (IsStopThread) {
	//			//qDebug() << "退出BaslerCameraThread run"
	//			std::cout << "BaslerCameraThread run exit" << std::endl;
	//			break;
	//		}
	//		try {
	//			// Wait for an image and then retrieve it. A timeout of 5000 ms is used.
	//			if (mCamera.WaitForFrameTriggerReady(5000, TimeoutHandling_ThrowException))
	//			{
	//				qDebug() << "未超时";
	//				//mCamera.ExecuteSoftwareTrigger();
	//				//continue;
	//			}
	//		}
	//		catch (const GenericException &e) {
	//			qDebug() << "超时";
	//			break;
	//		}
	//		/*qDebug() << "未超时";
	//		mCamera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
	//		qDebug() << "抓图";	*/
	//		/*if (!mCamera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_Return)) {
	//			if (IsComplete) {
	//				qDebug() << "完成线扫";
	//				emit sigCompleteCrab();
	//				break;
	//			}
	//			else {
	//				qDebug() << "超时";
	//				continue;
	//			}
	//		}*/
	//		// Image grabbed successfully?
	//		//if (ptrGrabResult->GrabSucceeded())
	//		//{
	//		//	// Access the image data.
	//		//	cout << countsum++ << ":SizeX: " << ptrGrabResult->GetWidth() << endl;
	//		//	cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
	//		//	const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
	//		//	cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;
	//		//	//strPicName.Format(bmp_path.c_str(), ++m_iCount);
	//		//	//CImagePersistence::Save(ImageFileFormat_Jpeg, (String_t)strPicName, ptrGrabResult);
	//		//	// Display the grabbed image.
	//		//	//Pylon::DisplayImage(1, ptrGrabResult);
	//		//}
	//		//else
	//		//{
	//		//	cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
	//		//}
	//	}
		/*mCamera.StopGrabbing();
		mCamera.Close();*/
		qDebug() << "线程停止";
	/*}
	catch(const GenericException &e){
		cerr << "ERR Run An exception occurred." << endl
			<< e.GetDescription() << endl;
	}*/
}