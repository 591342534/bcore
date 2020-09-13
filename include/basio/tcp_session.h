#include "asio.hpp"
#include <array>
#include <queue>
#include <atomic>
#include "thread_pool.h"
#include "bcore/buffer_pool.h"
#include "bnet/socket_buffer.h"
#include "bnet/msghandler/message_handler.h"
#include "bnet/tcp_option.h"
#include "bnet/socket_buffer.h"

using asio::ip::tcp;
namespace bcore_basio {
	class TcpSession : public std::enable_shared_from_this<TcpSession>, public bnet::ISession
	{
	public:
		TcpSession(std::shared_ptr<ThreadPoolContext> threadCtx, std::shared_ptr<bnet::SessionOption> option) :
			thread_context_(threadCtx),
			socket_(*(thread_context_->ctx)),
			option_(std::move(option)),
			message_handler_(option_->handler)
		{
			read_buffer_ = std::make_unique<bnet::SocketBuffer>(option_->read_buffer_size);
			read_buffer_bak_ = std::make_unique<bnet::SocketBuffer>(option_->read_buffer_size);
		}

		tcp::socket& Socket() { return socket_; }
		bool StartUp(bnet::ErrorCode& err) {
			std::error_code std_err;
			//�涨�׽������봦�����С�ֽ���
			//socket_.set_option(asio::socket_base::receive_low_watermark(100));
			//�涨�׽������ݷ��͵���С�ֽ���
			//socket_.set_option(asio::socket_base::send_low_watermark(100));
			//���Ϊtrue���׽����ܰ󶨵�һ�����õĵ�ַ�����ڶ�TCP�׽��ִ���TIME_WAIT״̬�µ�socket���ſ����ظ���ʹ�ã�
			//��ʽ����������������
			//socket_.set_option(asio::socket_base::reuse_address(true));
			//socket_.set_option(asio::socket_base::keep_alive(100), std_err);
			//CHECK_TCP_SESSION_STARTUP;
			//���Ϊtrue���׽�������δ�������ݵ�ʱ�����close
			//socket_.set_option(asio::socket_base::linger(true, 30), std_err);
			//CHECK_TCP_SESSION_STARTUP;
#define CHECK_TCP_SESSION_STARTUP \
			if (std_err) { \
				err = std::move(bnet::ErrorCode(bnet::ERROR_CODE::kSessionOptionError, std_err.message()));\
				return false;\
			}

			//�׽��ֽ��ܻ�������С
			socket_.set_option(asio::socket_base::receive_buffer_size(option_->read_buffer_size), std_err);
			CHECK_TCP_SESSION_STARTUP;
			//�׽��ַ��ͻ�������С
			socket_.set_option(asio::socket_base::send_buffer_size(option_->write_buffer_size), std_err);
			CHECK_TCP_SESSION_STARTUP;
			return true;
		}
		void Start() {
			DoRead();
		}
		void WriteBuffer(bcore::UniqueByteBuf buf) {
			bool can_write = false;
			{
				std::lock_guard<std::mutex> lock(write_buffer_mutex_);
				write_buffer_list_.emplace(std::move(buf));
				if (!write_flag_) {
					write_flag_ = true;
					can_write = true;
					std::swap(writing_buffer_list_, write_buffer_list_);
				}
			}
			if (can_write) {
				DoWrite();
			}
		}
		void SendMessage(void* message);
	private:
		void DoRead();
		void DoWrite();
	private:
		std::shared_ptr<ThreadPoolContext> thread_context_;
		tcp::socket socket_;
		std::shared_ptr<bnet::SessionOption> option_;
		std::shared_ptr<bnet::IMessageHandler> message_handler_;

		std::mutex write_buffer_mutex_;

		std::queue<bcore::UniqueByteBuf> writing_buffer_list_;
		std::queue<bcore::UniqueByteBuf> write_buffer_list_;
		std::atomic_bool write_flag_;

		bnet::SocketBufferUniquePtr read_buffer_;
		bnet::SocketBufferUniquePtr read_buffer_bak_;
	};
}