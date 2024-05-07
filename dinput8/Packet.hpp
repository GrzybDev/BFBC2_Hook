// Packet.hpp : Defines the Packet class, which handles the creation and manipulation of packets.
#pragma once

class Packet
{
public:
	Packet(const boost::array<char, PACKET_MAX_LENGTH>& data, size_t size);

	std::string service;
	unsigned int kind, length;
	std::string data;
};
