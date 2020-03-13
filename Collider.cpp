#include "Collider.hpp"

Collider::Collider(Shape* self) : Component("Collider")
{
	setShape(self);
}

 void Collider::update(GameObject* character, Vector2f* velocity)
{

	float x = velocity->x;
	float y = velocity->y;

	sf::FloatRect charBox = character->getShape()->getGlobalBounds();
	sf::FloatRect objBox = self->getGlobalBounds();

	charBox.left += x;

	// Horizontal check
	if (charBox.intersects(objBox))
	{
		velocity->x = 0.f;
	}

	charBox.left -= x;

	// Vertical check
	charBox.top += y;

	if (charBox.intersects(objBox))
	{
		velocity->y = 0.f;
	}

	charBox.top -= y;

}

void Collider::setShape(Shape* newSelf)
{
	self = newSelf;
}

Shape* Collider::getSelf()
{
	return self;
}
