#pragma once

#include <QJsonObject>
#include <QObject>
#include <QRandomGenerator>
#include <QTimer>

#include "messagefactory.h"

class DataGenerator : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerator(
        const QString& client_id,
        QObject* parent = nullptr);

    void Start();

    void Stop();

    bool IsActive() const;

signals:
    void DataGenerated(
        const QJsonObject& object);

private slots:
    void GenerateData();

private:
    void ScheduleNextGeneration();

private:
    QString client_id_;

    QTimer generation_timer_;

    bool active_ = false;
};
