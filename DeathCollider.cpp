#include "DeathCollider.hpp"
#include "Character.hpp"

DeathCollider::DeathCollider(Shape* self) : Collider(self)
{

}

void DeathCollider::update(GameObject* character, Vector2f* velocity)
{

	bool collision = false;

	float x = velocity->x;
	float y = velocity->y;

	sf::FloatRect charBox = character->getShape()->getGlobalBounds();
	sf::FloatRect objBox = getSelf()->getGlobalBounds();

	charBox.left += x;

	// Horizontal check
	if (charBox.intersects(objBox))
	{
		velocity->x = 0.f;
		collision = true;
	}

	charBox.left -= x;

	// Vertical check
	charBox.top += y;

	if (charBox.intersects(objBox))
	{
		velocity->y = 0.f;
		collision = true;
	}

	charBox.top -= y;

	if (collision) {
		Character* deadChar = (Character *)character;
		deadChar->getShape()->setPosition(deadChar->getCheckpoint());
	}

}
