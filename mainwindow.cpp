#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    aboutAct = new QAction("A&bout", this);
    aboutQtAct = new QAction("About Q&t", this);
    ui->menuBar->addAction(aboutAct);
    ui->menuBar->addAction(aboutQtAct);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAct, SIGNAL(triggered()), QApplication::instance(), SLOT(aboutQt()));


    QRegExpValidator *numberValidator =
               new QRegExpValidator(QRegExp("\\d{2,10}"), this);
    ui->numberEdit->setValidator(numberValidator);

    connect(ui->playerGuessesRadioButton, SIGNAL(clicked(bool)),
            this, SLOT(playerMode()));

    connect(ui->pcGuessesRadioButton, SIGNAL(clicked(bool)),
            this, SLOT(pcMode()));

    connect(ui->testCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(testMode(bool)));

    connect(ui->OKButton, SIGNAL(clicked(bool)),
            this, SLOT(play()));

    connect(ui->generateButton, SIGNAL(clicked(bool)),
            this, SLOT(generateSecret()));

    connect(ui->numberEdit, SIGNAL(textChanged(QString)),
            this, SLOT(checkNumbersLength(QString)));

    connect(ui->numberEdit, SIGNAL(returnPressed()),
            this, SLOT(numberEditReturnPressed()));

    connect(ui->lengthSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateMaxLength(int)));

    connect(&bnc, SIGNAL(newOutput(const QStringList &)),
            this, SLOT(updateResultsBrowser(const QStringList &)));

    testMode(false);

    playerMode();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::playerMode()
{
    ui->algorithmGroupBox->setHidden(true);

    ui->lengthSpinBox->setHidden(false);
    ui->lengthLabel->setHidden(false);
    ui->generateButton->setHidden(false);

    ui->numberEdit->setMaxLength(
                ui->lengthSpinBox->text().toInt() );

    clearSecret();
}


void MainWindow::pcMode()
{
    ui->algorithmGroupBox->setHidden(false);

    ui->lengthSpinBox->setHidden(true);
    ui->lengthLabel->setHidden(true);
    ui->generateButton->setHidden(true);

    ui->numberEdit->setMaxLength(10);

    clearSecret();
}

void MainWindow::updateResultsBrowser(const QStringList &strList)
{
    ui->resultsBrowser->appendPlainText(strList.join(' '));
    ui->resultsBrowser->appendPlainText("\n");
}

void MainWindow::play()
{
    bool pcIsGuessing = ui->pcGuessesRadioButton->isChecked();

    if (pcIsGuessing)
    {
        bool testMode = ui->testCheckBox->isChecked();

        ui->resultsBrowser->setPlainText("Solving...\n\n");
        ui->resultsBrowser->repaint();//avoid lag

        bool useOptimalAlgorithm =
                ui->optAlgorithmRadioButton->isChecked();

        bnc.setSecret( ui->numberEdit->text() );

        if (useOptimalAlgorithm)
            if (testMode)
                bnc.testOptimalSolution(
                            ui->lengthSpinBox->text().toInt(),
                            ui->NumberOfTestsSpinBox->text().toInt() );
            else
                bnc.optimalSolve();
        else
        {
            if (testMode)
                bnc.testSubOptimalSolution(
                            ui->lengthSpinBox->text().toInt(),
                            ui->NumberOfTestsSpinBox->text().toInt() );
            else
                bnc.subOptimalSolve();
        }

    } else { //player is guessing

        int length = ui->lengthSpinBox->text().toInt();
        QString currentSolution = ui->numberEdit->text();

        if (bnc.secretsLength() != length)
        {
            QMessageBox msgBox(this);
            msgBox.setText("Please generate secret at first.");
            msgBox.exec();
            return;
        }

        bnc.proposeSolution( currentSolution );

    }
}

void MainWindow::generateSecret()
{
    bnc.generateSecret( ui->lengthSpinBox->text().toInt() );

    ui->resultsBrowser->setPlainText("Secret of length "
                                + ui->lengthSpinBox->text()
                                + " generated.\n");
}

void MainWindow::clearSecret()
{
    bnc.setSecret("");
}

void MainWindow::updateMaxLength(int length)
{
    ui->numberEdit->setMaxLength(length);

    if (!ui->testCheckBox->isChecked()) //not in test mode
        checkNumbersLength(ui->numberEdit->text());
}

void MainWindow::checkNumbersLength(QString const &str)
{
    if ( str.isEmpty()
         || (ui->playerGuessesRadioButton->isChecked()
             && str.length() != ui->numberEdit->maxLength())
         || BullsAndCows::containsDuplicates( str ) )
        ui->OKButton->setDisabled(true);
    else
        ui->OKButton->setDisabled(false);
}

void MainWindow::testMode(bool isTestMode)
{
    if(isTestMode)
    {
        ui->lengthLabel->setHidden(false);
        ui->lengthSpinBox->setHidden(false);
        ui->NumberOfTestsLabel->setHidden(false);
        ui->NumberOfTestsSpinBox->setHidden(false);

        ui->numberLabel->setHidden(true);
        ui->numberEdit->setHidden(true);


        ui->OKButton->setEnabled(true);
    }
    else
    {
        ui->lengthLabel->setHidden(true);
        ui->lengthSpinBox->setHidden(true);
        ui->NumberOfTestsLabel->setHidden(true);
        ui->NumberOfTestsSpinBox->setHidden(true);

        ui->numberLabel->setHidden(false);
        ui->numberEdit->setHidden(false);

        checkNumbersLength(ui->numberEdit->text());
    }
}

void MainWindow::numberEditReturnPressed()
{
    if (ui->OKButton->isEnabled())
        ui->OKButton->click();
}

void MainWindow::about()
{
    QMessageBox::about(this, "Bulls and Cows",
                        "<h2>Bulls and Cows</h2>"
                        "<p>Bulls and Cows is an old code-breaking "
                        "mind or paper and pencil game for two or more players."
                        "<p>On a sheet of paper, the players each write a "
                        "4-digit secret number. The digits must be all different. "
                        "Then, in turn, the players try to guess their opponent's "
                        "number who gives the number of matches. If the matching "
                        "digits are in their right positions, they are \"bulls\", "
                        "if in different positions, they are \"cows\". Example:"
                        "<p>Secret number: 4271"
                        "<p>Opponent's try: 1234"
                        "<p>Answer: 1 bull and 2 cows. (The bull is \"2\", the cows "
                        "are \"4\" and \"1\".)"
                        "<p>The first one to reveal the other's secret number wins the game."
                        "<p>Created by Kostia Fursenko; 2016"
                        "<p><a href=\"mailto:spiritofsands@gmail.com\">spiritofsands@gmail.com</a>");
}
