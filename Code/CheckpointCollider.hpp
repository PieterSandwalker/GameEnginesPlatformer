#pragma once
#include "Collider.hpp"

class CheckpointCollider :
	public Collider
{

public:
	CheckpointCollider(Shape*);
	void update(GameObject*, Vector2f*);

};

