//! @file ArmDebugger/ActionManager.cpp
//! @brief The definition of an object which maintains a collection of
//! QActions and their associated metadata.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include "ActionManager.hpp"

#include <QGuiApplication>
#include <QStyleHints>

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// ActionManager Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
ActionManager::ActionManager(QObject *ownerObject) :
    QObject(ownerObject)
{
}

QAction *ActionManager::getAction(uint32_t id) const
{
    return _actions.find(id)->second.Action;
}

void ActionManager::reindex()
{
    _actions.reindex();
}

void ActionManager::reserve(size_t hintSize)
{
    _actions.reserve(hintSize);
}

QAction *ActionManager::addAction(uint32_t id, uint32_t groupId,
                                  const QString &displayName)
{
    QAction *action = new QAction(displayName, this);

    _actions.push_back(id, ActionDefinition(id, groupId, action, QString()));

    return action;
}

QAction *ActionManager::addAction(uint32_t id, uint32_t groupId,
                                  const QString &displayName,
                                  const QString &iconPath)
{
    QAction *action = new QAction(displayName, this);

    auto &mapping = _actions.push_back(id, ActionDefinition(id, groupId,
                                                            action, iconPath));

    // Ensure the image is applied.
    updateImage(mapping.second);

    return action;
}

ActionManager::ActionDefinition::ActionDefinition() :
    Action(nullptr),
    Id(0),
    GroupId(0)
{
}

ActionManager::ActionDefinition::ActionDefinition(uint32_t id, uint32_t groupId,
                                                  QAction *action,
                                                  const QString &imagePath) :
    Action(action),
    ImageResourcePath(imagePath),
    Id(id),
    GroupId(groupId)
{
}

void ActionManager::updateImage(ActionDefinition &actionInfo)
{
    if (actionInfo.ImageResourcePath.isEmpty())
        return;

    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    {
        if (actionInfo.DarkImage.isNull())
        {
            // TODO: Make a dark theme image from the resource.
            actionInfo.DarkImage = QIcon(actionInfo.ImageResourcePath);
        }

        actionInfo.Action->setIcon(actionInfo.DarkImage);
    }
    else
    {
        if (actionInfo.LightImage.isNull())
        {
            actionInfo.LightImage = QIcon(actionInfo.ImageResourcePath);
        }

        actionInfo.Action->setIcon(actionInfo.LightImage);
    }
}

void ActionManager::setActionState(uint32_t *actionIds, size_t count, bool enable)
{
    if (count < 1)
        return;

    std::sort(actionIds, actionIds + count);

    uint32_t *nextStateId = actionIds;
    uint32_t *endStateId = actionIds + count;
    auto actionEnd = _actions.getMappings().end();

    for (auto actionMapping = _actions.getMappings().begin();
         (actionMapping != actionEnd) && (nextStateId != endStateId); )
    {
        if (actionMapping->first == *nextStateId)
        {
            QAction *action = actionMapping->second.Action;

            action->setEnabled(enable);

            // Move on to the next action.
            ++actionMapping;
            ++nextStateId;
        }
        else if (actionMapping->first < *nextStateId)
        {
            // Move on to the next mapping.
            ++actionMapping;
        }
        else
        {
            // The action didn't exist, move on to the next one.
            ++nextStateId;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

