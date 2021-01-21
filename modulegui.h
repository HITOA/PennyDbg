#ifndef MODULEGUI_H
#define MODULEGUI_H

#include <QDialog>

namespace Ui {
class ModuleGUI;
}

class ModuleGUI : public QDialog
{
    Q_OBJECT

public:
    explicit ModuleGUI(QWidget *parent = nullptr);
    ~ModuleGUI();
    void SetModuleInformation(QString name, QString type, QString dir, QString mSize, QString dSize, QString creation, QString lastmodif, QString lastacess);

private:
    Ui::ModuleGUI *ui;
};

#endif // MODULEGUI_H
