#pragma once

#include "Demo.h"

class VirtueelPD : public Demo
{
public:
	VirtueelPD();
	~VirtueelPD();

	// Inherited via Demo
	virtual void init(vrlib::tien::Scene & scene, TienTest * app) override;
	virtual void update(float frameTime, vrlib::tien::Scene & scene, TienTest * app) override;
};

