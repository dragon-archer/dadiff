#include "pch.h"
#include "compressor.h"

DADIFF_BEGIN

uint32_t Compressor::makeToken(Setting setting, int32_t offset, uint32_t stride) noexcept {
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

// Internal function used by hashRange and rehashRange
static constexpr uint32_t hashRangeImpl(DataIter first, DataIter last) noexcept {
	uint32_t ret = 0;
	if(first > last) {
		std::swap(first, last);
	}
	// TODO: enable optimize by SIMD and align
	while(first != last) {
		ret = std::rotl(ret, 8) ^ *first;
		++first;
	}
	return ret;
}

// Note: Assume stride < 16
uint32_t Compressor::hashRange(DataIter it, uint32_t stride) noexcept {
	uint32_t ret = hashRangeImpl(it, it + strides[stride]);
	// Mix low 4 bit with high 4 bit
	ret ^= ret << 28;
	// Store stride in low 4 bit
	ret = (ret & ~0x0F) | stride;
	return ret;
}

uint32_t Compressor::rehashRange(DataIter it, uint32_t stride, DataIter oldIt, uint32_t oldV) noexcept {
	const DataIter end1 = it + strides[stride];
	const DataIter end2 = oldIt + strides[oldV & 0x0F];
	if((end1 - it) > std::abs(it - oldIt) + std::abs(end1 - end2)) {
		oldV &= ~0x0F;
		oldV ^= hashRangeImpl(it, oldIt);
		oldV ^= hashRangeImpl(end1, end2);
		oldV ^= oldV << 28;
		oldV = (oldV & ~0x0F) | stride;
	} else {
		return hashRange(it, stride); // No need to optimize
	}
	return oldV;
}

bool Compressor::makeDict() noexcept {
	if(!_dict.empty()) {
		return true; // Already generated
	}
	if(_setting.mode == Mode::Uncomp) {
		return true; // No need to generate dictionary
	}

	DataIter it;
	uint32_t v;
	uint32_t _maxStride = std::min(maxStride[size_t(_setting.mode)], std::bit_width(uint32_t(_refEnd - _refBegin)) - 1);

	for(uint32_t i = 0; i < _maxStride; ++i) {
		it = _refBegin;
		v  = 0;
		while(it < _refEnd - strides[i]) {
			if(v == 0) {
				v = hashRange(it, i);
			} else {
				v = rehashRange(it, i, it - dictStep, v);
			}
			it += dictStep;
			DA_VERIFY(_dict.emplace(v, DictValue{i, uint32_t(it - _refBegin) / dictStep}).second);
		}
	}
	return true;
}

Header Compressor::makeHeader(DataIter first, DataIter last) noexcept {
	Header ret;
	ret.magic	= DADIFF_MAGIC;
	ret.setting = _setting;
	ret.size	= last - first;
	ret.origCrc = crc32(first, last);
	ret.refCrc	= crc32(_refBegin, _refEnd);
	return ret;
}

DADIFF_END
