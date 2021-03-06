/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "command/DidAuthenticateEAC1Command.h"
#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

class test_StateDidAuthenticateEac1;

namespace governikus
{

class StateDidAuthenticateEac1
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDidAuthenticateEac1;

	explicit StateDidAuthenticateEac1(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onCardCommandDone(QSharedPointer<BaseCardCommand> pCommand);

};

} // namespace governikus
