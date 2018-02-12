// PUT YOUR NAME AND STUDENT NUMBER HERE //

#include "GameObject.h"
#include <iostream>
#include "TTK\GraphicsUtils.h"
#include "TTK\OBJMesh.h"

GameObject::GameObject()
	: localScale(1.0f), colour(glm::vec4(1.0)), mesh(nullptr), m_pParent(nullptr), uniqueId(-1)
{
	localScaleX = localScale;
	localScaleY = localScale;
	localScaleZ = localScale;
}

GameObject::~GameObject() {}

void GameObject::addChild(GameObject * newChild)
{
	if (newChild)
	{
		m_pChildren.push_back(newChild);
		newChild->m_pParent = this; // tell new child that this game object is its parent
	}
}

void GameObject::removeChild(GameObject * rip)
{
	for (int i = 0; i < m_pChildren.size(); ++i)
	{
		if (m_pChildren[i] == rip) // compare memory locations (pointers)
		{
			std::cout << "Removing child: " + rip->name << " from object: " << this->name;
			m_pChildren.erase(m_pChildren.begin() + i);
		}
	}
}

void GameObject::setLocalPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

void GameObject::setLocalRotationAngleX(float newAngle)
{
	rotationX = newAngle;
}

void GameObject::setLocalRotationAngleY(float newAngle)
{
	rotationY = newAngle;
}

void GameObject::setLocalRotationAngleZ(float newAngle)
{
	rotationZ = newAngle;
}

void GameObject::setLocalScale(float newScale)
{
	localScale = newScale;
}

glm::vec3 GameObject::getLocalPosition()
{
	return position;
}

glm::mat4 GameObject::getLocalToWorldMatrix()
{
	return m_pLocalToWorldMatrix;
}

glm::vec3 GameObject::getWorldPosition()
{
	if (m_pParent)
		return m_pParent->getLocalToWorldMatrix() * glm::vec4(position, 1.0f);
	else
		return position;
}

glm::mat4 GameObject::getWorldRotation()
{
	if (m_pParent)
		return m_pParent->getWorldRotation() * m_pLocalRotation;
	else
		return m_pLocalRotation;
}

std::vector<GameObject*> GameObject::getChildren()
{
	return m_pChildren;
}

GameObject * GameObject::getParent()
{
	return m_pParent;
}

void GameObject::update(float dt)
{
	// Create 4x4 transformation matrix
	
	// Create rotation matrix
	glm::mat4 rx = glm::rotate(glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 ry = glm::rotate(glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rz = glm::rotate(glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

	// Note: pay attention to rotation order, ZYX is not the same as XYZ
	m_pLocalRotation = rz * ry * rx;

	// Create translation matrix
	glm::mat4 tran = glm::translate(position);

	// Create localScale matrix
	glm::mat4 scal = glm::scale(glm::vec3(localScaleX, localScaleY, localScaleZ));

	// Combine all above transforms into a single matrix
	// This is the local transformation matrix, ie. where is this game object relative to it's parent
	// If a game object has no parent (it is a root node) then its local transform is also it's global transform
	// If a game object has a parent, then we must apply the parent's transform
	m_pLocalTransformMatrix = tran * m_pLocalRotation * scal;

	if (m_pParent) {
		glm::mat4 scaleMatrix = glm::scale(glm::vec3(1.0f / m_pParent->localScaleX, 1.0f / m_pParent->localScaleY, 1.0f / m_pParent->localScaleZ));
		m_pLocalToWorldMatrix = m_pParent->getLocalToWorldMatrix() * m_pLocalTransformMatrix * scaleMatrix;
	}	
	else
		m_pLocalToWorldMatrix = m_pLocalTransformMatrix;

	// Update children
	for (int i = 0; i < m_pChildren.size(); i++)
	{
		m_pChildren[i]->update(dt);
	}
		



}

void GameObject::draw()
{
	// Draw a coordinate frame for the object
	glm::vec3 wPos = getWorldPosition();
	glm::mat4 wRot = getWorldRotation();

	glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);

	// Draw Axis on the Game Object
	TTK::Graphics::DrawVector(wPos, wRot[0], 3.0f, red);
	TTK::Graphics::DrawVector(wPos, wRot[1], 3.0f, green);
	TTK::Graphics::DrawVector(wPos, wRot[2], 3.0f, blue);

	// Draw node
	TTK::Graphics::DrawSphere(m_pLocalToWorldMatrix, 0.5f, colour);


	// Draw line to parent 
	if (m_pParent)
	{
		glm::vec3 parentWorldPosition = m_pParent->getWorldPosition();
		glm::vec3 myWorldPositon = getWorldPosition();

		TTK::Graphics::DrawLine(myWorldPositon, parentWorldPosition, 5.0f);
	}

	// Draw children
	for (int i = 0; i < m_pChildren.size(); ++i)
		m_pChildren[i]->draw();
}