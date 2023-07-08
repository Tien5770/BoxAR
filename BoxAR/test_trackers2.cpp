#pragma once
#include"cmdstd.h"

#include <iostream>
_STATIC_BEG

void test_ardetectors()
{
	app()->setTempDir("D:/projects/boxar");

	ff::setCurrentDirectory("D:/projects/boxar/BoxAR/");
	/*********************************ģ������************************************
	* trackedModelFile����Ƶ�б����������objģ���ļ�·��
	* Ҫ����һ����ʵ�����пɹ�׷�ٵ�ʵ��Ͷ�Ӧ����άģ���ļ�
	* ���ڸ����㷨�ǻ��������ģ�����ģ�ͱ���Ӧ�þ��г��������Ե�����
	* 
	* videoFile�ǰ������������Ƶ�ļ�·�������ѡ��ʹ������ͷʵʱ���㣬��˱�������ע�͵���ɾ��
	*/
	std::string trackedModelFile = "D:\\projects\\boxar\\BoxAR\\scan\\vase\\obj\\qinghuaci.obj", videoFile = R"(.\scan\vase\test2.mp4)";
	//std::string trackedModelFile = R"(.\tests\model\test1\mesh.obj)", videoFile = R"(.\tests\video\test1_video\test_1_5_chest.mp4)";
	/*
	* ModelSet��Ҫ�����ٵ�ģ����Ϣ�ļ���
	* ��������´˴������޸�
	*/
	//config model-set
	ModelSet models;
	{
		std::vector<ModelInfos> modelInfos  = modelInfosFromSingleFile(trackedModelFile, "re3d");
		models.set(modelInfos);
	}
	/*
	* virtualModelPath����Ҫ��������ʵ��Ƶ�ϵ���άģ���ļ�·����virtualModel�Ƕ�ȡ���ļ���õ���ģ����
	* ����ֻ��Ҫģ���ļ����ɣ���Ƶ��������ִ�ģ�Ͷ�Ӧ����ʵ����
	*/
	string virtualModelPath = "D:\\projects\\boxar\\BoxAR\\scan\\flower\\test.obj";
	CVRModel virtualModel(virtualModelPath);
	/*
	* ���²������������������г�ʼ��
	*/
	auto detector = FrameProc::create("v1.Tracker");
	//init detector
	ff::CommandArgSet args;
	//args.setArgs("-d2dModelFile f:/sdk/torch_models/model_re3d6_v1.ts -d2dScoreT 0.5");
	detector->init(&models, &args);
	
	/*********************************VideoCapture����*************************************
	* VideoCapture��OpenCV�ṩ��������������ͷ���ȡ��Ƶ�ļ�����
	* ���ʹ��Ԥ������õ�������Ƶ��Ӧ��ʹ��cap.open(videoFile);����ʽ
	* ���Ҫʹ������ͷ����ʵʱ��Ƶ��Ӧ��ʹ��cap.open(0);����ʽ��
	*	���в���Ϊ����ͷ��ţ���������Դ�����ͷ����������������ͷ��Ӧ��ͨ��cap.open(0);��cap.open(1);ȷ������ͷ�ͱ�ŵĶ�Ӧ��ϵ
	* p.s. ���ʹ������ͷ����ʵʱ��Ƶ��һ��Ҫ��һ���˹���ʼ���Ĳ�����Ӧ�ý���ʵ�����е��������̬����Ļ����ʾ�Ľ�����ֻ���һ�£�֮�������ȷ����
	*/
	cv::VideoCapture cap;
	//cap.open(0);
	cap.open(videoFile);
	FrameData fd;
	/*
	* dK[]Ϊ������ڲξ�������Ѿ��������궨�Ĳ�����ȷ���궨��ȷ
	* Ӧ�ý�yml�ļ��е�camera_matrix��data�������ݸ��Ƶ�����
	*/
	float dK[] = {
		1.324595302424838110e+03, 0.000000000000000000e+00, 6.460060955956646467e+02,
		0.000000000000000000e+00, 1.330463970754883576e+03, 3.568279021773695945e+02,
		0.000000000000000000e+00, 0.000000000000000000e+00, 1.000000000000000000e+00
	};
	int fi = 0;
	Mat img;
	/*
	* VideoWriter��OpenCV���ڽ�ͼ��תдΪ��Ƶ����
	*	����1��ָ�������Ƶ��
	*	����2��ָ����Ƶ�����ʽ����������������޸�
	*	����3��ָ����Ƶ֡����fps�������Ǳ�����������Ƶ��ͬ��Ҳ����ָ������30.0
	*	����4�������Ƶ�ķֱ��ʣ�����ע��Ӧ����Ҫ����Ϊ��Ƶ������ͼƬ��С��ͬ
	*/
	int fourcc = CV_FOURCC('M', 'P', '4', '2');
	//VideoWriter writer3("output.avi", fourcc, cap.get(CAP_PROP_FPS), Size(1280,720), true);

	/*********************************�任����������************************************
	* CVRMats��CVF�ṩ�ı任���󼯺��࣬����ģ�;���ͶӰ������ͼ����
	* �������־�������ã�https://zhuanlan.zhihu.com/p/386204250
	*/
	CVRMats mats;
	
	while(cap.read(img))
	{
		//processInput(window);
		//ͼ��Ԥ����
		//if (!cap.read(img))
		//	break;
		
		if (img.rows > 1000)
		{
			img = imscale(img, 0.5);
			flip(img, img, 0);
			flip(img, img, 1);
		}

		if (fi == 0) {
			//fd.cameraK = cvrm::defaultK(img.size(), 1.5);
			fd.cameraK = cv::Matx33f(dK);
			//����openGL��ͶӰ����
			mats.mProjection = cvrm::fromK(fd.cameraK, img.size(), 0.1, 3000);
		}
		//���ٻ�ȡλ��
		detector->pro(img, fd);
		//glClear(GL_DEPTH_BUFFER_BIT);
		/*
		* CVF�ṩ��ģ�ͼ����࣬���������Ҫ��Ⱦ��ģ�ͼ�����
		* ��������ģ�ͼ������Ⱦ����
		* ��������AR�����У����µ�����������Ҫ��Ⱦ����Ӧ��ͨ�����Ƶ���ʽ
		* ������Ҫ��Ⱦ���������岢����ü���
		*/
		CVRModelArray modelArray;
		modelArray.push_back(virtualModel);
		//�˴�ֻ��һ�����壬�����ѭ�������ڶ�������٣���ʱ�������޸��������
		for (int i = 0; i < (int)fd.objs.size(); i++)
		{
			re3d::ImageObject obj = fd.objs[i];
			//pose��ȡ���Ǹ��ٵõ�������λ��
			auto pose = obj.pose.get<std::vector<RigidPose>>().front();
			/*
			* �������н�����������������Ⱦ����
			* ��ʽ��Թ̶�����ʱ����Ҫ����
			*/
			auto modelPtr = models.getModel(fd.objs[i].modelIndex);
			modelArray.push_back(modelPtr->get3DModel());
			/*
			* CVRender��CVF�ṩ����Ⱦ�����࣬�����������ʽ�̶�������һ������Ⱦ������Ϊ�������
			* setBgImageָ������ͼ��һ����Ⱦ������Ӧ���Ƕ�ȡ������Ƶ��ÿһ֡
			*/
			CVRender render(modelArray);
			render.setBgImage(img);
			/* 
			* mats.mView����OpenGL��ʽ����ͼ����
			* ����OpenCV�е���תƽ�Ʋ�����OpenGL�ı任�����ʽ��ͬ
			* ��Ҫcvrm::fromR33T����ת��������Ӧ�������޸�
			*/
			mats.mView = cvrm::fromR33T(pose.R, pose.t);
			/*
			* ����CVRender��exec����ִ����Ⱦ��
			*	����1���任���󼯺��࣬����OpenGL��ʽ����ת����ͼ��ͶӰ����
			*	����2����Ⱦ�õ���ͼ���С������������ͼ�񱣳�һ��
			*	����3��ָ����Ⱦ�������ʽ��CVRM_IMAGE��ʾRGBͼ��CVRM_DEPTH��ʾ���ͼ
			*	����4��5����������ʽ����
			*/
			auto rr = render.exec(mats, img.size(), CVRM_IMAGE | CVRM_DEPTH, CVRM_DEFAULT, nullptr);
			// rr.img��ȡ��Ⱦ��RGBͼ����ʾ
			cv::imshow("test", rr.img);
			cv::waitKey(1);
			/*
			* ����Ҫ�������AR��������Ҫ�����ǣ�
			* 1.��˼һ������ĳ����������������������Ҫ���ӵ�����ģ��
			* 2.����и��ӵı��������嵫û�ж�Ӧ����άģ���ļ������������̰�æɨ��ģ��
			*	p.s. ģ��һ���Ǳ����������Ե�
			* 3.������������ĵ������ڱ���������Ļ����ϣ������ҪԤ�����ú���������ͱ�����������ͬһ����ϵ�µ���Թ�ϵ
			*/
		}
		++fi;
	}
}
	
CMD_BEG()
CMD0("test_ardetectors", test_ardetectors)
CMD_END()

_STATIC_END
