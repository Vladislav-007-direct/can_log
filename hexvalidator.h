#ifndef HEXVALIDATOR_H
#define HEXVALIDATOR_H

#include <QObject>
#include <QValidator>

class HexIntegerValidator : public QValidator
{
    Q_OBJECT
public:
    explicit HexIntegerValidator(QObject *parent, int maxlen);

    QValidator::State validate(QString &input, int &) const;

    void setMaximum(uint maximum);

private:
    uint m_maximum = 0;
};

class HexStringValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HexStringValidator(QObject *parent, int maxlen);

    QValidator::State validate(QString &input, int &pos) const;

    void setMaxLength(int maxLength);

private:
    int m_maxLength = 0;
};

#endif // HEXVALIDATOR_H
