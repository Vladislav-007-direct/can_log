#ifndef UI_PERIPHERYROW_H
#define UI_PERIPHERYROW_H

#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QDoubleSpinBox>
#include "geostreampacketvirtual.h"

QT_BEGIN_NAMESPACE

class Ui_PeripheryRow {
public:
    QLabel* typeLbl = nullptr;
    QLabel* numberiLbl = nullptr;
    QComboBox* typeCb = nullptr;
    QComboBox* numberiCb = nullptr;
    QLabel* synLbl = nullptr;
    QLineEdit* synEdit = nullptr;
    QLabel* logLbl = nullptr;
    QCheckBox* logChb = nullptr;
    QLabel* filterLbl = nullptr;
    QComboBox* filterCb = nullptr;
    QLabel* ratioLbl = nullptr;
    QDoubleSpinBox* ratioEdit = nullptr;
    QLabel* bitLbl = nullptr;
    QSpinBox* bitEdit = nullptr;
    QLabel* sizeLbl = nullptr;
    QSpinBox* sizeEdit = nullptr;
    QLabel* descLbl = nullptr;
    QLineEdit* descEdit = nullptr;
    QLabel* canidLbl = nullptr;
    QLineEdit* canidEdit = nullptr;
    QLabel* deleteLbl = nullptr;
    QToolButton* deleteBn = nullptr;
    int code = -1;

    QStringList itypes =        { "CAN" };
    QStringList can_inumbers =  { "can_1", "can_2" };
    QStringList ifilters =      { "", "Фактическая величина груза", "Номинальная грузоподъемность крана" };

    QGridLayout* setupUi(int ifaces_cnt) {
        QGridLayout* layout = new QGridLayout();
        layout->setHorizontalSpacing(5);
        layout->addWidget(logLbl = new QLabel("лог"), 0, 1);
        layout->addWidget(typeLbl = new QLabel("тип"), 0, 2);
        layout->addWidget(numberiLbl = new QLabel("номер"), 0, 3);
        layout->addWidget(synLbl = new QLabel("синтаксис"), 0, 4);
        layout->addWidget(canidLbl = new QLabel("id фрейма"),0,5);
        layout->addWidget(bitLbl = new QLabel("старт bit"), 0, 6);
        layout->addWidget(sizeLbl = new QLabel("длина"), 0, 7);
        layout->addWidget(descLbl = new QLabel("описание"), 0, 8);
        layout->addWidget(ratioLbl = new QLabel("коэффициент"), 0, 9);
        layout->addWidget(filterLbl = new QLabel("опция"), 0, 10);
        layout->addWidget(logChb = new QCheckBox(), 1, 1);
        layout->addWidget(typeCb = new QComboBox(), 1, 2);
        typeCb->setFixedWidth(70);
        typeCb->addItems(itypes);
        layout->addWidget(numberiCb = new QComboBox(), 1, 3);
        numberiCb->setFixedWidth(70);
        numberiCb->addItems(can_inumbers);
        layout->addWidget(synEdit = new QLineEdit(), 1, 4);
        synEdit->setMaxLength(10);
        synEdit->setMinimumWidth(100);
        layout->addWidget(canidEdit = new QLineEdit(), 1, 5);
        synEdit->setMaximumWidth(150);
        canidEdit->setValidator(new QRegExpValidator(QRegExp("^[0-9A-Fa-f]+$")));
        canidEdit->setMaxLength(8);
        canidEdit->setFixedWidth(70);
        layout->addWidget(bitEdit = new QSpinBox(), 1, 6);
        bitEdit->setRange(0, 255);
        bitEdit->setFixedWidth(40);
        layout->addWidget(sizeEdit = new QSpinBox(), 1, 7);
        sizeEdit->setRange(1, 255);
        sizeEdit->setFixedWidth(40);
        layout->addWidget(descEdit = new QLineEdit(), 1, 8);
        descEdit->setMaxLength(sizeof (peripherystruct::description) / 2);
        descEdit->setMinimumWidth(100);
        layout->addWidget(ratioEdit = new QDoubleSpinBox(), 1, 9);
        ratioEdit->setRange(INT_MIN, INT_MAX);
        ratioEdit->setFixedWidth(70);
        layout->addWidget(filterCb = new QComboBox(), 1, 10);
        filterCb->setFixedWidth(150);
        filterCb->addItems(ifilters);
        layout->addWidget(deleteLbl = new QLabel(""), 0, 11);
        layout->addWidget(deleteBn = new QToolButton(), 1, 11);
        deleteBn->setIcon(QIcon(":/delete.png"));
        deleteBn->setToolTip("Удалить");
        deleteBn->setAutoRaise(true);
        layout->addItem(new QSpacerItem(20,10), 0, 12);
        layout->addItem(new QSpacerItem(20,10), 1, 12);
        code = ifaces_cnt;
        return layout;
    }

    QString toString() {
        QString res;
        res += QString("{");
        res += QString("typeCb: %1, ").arg(typeCb->currentIndex());
        res += QString("numberiCb: %1, ").arg(numberiCb->currentIndex());
        res += QString("synEdit: %1, ").arg(synEdit->text());
        res += QString("logChb: %1, ").arg(logChb->isChecked());
        res += QString("filterCb: %1, ").arg(filterCb->currentIndex());
        res += QString("ratioEdit: %1, ").arg(ratioEdit->text());
        res += QString("bitEdit: %1, ").arg(bitEdit->text());
        res += QString("sizeEdit: %1, ").arg(sizeEdit->text());
        res += QString("descEdit: %1, ").arg(descEdit->text());
        res += QString("canidEdit: %1 ").arg(canidEdit->text());
        res += QString("}");
        return res;
    }

    void clear() {
        if (typeLbl)    delete typeLbl;    if (typeCb)     delete typeCb;
        if (numberiLbl) delete numberiLbl; if (numberiCb)  delete numberiCb;
        if (synLbl)     delete synLbl;     if (synEdit)    delete synEdit;
        if (logLbl)     delete logLbl;     if (logChb)     delete logChb;
        if (filterLbl)  delete filterLbl;  if (filterCb)   delete filterCb;
        if (ratioLbl)   delete ratioLbl;   if (ratioEdit)  delete ratioEdit;
        if (bitLbl)     delete bitLbl;     if (bitEdit)    delete bitEdit;
        if (sizeLbl)    delete sizeLbl;    if (sizeEdit)   delete sizeEdit;
        if (descLbl)    delete descLbl;    if (descEdit)   delete descEdit;
        if (canidLbl)   delete canidLbl;   if (canidEdit)  delete canidEdit;
        if (deleteLbl)  delete deleteLbl;  if (deleteBn)   deleteBn->deleteLater();
    }
};

namespace Ui {
    class PeripheryRow: public Ui_PeripheryRow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PERIPHERYROW_H
