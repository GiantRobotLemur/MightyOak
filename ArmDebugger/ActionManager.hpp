//! @file ArmDebugger/ActionManager.hpp
//! @brief The declaration of an object which maintains a collection of
//! QActions and their associated metadata.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ARM_DEBUGGER_ACTION_MANAGER_HPP__
#define __ARM_DEBUGGER_ACTION_MANAGER_HPP__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <QObject>
#include <QAction>

#include "Ag/Core/LinearSortedMap.hpp"
#include "Ag/Core/Utils.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
//! @brief An object which maintains a collection of QActions and their
//! associated metadata.
class ActionManager : public QObject
{
Q_OBJECT
public:
    // Construction/Destruction
    ActionManager(QObject *ownerObject = nullptr);
    virtual ~ActionManager() = default;

    // Accessors
    QAction *getAction(uint32_t id) const;

    template<typename TAction> QAction *getAction(TAction id) const
    {
        return getAction(Ag::toScalar(id));
    }

    // Operations
    void reindex();
    void reserve(size_t hintSize);
    QAction *addAction(uint32_t id, uint32_t groupId, const QString &displayName);
    QAction *addAction(uint32_t id, uint32_t groupId, const QString &displayName,
                       const QString &iconPath);

    template<typename TAction>
    void updateActionState(bool enabled, const std::initializer_list<TAction> &ids)
    {
        std::vector<uint32_t> rawIds;
        rawIds.reserve(ids.size());

        for (auto id : ids)
        {
            rawIds.push_back(Ag::toScalar(id));
        }

        setActionState(rawIds.data(), rawIds.size(), enabled);
    }

    template<typename TAction, typename TGroup>
    QAction *addAction(TAction id, TGroup groupId, const QString &displayName)
    {
        return addAction(Ag::toScalar(id), Ag::toScalar(groupId), displayName);
    }

    template<typename TAction, typename TGroup>
    QAction *addAction(TAction id, TGroup groupId, const QString &displayName,
                       const QString &iconPath)
    {
        return addAction(Ag::toScalar(id), Ag::toScalar(groupId),
                         displayName, iconPath);
    }
private:
    // Internal Types
    struct ActionDefinition
    {
        QAction *Action;
        QString ImageResourcePath;
        QIcon LightImage;
        QIcon DarkImage;
        uint32_t Id;
        uint32_t GroupId;

        ActionDefinition();
        ActionDefinition(uint32_t id, uint32_t groupId,
                         QAction *action, const QString &imagePath);
    };

    using ActionMap = Ag::LinearSortedMap<uint32_t, ActionDefinition>;

    // Internal Functions
    void setActionState(uint32_t *actionIds, size_t count, bool enable);
    void updateImage(ActionDefinition &actionInfo);

    // Internal Fields
    ActionMap _actions;
};

////////////////////////////////////////////////////////////////////////////////
// Function Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Templates
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo

#endif // Header guard
////////////////////////////////////////////////////////////////////////////////
