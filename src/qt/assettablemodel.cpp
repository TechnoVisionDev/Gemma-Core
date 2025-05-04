// Copyright (c) 2011-2016 The Bitcoin Core developers
// Copyright (c) 2017-2020 The Gemma Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "assettablemodel.h"
#include "assetrecord.h"

#include "guiconstants.h"
#include "guiutil.h"
#include "walletmodel.h"
#include "wallet/wallet.h"

#include "core_io.h"

#include "amount.h"
#include "assets/assets.h"
#include "validation.h"
#include "platformstyle.h"

#include <QDebug>
#include <QStringList>



//class AssetTablePriv {
//public:
    //AssetTablePriv(AssetTableModel *_parent) :
            //parent(_parent)
    //{
    //}

    //AssetTableModel *parent;

    //QList<AssetRecord> cachedBalances;

    // loads all current balances into cache
/*#ifdef ENABLE_WALLET
    void refreshWallet() {
        qDebug() << "AssetTablePriv::refreshWallet";
        cachedBalances.clear();
        auto currentActiveAssetCache = GetCurrentAssetCache();
        if (currentActiveAssetCache) {
            {
                LOCK(cs_main);
                std::map<std::string, CAmount> balances;
                std::map<std::string, std::vector<COutput> > outputs;
                if (!GetAllMyAssetBalances(outputs, balances)) {
                    qWarning("AssetTablePriv::refreshWallet: Error retrieving asset balances");
                    return;
                }
                std::set<std::string> setAssetsToSkip;
                auto bal = balances.begin();
                for (; bal != balances.end(); bal++) {
                    // retrieve units for asset
                    uint8_t units = OWNER_UNITS;
                    bool fIsAdministrator = true;
                    std::string ipfsHash = "";

                    if (setAssetsToSkip.count(bal->first))
                        continue;

                    if (!IsAssetNameAnOwner(bal->first)) {
                        // Asset is not an administrator asset
                        CNewAsset assetData;
                        //if (!currentActiveAssetCache->GetAssetMetaDataIfExists(bal->first, assetData)) {
                            //qWarning("AssetTablePriv::refreshWallet: Error retrieving asset data");
                            //return;
                        //}
                        if (!currentActiveAssetCache->GetAssetMetaDataIfExists(bal->first, assetData)) {
                            qWarning() << "AssetTablePriv::refreshWallet: Metadata missing for" << QString::fromStdString(bal->first) << ", skipping...";
                            continue; // Skip this asset, do not return early
                        }
                        units = assetData.units;
                        ipfsHash = assetData.strIPFSHash;
                        // If we have the administrator asset, add it to the skip listå
                        if (balances.count(bal->first + OWNER_TAG)) {
                            setAssetsToSkip.insert(bal->first + OWNER_TAG);
                        } else {
                            fIsAdministrator = false;
                        }
                    } else {
                        // Asset is an administrator asset, if we own assets that is administrators, skip this balance
                        std::string name = bal->first;
                        name.pop_back();
                        if (balances.count(name)) {
                            setAssetsToSkip.insert(bal->first);
                            continue;
                        }
                    }
                    cachedBalances.append(AssetRecord(bal->first, bal->second, units, fIsAdministrator, EncodeAssetData(ipfsHash)));
                }
            }
        }
    }
#endif*/


#ifdef ENABLE_WALLET

std::string generatePlaceholderIPFSHash() {
    return "QmYwAPJzv5CZsnAzt8auVZRnXpG5f6WHjy6s3qJsz1xDUZ";
}

class AssetTablePriv {
public:
    QList<AssetRecord> cachedBalances;
    AssetTableModel* model;

    AssetTablePriv(AssetTableModel* _model) : model(_model) {}

    int size() const {
        return cachedBalances.size();
    }

    AssetRecord* index(int idx) {
        if (idx >= 0 && idx < cachedBalances.size()) {
            return &cachedBalances[idx];
        }
        return nullptr;
    }

    void refreshWallet() {
        qDebug() << "AssetTablePriv::refreshWallet";

        model->beginResetModel();  // PATCH: Prevent stale index crashes

        cachedBalances.clear();

        auto currentActiveAssetCache = GetCurrentAssetCache();
        if (!currentActiveAssetCache) {
            model->endResetModel();  // PATCH
            return;
        }

        LOCK(cs_main);
        std::map<std::string, CAmount> balances;
        std::map<std::string, std::vector<COutput>> outputs;

        if (!GetAllMyAssetBalances(outputs, balances)) {
            qWarning("AssetTablePriv::refreshWallet: Error retrieving asset balances");
            model->endResetModel();  // PATCH
            return;
        }

        std::set<std::string> setAssetsToSkip;
        for (auto& bal : balances) {
            uint8_t units = OWNER_UNITS;
            bool fIsAdministrator = true;
            std::string ipfsHash;

            if (setAssetsToSkip.count(bal.first))
                continue;

            if (!IsAssetNameAnOwner(bal.first)) {
                CNewAsset assetData;
                if (!currentActiveAssetCache->GetAssetMetaDataIfExists(bal.first, assetData)) {
                    qWarning() << "AssetTablePriv::refreshWallet: Metadata missing for"
                               << QString::fromStdString(bal.first) << ", skipping...";
                    continue;
                }
                units = assetData.units;
                ipfsHash = assetData.strIPFSHash;

                if (balances.count(bal.first + OWNER_TAG))
                    setAssetsToSkip.insert(bal.first + OWNER_TAG);
                else
                    fIsAdministrator = false;
            } else {
                std::string name = bal.first;
                name.pop_back();  // Remove '~'
                if (balances.count(name)) {
                    setAssetsToSkip.insert(bal.first);
                    continue;
                }
            }

            if (ipfsHash.empty() || ipfsHash.length() < 10 || !std::all_of(ipfsHash.begin(), ipfsHash.end(), ::isalnum)) {
                ipfsHash = generatePlaceholderIPFSHash();
                qWarning() << "Asset" << QString::fromStdString(bal.first)
                           << "had an empty or invalid IPFS hash. Assigned placeholder:"
                           << QString::fromStdString(ipfsHash);
            }

            QString encodedIPFS;
            try {
                if (!ipfsHash.empty()) {
                    encodedIPFS = QString::fromStdString(EncodeAssetData(ipfsHash));
                }
                if (encodedIPFS.isEmpty())
                    throw std::runtime_error("EncodeAssetData returned empty");
            } catch (const std::exception& e) {
                qWarning() << "Bypass: IPFS encode failed for asset"
                           << QString::fromStdString(bal.first)
                           << "reason:" << e.what()
                           << "| Using raw hash string.";
                encodedIPFS = QString::fromStdString(ipfsHash);
            } catch (...) {
                qWarning() << "Bypass: Unknown error encoding IPFS hash for asset"
                           << QString::fromStdString(bal.first)
                           << "| Using raw hash string.";
                encodedIPFS = QString::fromStdString(ipfsHash);
            }

            if (ipfsHash == generatePlaceholderIPFSHash()) {
                qDebug() << "Note: Using placeholder IPFS hash for asset"
                         << QString::fromStdString(bal.first);
            }

            cachedBalances.append(AssetRecord(
                bal.first,
                bal.second,
                units,
                fIsAdministrator,
                encodedIPFS.toStdString()
            ));

            qDebug() << "Added asset to table:" << QString::fromStdString(bal.first)
                     << "units:" << units << "admin:" << fIsAdministrator
                     << "ipfs:" << encodedIPFS;
        }

        model->endResetModel();  // PATCH
    }
};

#endif // ENABLE_WALLET

AssetTableModel::AssetTableModel(WalletModel* parent) :
    QAbstractTableModel(parent),
    walletModel(parent),
    priv(new AssetTablePriv(this))
{
    columns << tr("Name") << tr("Quantity");
#ifdef ENABLE_WALLET
    priv->refreshWallet();
#endif
}

AssetTableModel::~AssetTableModel()
{
    delete priv;
}

void AssetTableModel::checkBalanceChanged() {
    qDebug() << "AssetTableModel::CheckBalanceChanged";
    Q_EMIT layoutAboutToBeChanged();
#ifdef ENABLE_WALLET
    priv->refreshWallet();
#endif
    if (priv->size() > 0) {
        QModelIndex topLeft = index(0, 0);
        QModelIndex bottomRight = index(priv->size() - 1, columns.size() - 1);
        if (topLeft.isValid() && bottomRight.isValid()) {
            Q_EMIT dataChanged(topLeft, bottomRight);
        }
    }
    Q_EMIT layoutChanged();
    qDebug() << "AssetTableModel::LayoutChanged emitted";
}

int AssetTableModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return priv->size();
}

int AssetTableModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return columns.size();
}

QVariant AssetTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    // PATCH: Defensive pointer check
    AssetRecord* rec = static_cast<AssetRecord*>(index.internalPointer());
    if (!rec || index.row() >= priv->size()) return QVariant();

    switch (role) {
    case AmountRole:
        return static_cast<qulonglong>(rec->quantity);
    case AssetNameRole:
        return QString::fromStdString(rec->name);
    case FormattedAmountRole:
        return QString::fromStdString(rec->formattedQuantity());
    case AdministratorRole:
        return rec->fIsAdministrator;
    case AssetIPFSHashRole:
        return QString::fromStdString(rec->ipfshash);
    case AssetIPFSHashDecorationRole:
        if (index.column() == Quantity) return QVariant();
        if (rec->ipfshash.empty() || rec->ipfshash.length() < 10 || !std::all_of(rec->ipfshash.begin(), rec->ipfshash.end(), ::isalnum))
            return QVariant();
        {
            QPixmap pixmap = darkModeEnabled ? QPixmap(":/icons/external_link_dark") : QPixmap(":/icons/external_link");
            return pixmap.isNull() ? QVariant() : pixmap;
        }
    case Qt::DecorationRole:
        if (index.column() == Quantity || !rec->fIsAdministrator) return QVariant();
        {
            QPixmap pixmap = darkModeEnabled ? QPixmap(":/icons/asset_administrator_dark") : QPixmap(":/icons/asset_administrator");
            return pixmap.isNull() ? QVariant() : pixmap;
        }
    case Qt::DisplayRole:
        if (index.column() == Name) return QString::fromStdString(rec->name);
        if (index.column() == Quantity) return QString::fromStdString(rec->formattedQuantity());
        break;
    case Qt::ToolTipRole:
        return formatTooltip(rec);
    case Qt::TextAlignmentRole:
        if (index.column() == Quantity) return Qt::AlignRight + Qt::AlignVCenter;
        break;
    default:
        return QVariant();
    }

    return QVariant();
}

QVariant AssetTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && section < columns.size())
            return columns.at(section);
        else if (orientation == Qt::Vertical)
            return section;
    } else if (role == Qt::SizeHintRole && orientation == Qt::Vertical) {
        return QSize(30, 50);
    } else if (role == Qt::TextAlignmentRole) {
        return orientation == Qt::Vertical ?
            Qt::AlignLeft + Qt::AlignVCenter :
            Qt::AlignHCenter + Qt::AlignVCenter;
    }
    return QVariant();
}

QModelIndex AssetTableModel::index(int row, int column, const QModelIndex& parent) const {
    Q_UNUSED(parent);
    AssetRecord* data = priv->index(row);
    if (data) return createIndex(row, column, data);
    return QModelIndex();
}

QString AssetTableModel::formatTooltip(const AssetRecord* rec) const {
    return formatAssetName(rec) + "\n" + formatAssetQuantity(rec) + "\n" + formatAssetData(rec);
}

QString AssetTableModel::formatAssetName(const AssetRecord* rec) const {
    return QString::fromStdString(rec->name);
}

QString AssetTableModel::formatAssetQuantity(const AssetRecord* rec) const {
    return QString::fromStdString(rec->formattedQuantity());
}

QString AssetTableModel::formatAssetData(const AssetRecord* rec) const {
    return QString::fromStdString(rec->ipfshash);
}
