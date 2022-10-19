#ifndef UI_INFOCRANE_H
#define UI_INFOCRANE_H

#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>

QT_BEGIN_NAMESPACE

class Ui_InfoCrane {
public:    
    QLineEdit *elem0;
    QLineEdit *elem1;
    QLineEdit *elem2;
    QLineEdit *elem3;
    QSpinBox  *elem4;
    QSpinBox  *elem5;
    QSpinBox  *elem6;
    QLineEdit *elem7;
    QLineEdit *elem8;
    QLineEdit *elem9;
    QDateEdit *elem10;
    QSpinBox  *elem11;
    QLineEdit *elem12;
    QLineEdit *elem13;
    QLineEdit *elem14;
    QLineEdit *elem15;
    QSpinBox  *elem16;
    QDateEdit *elem17;
    QLineEdit *elem18;
    QLineEdit *elem19;
    QLineEdit *elem20;
    QSpinBox  *elem21;
    QSpinBox  *elem22;
    QSpinBox  *elem23;
    QSpinBox  *elem24;
    QSpinBox  *elem25;
    QSpinBox  *elem26;
    QSpinBox  *elem27;
    QSpinBox  *elem28;
    QSpinBox  *elem29;
    QSpinBox  *elem30;
    QDoubleSpinBox  *elem31;
    QDoubleSpinBox  *elem32;
    QPlainTextEdit *log;
    QPushButton *sendCrane;
    QPushButton *updateCrane;
    QFrame *lineH1;
    QFrame *lineH2;
    QFrame *lineH3;

    QSpinBox* create_spin(bool readonly, QDialog *InfoCrane) {
        QSpinBox* spin = new QSpinBox(InfoCrane);
        spin->setMinimumSize(QSize(150, 20));
        spin->setMaximumSize(QSize(150, 20));
        spin->setReadOnly(readonly);
        return spin;
    }

    QDoubleSpinBox* create_ddspin(bool readonly, QDialog *InfoCrane) {
        QDoubleSpinBox* spin = new QDoubleSpinBox(InfoCrane);
        spin->setMinimumSize(QSize(150, 20));
        spin->setMaximumSize(QSize(150, 20));
        spin->setReadOnly(readonly);
        return spin;
    }

    QLineEdit* create_line_edit(bool readonly, QDialog *InfoCrane) {
        QLineEdit* line = new QLineEdit(InfoCrane);
        line->setMinimumSize(QSize(150, 20));
        line->setMaximumSize(QSize(150, 20));
        line->setReadOnly(readonly);
        return line;
    }

    QDateEdit* create_calendar(bool readonly, QDialog *InfoCrane) {
        QDateEdit* calendar = new QDateEdit(InfoCrane);
        calendar->setDisplayFormat("dd.MM.yyyy");
        calendar->setMinimumSize(QSize(150, 20));
        calendar->setMaximumSize(QSize(150, 20));
        calendar->setReadOnly(readonly);
        return calendar;
    }

    QDateTimeEdit* create_datetime(bool readonly, QDialog *InfoCrane) {
        QDateTimeEdit* calendar = new QDateTimeEdit(InfoCrane);
        calendar->setDisplayFormat("hh:mm:ss dd.MM.yyyy");
        calendar->setMinimumSize(QSize(150, 20));
        calendar->setMaximumSize(QSize(150, 20));
        calendar->setReadOnly(readonly);
        return calendar;
    }

    template<class T>
    QHBoxLayout* create_cell(T elemN, const QString& name) {
        auto horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(5);
        horizontalLayout->addWidget(new QLabel(name));
        horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        horizontalLayout->addWidget(elemN);
        return horizontalLayout;
    }

    void setupUi(QDialog *InfoCrane) {
        InfoCrane->setWindowTitle("Информация о кране");
        InfoCrane->resize(1080, 470);
        InfoCrane->setMinimumSize(QSize(1080, 470));
        lineH1 = new QFrame();
        lineH1->setFrameShape(QFrame::HLine);
        lineH1->setFrameShadow(QFrame::Sunken);
        lineH2 = new QFrame();
        lineH2->setFrameShape(QFrame::HLine);
        lineH2->setFrameShadow(QFrame::Sunken);
        lineH3 = new QFrame();
        lineH3->setFrameShape(QFrame::HLine);
        lineH3->setFrameShadow(QFrame::Sunken);
        QVBoxLayout* verticalLayout = new QVBoxLayout(InfoCrane);
        verticalLayout->setSpacing(5);
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setSpacing(5);
        ///////////
        gridLayout->addWidget(new QLabel("Информация о кране:"), 0, 0);
        gridLayout->addWidget(lineH1, 0, 1, 1, 2);
        elem0  = create_line_edit(false, InfoCrane);
        elem0->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem0, "Тип:"), 1, 0);
        elem1  = create_line_edit(false, InfoCrane);
        elem1->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem1, "Индекс:"), 1, 1);
        elem2  = create_line_edit(false, InfoCrane);
        elem2->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem2, "Изготовитель:"), 3, 2);
        elem3  = create_line_edit(false, InfoCrane);
        elem3->setMaxLength(32);
        gridLayout->addLayout(create_cell(elem3, "Серийный номер:"), 1, 2);
        elem4  = create_spin(false, InfoCrane);
        elem4->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem4, "Год изготовления:"), 4, 0);
        elem5  = create_spin(false, InfoCrane);
        elem5->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem5, "Грузоподъемность:"), 2, 0);
        elem6  = create_spin(false, InfoCrane);
        elem6->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem6, "Грузоподъемность в режиме №1:"), 2, 1);
        elem7  = create_line_edit(false, InfoCrane);
        elem7->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem7, "Группа классификации для крана:"), 2, 2);
        elem8  = create_line_edit(false, InfoCrane);
        elem8->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem8, "Группа классификации для лебедки:"), 3, 0);
        elem9  = create_line_edit(false, InfoCrane);
        elem9->setMaxLength(16);
        gridLayout->addLayout(create_cell(elem9, "Группа классификации для механизмов:"), 3, 1);
        elem10 = create_calendar(false, InfoCrane);
        gridLayout->addLayout(create_cell(elem10, "Дата ввода в эксплуатацию:"), 4, 1);
        elem11 = create_spin(false, InfoCrane);
        elem11->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem11, "Нормативный срок службы (лет):"), 4, 2);
        ///////////
        gridLayout->addWidget(new QLabel("Информация о РП:"), 5, 0);
        gridLayout->addWidget(lineH2, 5, 1, 1, 2);
        elem12 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem12, "Тип:"), 6, 0);
        elem13 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem13, "Модификация:"), 6, 1);
        elem14 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem14, "Изготовитель:"), 7, 0);
        elem15 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem15, "Серийный номер:"), 6, 2);
        elem16 = create_spin(true, InfoCrane);
        elem16->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem16, "Год изготовления:"), 7, 1);
        elem17 = create_calendar(false, InfoCrane);
        gridLayout->addLayout(create_cell(elem17, "Дата установки на кран:"), 7, 2);
        elem18 = create_line_edit(false, InfoCrane);
        elem18->setMaxLength(32);
        gridLayout->addLayout(create_cell(elem18, "Компания-установщик:"), 8, 0);
        ///////////
        gridLayout->addWidget(new QLabel("Информация об эксплуатации крана:"), 9, 0);
        gridLayout->addWidget(lineH3, 9, 1, 1, 2);
        elem19 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem19, "Общее время работы устройства:"), 10, 0);
        elem20 = create_line_edit(true, InfoCrane);
        gridLayout->addLayout(create_cell(elem20, "Моточасы:"), 10, 1);
        elem21 = create_spin(true, InfoCrane);
        elem21->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem21, "Суммарная масса поднятых грузов:"), 10, 2);
        elem22 = create_spin(true, InfoCrane);
        elem22->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem22, "Кол.-во срабатываний ограничителя:"), 11, 0);
        elem23 = create_spin(true, InfoCrane);
        elem23->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem23, "Суммарное число рабочих циклов:"), 11, 1);
        elem24 = create_spin(true, InfoCrane);
        elem24->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem24, "Рабочие циклы >=5% и <=20%:"), 11, 2);
        elem25 = create_spin(true, InfoCrane);
        elem25->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem25, "Рабочие циклы >=21% и <=40%:"), 12, 0);
        elem26 = create_spin(true, InfoCrane);
        elem26->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem26, "Рабочие циклы >=41% и <=60%:"), 12, 1);
        elem27 = create_spin(true, InfoCrane);
        elem27->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem27, "Рабочие циклы >=61% и <=80%:"), 12, 2);
        elem28 = create_spin(true, InfoCrane);
        elem28->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem28, "Рабочие циклы >=81% и <=100%:"), 13, 0);
        elem29 = create_spin(true, InfoCrane);
        elem29->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem29, "Рабочие циклы >=101% и <=120%:"), 13, 1);
        elem30 = create_spin(true, InfoCrane);
        elem30->setRange(0, INT_MAX);
        gridLayout->addLayout(create_cell(elem30, "Рабочие циклы >=121% и <=140%:"), 13, 2);
        elem31 = create_ddspin(true, InfoCrane);
        elem31->setRange(INT_MIN, INT_MAX);
        gridLayout->addLayout(create_cell(elem31, "Текущее характеристическое число:"), 14, 0);
        elem32 = create_ddspin(true, InfoCrane);
        elem32->setRange(INT_MIN, INT_MAX);
        gridLayout->addLayout(create_cell(elem32, "Коэффициент распределения нагрузки:"), 14, 1);
        verticalLayout->addLayout(gridLayout);

        auto horizontalLayout = new QHBoxLayout();
        log = new QPlainTextEdit(InfoCrane);
        log->setReadOnly(true);
        horizontalLayout->addWidget(log);
        auto verticalLayout_2 = new QVBoxLayout();
        sendCrane = new QPushButton(InfoCrane);
        sendCrane->setText("Отправить");
        verticalLayout_2->addWidget(sendCrane);
        updateCrane = new QPushButton(InfoCrane);
        updateCrane->setText("Обновить");
        verticalLayout_2->addWidget(updateCrane);
        horizontalLayout->addLayout(verticalLayout_2);
        verticalLayout->addLayout(horizontalLayout);
        QMetaObject::connectSlotsByName(InfoCrane);
    } // setupUi
};

namespace Ui {
    class InfoCrane: public Ui_InfoCrane {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFOCRANE_H
