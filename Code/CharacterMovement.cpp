#include "CharacterMovement.hpp"
#include "Collider.hpp"

CharacterMovement::CharacterMovement(Shape* shape) : jump(true), Component("CharacterMovement")
{

	setSelf(shape);

}

Vector2f CharacterMovement::movement(int time)
{
	Vector2f characterMovement(0.f, 0.f);

	// movement speed
	float horizontalMovement = .25f * time;
	float jumpSpd = 1.f * time;

	// gravity
	characterMovement.y += .25f;

	// character keyboard commands
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		characterMovement.x += horizontalMovement;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		characterMovement.x -= horizontalMovement;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		characterMovement.y -= jumpSpd;
		toggleJump();
	}

	return characterMovement;
}

void CharacterMovement::setSelf(Shape* shape)
{
	self = shape;
}

Shape* CharacterMovement::getSelf()
{
	return self;
}

void CharacterMovement::toggleJump()
{
	jump = !jump;
}

bool CharacterMovement::getJump()
{
	return jump;
}

void CharacterMovement::update(Vector2f movement)
{

	getSelf()->move(movement);

}



