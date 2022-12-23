#ifndef _DADIFF_COMPRESSOR_H_
#define _DADIFF_COMPRESSOR_H_

#include "pch.h"
#include "config.h"
#include "crc32.h"

DADIFF_BEGIN

enum class Mode : uint16_t {
	Uncomp,
	CompShort,
	Comp,
	CompLong,
};

inline constexpr uint32_t dictStep	 = 8;
inline constexpr uint32_t maxRefSize = 1 << 31;

inline constexpr uint32_t maxStride[] = {
	0,	// Mode::Uncomp
	7,	// Mode::CompShort
	15, // Mode::Comp
	16, // Mode::CompLong
};

inline constexpr uint32_t maxOffsetMask[] = {
	uint32_t(-1),			  // Mode::Uncomp
	~uint32_t((1 << 13) - 1), // Mode::CompShort
	~uint32_t((1 << 20) - 1), // Mode::Comp
	~uint32_t((1 << 24) - 1), // Mode::CompLong
};

inline constexpr uint32_t strides[] = {
	8,
	16,
	32,
	64,
	128,
	256,
	512,
	1 << 10,
	2 << 10,
	4 << 10,
	8 << 10,
	16 << 10,
	32 << 10,
	64 << 10,
	128 << 10,
	256 << 10,
};

struct TokenCompShort {
	uint32_t escape : 8;
	uint32_t stride : 3;
	int32_t	 offset : 13;
};

struct TokenComp {
	uint32_t escape : 8;
	uint32_t stride : 4;
	int32_t	 offset : 20;
};

struct TokenCompLong {
	uint32_t escape : 4;
	uint32_t stride : 4;
	int32_t	 offset : 24;
};

struct Setting {
	Mode	mode;
	uint8_t version;
	uint8_t escape;
};

struct Header {
	uint32_t magic;	  // Magic number to identify dadiff file
	Setting	 setting; // Setting of this file
	uint32_t size;	  // Original size
	uint32_t origCrc; // Original CRC32
	uint32_t refCrc;  // Reference CRC32
};

struct DictValue {
	uint32_t stride : 4;
	uint32_t offset : 28;
};

using Data	   = std::vector<uint8_t>;
using DataIter = uint8_t*;
using Map	   = std::unordered_map<uint32_t, DictValue>;

class Compressor {
	private:
	Map		 _dict{};
	DataIter _refBegin{};
	DataIter _refEnd{};
	Setting	 _setting{Mode::Comp, DADIFF_VERSION, DADIFF_ESCAPE_CHAR};

	public: // Main interface
	Data compress(DataIter first, DataIter last) noexcept;

	public:
	Compressor() = default;

	void setMode(Mode m) noexcept {
		if(_setting.mode != m) {
			_setting.mode = m;
			_dict.clear();
		}
	}

	constexpr Mode mode() const noexcept {
		return _setting.mode;
	}

	bool setReference(DataIter first, DataIter last) noexcept {
		// Currently not support reference larger than 2G
		DA_VERIFY(first < last && (last - first) < maxRefSize);
		_refBegin = first;
		_refEnd	  = last;
		_dict.clear();
		return true;
	}


	static uint32_t makeToken(Setting setting, int32_t offset, uint32_t stride) noexcept;
	static uint32_t hashRange(DataIter it, uint32_t stride) noexcept;
	static uint32_t rehashRange(DataIter it, uint32_t stride, DataIter oldIt, uint32_t oldV) noexcept;

	private:
	bool makeDict() noexcept;
	Header makeHeader(DataIter first, DataIter last) noexcept;
};

DADIFF_END

#endif // _DADIFF_COMPRESSOR_H_
