#include "pch.h"
#include "HopeThreadPool.h"

// �⺻ ������ ( PC�� �����μ��� ������ŭ ���� )
CHopeThreadPool::CHopeThreadPool()
{
	m_bStopAll = FALSE;

	// PC�� �����μ��� ������ŭ ��������.
	UINT unPcProcessCount = std::jthread::hardware_concurrency();
	m_vWorkerThreads.reserve(unPcProcessCount);

	for (size_t i = 0; i < unPcProcessCount; ++i)
	{
		m_vWorkerThreads.emplace_back([this]() { this->WorkerThread(); });
	}
}
// ���ϴ� ���� ��ŭ�� ������ ����
CHopeThreadPool::CHopeThreadPool(size_t unNumThreads)
	: m_unThreadNum(unNumThreads), m_bStopAll(FALSE)
{
	// Ŭ���� �����Ҷ� �۾� �����带 ��� ��������
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

		// �� ���� job �� ����.
		std::function<void()> job = move(m_qJobQueue.front());
		m_qJobQueue.pop();
		lock.unlock();

		// �ش� job �� �����Ѵ� :)
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
		throw std::runtime_error("ThreadPool ��� ������");
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_Job_Mutex);
		m_qJobQueue.push(std::move(job));
	}
	m_CvJobQ.notify_one();
}
