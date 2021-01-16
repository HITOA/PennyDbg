#ifndef PENNYGUI_H
#define PENNYGUI_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "pennydbg.h"
#include "loadeddlltablemodel.h"

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
    void on_loadedDll_GUI_update();

    void on_actionConsole_triggered();

    void on_actionOpen_Process_triggered();

private:
    PennyDbg *dbg;
    Ui::PennyGUI *ui;

    LoadedDllTableModel *lpLoadedDllTableModel;

    void InitDbg(LPDebuggedProcessData pPData);
};
#endif // PENNYGUI_H
