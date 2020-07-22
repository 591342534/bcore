#pragma once
#include <mutex>
#include <memory>

namespace bcore {
	template <class T>
	class Singleton {
	public:
		static T& Instance() {
			static T instance;
			static std::once_flag flag;
			std::call_once(flag, [&]() {
				instance.Init();
			});
			return instance;
		}
	protected:
		virtual void Init() {}
	
		Singleton() {};
		Singleton(const Singleton&) = delete;
		Singleton& operator= (const Singleton&) = delete;
	};
}
