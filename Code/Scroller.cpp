#include "Scroller.hpp"
#include "CharacterMovement.hpp"

Scroller::Scroller(Shape* shape, Vector2f scroll) : Component("Scroller")
{
	setSelf(shape);
	setScroll(scroll);
}

void Scroller::update(GameObject* character, vector<GameObject*>* scene)
{
	Vector2f movement = getScroll() * (float)shifts;
	Character* move = (Character*)character;
	move->getShape()->move(getScroll());
	move->setCheckpoint(move->getCheckpoint() + getScroll());
	for (int i = 0; i < scene->size(); i++) {
		scene->at(i)->getShape()->move(getScroll());
	}
}

void Scroller::collision(GameObject* character, Vector2f* velocity)
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
		shifts++;
	}
}

void Scroller::setSelf(Shape* newSelf)
{
	self = newSelf;
}

Shape* Scroller::getSelf()
{
	return self;
}

void Scroller::setScroll(Vector2f newScroll)
{
	scroll = newScroll;
}

Vector2f Scroller::getScroll()
{
	return scroll;
}
