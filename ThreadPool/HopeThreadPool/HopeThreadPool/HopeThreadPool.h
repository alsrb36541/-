#pragma once

/************************************************************************************\
*  File Name	: HopeThreadPool.h
*
*  Description	: 
*
*  Notes		: ������Ǯ 
				: size_t ������ŭ �����带 ����, EnqueueJob���� ���ϴ� �Լ��� �־��ָ� �������� �����忡�� �ش� �Լ��� ó���� ���� �ݳ��Ѵ�.
				: ���� URL : https://modoocode.com/285

*  Version		: v 1.0.1
*
*  Dependency	: std::queue, std::vector, std::thread, std::mutex
*
*  Change History:
*
*  Date		| Name				| Changes
*  ---------+-------------------+-----------------------------------------------------
*  22/01/05	| YMG				| ���� ���� ( VS2022 , 32, 64 BIT �����ڵ� �������� )
*  ---------+-------------------+-----------------------------------------------------
************************************************************************************/

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class AFX_EXT_CLASS CHopeThreadPool
{
private:
	typedef std::function<void()> fnWorkFunc;

public:
	CHopeThreadPool();								// �⺻ ������ ( �ش� PC�� �����μ��� ������ŭ �ڵ� ������ ���� )
	CHopeThreadPool(size_t unNumThreads);
	virtual ~CHopeThreadPool();

	//--------- �ܺ� ��� ------------------------------------//
public:
	void EnqueueJob(fnWorkFunc job);				// job �� �߰��Ѵ�.

	//--------- ���� ��� ------------------------------------//
private:
	size_t m_unThreadNum;								// �� Worker �������� ����.
	std::vector<std::jthread> m_vWorkerThreads;			// Worker �����带 �����ϴ� ����.
	std::queue<fnWorkFunc> m_qJobQueue;					// ���ϵ��� �����ϴ� job ť.
	std::condition_variable m_CvJobQ;					// ���� job ť�� ���� cv �� m.
	std::mutex m_Job_Mutex;								// ��ť�� �����ִ� ������ ����ó���� ���� ���ؽ� ��ü

	BOOL m_bStopAll;									// ���� job ť�� ���� cv �� m.

	void WorkerThread();								// Worker ������
};


