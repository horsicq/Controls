#include "dialogbits8.h"
#include "ui_dialogbits8.h"

DialogBits8::DialogBits8(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBits8)
{
    ui->setupUi(this);

    g_nValue = 0;
    g_bIsReadonly = true;
}

DialogBits8::~DialogBits8()
{
    delete ui;
}

void DialogBits8::setData(quint8 nValue, bool bIsReadonly)
{
    g_nValue = nValue;
    g_bIsReadonly = bIsReadonly;

    ui->pushButtonOK->setEnabled(!g_bIsReadonly);

    ui->pushButtonCancel->setFocus();

    reload();
}

void DialogBits8::reload()
{
    // TODO
}
