#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"bullsandcowssolver.h"

#include <QMainWindow>
#include <QStringList>
#include <QAction>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void generateSecret();
    void playerMode();
    void pcMode();
    void testMode(bool);
    void play();
    void updateResultsBrowser(const QStringList &strList);

    void numberEditReturnPressed();
    void updateMaxLength(int);
    void checkNumbersLength(QString const &str);

    void about();

private:
    Ui::MainWindow *ui;

    BullsAndCows bnc;

    QAction *aboutAct;
    QAction *aboutQtAct;

    void clearSecret();
    bool containsDuplicates(QString const &str);
};

#endif // MAINWINDOW_H
