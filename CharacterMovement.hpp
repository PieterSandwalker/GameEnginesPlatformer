#pragma once
#include "Component.hpp"
#include "GameObject.hpp"

class CharacterMovement :
	public Component
{

private:
	Shape* self;
	bool jump;

public:
	CharacterMovement(Shape *);
	Vector2f movement(int time);
	void update(Vector2f);
	void setSelf(Shape*);
	Shape* getSelf();
	void toggleJump();
	bool getJump();

};

