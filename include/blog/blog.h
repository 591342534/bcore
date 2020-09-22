#ifndef BCORE_BLOG_BLOG_H_
#define BCORE_BLOG_BLOG_H_

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <sstream>
#include <iomanip>
#include "log_file.h"

// �����Ҫ�����ָ��������ض�����������.  ��������Ŀ����ʱ�滻
#ifndef NLOG_SEP
#define NLOG_SEP "|"
#endif // NLOG_SEP
#ifndef NLOG_EQ
#define NLOG_EQ "="
#endif // NLOG_SEP

// �����־���ݳ��ȣ������ⳤ�Ȼ�ضϣ�ע��Ҫ����Ϣʵ���ܽ��ܵ������Զ�һЩ
#ifndef NLOG_MAX_LENGTH
#define NLOG_MAX_LENGTH 4000
#endif // NLOG_MAX_LENGTH

// ����������Ϣ����ϵ�ԭ��־ͷ������
#ifndef NLOG_TRUNCATE_INFO_LENGTH
#define NLOG_TRUNCATE_INFO_LENGTH 128
#endif // NLOG_TRUNCATE_INFO_LENGTH


/*
 * ʹ�÷���
 *	struct T1{ int a; int b;};
 *	namespace GNET
 *	{
 *	// ����������Ҫ����AppendLogValue����
 *	// ����Ҳ��������������
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
 *	NLOG(ERR, "testlog") // ������־����͹ؼ���
 *		.P("userid", userid) // ���Octets
 *		.P("roleid", 123LL) // ����
 *		.P("fv", 23.4f) // ������
 *		.P("items", l) // �Զ���ṹ�б�
 *		.P("ted", 1.22222, 2) // ָ��С������λ��
 *		.X("hex", 33445) // 16��������
 *		.O("p1=v1:p2=v2:p3=v3"); // ԭ�����
 *
 * ���ϴ��뽫���:
 * 2016-07-28 16:22:07|host|process|testlog:userid=aaa@pp:roleid=123:fv=23.4:items=1,2;2,3:ted=1.22:hex=0x82a5:p1=v1:p2=v2:p3=v3
 *
 * A�������ڵ����Զ���������־�������
 * 1. �ȶ���AppendLog����
 * void gplayer_imp::AppendLog(GNET::LogStream& ls, const std::string& prefix) const
 * {
 * 		// XXX ע�������prefixҪ��ӵ�attr��ǰ��
 * 		ls.P(prefix + "roleid", GetParent()->ID.id)
 * 				.P(prefix + "rolelevel", static_cast<int>(GetLevel()));
 * }
 *
 * NLOG(FORMAT, "enterworld")
 * 		.A(*pPlayerImp);
 * ���ϴ��뽫���
 * 2016-07-28 16:22:07|host|process|enterworld:roleid=111:rolelevel=90
 *
 * NLOG(FORMAT, "trade")
 * 		.A("seller", *pPlayerImp1);
 * 		.A("buyer", *pPlayerImp2);
 * ���ϴ��뽫���
 * 2016-07-28 16:22:07|host|process|trade:sellerroleid=111:sellerrolelevel=90:buyersoleid=222:buyerrolelevel=100
 */

namespace bcore
{
	template<typename T>
	struct HasMember_tolog
	{
		template<typename U, void(U::*)(std::ostream& os) const> struct SFINAE {};
		template<typename U> static char check(SFINAE<U, &U::tolog>*);
		template<typename U> static int check(...);
		static const bool value = sizeof(check<T>(0)) == sizeof(char);
	};
	template<bool B, class = void>
	struct enable_if {};

	template<class T>
	struct enable_if<true, T> { typedef T type; };

	//1 -- ���T�� void tolog(std::ostream& os) const ������ ��ʵ�����±ߵ���tolog�ĺ���
	template<typename T>
	inline typename enable_if<HasMember_tolog<T>::value>::type AppendLogValue(std::ostream& os, const T& v)
	{
		v.tolog(os);
	}
	//2-- ���û�У���ʵ�����±ߵ��� operator <<�ĺ���
	template<typename T>
	inline typename enable_if<!HasMember_tolog<T>::value>::type AppendLogValue(std::ostream& os, const T& v)
	{
		os << v;
	}

	template<typename T>
	inline void AppendLogValue(std::ostream& os, const std::vector<T>& v) {
		os << "[";
		typename std::vector<T>::const_iterator it;
		for (it = v.begin(); it != v.end(); ++it) {
			if (it != v.begin()) os << ",";
			AppendLogValue(os, *it);
		}
		os << "]";
	}

	template<class T>
	inline void AppendLogValue(std::ostream& os, const std::list<T>& v) {
		os << "[";
		typename std::list<T>::const_iterator it;
		for (it = v.begin(); it != v.end(); ++it) {
			if (it != v.begin()) os << ",";
			AppendLogValue(os, *it);
		}
		os << "[";
	}

	template<class T>
	inline void AppendLogValue(std::ostream& os, const std::deque<T>& v) {
		os << "[";
		typename std::deque<T>::const_iterator it;
		for (it = v.begin(); it != v.end(); ++it) {
			if (it != v.begin()) os << ",";
			AppendLogValue(os, *it);
		}
		os << "]";
	}

	template<class T>
	inline void AppendLogValue(std::ostream& os, const std::queue<T>& v) {
		typename std::queue<T>::const_iterator it;
		for (it = v.begin(); it != v.end(); ++it){
			if (it != v.begin()) os << ";";
			AppendLogValue(os, *it);
		}
	}

	//template<class T>
	//inline void AppendLogValue(std::ostream& os, const T& v) {
	//	for (auto it = v.begin(); it != v.end(); ++it) {
	//		if (it != v.begin()) os << ";";
	//		AppendLogValue(os, *it);
	//	}
	//}

	/* ���ϱ�׼��ʽ��log��, Ϊ������log�����׼�� */
	class LogStream : public std::stringstream
	{
	public:
		//�ָ�������static������ÿ����ĿҲ����ѡ������ʱ�޸�, io����Ҳ����˸ı�
		static std::string& SEP() { static std::string sep = NLOG_SEP; return sep; }
		static std::string& EQ() { static std::string eq = NLOG_EQ; return eq; }

		virtual ~LogStream() {}

		template<class T>
		LogStream& P(const char* attr, const T& v) {
			*this << SEP() << attr << EQ();
			AppendLogValue(*this, v);
			return *this;
		}

		template<class T>
		LogStream& P(const std::string& attr, const T& v) {
			*this << SEP() << attr << EQ();
			AppendLogValue(*this, v);
			return *this;
		}

		// ����С���㱶������������T��stl��������Ӱ������������Ԫ�ص����
		template<class T>
		LogStream& P(const char* attr, const T& v, int precision) {
			*this << SEP() << attr << EQ();

			std::ios_base::fmtflags saved_flags = flags();
			std::streamsize saved_precision = this->precision();

			*this << std::setiosflags(std::ios::fixed | std::ios::showpoint)
				<< std::setprecision(precision);
			AppendLogValue(*this, v);

			*this << std::setprecision(saved_precision);
			flags(saved_flags);
			return *this;
		}

		template<class T>
		LogStream& P(const std::string& attr, const T& v, int precision) {
			*this << SEP() << attr << EQ();

			std::ios_base::fmtflags saved_flags = flags();
			std::streamsize saved_precision = this->precision();

			*this << std::setiosflags(std::ios::fixed | std::ios::showpoint)
				<< std::setprecision(precision);
			AppendLogValue(*this, v);

			*this << std::setprecision(saved_precision);
			flags(saved_flags);
			return *this;
		}

		// 16���������ֻ������������
		template<class T>
		LogStream& X(const char* attr, const T& v) {
			*this << SEP() << attr << EQ() << "0x";

			std::ios_base::fmtflags saved_flags = flags();

			*this << std::hex;
			AppendLogValue(*this, v);

			flags(saved_flags);
			return *this;
		}

		template<class T>
		LogStream& X(const std::string& attr, const T& v) {
			*this << SEP() << attr << EQ() << "0x";

			std::ios_base::fmtflags saved_flags = flags();
			*this << std::hex;
			AppendLogValue(*this, v);
			flags(saved_flags);
			return *this;
		}

		// ���ö����AppendLog�ӿڣ�������Զ���������־
		// prefixΪǰ׺����AppendLog�Ķ����б�����ӵ����е�attrǰ��
		template<class T>
		LogStream& A(const std::string& prefix, const T& obj) {
			obj.AppendLog(*this, prefix);
			return *this;
		}

		template<class T>
		LogStream& A(const T& obj) {
			return A("", obj);
		}

		// ԭ���������������Ϊ�˱���ʹ����������ͳһ�ӿ�
		// ע��seg�ڲ��ķָ������ڷ���Ҫ��NLOG_SEP��NLOG_EQ����һ��
		template<class T>
		LogStream& O(const T& seg) {
			AppendLogValue(*this, seg);
			return *this;
		}
	};
	
	class LogMessage : public LogStream
	{
		int _prior;
	public:

		LogMessage(int log_prior, const char* title) : _prior(log_prior) {
			*this << title;
		}
		LogMessage(int log_prior, const char* title, const char* file, int line) : _prior(log_prior) {
			*this << LastFile(file) << ":" << line << NLOG_SEP << title;
		}
		virtual ~LogMessage()
		{
			std::string content = str();
			if (str().size() > NLOG_MAX_LENGTH)
			{
				// �����ضϣ������һ��������־
				content.resize(NLOG_MAX_LENGTH);
				std::cout << content << std::endl;
				//Log::logvital(_prior, "%s", content.c_str());
				//// ������־����ϱ���־��ǰһ���ֽ�
				//Log::logvital(LOG_ERR, "��־����: %.*s",
				//	NLOG_TRUNCATE_INFO_LENGTH, content.c_str());
			}
			else
			{
				//Log::logvital(_prior, "%s", content.c_str());

				std::cout << content << std::endl;
				LogFileManager::WriteLog((LOG_PRIOR)_prior, content);
			}
		}
	private:
		const char* LastFile(const char* file) {
			char* ptr = (char*)file + strlen(file);
			while (ptr != file && *(ptr - 1) != '\\') {
				--ptr;
			}
			return ptr;
		}
	};

} // end namespace bcore

#define BLOG(t, x) bcore::LogMessage(t, (x), __FILE__, __LINE__)

#define EXPAND(...) __VA_ARGS__

//debug
#define BLOG_DEBUG(title,...) BLOG_DEBUG_(title, ##__VA_ARGS__,8,7,6,5,4,3,2,1,0)
#define BLOG_DEBUG_(...) EXPAND(BLOG_DEBUG_LATER(__VA_ARGS__))
#define BLOG_DEBUG_LATER(title,x1,x2,x3,x4,x5,x6,x7,x8,count,...) BLOG_DEBUG_##count(title,x1,x2,x3,x4,x5,x6,x7,x8)
#define BLOG_DEBUG_0(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG(LOG_DEBUG, title)
#define BLOG_DEBUG_2(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_DEBUG_0(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x1,x2)
#define BLOG_DEBUG_4(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_DEBUG_2(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x3,x4)
#define BLOG_DEBUG_6(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_DEBUG_4(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x5,x6)
#define BLOG_DEBUG_8(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_DEBUG_6(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x7,x8)
//trace
#define BLOG_TRACE(title,...) BLOG_TRACE_(title, ##__VA_ARGS__,8,7,6,5,4,3,2,1,0)
#define BLOG_TRACE_(...) EXPAND(BLOG_TRACE_LATER(__VA_ARGS__))
#define BLOG_TRACE_LATER(title,x1,x2,x3,x4,x5,x6,x7,x8,count,...) BLOG_TRACE_##count(title,x1,x2,x3,x4,x5,x6,x7,x8)
#define BLOG_TRACE_0(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG(LOG_TRACE, title)
#define BLOG_TRACE_2(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_TRACE_0(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x1,x2)
#define BLOG_TRACE_4(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_TRACE_2(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x3,x4)
#define BLOG_TRACE_6(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_TRACE_4(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x5,x6)
#define BLOG_TRACE_8(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_TRACE_6(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x7,x8)
//format
#define BLOG_FORMAT(title,...) BLOG_FORMAT_(title, ##__VA_ARGS__,8,7,6,5,4,3,2,1,0)
#define BLOG_FORMAT_(...) EXPAND(BLOG_FORMAT_LATER(__VA_ARGS__))
#define BLOG_FORMAT_LATER(title,x1,x2,x3,x4,x5,x6,x7,x8,count,...) BLOG_FORMAT_##count(title,x1,x2,x3,x4,x5,x6,x7,x8)
#define BLOG_FORMAT_0(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG(LOG_FORMAT, title)
#define BLOG_FORMAT_2(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_FORMAT_0(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x1,x2)
#define BLOG_FORMAT_4(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_FORMAT_2(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x3,x4)
#define BLOG_FORMAT_6(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_FORMAT_4(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x5,x6)
#define BLOG_FORMAT_8(title,x1,x2,x3,x4,x5,x6,x7,x8) BLOG_FORMAT_6(title,x1,x2,x3,x4,x5,x6,x7,x8).P(x7,x8)

#endif // _NLOG_H

