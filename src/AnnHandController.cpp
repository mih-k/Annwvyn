#include "stdafx.h"
#include "AnnHandController.hpp"

using namespace Annwvyn;

AnnHandController::AnnHandController(std::string type, Ogre::SceneNode* handNode, AnnHandControllerID controllerID, AnnHandControllerSide controllerSide) :
	controllerType(type),
	id(controllerID),
	side(controllerSide),
	node(handNode),
	grabbed(nullptr),
	model(nullptr),
	tracked(false),
	trackedAngularSpeed(AnnVect3::ZERO),
	trackedLinearSpeed(AnnVect3::ZERO),
	invalidAxis("INVALID", 0)
{
	std::cerr << "HandController ID : " << id << " created";
	std::cerr << "For side : " << getSideAsString(side);
}

inline std::string AnnHandController::getSideAsString(AnnHandControllerSide s)
{
	if (s == leftHandController) return "Left Hand";
	return "Right Hand";
}

void AnnHandController::attachModel(Ogre::Entity* handModel)
{
	if (model) node->detachObject(model);
	model = handModel;
	node->attachObject(model);
}

void AnnHandController::detachModel()
{
	if (model) node->detachObject(model);
	model = nullptr;
}

Ogre::Entity* AnnHandController::getModel()
{
	return model;
}

AnnVect3 AnnHandController::getWorldPosition()
{
	return node->getPosition();
}

AnnQuaternion AnnHandController::getWorldOrientation()
{
	return node->getOrientation();
}

AnnVect3 AnnHandController::getAngularSpeed()
{
	return trackedAngularSpeed;
}

AnnVect3 AnnHandController::getLinearSpeed()
{
	return trackedLinearSpeed;
}

AnnVect3 AnnHandController::getPointingDirection()
{
	return node->getOrientation() * AnnVect3::NEGATIVE_UNIT_Z;
}

void AnnHandController::attachNode(Ogre::SceneNode* grabbedObject)
{
	if (grabbedObject->getParentSceneNode())
		grabbedObject->getParentSceneNode()->removeChild(grabbedObject);
	node->addChild(grabbedObject);
	grabbed = grabbedObject;
}

void AnnHandController::setTrackedPosition(AnnVect3 position)
{
	tracked = true;
	node->setPosition(position);
}

void AnnHandController::setTrackedOrientation(AnnQuaternion orientation)
{
	tracked = true;
	node->setOrientation(orientation);
}

void AnnHandController::setTrackedLinearSpeed(AnnVect3 v)
{
	tracked = true;
	trackedLinearSpeed = v;
}

void AnnHandController::setTrackedAngularSpeed(AnnVect3 v)
{
	tracked = true;
	trackedAngularSpeed = v;
}

bool AnnHandController::isTracked()
{
	return tracked;
}

bool AnnHandController::getButtonState(uint8_t buttonIndex)
{
	return buttonsState[buttonIndex];
}

size_t AnnHandController::getNbButton()
{
	return buttonsState.size();
}

bool AnnHandController::hasBeenPressed(uint8_t buttonIndex)
{
	for (auto button : pressedButtons)
		if (button == buttonIndex) return true;
	return false;
}

bool AnnHandController::hasBeenReleased(uint8_t buttonIndex)
{
	for (auto button : releasedButtons)
		if (button == buttonIndex) return true;
	return false;
}

size_t AnnHandController::getNbAxes()
{
	return axes.size();
}

AnnHandControllerAxis& AnnHandController::getAxis(size_t index)
{
	if (index < axes.size()) return axes[index];
	return invalidAxis;
}

AnnHandController::AnnHandControllerSide AnnHandController::getSide()
{
	return side;
}

std::string AnnHandController::getType()
{
	return controllerType;
}

std::vector<AnnHandControllerAxis>& AnnHandController::getAxesVector()
{
	return axes;
}

std::vector<bool>& AnnHandController::getButtonStateVector()
{
	return buttonsState;
}

std::vector<uint8_t>& AnnHandController::getPressedButtonsVector()
{
	return pressedButtons;
}

std::vector<uint8_t>& AnnHandController::getReleasedButtonsVector()
{
	return releasedButtons;
}

AnnHandControllerAxis::AnnHandControllerAxis(const std::string& AxisName, float normalizedValue) :
	name(AxisName),
	value(0) {
	updateValue(normalizedValue);
}

std::string AnnHandControllerAxis::getName() const { return name; }
float AnnHandControllerAxis::getValue() const { return value; }

void AnnHandControllerAxis::updateValue(float normalizedValue)
{
	if (isInRange(normalizedValue))
		value = normalizedValue;
}

bool AnnHandControllerAxis::isInRange(float v) { return (v >= -1 && v <= 1); }