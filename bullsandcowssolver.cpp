#include "bullsandcowssolver.h"

#include <iostream>

BullsAndCows::BullsAndCows()
 : randomNumberEngine(randomNumberGenerator())
{
    //nope
}

bool BullsAndCows::containsDuplicates(QString const &str)
{
    int length = str.length();
    for (int pos = 0; pos < length; ++pos)
        if (str.indexOf( str[pos], pos+1 ) != -1) //found duplicate from next pos
            return true;
    return false;
}


void BullsAndCows::setSecret(QString const &new_secret)
{
    //new_secret is checked in the UI
    secret = new_secret;
    length = secret.length();
}

void BullsAndCows::generateSecret(int new_length)
{
    //length is checked in the UI
    if (new_length > 10) {
        secret = QString();
        length = 0;
        return;
    }

    QString symbols = "0123456789";
    std::shuffle(symbols.begin(), symbols.end(), randomNumberEngine);

    secret = symbols.left(new_length);
    length = new_length;
}

void BullsAndCows::generateSolutions()
{
    solutions.clear();

    //0123
    int minSolution = 0;
    for (int digit = 0; digit < length; ++digit)
        minSolution = minSolution*10 + digit;

    //9876
    int maxSolution = 0;
    for (int digit = 0; digit < length; ++digit)
        maxSolution = maxSolution*10 + 9-digit;

    int currentSolution = minSolution;
    QString solutionStr;
    do {
        solutionStr = QString::number(currentSolution);
        if (solutionStr.length() < length)
            solutionStr.insert(0, '0');

        if (!containsDuplicates(solutionStr))
            solutions.append(solutionStr);

        ++currentSolution;
    } while (currentSolution <= maxSolution);
}

Answer BullsAndCows::scoreStrings(QString const &secret,
                    QString const &solution)
{
    //solution.length == secret.length (checked in the UI)
    Answer ans;

    for (int pos = 0; pos < length; ++pos)
    {
        if (secret[pos] == solution[pos])
            ans.bulls++;
        else if (secret.contains( solution[pos]))
            ans.cows++;
    }

    return ans;
}

void BullsAndCows::testSubOptimalSolution(int secretLength,
                                       int numberOfTests)
{
    int totalAttempts = 0;

    verbose = false;
    for (int i = 0; i < numberOfTests; ++i)
    {
        generateSecret(secretLength);
        subOptimalSolve();
        totalAttempts += attempts;
    }
    verbose = true;

    double avgAttempts = static_cast<double>(totalAttempts) / numberOfTests;

    QStringList output;
    output << "Average length of the solution:"
           << QString::number(totalAttempts) << "/"
           << QString::number(numberOfTests) << " = "
           << QString::number(avgAttempts, 'g', 4);
    emit newOutput(output);
}

void BullsAndCows::subOptimalSolve()
{
/*    Algorithm
 *    generate list of all passwords.
 *    while(passwords left in the list)
 *    do:
 *        set p = the first password in the list.
 *        get B, C for attempted password p
 *        if B = 4, then...
 *            return p
 *        else...
 *            eliminate all those which:
 *                do not match exactly B ordered digits
 *                do not match exactly B+C unordered digits
 *            eliminate p
 *        end if.
 *    loop.
 *
 *    Reference https://www.physicsforums.com/threads/guessing-game-algorithm.301897/
*/

    if (length == 0)
        return;

    generateSolutions();

    QString currentSolution;
    Answer ans;

    attempts = 0;
    while (solutions.size() != 0)
    {
        currentSolution = solutions.takeAt(
                    randomNumberEngine() % solutions.size() );
        ans = scoreStrings(secret, currentSolution);

        if (ans.bulls == length)
        {
            if (verbose) {
                QStringList output;
                output << "#" << currentSolution
                         << "\nfound in" << QString::number(attempts) << "attempts";
                emit newOutput(output);
            }

            return;
        }
        else
        {
            if (verbose) {
                QStringList output;
                output << "#" << currentSolution;
                output << "\nBulls:" << QString::number(ans.bulls)
                       << "Cows:" << QString::number(ans.cows);
                emit newOutput(output);
            }

            ++attempts;
            eliminateOrderedDigits(currentSolution, ans.bulls);
            eliminateUnorderedDigits(currentSolution, ans.bulls + ans.cows);
        }
    }
}

void BullsAndCows::eliminateOrderedDigits(QString const &solution, int count)
{
    if (count != 0)
    {
        int matchesCount;
        int currentSolution = 0;
        while (currentSolution < solutions.size())
        {
            matchesCount = 0;
            for (int pos = 0; pos < length; ++pos)
                if (solutions[currentSolution][pos] == solution[pos])
                    matchesCount++;

            if (matchesCount != count)
                solutions.removeAt(currentSolution);
            else
                ++currentSolution;
        }
    }
}

void BullsAndCows::eliminateUnorderedDigits(QString const &solution, int count)
{
    if (count != 0)
    {
        int sameDigitsCount;
        int currentSolution = 0;
        while (currentSolution < solutions.size())
        {
            sameDigitsCount = 0;
            for (int pos = 0; pos < length; ++pos)
                if (solution.contains( solutions[currentSolution][pos] ))
                    sameDigitsCount++;

            if (sameDigitsCount != count)
                solutions.removeAt(currentSolution);
            else
                ++currentSolution;
        }
    }
}

void BullsAndCows::testOptimalSolution(int secretLength,
                                       int numberOfTests)
{
    int totalAttempts = 0;

    verbose = false;
    for (int i = 0; i < numberOfTests; ++i)
    {
        generateSecret(secretLength);
        optimalSolve();
        totalAttempts += attempts;
    }
    verbose = true;

    double avgAttempts = static_cast<double>(totalAttempts) / numberOfTests;

    QStringList output;
    output << "Average length of the solution:"
           << QString::number(totalAttempts) << "/"
           << QString::number(numberOfTests) << " = "
           << QString::number(avgAttempts, 'g', 4);
    emit newOutput(output);
}

void BullsAndCows::optimalSolve()
{
/*    Algorithm
 *    generate list of all passwords.
 *    while(passwords left in the list)
 *    do:
 *        set p = the first password in the list.
 *        get B, C for attempted password p
 *        if B = 4, then...
 *            return p
 *        else...
 *            eliminate all those which:
 *                has other bulls and cows number
 *            eliminate p
 *        end if.
 *    loop.
 *
 *    my modification
*/

    if (length == 0)
        return;

    generateSolutions();

    QString currentSolution;
    Answer ans;

    attempts = 0;
    while (solutions.size() != 0)
    {
        currentSolution = solutions.takeAt(
                    randomNumberEngine() % solutions.size() );
        ans = scoreStrings(secret, currentSolution);

        if (ans.bulls == length)
        {
            if (verbose) {
                QStringList output;
                output << "#" << currentSolution
                         << "\nfound in" << QString::number(attempts) << "attempts";
                emit newOutput(output);
            }

            return;
        }
        else
        {
            if (verbose) {
                QStringList output;
                output << "#" << currentSolution;
                output << "\nBulls:" << QString::number(ans.bulls)
                       << "Cows:" << QString::number(ans.cows);
                emit newOutput(output);
            }

            ++attempts;
            eliminate(currentSolution, ans);
        }
    }
}

void BullsAndCows::eliminate(QString const &proposedSolution, Answer const &ans)
{
    Answer currentAns;

    int currentSolution = 0;
    while (currentSolution < solutions.size())
    {
        currentAns = scoreStrings(proposedSolution, solutions[currentSolution]);
        if (currentAns.bulls != ans.bulls
                || currentAns.cows != ans.cows) {
            solutions.removeAt(currentSolution);
        } else {
            ++currentSolution;
        }
    }
}

void BullsAndCows::proposeSolution(QString const &solution)
{
    Answer ans;
    ans = scoreStrings(secret, solution);

    if (ans.bulls == length)
    {
        if (verbose) {
            QStringList output;
            output << "#" << solution
                     << "is found. Congratulations!";
            emit newOutput(output);
        }
    }
    else
    {
        if (verbose) {
            QStringList output;
            output << "#" << solution;
            output << "\nBulls:" << QString::number(ans.bulls)
                   << "Cows:" << QString::number(ans.cows);
            emit newOutput(output);
        }
    }
}
