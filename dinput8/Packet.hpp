#pragma once

class Packet
{
public:
	Packet(const boost::array<char, PACKET_MAX_LENGTH>& data, size_t size);

	std::string service;
	unsigned int kind, length;
	std::string data;
};
