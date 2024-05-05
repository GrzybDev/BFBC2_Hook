#pragma once

class Proxy
{
public:
	Proxy();

	static Proxy& GetInstance()
	{
		static Proxy* instance = nullptr;

		if (instance == nullptr)
			instance = new Proxy();

		return *instance;
	}
};
