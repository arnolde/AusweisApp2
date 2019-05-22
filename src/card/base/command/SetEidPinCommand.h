/*!
 * \brief Command to set the Eid PIN of a card.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BaseCardCommand.h"

class test_SetEidPinCommand;

namespace governikus
{

class SetEidPinCommand
	: public BaseCardCommand
{
	Q_OBJECT

	private:
		friend class ::test_SetEidPinCommand;

		QString mNewPin;
		quint8 mTimeoutSeconds;
		ResponseApdu mResponseApdu;

	protected:
		virtual void internalExecute() override;
		virtual ~SetEidPinCommand() override = default;

	public:
		SetEidPinCommand(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
				const QString& pNewPin, quint8 pTimeoutSeconds);

		const ResponseApdu& getResponseApdu() const;
};

} // namespace governikus
