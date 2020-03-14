#include "PathCollider.hpp"
#include <iostream>

PathCollider::PathCollider(Shape* shape, Path* selfPath) : Collider(shape)
{
	setPath(selfPath);
}

void PathCollider::update(GameObject* character, Vector2f* velocity)
{
	float x = velocity->x;
	float y = velocity->y;

	sf::FloatRect charBox = character->getShape()->getGlobalBounds();
	sf::FloatRect objBox = getSelf()->getGlobalBounds();

	float newX = getPath()->getVelocity().x;
	float newY = getPath()->getVelocity().y;

	charBox.left += x;

	// Horizontal check
	if (charBox.intersects(objBox))
	{
		velocity->x = newX;
		velocity->y += newY;
	}

	charBox.left -= x;

	// Vertical check
	charBox.top += y;

	if (charBox.intersects(objBox))
	{
		velocity->x += newX;
		velocity->y = newY;
	}

	charBox.top -= y;

}

void PathCollider::setPath(Path* velocity)
{
	selfPath = velocity;
}

Path* PathCollider::getPath()
{
	return selfPath;
}
