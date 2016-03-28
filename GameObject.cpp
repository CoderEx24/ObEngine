#include "GameObject.hpp"

GameObjectHandler gameObjectHandlerCore;

void loadScrGameObjectLib(GameObject* obj, kaguya::State* lua)
{
	(*lua)["CPP_GameObject"].setClass(kaguya::ClassMetatable<GameObject>()
		.addMember("getDeltaTime", &GameObject::getDeltaTime)
		.addMember("getLevelSprite", &GameObject::getLevelSprite)
		.addMember("getCollider", &GameObject::getCollider)
		.addMember("canCollide", &GameObject::canCollide)
		.addMember("canClick", &GameObject::canClick)
		.addMember("canDisplay", &GameObject::canDisplay)
		.addMember("getPriority", &GameObject::getPriority)
		.addMember("setAnimationKey", &GameObject::setAnimationKey)
		);
	(*lua)["This"] = obj;
	(*lua)["CPP_Import"] = &loadLibBridge;
	(*lua)["CPP_Hook"] = &loadHookBridge;
	(*lua)["CPP_UseLocalTrigger"] = &useLocalTrigger;
	(*lua)["CPP_UseGlobalTrigger"] = &useGlobalTrigger;
	(*lua)["CPP_UseCustomTrigger"] = &useCustomTrigger;
}

void useLocalTrigger(std::string scrKey, std::string trName)
{
	gameObjectHandlerCore.objHandlerMap[scrKey]->registerTrigger(triggerDatabaseCore.getCustomTrigger(scrKey, "Local", trName));
}

void useGlobalTrigger(std::string scrKey, std::string trName)
{
	gameObjectHandlerCore.objHandlerMap[scrKey]->registerTrigger(triggerDatabaseCore.getTrigger(trName));
}

void useCustomTrigger(std::string scrKey, std::string trNsp, std::string trGrp, std::string trName, std::string useAs)
{
	if (trName == "*")
	{
		std::vector<std::string> allEv = triggerDatabaseCore.getAllTriggersNameFromCustomGroup(trNsp, trGrp);
		for (int i = 0; i < allEv.size(); i++)
		{
			gameObjectHandlerCore.objHandlerMap[scrKey]->registerTrigger(triggerDatabaseCore.getCustomTrigger(trNsp, trGrp, allEv[i]));
		}
	}
	else gameObjectHandlerCore.objHandlerMap[scrKey]->registerTrigger(triggerDatabaseCore.getCustomTrigger(trNsp, trGrp, trName));
	if (useAs != "")
	{
		gameObjectHandlerCore.objHandlerMap[scrKey]->registeredAliases.push_back(std::tuple<std::string, std::string, std::string>(trNsp, trGrp, useAs));
	}
}

void loadLibBridge(std::string scrKey, std::string lib)
{
	loadLib(gameObjectHandlerCore.scrHandlerMap[scrKey], lib);
}

void loadHookBridge(std::string scrKey, std::string hookname)
{
	hookCore.getValue(gameObjectHandlerCore.scrHandlerMap[scrKey], hookname);
}

bool orderScrPriority(GameObject* g1, GameObject* g2)
{
	return (g1->getPriority() > g2->getPriority());
}

void GameObjectHandler::orderUpdateScrArray()
{
	updateObjArray.clear();
	for (auto it = objHandlerMap.begin(); it != objHandlerMap.end(); it++)
	{
		updateObjArray.push_back(it->second);
	}
	std::sort(updateObjArray.begin(), updateObjArray.end(), orderScrPriority);
}

//GameObjectHandler
GameObject* GameObjectHandler::getGameObject(std::string id)
{
	for (auto it = objHandlerMap.begin(); it != objHandlerMap.end(); it++)
	{
		if (it->second->id == id) return it->second;
	}
	std::cout << "<Error:GameObject:GameObjectHandler>[getGameObject] : GameObject : " << id << " does not exists" << std::endl;
	return nullptr;
}
std::vector<GameObject*> GameObjectHandler::getAllGameObject(std::vector<std::string> filters)
{
	std::vector<GameObject*> returnVec;
	for (auto it = objHandlerMap.begin(); it != objHandlerMap.end(); it++)
	{
		if (filters.size() == 0) returnVec.push_back(it->second);
		else
		{
			bool allFilters = true;
			if (fn::Vector::isInList(std::string("Display"), filters)) { if (!it->second->canDisplay()) allFilters = false; }
			if (fn::Vector::isInList(std::string("Collide"), filters)) { if (!it->second->canCollide()) allFilters = false; }
			if (fn::Vector::isInList(std::string("Click"), filters)) { if (!it->second->canClick()) allFilters = false; }
			if (allFilters) returnVec.push_back(it->second);
		}
	}
	return returnVec;
}
GameObject* GameObjectHandler::createGameObject(std::string id, std::string type, std::string obj)
{
	GameObject* newGameObject = new GameObject(id);
	DataParser getGameObjectFile;
	getGameObjectFile.parseFile("Data/" + type + "/" + obj + "/" + obj + ".obj.msd", true);
	DataObject* gameObjectData = getGameObjectFile.accessDataObject(obj);
	newGameObject->loadGameObject(gameObjectData);
	kaguya::State* newLuaState = new kaguya::State;
	std::string key = fn::String::getRandomKey("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 12);
	std::cout << "Create GameObject : " << id << "/" << obj << " with key : " << key << std::endl;
	this->objHandlerMap[key] = newGameObject;
	this->objHandlerMap[key]->key = key;
	this->objHandlerMap[key]->publicKey = fn::String::getRandomKey("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 12);
	this->scrHandlerMap[key] = newLuaState;
	triggerDatabaseCore.createCustomNamespace(key);
	triggerDatabaseCore.createCustomNamespace(this->objHandlerMap[key]->publicKey);
	this->trgHandlerMap[key] = triggerDatabaseCore.createTriggerGroup(key, "Local");
	//Script Loading
	this->scrHandlerMap[key]->dofile("Data/Scripts/ScrInit.lua");
	loadScrGameObjectLib(this->objHandlerMap[key], this->scrHandlerMap[key]);
	(*this->scrHandlerMap[key])["ID"] = id;
	(*this->scrHandlerMap[key])["Key"] = this->objHandlerMap[key]->key;
	(*this->scrHandlerMap[key])["PKey"] = this->objHandlerMap[key]->publicKey;
	(*this->scrHandlerMap[key])("protect(\"ID\")");
	(*this->scrHandlerMap[key])("protect(\"Key\")");
	this->trgHandlerMap[key]->addTrigger("Init");
	this->setTriggerState(id, "Init", true);
	this->trgHandlerMap[key]->addTrigger("Update");
	this->trgHandlerMap[key]->setPermanent("Update", true);
	this->setTriggerState(id, "Update", true);
	this->trgHandlerMap[key]->addTrigger("Collide");
	this->trgHandlerMap[key]->addTrigger("Click");
	this->trgHandlerMap[key]->addTrigger("Press");
	this->trgHandlerMap[key]->addTrigger("Delete");
	this->objHandlerMap[key]->hookLuaState(this->scrHandlerMap[key]);
	if (gameObjectData->listExists(convertPath("Script"), "scriptList"))
	{
		int scriptListSize = gameObjectData->getListAttribute(convertPath("Script"), "scriptList")->getSize();
		for (int i = 0; i < scriptListSize; i++)
		{
			std::string getScrName;
			gameObjectData->getListAttribute(convertPath("Script"), "scriptList")->getElement(i)->getData(&getScrName);
			this->executeFile(id, getScrName);
		}
	}
	this->orderUpdateScrArray();
	return objHandlerMap[key];
}
void GameObjectHandler::executeFile(std::string object, std::string path)
{
	this->scrHandlerMap[getGameObject(object)->key]->dofile(path);
}
void GameObjectHandler::executeLine(std::string object, std::string line)
{
	this->scrHandlerMap[getGameObject(object)->key]->dostring(line);
}
void GameObjectHandler::setTriggerState(std::string object, std::string trigger, bool state)
{
	this->trgHandlerMap[getGameObject(object)->key]->setTriggerState(trigger, state);
	if (state)
	{
		if (!this->trgHandlerMap[getGameObject(object)->key]->getTrigger(trigger)->isPermanent())
		{
			triggerToDisable.push_back(new std::pair<std::string, std::string>(getGameObject(object)->key, trigger));
		}
	}
}
void GameObjectHandler::setGlobalTriggerState(std::string trigger, bool state)
{
	for (auto it = trgHandlerMap.begin(); it != trgHandlerMap.end(); it++)
	{
		it->second->setTriggerState(trigger, state);
	}
}
void GameObjectHandler::update()
{
	for (int i = 0; i < updateObjArray.size(); i++)
	{
		updateObjArray[i]->update();
	}
}

//GameObject
GameObject::GameObject(std::string id) : objectCollider(id), objectLevelSprite(id)
{
	this->id = id;
	objectLevelSprite.useDirtyAnimation(false, true);
}

void GameObject::registerTrigger(Trigger* trg)
{
	this->registeredTriggers.push_back(trg);
}

void GameObject::loadGameObject(DataObject* obj)
{
	this->objFile = obj;
	std::string animatorPath;
	//Animator
	if (obj->complexExists(convertPath(""), "Animator"))
	{
		obj->getAttribute(convertPath("Animator"), "path")->getData(&animatorPath);
		this->objectAnimator.setPath(animatorPath);
		this->objectAnimator.loadAnimator();
	}
	else
	{
		hasAnimator = false;
	}
	//Collider
	if (obj->complexExists(convertPath(""), "Collider"))
	{
		obj->getAttribute(convertPath("Collider"), "solid")->getData(&colliderSolid);
		obj->getAttribute(convertPath("Collider"), "click")->getData(&colliderClick);
		objectCollider.setSolid(colliderSolid);
		int colliderPointSize;
		colliderPointSize = obj->getListAttribute(convertPath("Collider"), "polygonPoints")->getSize();
		for (int i = 0; i < colliderPointSize; i++)
		{
			std::string getPt;
			obj->getListAttribute(convertPath("Collider"), "polygonPoints")->getElement(i)->getData(&getPt);
			std::vector<std::string> tPoint = fn::String::split(getPt, ",");
			objectCollider.addPoint(std::stoi(tPoint[0]), std::stoi(tPoint[1]));
		}
	}
	else
	{
		hasCollider = false;
	}
	//LevelSprite
	if (obj->complexExists(convertPath(""), "LevelSprite"))
	{
		int decoPosX, decoPosY;
		int decoRot = 0;
		double decoSca = 1.0;
		std::vector<std::string> decoAtrList;
		std::string attrBuffer;
		int layer;
		int zdepth;
		obj->getAttribute(convertPath("LevelSprite"), "posX")->getData(&decoPosX);
		obj->getAttribute(convertPath("LevelSprite"), "posY")->getData(&decoPosY);
		obj->getAttribute(convertPath("LevelSprite"), "rotation")->getData(&decoRot);
		obj->getAttribute(convertPath("LevelSprite"), "scale")->getData(&decoSca);
		obj->getAttribute(convertPath("LevelSprite"), "layer")->getData(&layer);
		obj->getAttribute(convertPath("LevelSprite"), "z-depth")->getData(&zdepth);
		if (obj->listExists(convertPath("LevelSprite"), "attributeList"))
		{
			int atrListSize = obj->getListAttribute(convertPath("LevelSprite"), "attributeList")->getSize();
			for (int j = 0; j < atrListSize; j++)
				decoAtrList.push_back(obj->getListAttribute(convertPath("LevelSprite"), "attributeList")->getElement(j)->getData(&attrBuffer));
		}
		objectLevelSprite.move(decoPosX, decoPosY);
		objectLevelSprite.setRotation(decoRot);
		objectLevelSprite.setScale(decoSca);
		objectLevelSprite.setAtr(decoAtrList);
		objectLevelSprite.setLayer(layer);
		objectLevelSprite.setZDepth(zdepth);
	}
	else
	{
		hasLevelSprite = false;
	}
	//Script
	if (obj->complexExists(convertPath(""), "Script"))
	{
		if (obj->attributeExists(convertPath("Script"), "priority")) obj->getAttribute(convertPath("Script"), "priority")->getData(&scrPriority);
	}
}

void GameObject::hookLuaState(kaguya::State* lua)
{
	this->scriptEngine = lua;
}

void GameObject::update()
{
	for (int i = 0; i < registeredTriggers.size(); i++)
	{
		if (registeredTriggers[i]->getState())
		{
			std::string useGrp = registeredTriggers[i]->getGroup();
			for (int j = 0; j < registeredAliases.size(); j++)
			{
				std::string alNsp, alGrp, alRep;
				std::tie(alNsp, alGrp, alRep) = registeredAliases[j];
				if (alNsp == registeredTriggers[i]->getNamespace() && alGrp == registeredTriggers[i]->getGroup())
				{
					useGrp = alRep;
				}
			}
			std::string funcname = useGrp + "." + registeredTriggers[i]->getName();
			(*this->scriptEngine)["cpp_param"] = registeredTriggers[i]->getParameters();
			this->scriptEngine->dostring("if type(" + funcname + ") == \"function\" then " + funcname + "(cpp_param) end");
		}
	}
	if (hasAnimator)
	{
		this->objectAnimator.update();
		if (hasLevelSprite) this->objectLevelSprite.setSprite(this->objectAnimator.getSprite());
	}
}

void GameObject::setDeltaTime(double dt)
{
	this->currentDeltaTime = dt;
}

std::string GameObject::getID()
{
	return this->id;
}

std::string GameObject::getPublicKey()
{
	return publicKey;
}

double GameObject::getDeltaTime()
{
	return this->currentDeltaTime;
}

void GameObject::setAnimationKey(std::string key)
{
	this->objectAnimator.setKey(key);
}

int GameObject::getPriority()
{
	return scrPriority;
}

bool GameObject::canDisplay()
{
	return (hasAnimator && hasLevelSprite);
}

bool GameObject::canCollide()
{
	return (hasCollider && colliderSolid);
}

bool GameObject::canClick()
{
	return (hasCollider && colliderClick);
}

LevelSprite* GameObject::getLevelSprite()
{
	return &objectLevelSprite;
}

Collision::PolygonalCollider* GameObject::getCollider()
{
	if (hasCollider) return &objectCollider;
	else std::cout << "<Error:GameObject:GameObject>[getCollider] : GameObject " << id << " has no Collider" << std::endl;
}