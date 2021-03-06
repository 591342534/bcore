#pragma once
#include <mutex>
#include <memory>

namespace bcore {
	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		NonCopyable& operator = (const NonCopyable&) = delete;
	};

	class NonMovable
	{
	public:
		NonMovable() = default;
		NonMovable(NonMovable&&) = delete;
		NonMovable& operator = (NonMovable&&) = delete;
	};
	class IInit {
	public:
		virtual void Init() {}
	};

	template <class T>
	class Singleton : public NonCopyable, public NonMovable {
	public:
		static T& Instance() {
			static T instance;
			return instance;
		}
	protected:
		Singleton() { };
		Singleton(const Singleton&) = delete;
		Singleton& operator= (const Singleton&) = delete;
	};
}
