#ifndef MCLIB_UTIL_OBSERVER_SUBJECT_H_
#define MCLIB_UTIL_OBSERVER_SUBJECT_H_

template <typename T>
class ObserverSubject
{
public:
	void RegisterListener(T* listener)
	{
		m_Listeners.push_back(listener);
	}

	void UnregisterListener(T* listener)
	{
		auto iter = std::find(m_Listeners.begin(), m_Listeners.end(), listener);

		m_Listeners.erase(iter);
	}

	template <typename Func, typename... Args>
	void NotifyListeners(Func f, Args... args)
	{
		for (T* listener : m_Listeners)
			std::bind(f, listener, args...)();
	}

protected:
	std::vector<T*> m_Listeners;
};

#endif
