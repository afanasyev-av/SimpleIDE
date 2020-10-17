#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void showOutput();
    void on_actionCompile_triggered();
    void on_actionRun_triggered();
    void on_tableWidget_cellClicked(int row, int column);
    void on_actionDebug_triggered();
    void on_actionStep_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;

    QString mFileName;
    QProcess mProcessCompile;
    QProcess mProcessRun;
};

#endif // MAINWINDOW_H



