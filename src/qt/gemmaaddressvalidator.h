// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Gemma Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef gemma_QT_gemmaADDRESSVALIDATOR_H
#define gemma_QT_gemmaADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class GemmaAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GemmaAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Gemma address widget validator, checks for a valid gemma address.
 */
class GemmaAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GemmaAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // gemma_QT_gemmaADDRESSVALIDATOR_H
