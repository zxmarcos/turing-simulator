#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

class TapeView;
class AutomataView;
class Toolbox;
class MachineScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void newMachine();
    void saveMachine();
    void loadMachine();
    void writeToTape();
    void rewindTape();
    void clearTape();
    void stepIn();
    void playOrPause(bool playing);
    void playChanged(bool playing);
    void resetMachine();
    void updateCounter(int value);
    void playTimeChanged(int value);
    void formalize();
    void about();
private:
    void lockPlayControls(bool lock);
    TapeView *m_tapeView;
    AutomataView *m_automataView;
    MachineScene *m_machScene;
    QLineEdit *m_inputLine;
    QPushButton *m_sendToTape;
    QPushButton *m_clearTape;
    QPushButton *m_rewindTape;
    Toolbox *m_toolbox;
    QMenu *m_machineMenu;
    QMenu *m_aboutMenu;
    QAction *m_actionExit;
    QAction *m_actionNew;
    QAction *m_actionSave;
    QAction *m_actionLoad;
    QAction *m_actionReset;
    QAction *m_actionNext;
    QAction *m_actionPlayPause;
    QAction *m_actionAbout;
    QAction *m_actionAboutQt;
    QAction *m_actionFormalize;
    QSlider *m_slider;
    QSlider *m_playTime;
    QSplitter *m_splitter;
    QLabel *m_counterLabel;
    QPlainTextEdit *m_comments;


    void createMenus();
    void createControls();
    void createActions();
    void connectActions();
};

#endif // MAINWINDOW_H
