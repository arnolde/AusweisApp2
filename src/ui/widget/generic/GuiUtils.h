/*!
 * \brief Gui utility functions.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"

#include <QWidget>

namespace governikus
{

class GuiUtils
	: private QObject
{
	Q_OBJECT

	public:
		static bool showPinCanPukErrorDialog(CardReturnCode pReturnCode, bool pCanAllowedMode, QWidget* pParent);
		static bool showWrongPinBlockedDialog(QWidget* pParent);
};

} // namespace governikus
