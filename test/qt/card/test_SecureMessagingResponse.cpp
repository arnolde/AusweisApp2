/*!
 * \brief Tests for SecureMessagingResponse
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "SecureMessagingResponse.h"


using namespace governikus;


class test_SecureMessagingResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void noData()
		{
			QByteArray bytes = QByteArray::fromHex("");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void noPaddingContentIndicator()
		{
			QByteArray bytes = QByteArray::fromHex("87060F1020304050990290008E0811213141516171819000");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void noProcessingStatus()
		{
			QByteArray bytes = QByteArray::fromHex("87060110203040508E0811213141516171819000");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void processingStatusWithWrongLength()
		{
			QByteArray bytes = QByteArray::fromHex("870601102030405099039090008E0811213141516171819000");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void noMac()
		{
			QByteArray bytes = QByteArray::fromHex("990290009000");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void macWithWrongLength()
		{
			QByteArray bytes = QByteArray::fromHex("990290008E07112131415161719000");

			SecureMessagingResponse response(bytes);
			QVERIFY(response.isInvalid());
		}


		void secureMessagingResponseNoCryptogram()
		{
			QByteArray bytes = QByteArray::fromHex("990290008E0811213141516171819000");

			SecureMessagingResponse response(bytes);
			QCOMPARE(response.getReturnCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getEncryptedData(), QByteArray());
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray());
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray::fromHex("9000"));
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray::fromHex("99029000"));
			QCOMPARE(response.getMac(), QByteArray::fromHex("1121314151617181"));
		}


		void secureMessagingResponse()
		{
			QByteArray bytes = QByteArray::fromHex("8706011020304050990290008E0811213141516171819000");

			SecureMessagingResponse response(bytes);
			QCOMPARE(response.getReturnCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getEncryptedData(), QByteArray::fromHex("1020304050"));
			QCOMPARE(response.getEncryptedDataObjectEncoded(), QByteArray::fromHex("8706011020304050"));
			QCOMPARE(response.getSecuredStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(response.getSecuredStatusCodeBytes(), QByteArray::fromHex("9000"));
			QCOMPARE(response.getSecuredStatusCodeObjectEncoded(), QByteArray::fromHex("99029000"));
			QCOMPARE(response.getMac(), QByteArray::fromHex("1121314151617181"));
		}


};

QTEST_GUILESS_MAIN(test_SecureMessagingResponse)
#include "test_SecureMessagingResponse.moc"
