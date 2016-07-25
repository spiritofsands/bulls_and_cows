#ifndef BULLSANDCOWSSOLVER_H
#define BULLSANDCOWSSOLVER_H

#include <QObject>
#include <QList>

struct Answer
{
    int bulls = 0;
    int cows = 0;
};

class BullsAndCows : public QObject
{
    Q_OBJECT

public:
    BullsAndCows();
    int secretsLength() { return length; }
    //pc
    void setSecret(QString const &secret);
    void testSubOptimalSolution(int secretLength,
                             int numberOfTests);
    void subOptimalSolve();
    void testOptimalSolution(int secretLength,
                             int numberOfTests);
    void optimalSolve();

    //player
    void generateSecret(int length);
    void proposeSolution(QString const &solution);

    static bool containsDuplicates(QString const &str);

signals:
    void newOutput(const QStringList &strList);

private:
    QString secret;
    int length;
    int attempts;
    bool verbose = true;

    const QString digits = "0123456789";

    QList<QString> solutions;

    std::random_device randomNumberGenerator;
    std::mt19937 randomNumberEngine;

    Answer scoreStrings(QString const &secret,
                        QString const &solution);
    void generateSolutions();
    void eliminateOrderedDigits(QString const &, int);
    void eliminateUnorderedDigits(QString const &, int);
    void eliminate(QString const &solution, Answer const &ans);
};

#endif // BULLSANDCOWSSOLVER_H
