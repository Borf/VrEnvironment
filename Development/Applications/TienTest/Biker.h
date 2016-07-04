#pragma once

#include "Demo.h"


class Biker : public Demo
{
public:
	Biker();
	~Biker();

	// Inherited via Demo
	virtual void init(vrlib::tien::Scene & scene, TienTest * app) override;
	virtual void update(float frameTime, vrlib::tien::Scene & scene, TienTest * app) override;
};

