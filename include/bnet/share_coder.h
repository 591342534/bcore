#pragma once
#include "bcore/buffer_pool.h"
using namespace bcore;
namespace bnet {
	class ShareCoder {
		static const uint64_t EncodeVarintCompare = 0x7F;
	public:
		static void EncodeVarint(SliceSharedPtr slice, uint64_t x) {
			while (true) {
				if (x > EncodeVarintCompare) {
					slice->append((char)(x & 0x7F | 0x80));
					x >>= 7;
				}
				else {
					slice->append((char)(x & 0x7F));
					return;
				}
			}
		}
		static uint32_t EncodeVarintSize(uint64_t x) {
			uint32_t size = 0;
			while (true) {
				if (x > EncodeVarintCompare) {
					size++;
					x >>= 7;
				}
				else {
					size++;
					return size;
				}
			}
		}
		static bool DecodeVarint(SliceSharedPtr slice, uint32_t& read_len, uint64_t& x) {
			x = 0;
			read_len = 0;
			auto len = slice->len();
			for (uint32_t shift = 0; shift < 64; shift += 7) {
				if (read_len >= len) {
					return false;
				}
				auto b = uint8_t(slice->get(read_len++));
				x |= (uint64_t)(b & 0x7F) << shift;
				if (b < (uint8_t)0x80) {
					return true;
				}
			}
			return false;
		}
	};
}