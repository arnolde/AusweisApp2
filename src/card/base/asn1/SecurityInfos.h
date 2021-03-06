/*!
 * \brief Implementation of SecurityInfos
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ChipAuthenticationInfo.h"
#include "PaceInfo.h"
#include "SecurityInfo.h"

#include <QByteArray>
#include <QSharedPointer>
#include <QVector>


namespace governikus
{
typedef struct stack_st_securityinfo_st securityinfos_st;

DECLARE_ASN1_FUNCTIONS(securityinfos_st)
DECLARE_ASN1_OBJECT(securityinfos_st)

/*!
 * SecurityInfos ::= SET OF SecurityInfo

 * defined in TR 3110 Part 3
 */
class SecurityInfos
{
	Q_DISABLE_COPY(SecurityInfos)
	friend class QSharedPointer<SecurityInfos>;

	const QByteArray mContentBytes;
	const QVector<QSharedPointer<const SecurityInfo> > mSecurityInfos;
	const QVector<QSharedPointer<const PaceInfo> > mPaceInfos;
	const QVector<QSharedPointer<const ChipAuthenticationInfo> > mChipAuthenticationInfos;

	SecurityInfos(const QByteArray& pBytes,
			const QVector<QSharedPointer<const SecurityInfo> >& pSecurityInfos,
			const QVector<QSharedPointer<const PaceInfo> >& pPaceInfos,
			const QVector<QSharedPointer<const ChipAuthenticationInfo> >& pChipAuthenticationInfos);

	public:
		static QSharedPointer<SecurityInfos> fromHex(const QByteArray& pHexString);
		static QSharedPointer<SecurityInfos> decode(const QByteArray& pBytes);

		const QByteArray& getContentBytes() const;
		const QVector<QSharedPointer<const SecurityInfo> >& getSecurityInfos() const;
		const QVector<QSharedPointer<const PaceInfo> >& getPaceInfos() const;
		const QVector<QSharedPointer<const ChipAuthenticationInfo> >& getChipAuthenticationInfos() const;
};


/*!
 * EFCardAccess ::= SecurityInfos
 *
 * defined in TR 3110 Part 3
 */
typedef SecurityInfos EFCardAccess;


} // namespace governikus
