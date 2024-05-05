#include "pch.hpp"

Packet::Packet(const boost::array<char, PACKET_MAX_LENGTH>& data, const size_t size)
{
	const auto dataRaw = data.data();

	// Service that should handle this packet (fsys, acct, etc...)
	service = std::string(dataRaw, HEADER_VALUE_LENGTH);

	// Transaction kind (Simple, Chunked, etc...)
	kind = Utils::DecodeInt(dataRaw + TYPE_OFFSET, HEADER_VALUE_LENGTH);
	length = Utils::DecodeInt(dataRaw + LENGTH_OFFSET, HEADER_VALUE_LENGTH);

	this->data = Utils::GetPacketData(dataRaw + HEADER_LENGTH);
}
