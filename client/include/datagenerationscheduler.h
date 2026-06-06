#pragma once

#include <QJsonObject>
#include <QObject>
#include <QRandomGenerator>
#include <QTimer>

#include "datagenerator.h"

class DataGenerationScheduler : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerationScheduler(
        QObject* parent = nullptr);

    void start();

    void stop();

    bool isActive() const;

signals:
    void dataGenerated(
        const QJsonObject& object);

private slots:
    void generateData();

private:
    void scheduleNextGeneration();

private:
    QTimer generation_timer_;

    bool active_ = false;
};
