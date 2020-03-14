#pragma once
#include "Collider.hpp"

class DeathCollider :
	public Collider
{

public:
	DeathCollider(Shape*);
	void update(GameObject*, Vector2f*);

};

