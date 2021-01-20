#ifndef PENNYGUI_H
#define PENNYGUI_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include "pennydbg.h"
#include "loadeddlltablemodel.h"
#include "threadtablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PennyGUI; }
QT_END_NAMESPACE

#define HEXVIEW_INDEX 0

class PennyGUI : public QMainWindow
{
    Q_OBJECT

public:
    PennyGUI(QWidget *parent = nullptr);
    ~PennyGUI();
signals:
    void dump_process_memory(std::wstring dumpFileName);
    void dump_module_memory(LPVOID address);
private slots:
    void on_console_log(QString txt);
    void on_console_err(QString txt);
    void on_loadedDll_GUI_update();
    void on_threads_GUI_update();

    void on_actionConsole_triggered();

    void on_actionOpen_Process_triggered();

    void on_actionModules_triggered();

    void on_actionHex_view_triggered();

    void on_MainTab_tabCloseRequested(int index);

    void on_actionDump_debugged_process_triggered();

    void on_ModulesTable_doubleClicked(const QModelIndex &index);

private:
    PennyDbg *dbg;
    Ui::PennyGUI *ui;

    std::vector<QWidget*> mainTabWidgetList;

    LoadedDllTableModel *lpLoadedDllTableModel;
    ThreadTableModel *lpThreadTableModel;

    void InitDbg(LPDebuggedProcessData pPData);
};
#endif // PENNYGUI_H
