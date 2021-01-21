#include "modulegui.h"
#include "ui_modulegui.h"

ModuleGUI::ModuleGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModuleGUI)
{
    ui->setupUi(this);
}

ModuleGUI::~ModuleGUI()
{
    delete ui;
}

void ModuleGUI::SetModuleInformation(QString name,
                                     QString type,
                                     QString dir,
                                     QString mSize,
                                     QString dSize,
                                     QString creation,
                                     QString lastmodif,
                                     QString lastacess) {
    ui->Title->setText(name);
    ui->Filetype->setText("Application type : " + type);
    ui->Directory->setText("Directory : " + dir);
    ui->MSize->setText("Mapped size : " + mSize + " bytes");
    ui->DSize->setText("Disk size : " + dSize + " bytes");
    ui->CreationTime->setText("Creation time : " + creation);
    ui->LasteWriteTime->setText("Laste write time : " + lastmodif);
    ui->LastAcessTime->setText("Laste acess time : " + lastacess);
}
