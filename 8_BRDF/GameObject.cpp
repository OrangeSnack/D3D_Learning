#include "GameObject.h"

GameObject::GameObject()
{
	transform = AddComponent<Transform>();
}
