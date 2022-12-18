#include "pch.h"
#include "compressor.h"

DADIFF_BEGIN

constexpr bool Compressor::compress() noexcept {
	if(!genDict()) {
		return false;
	}
	return true;
}

// Note: current hash: xor for every 4 bytes
constexpr bool Compressor::genDict() noexcept {
	if(_dictParsed) {
		return true;
	}
	if(_mode == Mode::Uncomp) {
		return true; // No need to generate dictionary
	}

	// Currently not support reference larger than 2G
	DA_VERIFY(_refBegin < _refEnd && (_refEnd - _refBegin) < maxRefSize);

	DataIter it1, it2;
	uint32_t v;
	uint32_t _maxStride = std::min(maxStride[size_t(_mode)], std::bit_width(uint32_t(_refEnd - _refBegin)) - 1);

	auto hash8 = [](DataIter it) {
		uint32_t ret;
		for(int i = 0; i < 8; ++i) {
			ret = std::rotl(ret, 8) ^ *it;
			++it;
		}
		return ret;
	};
	for(uint32_t i = 0; i < _maxStride; ++i) {
		it2 = _refBegin;
		v	= 0;
		while(it2 != _refEnd) {
			if(v == 0) {
				for(uint32_t j = 0; j < (strides[i] >> 3); ++j) {
					v ^= hash8(it2++);
				}
				it1 = _refBegin;
			} else {
				v ^= hash8(it1++) ^ hash8(it2++);
			}
			DA_VERIFY(_dict.emplace(v, DictValue{i, uint32_t(it1 - _refBegin) >> 3}).second);
		}
	}
	_dictParsed = true;
	return true;
}

constexpr uint32_t Compressor::makeToken(Setting setting, int32_t offset, uint32_t stride) noexcept {
	DA_VERIFY(setting.mode != Mode::Uncomp);
	DA_VERIFY(stride < maxStride[size_t(setting.mode)]);
	DA_VERIFY((uint32_t(offset) & maxOffsetMask[size_t(setting.mode)]) == 0);
	switch(setting.mode) {
		case Mode::CompShort: {
			TokenCompShort ret{setting.escape, stride, offset};
			return std::bit_cast<uint32_t>(ret);
		}
		case Mode::Comp: {
			TokenComp ret{setting.escape, stride, offset};
			return std::bit_cast<uint32_t>(ret);
		}
		case Mode::CompLong: {
			TokenCompLong ret{setting.escape, stride, offset};
			return std::bit_cast<uint32_t>(ret);
		}
		default:
			DA_UNREACHABLE();
	}
	DA_UNREACHABLE();
}

DADIFF_END
