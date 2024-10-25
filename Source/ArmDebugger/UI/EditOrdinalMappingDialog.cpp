//! @file ArmDebugger/UI/EditOrdinalMappingDialog.cpp
//! @brief The definition of a dialog which can edit sets of name/number
//! mappings.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2024
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Header File Includes
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <set>

#include <QAbstractTableModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>

#include "Ag/Core/Binary.hpp"
#include "Ag/Core/Utils.hpp"

#include "EditOrdinalMappingDialog.hpp"

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

namespace Mo {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Data Types
////////////////////////////////////////////////////////////////////////////////
using OrdinalMapping = std::pair<uint32_t, QString>;
using MappingCollection = std::vector<OrdinalMapping>;
using MappingIterator = MappingCollection::iterator;
using MappingCIterator = MappingCollection::const_iterator;
using IdSet = std::set<uint32_t>;
using SymbolSet = QSet<QString>;

struct GenerateNewMappings
{
private:
    QString _prefixFormat;
    IdSet &_usedIds;
    SymbolSet &_usedNames;
    uint32_t _idSeed;
    uint32_t _symbolSeed;
public:
    GenerateNewMappings(const QString &prefix, IdSet &usedIds, SymbolSet &usedNames) :
        _prefixFormat(prefix + "%1"),
        _usedIds(usedIds),
        _usedNames(usedNames),
        _idSeed(0),
        _symbolSeed(1)
    {
    }

    OrdinalMapping operator()()
    {
        while (_usedIds.find(_idSeed) != _usedIds.end())
            ++_idSeed;

        QString symbol;

        do
        {
            symbol = _prefixFormat.arg(_symbolSeed++);

        } while (_usedNames.contains(symbol));

        _usedIds.insert(_idSeed);
        _usedNames.insert(symbol);

        return OrdinalMapping(_idSeed, symbol);
    }
};

struct CompareByID
{
private:
    bool _ascending;
public:
    CompareByID(bool ascending) :
        _ascending(ascending)
    {
    }

    bool operator()(const OrdinalMapping &lhs, const OrdinalMapping &rhs) const
    {
        return _ascending ? (lhs.first < rhs.first) : (lhs.first > rhs.first);
    }
};

struct CompareBySymbol
{
private:
    bool _ascending;
public:
    CompareBySymbol(bool ascending) :
        _ascending(ascending)
    {
    }

    bool operator()(const OrdinalMapping &lhs, const OrdinalMapping &rhs) const
    {
        return _ascending ? (lhs.second < rhs.second) : (lhs.second > rhs.second);
    }
};

class OrdinalModel : public QAbstractTableModel
{
private:
    // Internal Fields
    MappingCollection _mappings;
    IdSet _usedIds;
    SymbolSet _usedSymbols;
    QString _headers[2];

    // Internal Functions
    uint32_t getHighestID() const
    {
        uint32_t highestID = 0;

        if (_usedIds.empty() == false)
        {
            // Get the largest ID defined.
            auto last = _usedIds.end();
            --last;

            highestID = *last;
        }

        return highestID;
    }

    int getIDPrecision() const
    {
        int precision = 2;

        // Calculate the most significant bit set.
        int msb;
        if (Ag::Bin::bitScanReverse(getHighestID(), msb))
        {
            // Round up to a whole number of hex digits.
            precision = std::max((msb + 3) / 4, precision);
        }

        return precision;
    }

    bool isValidIndex(const QModelIndex &index, MappingIterator &pos)
    {
        bool isValid;

        if ((index.parent().isValid() == false) &&
            (index.column() >= 0) &&
            (index.column() < 2) &&
            (index.row() >= 0) &&
            (static_cast<size_t>(index.row()) < _mappings.size()))
        {
            isValid = true;
            pos = _mappings.begin() + index.row();
        }
        else
        {
            isValid = false;
            pos = _mappings.end();
        }

        return isValid;
    }

    bool isValidIndex(const QModelIndex &index, MappingCIterator &pos) const
    {
        bool isValid;

        if ((index.parent().isValid() == false) &&
            (index.column() >= 0) &&
            (index.column() < 2) &&
            (index.row() >= 0) &&
            (static_cast<size_t>(index.row()) < _mappings.size()))
        {
            isValid = true;
            pos = _mappings.begin() + index.row();
        }
        else
        {
            isValid = false;
            pos = _mappings.end();
        }

        return isValid;
    }
public:
    OrdinalModel(QObject *owner, const QString &idName, const QString &mappingName) :
        QAbstractTableModel(owner)
    {
        _headers[0] = idName;
        _headers[1] = mappingName;
    }

    OrdinalMap getMappings() const
    {
        return OrdinalMap(_mappings.begin(), _mappings.end());
    }

    // Operations
    QModelIndex newRow()
    {
        int rowIndex = static_cast<int>(_mappings.size());

        beginInsertRows(QModelIndex(), rowIndex, rowIndex);
        _mappings.emplace_back(getHighestID() + 1, QString());
        endInsertRows();
    }

    void setMapping(const OrdinalMap &mappings)
    {
        _mappings.clear();
        _usedIds.clear();
        _usedSymbols.clear();
        _mappings.reserve(mappings.size());

        beginResetModel();

        for (auto &[id, symbol] : mappings)
        {
            if ((_usedIds.find(id) == _usedIds.end()) &&
                (_usedSymbols.constFind(symbol) == _usedSymbols.constEnd()))
            {
                _mappings.emplace_back(id, symbol);
                _usedIds.insert(id);
                _usedSymbols.insert(symbol);
            }
        }

        endResetModel();
    }

    void mergeMappings(const OrdinalMap &mappings, bool overwriteDuplicates)
    {
        if (mappings.empty())
        {
            return;
        }
        else if (_mappings.empty())
        {
            setMapping(mappings);
        }

        OrdinalMap importMap;

        if (overwriteDuplicates)
        {
            // Overwrite with new duplicates.
            QSet<QString> importedSymbols;

            for (const auto &[key, symbol] : mappings)
            {
                importedSymbols.insert(symbol);
                importMap[key] = symbol;
            }

            for (const auto &[key, symbol] : _mappings)
            {
                if ((importMap.find(key) == importMap.end()) &&
                    (importedSymbols.contains(symbol) == false))
                {
                    importMap[key] = symbol;
                }
            }
        }
        else
        {
            // Discard new duplicates.
            importMap = getMappings();

            for (const auto &[key, symbol] : mappings)
            {
                if ((_usedIds.find(key) == _usedIds.end()) &&
                    (_usedSymbols.contains(symbol) == false))
                {
                    importMap[key] = symbol;
                }
            }
        }

        // Overwrite the entire model with the merged set.
        setMapping(importMap);
    }

    // Overrides
    Q_INVOKABLE virtual int rowCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? static_cast<int>(0) :
                                  static_cast<int>(_mappings.size());
    }

    Q_INVOKABLE virtual int columnCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? 0 : 2;
    }

    Q_INVOKABLE virtual QVariant data(const QModelIndex &index,
                                      int role /*= Qt::DisplayRole*/) const override
    {
        QVariant value;

        static const std::string_view displayFormat("0x{0:X8}");
        static const std::string_view editFormat("{0:X8}");

        MappingCIterator mapping;

        if (isValidIndex(index, mapping))
        {
            switch (role)
            {
            case Qt::DisplayRole:
            case Qt::EditRole:
                if (index.column() == 0)
                {
                    // Format as hex with Ag, as Qt uses lower case letters.
                    std::string buffer;

                    Ag::appendFormat(Ag::LocaleInfo::getNeutral(),
                                     (role == Qt::DisplayRole) ? displayFormat : editFormat,
                                     buffer, { mapping->first });

                    value = QString::fromUtf8(buffer.c_str(), buffer.length());
                }
                else
                {
                    value = mapping->second;
                }
                break;

            case Qt::TextAlignmentRole:
                if (index.column() == 0)
                {
                    value = static_cast<int>(Qt::Alignment(Qt::AlignRight | Qt::AlignCenter));
                }
                break;
            }
        }

        return value;
    }

    Q_INVOKABLE virtual bool setData(const QModelIndex &index, const QVariant &value,
                                     int role /*= Qt::EditRole*/) override
    {
        MappingIterator mapping;
        bool isChanged = false;

        if ((role == Qt::EditRole) && isValidIndex(index, mapping))
        {
            if (index.column() == 0)
            {
                // Updating the ordinal.
                bool isOK = false;
                uint32_t ordinal = 0;

                if (value.canConvert<QString>())
                {
                    // Assume the string is in hex format
                    ordinal = value.toString().toUInt(&isOK, 16);
                }

                if (isOK)
                {
                    if (ordinal == mapping->first)
                    {
                        // Nothing changed, don't sweat it.
                        isChanged = true;
                    }
                    else if (_usedIds.find(ordinal) == _usedIds.end())
                    {
                        // The value has changed.
                        isChanged = true;
                        _usedIds.erase(mapping->first);
                        _usedIds.insert(ordinal);
                        mapping->first = ordinal;
                    }
                }
            }
            else if (index.column() == 1)
            {
                // Updating the symbol.
                QString symbol = value.toString();

                if (symbol == mapping->second)
                {
                    // No change, don't sweat it.
                    isChanged = true;
                }
                else if ((symbol.isEmpty() == false) &&
                         (_usedSymbols.constFind(symbol) == _usedSymbols.constEnd()))
                {
                    // The symbol is a non-empty, unique value.
                    isChanged = true;
                    _usedSymbols.remove(mapping->second);
                    _usedSymbols.insert(symbol);
                    mapping->second = symbol;
                }
            }
        }

        if (isChanged)
        {
            static const QList<int> rolesChanged = { Qt::DisplayRole };

            emit dataChanged(index, index, rolesChanged);
        }

        return isChanged;
    }

    Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
                                            int role /*= Qt::DisplayRole*/) const
    {
        QVariant value;
        bool hasValue = false;

        if ((orientation == Qt::Horizontal) &&
            (section >= 0) &&
            (section < std::size(_headers)))
        {
            switch (role)
            {
            case Qt::DisplayRole:
                value = _headers[section];
                hasValue = true;
                break;
            }
        }

        if (!hasValue)
        {
            value = QAbstractTableModel::headerData(section, orientation, role);
        }

        return value;
    }

    Q_INVOKABLE virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        MappingCIterator mapping;
        Qt::ItemFlags flags;

        if (isValidIndex(index, mapping))
        {
            flags = Qt::ItemFlag::ItemIsEditable |
                Qt::ItemFlag::ItemIsEnabled |
                Qt::ItemFlag::ItemIsSelectable;
        }
        else
        {
            flags = QAbstractTableModel::flags(index);
        }

        return flags;
    }

    Q_INVOKABLE virtual bool insertRows(int row, int count,
                                        const QModelIndex &parent /*= QModelIndex()*/) override
    {
        bool isInserted = false;

        if ((parent.isValid() == false) && (count > 0))
        {
            size_t insertAt = std::min(Ag::toSize(row), _mappings.size());
            size_t originalSize = _mappings.size();
            row = static_cast<int>(insertAt);

            beginInsertRows(parent, row, row + count - 1);

            // Make space for the new items.
            _mappings.resize(originalSize + count);

            // Move old items out of the way.
            std::move_backward(_mappings.begin() + insertAt,
                               _mappings.begin() + originalSize,
                               _mappings.begin() + insertAt + count);

            // Generate new symbols to insert into the table.
            std::generate_n(_mappings.begin() + insertAt, count,
                            GenerateNewMappings("Symbol_", _usedIds, _usedSymbols));

            endInsertRows();
            isInserted = true;
        }

        return isInserted;
    }

    Q_INVOKABLE virtual bool removeRows(int row, int count,
                                        const QModelIndex &parent /*= QModelIndex()*/) override
    {
        bool isRemoved = false;

        if ((parent.isValid() == false) && (count > 0) &&
            (row >= 0) && (Ag::toSize(row) < _mappings.size()))
        {
            beginRemoveRows(parent, row, row + count - 1);

            auto firstToRemove = _mappings.begin() + row;
            auto endToRemove = _mappings.begin() + row + count;

            for (auto pos = firstToRemove; pos != endToRemove; ++pos)
            {
                _usedIds.erase(pos->first);
                _usedSymbols.remove(pos->second);
            }

            _mappings.erase(firstToRemove, endToRemove);

            endRemoveRows();
        }

        return isRemoved;
    }

    Q_INVOKABLE virtual void sort(int column, Qt::SortOrder order /*= Qt::AscendingOrder*/)
    {
        bool isSorted = true;

        if (column == 0)
        {
            isSorted = !_mappings.empty();
            std::sort(_mappings.begin(), _mappings.end(),
                      CompareByID(order == Qt::AscendingOrder));
        }
        else if (column == 1)
        {
            isSorted = !_mappings.empty();
            std::sort(_mappings.begin(), _mappings.end(),
                      CompareBySymbol(order == Qt::AscendingOrder));
        }
        else
        {
            isSorted = false;
        }

        if (isSorted)
        {
            emit dataChanged(index(0, 0),
                             index(static_cast<int>(_mappings.size() - 1), 1),
                             { Qt::DisplayRole });
        }
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
bool hasOverlap(const OrdinalMap &lhs, const OrdinalMap &rhs)
{
    if (lhs.empty() || rhs.empty())
        return false;

    if (lhs.begin()->second == rhs.begin()->second)
        return true;

    auto lhsEnd = lhs.end();
    auto rhsEnd = rhs.end();
    QSet<QString> symbols;

    symbols.insert(lhs.begin()->second);
    symbols.insert(rhs.begin()->second);

    for (auto lhsPos = lhs.begin(), rhsPos = rhs.begin();
         (lhsPos != lhsEnd) && (rhsPos != rhsEnd); )
    {
        if (lhsPos->first == rhsPos->first)
        {
            return true;
        }
        else if (lhsPos->first < rhsPos->first)
        {
            ++lhsPos;

            if (lhsPos != lhsEnd)
            {
                if (symbols.contains(lhsPos->second))
                {
                    return true;
                }
                else
                {
                    symbols.insert(lhsPos->second);
                }
            }
        }
        else
        {
            ++rhsPos;

            if (rhsPos != rhsEnd)
            {
                if (symbols.contains(rhsPos->second))
                {
                    return true;
                }
                else
                {
                    symbols.insert(rhsPos->second);
                }
            }
        }
    }

    return false;
}

} // Anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// EditOrdinalMappingDialog Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
EditOrdinalMappingDialog::EditOrdinalMappingDialog(QWidget *ownerWidget) :
    QDialog(ownerWidget),
    _ordinalModel(nullptr)
{
    _ui.setupUi(this);

    _ui._addButton->setText(QString());
    _ui._addButton->setIcon(QIcon(":/images/AddItem.svg"));

    _ui._removeButton->setText(QString());
    _ui._removeButton->setIcon(QIcon(":/images/RemoveItem.svg"));

    connect(_ui._addButton, &QPushButton::clicked,
            this, &EditOrdinalMappingDialog::onAddRow);
    connect(_ui._removeButton, &QPushButton::clicked,
            this, &EditOrdinalMappingDialog::onDeleteRow);
    connect(_ui._importButton, &QPushButton::clicked,
            this, &EditOrdinalMappingDialog::onImport);
    connect(_ui._exportButton, &QPushButton::clicked,
            this, &EditOrdinalMappingDialog::onExport);
}

OrdinalMap EditOrdinalMappingDialog::getMappings() const
{
    OrdinalMap mappings;

    if (auto *model = dynamic_cast<OrdinalModel *>(_ui._mappingTableView->model()))
    {
        mappings = model->getMappings();
    }

    return mappings;
}

void EditOrdinalMappingDialog::initialise(const QString &domain, const QString &idName,
                                          const QString &valueName, const OrdinalMap &ordinals,
                                          const QString &fileExt)
{
    setWindowTitle(tr("Edit %1s").arg(domain));
    _ui._importButton->setToolTip(tr("Import %1s from a file").arg(domain));
    _ui._exportButton->setToolTip(tr("Export %1s to a file").arg(domain));
    _domain = domain;
    _fileExt = fileExt;

    OrdinalModel *model = new OrdinalModel(this, idName, valueName);
    model->setMapping(ordinals);

    // Ensure the new model is in place before triggering any events, but
    // that we retain the old model so we can dispose of it.
    QAbstractTableModel *tableModel = model;
    std::swap(tableModel, _ordinalModel);
    _ui._mappingTableView->setModel(_ordinalModel);

    if (tableModel != nullptr)
    {
        delete tableModel;
    }
}

void EditOrdinalMappingDialog::onAddRow()
{
    if (auto *model = dynamic_cast<OrdinalModel *>(_ui._mappingTableView->model()))
    {
        int rowCount = model->rowCount(QModelIndex());

        if (model->insertRow(rowCount))
        {
            _ui._mappingTableView->selectRow(rowCount);
        }
    }
}

void EditOrdinalMappingDialog::onDeleteRow()
{
    if (auto *model = dynamic_cast<OrdinalModel *>(_ui._mappingTableView->model()))
    {
        QModelIndex rowIndex = _ui._mappingTableView->currentIndex();

        model->removeRow(rowIndex.row(), rowIndex.parent());
    }
}

void EditOrdinalMappingDialog::onImport()
{
    if (auto *model = dynamic_cast<OrdinalModel *>(_ui._mappingTableView->model()))
    {
        // Prompt the user to select a file.
        QFileDialog importDialog(this);
        importDialog.setFileMode(QFileDialog::ExistingFile);
        importDialog.setNameFilters({ tr("%1 files (*.%2)").arg(_domain).arg(_fileExt),
                                      tr("All files (*)") });
        importDialog.setWindowTitle(tr("Import %1 File").arg(_domain));

        // Read in the file.
        QString message;
        OrdinalMap ordinals;

        if (importDialog.exec() == QDialog::Accepted)
        {
            if (tryReadOrdinals(importDialog.selectedFiles().first(),
                                ordinals, message))
            {
                OrdinalMap currentMappings = model->getMappings();
                bool overwriteDuplicates = false;

                if (hasOverlap(ordinals, currentMappings))
                {
                    QMessageBox mergeQuery(this);
                    mergeQuery.setWindowTitle(tr("Merge Imported %1s").arg(_domain));
                    mergeQuery.setText(tr("Some imported mappings overlap with existing ones.\n\n"
                                          "Would you like to overwrite duplicates with new "
                                          "mappings or discard new duplicate mappings?"));
                    mergeQuery.addButton(tr("Overwrite"), QMessageBox::DestructiveRole);
                    mergeQuery.addButton(tr("Discard"), QMessageBox::ApplyRole);
                    mergeQuery.addButton(tr("Cancel"), QMessageBox::RejectRole);

                    int result = mergeQuery.exec();

                    if (result == QMessageBox::RejectRole)
                    {
                        // The user cancelled, do nothing.
                        return;
                    }

                    overwriteDuplicates = (result == QMessageBox::DestructiveRole);
                }

                model->mergeMappings(ordinals, overwriteDuplicates);
            }
            else
            {
                QMessageBox::warning(this, tr("%1 Import Failure").arg(_domain),
                                     message, QMessageBox::Ok);
            }
        }
    }
}

void EditOrdinalMappingDialog::onExport()
{
    if (auto *model = dynamic_cast<OrdinalModel *>(_ui._mappingTableView->model()))
    {
        // Prompt the user to select a file to write.
        QFileDialog exportDialog(this);
        exportDialog.setFileMode(QFileDialog::AnyFile);
        exportDialog.setAcceptMode(QFileDialog::AcceptSave);
        exportDialog.setNameFilters({ tr("%1 files (*.%2)").arg(_domain).arg(_fileExt),
                                      tr("All files (*)") });
        exportDialog.setWindowTitle(tr("Export %1 File").arg(_domain));

        if (exportDialog.exec() == QDialog::Accepted)
        {
            QString message;
            OrdinalMap mappings = model->getMappings();

            if (tryWriteOrdinals(exportDialog.selectedFiles().first(),
                                 mappings, message) == false)
            {
                QMessageBox::warning(this, tr("%1 Export Failure").arg(_domain),
                                     message, QMessageBox::Ok);
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Global Function Definitions
////////////////////////////////////////////////////////////////////////////////

} // namespace Mo
////////////////////////////////////////////////////////////////////////////////

