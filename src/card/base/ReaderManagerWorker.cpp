/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerWorker.h"

#include "Initializer.h"
#include "Reader.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

static Initializer::Entry X([] {
			qRegisterMetaType<QSharedPointer<CardConnectionWorker> >("QSharedPointer<CardConnectionWorker>");
		});


ReaderManagerWorker::ReaderManagerWorker()
	: QObject()
	, mPlugIns()
{
}


ReaderManagerWorker::~ReaderManagerWorker()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& plugin : qAsConst(mPlugIns))
	{
		qCDebug(card) << "Shutdown plugin:" << plugin->metaObject()->className();
		plugin->shutdown();
	}
}


void ReaderManagerWorker::onThreadStarted()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	qCDebug(card) << "Thread started";
	registerPlugIns();

	Q_EMIT fireInitialized();
}


void ReaderManagerWorker::registerPlugIns()
{
	qCDebug(card) << "Try to register plugins";
	const auto& plugins = QPluginLoader::staticPlugins();
	for (const auto& plugin : plugins)
	{
		if (isPlugIn(plugin.metaData()))
		{
			qCDebug(card) << "Register and initialize plugin:" << plugin.metaData();
			ReaderManagerPlugIn* pluginInstance = qobject_cast<ReaderManagerPlugIn*>(plugin.instance());
			if (pluginInstance == nullptr)
			{
				qCWarning(card) << "Cannot cast to plugin instance:" << plugin.instance();
			}
			else
			{
				registerPlugIn(pluginInstance);
				pluginInstance->init();

				Q_EMIT firePluginAdded(pluginInstance->getInfo());
			}
		}
	}
}


bool ReaderManagerWorker::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.ReaderManagerPlugIn");
}


void ReaderManagerWorker::registerPlugIn(ReaderManagerPlugIn* pPlugIn)
{
	Q_ASSERT(pPlugIn != nullptr);
	Q_ASSERT(!mPlugIns.contains(pPlugIn));

	mPlugIns << pPlugIn;

	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderAdded, this, &ReaderManagerWorker::fireReaderAdded);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderRemoved, this, &ReaderManagerWorker::fireReaderRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderDeviceError, this, &ReaderManagerWorker::fireReaderDeviceError);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderPropertiesUpdated, this, &ReaderManagerWorker::fireReaderPropertiesUpdated);
	connect(pPlugIn, &ReaderManagerPlugIn::fireStatusChanged, this, &ReaderManagerWorker::fireStatusChanged);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardInserted, this, &ReaderManagerWorker::fireCardInserted);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardRemoved, this, &ReaderManagerWorker::fireCardRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardRetryCounterChanged, this, &ReaderManagerWorker::fireCardRetryCounterChanged);
}


void ReaderManagerWorker::startScan(ReaderManagerPlugInType pType, bool pAutoConnect)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& plugin : qAsConst(mPlugIns))
	{
		if (plugin->getInfo().getPlugInType() == pType)
		{
			qCDebug(card) << "Start scan on plugin:" << plugin->metaObject()->className();
			plugin->startScan(pAutoConnect);
		}
	}
}


void ReaderManagerWorker::stopScan(ReaderManagerPlugInType pType)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& plugin : qAsConst(mPlugIns))
	{
		if (plugin->getInfo().getPlugInType() == pType)
		{
			qCDebug(card) << "Stop scan on plugin:" << plugin->metaObject()->className();
			plugin->stopScan();
		}
	}
}


bool ReaderManagerWorker::isScanRunning() const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (const auto& plugin : qAsConst(mPlugIns))
	{
		if (plugin->isScanRunning())
		{
			return true;
		}
	}
	return false;
}


QVector<ReaderManagerPlugInInfo> ReaderManagerWorker::getPlugInInfos() const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QVector<ReaderManagerPlugInInfo> infos;
	infos.reserve(mPlugIns.size());
	for (const ReaderManagerPlugIn* plugIn : mPlugIns)
	{
		infos += plugIn->getInfo();
	}

	return infos;
}


QVector<ReaderInfo> ReaderManagerWorker::getReaderInfos(const ReaderFilter& pFilter) const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QVector<ReaderInfo> list;
	const QVector<ReaderManagerPlugIn*>& plugIns = pFilter.apply(mPlugIns);
	for (const auto& plugIn : plugIns)
	{
		const auto& readerList = plugIn->getReaders();
		for (const Reader* reader : readerList)
		{
			list += reader->getReaderInfo();
		}
	}
	return list;
}


ReaderInfo ReaderManagerWorker::getReaderInfo(const QString& pReaderName) const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	const Reader* reader = getReader(pReaderName);
	return reader ? reader->getReaderInfo() : ReaderInfo(pReaderName);
}


void ReaderManagerWorker::updateReaderInfo(const QString& pReaderName)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	Reader* reader = getReader(pReaderName);
	if (!reader)
	{
		qCWarning(card) << "Requested reader does not exist:" << pReaderName;
		return;
	}
	reader->update();
}


Reader* ReaderManagerWorker::getReader(const QString& pReaderName) const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& plugin : qAsConst(mPlugIns))
	{
		const auto& readerList = plugin->getReaders();
		for (Reader* reader : readerList)
		{
			if (reader->getName() == pReaderName)
			{
				return reader;
			}
		}
	}

	qCWarning(card) << "Requested reader does not exist:" << pReaderName;
	return nullptr;
}


void ReaderManagerWorker::createCardConnectionWorker(const QString& pReaderName)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QSharedPointer<CardConnectionWorker> worker;
	if (auto reader = getReader(pReaderName))
	{
		worker = reader->createCardConnectionWorker();
	}
	Q_EMIT fireCardConnectionWorkerCreated(worker);
}


void ReaderManagerWorker::connectReader(const QString& pReaderName)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	if (ConnectableReader* reader = qobject_cast<ConnectableReader*>(getReader(pReaderName)))
	{
		reader->connectReader();
	}
}


void ReaderManagerWorker::disconnectReader(const QString& pReaderName)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	if (ConnectableReader* reader = qobject_cast<ConnectableReader*>(getReader(pReaderName)))
	{
		reader->disconnectReader();
	}
}


void ReaderManagerWorker::disconnectAllReaders()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& info : getReaderInfos())
	{
		disconnectReader(info.getName());
	}
}


void ReaderManagerWorker::updateRetryCounters()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	const auto& readerInfos = getReaderInfos();
	for (const auto& readerInfo : readerInfos)
	{
		QSharedPointer<CardConnectionWorker> worker;
		if (const auto& reader = getReader(readerInfo.getName()))
		{
			worker = reader->createCardConnectionWorker();
			if (worker)
			{
				worker->updateRetryCounter();
			}
		}
	}
}
