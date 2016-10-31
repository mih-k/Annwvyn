//This file only exist to document the scripting API with doxygen. This file should NEVER be included ANYWERE

#pragma once

#include "Annwvyn.h"
#include "doxygen_dummy.hpp"
#include "AnnGameObject.hpp"
#include "AnnLevelManager.hpp"

namespace Annwvyn
{
	//As a safety measure, if anybody include this file even with the warning in the 1st line, it will put it's junk in a sub-namespace
	///List prototypes of C++ non-existing functions that match the interface of the Script functions you can use!
	namespace ChaiScriptAPIDoc
	{
		///ScriptFunction: Log a string
		/// \param s String to log
		void AnnDebugLog(const std::string& s);
		///ScriptFunction: Log a string
		/// \param v Vector to log
		void AnnDebugLog(const Ogre::Vector2& v);
		///ScriptFunction: Log a string
		/// \param v Vector to log
		void AnnDebugLog(const Ogre::Vector3& v);
		///ScriptFunction: Log a string
		/// \param q Quaternion to log
		void AnnDebugLog(const Ogre::Quaternion& q);
		///ScriptFunction: Log a string
		/// \param a Angle to log
		void AnnDebugLog(const Ogre::Radian& a);
		///ScriptFunction: Log a string
		/// \param a Angle to log
		void AnnDebugLog(const Ogre::Degree& a);

		///ScriptFunction: get a GameObject from it's ID
		/// \param id The string ID of the object you want
		std::shared_ptr<AnnGameObject> AnnGetGameObject(std::string id);

		///ScriptFunction: Jump the level manager to another level
		/// \param id The ID number of the level
		void AnnJumpLevel(Annwvyn::level_id id);
	}
}