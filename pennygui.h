#ifndef PENNYGUI_H
#define PENNYGUI_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "pennydbg.h"
#include "moduleslistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PennyGUI; }
QT_END_NAMESPACE

class PennyGUI : public QMainWindow
{
    Q_OBJECT

public:
    PennyGUI(QWidget *parent = nullptr);
    ~PennyGUI();
private slots:
    void on_console_log(QString txt);
    void on_console_err(QString txt);
    void on_modules_update(std::map<LPVOID, PNMODULE> modules);

    void on_actionConsole_triggered();

    void on_actionOpen_Process_triggered();

private:
    PennyDbg *dbg;
    QTimer *pennyGUIUpdateTimer;
    Ui::PennyGUI *ui;

    ModulesListModel *modulesListModel;

    void InitDbg(PPennyDbgStruct dbgStruct);
};
#endif // PENNYGUI_H
