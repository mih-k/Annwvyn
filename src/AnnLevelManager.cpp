#include "stdafx.h"
#include "AnnLevelManager.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLevelManager::AnnLevelManager() : AnnSubSystem("LevelManager"),
current(nullptr),
jumpRequested(false),
jumpTo(0)
{
}

AnnLevelManager::~AnnLevelManager()
{
	AnnGetEngine()->log("Deleting the Level Manager. Destroying every level known by the Level Manager before.");
	//clear the levels
	levelList.clear();
}

void AnnLevelManager::jump(level_id levelId)
{
	AnnDebug() << "LevelManager jumping to levelId : " << levelId;

	if (!(levelId < levelList.size())) return;

	//Deferred level jump
	if (!jumpRequested)
	{
		jumpRequested = true;
		jumpTo = levelId;
		return;
	}

	jumpRequested = false;
	jumpTo = 0;
	if (current)
		current->unload();
	current = levelList[levelId];
	current->load();
}

void AnnLevelManager::jump(std::shared_ptr<AnnLevel> level)
{
	for (level_id i(0); i < levelList.size(); i++)
		if (levelList[i] == level)
		{
			jump(i);
			break;
		}
}

void AnnLevelManager::addLevel(std::shared_ptr<AnnLevel> level)
{
	AnnDebug() << "Adding level " << level << "to LevelManager";
	levelList.push_back(level);
}

void AnnLevelManager::jumpToFirstLevel()
{
	jump(level_id(0));
}

void AnnLevelManager::update()
{
	if (jumpRequested)
		return jump(jumpTo);
	if (current) current->runLogic();
}

void AnnLevelManager::unloadCurrentLevel()
{
	if (current) current->unload();
	current = nullptr;
}

std::shared_ptr<AnnLevel> AnnLevelManager::getLastLevelLoaded()
{
	return levelList.back();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getFirstLevelLoaded()
{
	return levelList.front();
}

std::shared_ptr<AnnLevel> AnnLevelManager::getLevelByIndex(level_id id)
{
	if (id >= levelList.size()) return nullptr;
	return levelList[id];
}

void AnnLevelManager::addToCurrentLevel(std::shared_ptr<AnnGameObject> obj)
{
	if (!current || !obj) return;
	current->levelContent.push_back(obj);
}

void AnnLevelManager::removeFromCurrentLevel(std::shared_ptr<AnnGameObject> obj)
{
	if (!current || !obj) return;
	current->levelContent.remove(obj);
}