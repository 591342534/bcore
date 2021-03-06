#pragma once
#include "bnet/net_interface.h"
#include <string>

namespace bnet {
	class StringCoder : public IProtoCoder {
	public:
		static std::shared_ptr<StringCoder> NewInstance() {
			static std::shared_ptr<StringCoder> g_instance = std::make_shared<StringCoder>();
			return g_instance;
		}
		virtual uint32_t ProtocolSize(void* message, int& msg_type, ErrorCode& err) override
		{
			return static_cast<std::string*>(message)->size();
		}
		virtual void ProtocolEncode(void* message, bcore::Slice& buf, int msg_type, ErrorCode& err) override
		{
			auto str = static_cast<std::string*>(message);
			if (!buf.append(str->c_str(), str->size())) {
				err = ERROR_CODE::kProtocolEncodeBufNotEnough;
				return;
			}
		}
		virtual void* ProtocolDecode(bcore::Slice& buf, ErrorCode& err, int& msg_type, bool is_part) override
		{
			return new std::string(buf.data(), buf.len());
		}
	};
}