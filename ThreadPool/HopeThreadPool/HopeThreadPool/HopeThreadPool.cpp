#include "pch.h"
#include "HopeThreadPool.h"

// 기본 생성자 ( PC의 논리프로세서 개수만큼 생성 )
CHopeThreadPool::CHopeThreadPool()
{
	m_bStopAll = FALSE;

	// PC의 논리프로세서 개수만큼 생성하자.
	UINT unPcProcessCount = std::jthread::hardware_concurrency();
	m_vWorkerThreads.reserve(unPcProcessCount);

	for (size_t i = 0; i < unPcProcessCount; ++i)
	{
		m_vWorkerThreads.emplace_back([this]() { this->WorkerThread(); });
	}
}
// 원하는 개수 만큼의 스레드 생성
CHopeThreadPool::CHopeThreadPool(size_t unNumThreads)
	: m_unThreadNum(unNumThreads), m_bStopAll(FALSE)
{
	// 클래스 생성할때 작업 스레드를 몇개를 소유할지
	m_vWorkerThreads.reserve(m_unThreadNum);

	for (size_t i = 0; i < m_unThreadNum; ++i)
	{
		m_vWorkerThreads.emplace_back([this]() { this->WorkerThread(); });
	}
}


CHopeThreadPool::~CHopeThreadPool()
{
	m_bStopAll = true;
	m_CvJobQ.notify_all();

	for (auto& t : m_vWorkerThreads)
	{
		t.join();
	}
}

void CHopeThreadPool::WorkerThread()
{
	while (TRUE)
	{
		std::unique_lock<std::mutex> lock(m_Job_Mutex);
		m_CvJobQ.wait(lock, [this]() { return !this->m_qJobQueue.empty() || m_bStopAll; });
		if (m_bStopAll && this->m_qJobQueue.empty())
		{
			return;
		}

		// 맨 앞의 job 을 뺀다.
		std::function<void()> job = move(m_qJobQueue.front());
		m_qJobQueue.pop();
		lock.unlock();

		// 해당 job 을 수행한다 :)
		if (job != nullptr)
		{
			job();
		}
	}
}

void CHopeThreadPool::EnqueueJob(std::function<void()> job)
{
	if (m_bStopAll)
	{
		throw std::runtime_error("ThreadPool 사용 중지됨");
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_Job_Mutex);
		m_qJobQueue.push(std::move(job));
	}
	m_CvJobQ.notify_one();
}
