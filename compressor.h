#ifndef _DADIFF_COMPRESSOR_H_
#define _DADIFF_COMPRESSOR_H_

#include "pch.h"
#include "config.h"

DADIFF_BEGIN

enum class Mode : uint16_t {
	Uncomp,
	CompShort,
	Comp,
	CompLong,
};

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
	uint32_t magic = 0x46444144; // ASCII of "DADF" in little endian mode
	uint32_t size;
	uint32_t orig_crc;
	uint32_t ref_crc;
	Setting	 setting;
};

struct DictValue {
	uint32_t stride : 4;
	uint32_t offset : 28;
};

using Data	   = std::vector<uint8_t>;
using DataIter = Data::iterator;
using Map	   = std::unordered_map<uint32_t, DictValue>;

class Compressor {
	private:
	Map		 _dict{};
	DataIter _refBegin{nullptr};
	DataIter _refEnd{nullptr};
	Mode	 _mode{Mode::Comp};
	bool	 _dictParsed{false};

	public:
	Compressor() = default;

	constexpr void setMode(Mode m) noexcept {
		_mode = m;
	}

	constexpr Mode mode() const noexcept {
		return _mode;
	}

	constexpr void setReference(DataIter b, DataIter e) noexcept {
		_refBegin	= b;
		_refEnd		= e;
		_dictParsed = false;
	}

	static constexpr uint32_t makeToken(Setting setting, int32_t offset, uint32_t stride) noexcept;

	constexpr bool compress() noexcept;
	constexpr bool genDict() noexcept;
};

DADIFF_END

#endif // _DADIFF_COMPRESSOR_H_
