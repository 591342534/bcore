#ifndef _NLOG_H
#define _NLOG_H
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <sstream>
#include "log.h"
#include <iomanip>
#include "octets.h"

// 如果需要更换分隔符，则重定义这两个宏.  或者在项目中临时替换
#ifndef NLOG_SEP
#define NLOG_SEP ":"
#endif // NLOG_SEP
#ifndef NLOG_EQ
#define NLOG_EQ "="
#endif // NLOG_SEP

// 最大日志内容长度，超过这长度会截断，注意要比消息实际能接受的内容稍短一些
#ifndef NLOG_MAX_LENGTH
#define NLOG_MAX_LENGTH 4000
#endif // NLOG_MAX_LENGTH

// 超长错误信息里，带上的原日志头部长度
#ifndef NLOG_TRUNCATE_INFO_LENGTH
#define NLOG_TRUNCATE_INFO_LENGTH 128
#endif // NLOG_TRUNCATE_INFO_LENGTH


/*
 * 使用方法
 *	struct T1{ int a; int b;};
 *	namespace GNET
 *	{
 *	// 定义类型需要重载AppendLogValue函数
 *	// 或者也可以重载流操作
 *	template<>
 *	void AppendLogValue(std::ostream& os, const T1& v)
 *	{
 *		os << v.a << "," << v.b;
 *	}
 *
 *	};
 *
 *	std::deque<T1> l;
 *	l.push_back({1,2});
 *	l.push_back({2,3});
 *	Octets userid("aaa@pp", 6);
 *	NLOG(ERR, "testlog") // 声明日志级别和关键字
 *		.P("userid", userid) // 输出Octets
 *		.P("roleid", 123LL) // 整数
 *		.P("fv", 23.4f) // 浮点数
 *		.P("items", l) // 自定义结构列表
 *		.P("ted", 1.22222, 2) // 指定小数点后的位数
 *		.X("hex", 33445) // 16进制整数
 *		.O("p1=v1:p2=v2:p3=v3"); // 原样输出
 *
 * 以上代码将输出:
 * 2016-07-28 16:22:07|host|process|testlog:userid=aaa@pp:roleid=123:fv=23.4:items=1,2;2,3:ted=1.22:hex=0x82a5:p1=v1:p2=v2:p3=v3
 *
 * A操作用于调用自定义对象的日志输出方法
 * 1. 先定义AppendLog方法
 * void gplayer_imp::AppendLog(GNET::LogStream& ls, const std::string& prefix) const
 * {
 * 		// XXX 注意这里的prefix要添加到attr的前面
 * 		ls.P(prefix + "roleid", GetParent()->ID.id)
 * 				.P(prefix + "rolelevel", static_cast<int>(GetLevel()));
 * }
 *
 * NLOG(FORMAT, "enterworld")
 * 		.A(*pPlayerImp);
 * 以上代码将输出
 * 2016-07-28 16:22:07|host|process|enterworld:roleid=111:rolelevel=90
 *
 * NLOG(FORMAT, "trade")
 * 		.A("seller", *pPlayerImp1);
 * 		.A("buyer", *pPlayerImp2);
 * 以上代码将输出
 * 2016-07-28 16:22:07|host|process|trade:sellerroleid=111:sellerrolelevel=90:buyersoleid=222:buyerrolelevel=100
 */

namespace GNET
{

template<typename T>
struct HasMember_tolog
{
	template<typename U, void(U::*)(std::ostream& os) const> struct SFINAE{};
	template<typename U> static char check(SFINAE<U,&U::tolog>*);
	template<typename U> static int check(...);
	static const bool value = sizeof(check<T>(0)) == sizeof(char);
};
template<bool B, class = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {typedef T type;};

//1 -- 如果T有 void tolog(std::ostream& os) const 函数， 则实例化下边调用tolog的函数
template<typename T>
inline typename enable_if<HasMember_tolog<T>::value>::type AppendLogValue(std::ostream & os, const T& v) 
{
	v.tolog(os); 
}
//2-- 如果没有，则实例化下边调用 operator <<的函数
template<typename T>
inline typename enable_if<!HasMember_tolog<T>::value>::type AppendLogValue(std::ostream & os, const T& v) 
{
	os << v; 
}
//下边的Octets,std::vector,std::map因为没有tolog,也没有<<,所以需要特化一下AppendLogValue
template<>
inline void AppendLogValue(std::ostream& os, const Octets& v)
{
	os.write(reinterpret_cast<const char*>(v.cbegin()), v.size());
}

template<typename T>
inline void AppendLogValue(std::ostream& os, const std::vector<T>& v) {
	typename std::vector<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it){
		if (it != v.begin()) os << ";";
		AppendLogValue(os, *it);
	}
}

template<class T>
inline void AppendLogValue(std::ostream& os, const std::list<T>& v) {
	typename std::list<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it){
		if (it != v.begin()) os << ";";
		AppendLogValue(os, *it);
	}
}

template<class T>
inline void AppendLogValue(std::ostream& os, const std::deque<T>& v) {
	typename std::deque<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it){
		if (it != v.begin()) os << ";";
		AppendLogValue(os, *it);
	}
}

template<class T>
inline void AppendLogValue(std::ostream& os, const std::queue<T>& v) {
	typename std::queue<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it){
		if (it != v.begin()) os << ";";
		AppendLogValue(os, *it);
	}
}

template<class T>
inline void AppendLogValue(std::ostream& os, const std::set<T>& v) {
	typename std::set<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it){
		if (it != v.begin()) os << ";";
		AppendLogValue(os, *it);
	}
}

// 用于对日志进行过滤，比如截取部分日志转为紫龙日志
struct LogFilter {
	virtual void OnLogValue(const std::string& attr, const std::string& value) = 0;
	virtual void OnLogFinished() = 0;
	virtual ~LogFilter() {}
};
typedef std::vector<LogFilter*> LogFilterList;

/* 符合标准格式的log流, 为后续的log输出做准备 */
class LogStream: public std::stringstream
{
public:
	//分隔符。用static函数，每个项目也可以选择运行时修改, io库里也会因此改变
	static std::string& SEP() { static std::string sep = NLOG_SEP; return sep;}
	static std::string& EQ() { static std::string eq = NLOG_EQ; return eq;}

	virtual ~LogStream() {}
	virtual const std::vector<LogFilter*>& Filters() const = 0;

	template<class T>
	LogStream& P(const char* attr, const T& v) {
		*this << SEP() << attr << EQ();

		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	template<class T>
	LogStream& P(const std::string& attr, const T& v) {
		*this << SEP() << attr << EQ();

		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	// 限制小数点倍数的输出，如果T是stl容器，会影响容器内所有元素的输出
	template<class T>
	LogStream& P(const char* attr, const T& v, int precision) {
		*this << SEP() << attr << EQ();

		std::ios_base::fmtflags saved_flags = flags();
		std::streamsize saved_precision = this->precision();

		*this << std::setiosflags(std::ios::fixed | std::ios::showpoint)
			<< std::setprecision(precision);
		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		*this << std::setprecision(saved_precision);
		flags(saved_flags);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	template<class T>
	LogStream& P(const std::string& attr, const T& v, int precision) {
		*this << SEP() << attr << EQ();

		std::ios_base::fmtflags saved_flags = flags();
		std::streamsize saved_precision = this->precision();

		*this << std::setiosflags(std::ios::fixed | std::ios::showpoint)
			<< std::setprecision(precision);
		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		*this << std::setprecision(saved_precision);
		flags(saved_flags);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	// 16进制输出，只用于整数类型
	template<class T>
	LogStream& X(const char* attr, const T& v) {
		*this << SEP() << attr << EQ() << "0x";

		std::ios_base::fmtflags saved_flags = flags();

		*this << std::hex;
		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		flags(saved_flags);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	template<class T>
	LogStream& X(const std::string& attr, const T& v) {
		*this << SEP() << attr << EQ() << "0x";

		std::ios_base::fmtflags saved_flags = flags();

		*this << std::hex;
		std::streamoff old_pos = tellp();
		AppendLogValue(*this, v);

		flags(saved_flags);

		std::string value = str().substr(old_pos, tellp() - old_pos);
		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogValue(attr, value);
		}
		return *this;
	}

	// 调用对象的AppendLog接口，方便对自定义对象记日志
	// prefix为前缀，在AppendLog的定义中必须添加到所有的attr前面
	template<class T>
	LogStream& A(const std::string& prefix, const T& obj) {
		obj.AppendLog(*this, prefix);
		return *this;
	}

	template<class T>
	LogStream& A(const T& obj) {
		return A("", obj);
	}

	// 原样输出到流，这是为了避免使用流操作，统一接口
	// 注意seg内部的分隔符等于符需要与NLOG_SEP和NLOG_EQ保持一致
	template<class T>
	LogStream& O(const T& seg) {
		AppendLogValue(*this, seg);
		return *this;
	}
};

class LogMessage;
struct LogFilterManager {
	virtual void OnMessageCreated(int log_prior, const char* title, LogMessage* log_msg) = 0;
};

class LogManager {
public:
	typedef std::vector<LogFilterManager*> FilterMgrMap;
	static LogManager& Instance() { static LogManager sInstance; return sInstance; }
	void AddFilterManager(LogFilterManager* filter_mgr) { _filter_mgrs.push_back(filter_mgr); }
	virtual void OnMessageCreated(int log_prior, const char* title, LogMessage* log_msg) {
		for (FilterMgrMap::iterator it = _filter_mgrs.begin();
				it != _filter_mgrs.end(); ++it) {
			(*it)->OnMessageCreated(log_prior, title, log_msg);
		}
	}

private:
	~LogManager() {}
	FilterMgrMap _filter_mgrs;
};

class LogMessage : public LogStream
{
	int _prior;
public:

	LogMessage(int log_prior, const char* title): _prior(log_prior) {
		*this << title;

		LogManager::Instance().OnMessageCreated(log_prior, title, this);
	}
	const LogFilterList& Filters() const { return _filters; }
	void AddFilter(LogFilter* filter) {
		_filters.push_back(filter);
	}
	virtual ~LogMessage()
	{
		std::string content = str();
		if (str().size() > NLOG_MAX_LENGTH)
		{
			// 超长截断，并输出一条错误日志
			content.resize(NLOG_MAX_LENGTH);
			Log::logvital(_prior, "%s", content.c_str());
			// 错误日志里带上本日志的前一定字节
			Log::logvital(LOG_ERR, "日志超长: %.*s",
					NLOG_TRUNCATE_INFO_LENGTH, content.c_str());
		}
		else
		{
			Log::logvital(_prior, "%s",content.c_str());
		}

		for (LogFilterList::const_iterator it = Filters().begin();
				it != Filters().end(); ++it) {
			(*it)->OnLogFinished();
		}
	}
protected:
	std::vector<LogFilter*> _filters;
};

} // end namespace GNET

#define NLOG_MESSAGE_ALERT(x) GNET::LogMessage(LOG_ALERT, (x))
#define NLOG_MESSAGE_CRIT(x) GNET::LogMessage(LOG_CRIT, (x))
#define NLOG_MESSAGE_ERR(x) GNET::LogMessage(LOG_ERR, (x))
#define NLOG_MESSAGE_WARNING(x) GNET::LogMessage(LOG_WARNING, (x))
#define NLOG_MESSAGE_FORMAT(x) GNET::LogMessage(LOG_NOTICE, (x))
#define NLOG_MESSAGE_TRACE(x) GNET::LogMessage(LOG_DEBUG, (x))
#define NLOG_MESSAGE_INFO(x) GNET::LogMessage(LOG_INFO, (x))

#define NLOG(t, x) NLOG_MESSAGE_##t(x)
#define DLOG(t, x) NLOG(t, (x)).P("file", __FILE__).P("line", __LINE__)

//下边的宏可以用于不想写变量名的地方，用法示例： 
//      NTRACE("rolelogin", roleid, userid, level);
// 或者 NTRACE("rolelogout", roleid);

#define NTRACE(title,...) NTRACE_(title,##__VA_ARGS__,8,7,6,5,4,3,2,1,0)
#define NTRACE_(title,x1,x2,x3,x4,x5,x6,x7,x8,count,...) NTRACE##count(title,x1,x2,x3,x4,x5,x6,x7,x8)

#define NTRACE0(title,x1,x2,x3,x4,x5,x6,x7,x8) NLOG_MESSAGE_TRACE(title).P("file",__FILE__).P("line", __LINE__)
#define NTRACE1(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE0(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x1, x1)
#define NTRACE2(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE1(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x2,x2)
#define NTRACE3(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE2(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x3,x3)
#define NTRACE4(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE3(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x4,x4)
#define NTRACE5(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE4(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x5,x5)
#define NTRACE6(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE5(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x6,x6)
#define NTRACE7(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE6(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x7,x7)
#define NTRACE8(title,x1,x2,x3,x4,x5,x6,x7,x8) NTRACE7(title,x1,x2,x3,x4,x5,x6,x7,x8).P(#x8,x8)

#endif // _NLOG_H
