#pragma once

/************************************************************************************\
*  File Name	: HopeThreadPool.h
*
*  Description	: 
*
*  Notes		: 스레드풀 
				: size_t 개수만큼 스레드를 생성, EnqueueJob으로 원하는 함수를 넣어주면 만들어놓은 스레드에서 해당 함수를 처리한 이후 반납한다.
				: 참조 URL : https://modoocode.com/285

*  Version		: v 1.0.1
*
*  Dependency	: std::queue, std::vector, std::thread, std::mutex
*
*  Change History:
*
*  Date		| Name				| Changes
*  ---------+-------------------+-----------------------------------------------------
*  22/01/05	| YMG				| 최초 생성 ( VS2022 , 32, 64 BIT 유니코드 문자집합 )
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
	CHopeThreadPool();								// 기본 생성자 ( 해당 PC의 논리프로세서 개수만큼 자동 스레드 생성 )
	CHopeThreadPool(size_t unNumThreads);
	virtual ~CHopeThreadPool();

	//--------- 외부 사용 ------------------------------------//
public:
	void EnqueueJob(fnWorkFunc job);				// job 을 추가한다.

	//--------- 내부 사용 ------------------------------------//
private:
	size_t m_unThreadNum;								// 총 Worker 쓰레드의 개수.
	std::vector<std::jthread> m_vWorkerThreads;			// Worker 쓰레드를 보관하는 벡터.
	std::queue<fnWorkFunc> m_qJobQueue;					// 할일들을 보관하는 job 큐.
	std::condition_variable m_CvJobQ;					// 위의 job 큐를 위한 cv 와 m.
	std::mutex m_Job_Mutex;								// 잡큐에 들어와있는 스레드 병렬처리를 위한 뮤텍스 객체

	BOOL m_bStopAll;									// 위의 job 큐를 위한 cv 와 m.

	void WorkerThread();								// Worker 쓰레드
};


