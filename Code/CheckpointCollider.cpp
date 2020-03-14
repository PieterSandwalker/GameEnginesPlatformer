#include "CheckpointCollider.hpp"
#include "Character.hpp"

CheckpointCollider::CheckpointCollider(Shape* shape) : Collider(shape)
{

}

void CheckpointCollider::update(GameObject* character, Vector2f* velocity)
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
		collision = true;
	}

	charBox.left -= x;

	// Vertical check
	charBox.top += y;

	if (charBox.intersects(objBox))
	{
		collision = true;
	}

	charBox.top -= y;

	if (collision) {
		Character* checkChar = (Character*)character;
		checkChar->setCheckpoint(getSelf()->getPosition());
	}

}
