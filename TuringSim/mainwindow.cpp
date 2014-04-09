#include <QtWidgets>
#include "mainwindow.h"
#include "tapeview.h"
#include "automataview.h"
#include "stateitem.h"
#include "transitionitem.h"
#include "machinescene.h"
#include "toolbox.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();
    createControls();
    connectActions();

    setWindowTitle(tr("Simulador da Máquina de Turing"));
}

MainWindow::~MainWindow()
{

}

void MainWindow::newMachine()
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::question(this, "Limpar?", "Tem certeza que deseja limpar a cena?",
                                QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        m_automataView->clear();
        m_comments->clear();
    }
}

void MainWindow::saveMachine()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Salvar XML"), ".",
                                                tr("Arquivos XML (*.xml)"));
    if (!name.isEmpty()) {
        m_automataView->setMachineComments(m_comments->toPlainText());
        m_automataView->saveMachine(name);
    }
}

void MainWindow::loadMachine()
{
    QString name = QFileDialog::getOpenFileName(this, tr("Salvar XML"), ".",
                                                tr("Arquivos XML (*.xml)"));
    if (!name.isEmpty()) {
        m_automataView->loadMachine(name);
        m_comments->setPlainText(m_automataView->getMachineComments());
    }
}

void MainWindow::writeToTape()
{
    m_tapeView->writeString(m_inputLine->text().simplified());
}

void MainWindow::rewindTape()
{
  //  m_tapeView->seekTo(0);
    m_slider->setValue(0);
}

void MainWindow::clearTape()
{
    m_tapeView->clear();
    m_slider->setValue(0);
}

void MainWindow::stepIn()
{
    m_automataView->stepIn();
}

void MainWindow::playOrPause(bool playing)
{
    if (playing) {
        m_automataView->play(m_playTime->value());
    } else {
        if (m_automataView->isPlaying())
            m_automataView->stop();
    }
}

void MainWindow::playChanged(bool playing)
{
    if (playing) {
        lockPlayControls(true);
        m_actionPlayPause->setIcon(QIcon(tr(":/images/pause.png")));
    } else {
        lockPlayControls(false);
        m_actionPlayPause->setIcon(QIcon(tr(":/images/play.png")));
        m_actionPlayPause->setChecked(false);
    }
}

void MainWindow::resetMachine()
{
    m_automataView->resetMachine();
}

void MainWindow::updateCounter(int value)
{
    m_counterLabel->setText(tr("%1 passos").arg(value));
}

void MainWindow::playTimeChanged(int value)
{
    if (m_automataView->isPlaying())
        m_automataView->play(value);
}

void MainWindow::formalize()
{
    m_automataView->describe(m_comments->toPlainText());
}

static QString loadAppStyle(const QString &name)
{
    QFile file(name);
    file.open(QFile::ReadOnly);

    return QString(file.readAll());
}

void MainWindow::about()
{
    QMessageBox::about(this, "Sobre", loadAppStyle(":/about.html"));
}

void MainWindow::lockPlayControls(bool lock)
{
    m_actionReset->setEnabled(!lock);
    m_actionNext->setEnabled(!lock);
}

void MainWindow::createControls()
{
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::NoFrame);

    QVBoxLayout *right = new QVBoxLayout;
    QHBoxLayout *left = new QHBoxLayout;
    QHBoxLayout *input = new QHBoxLayout;

    m_tapeView = new TapeView();
    m_automataView = new AutomataView(m_tapeView);
    m_machScene = new MachineScene(m_automataView);
    m_toolbox = new Toolbox();
    m_inputLine = new QLineEdit;
    m_sendToTape = new QPushButton(QIcon(":/images/tape-write.png"), "");
    m_sendToTape->setToolTip("Escreve na posição atual da fita");
    m_counterLabel = new QLabel("0 passos");
    m_counterLabel->setAttribute(Qt::WA_TranslucentBackground);

    m_clearTape = new QPushButton(QIcon(":/images/tape-erase.png"), "");
    m_clearTape->setToolTip("Limpa todo o conteúdo da fita");

    m_rewindTape = new QPushButton(QIcon(":/images/tape-rewind.png"), "");
    m_rewindTape->setToolTip("Rebobina a fita para a posição 0");
    m_slider = new QSlider(Qt::Horizontal);

    m_splitter = new QSplitter(Qt::Vertical);
    m_comments = new QPlainTextEdit;

    m_splitter->insertWidget(0, m_automataView);
    m_splitter->insertWidget(1, m_comments);
    m_splitter->setCollapsible(0, false);
    m_splitter->setCollapsible(1, true);

    input->addWidget(m_inputLine);
    input->addWidget(m_sendToTape);
    input->addWidget(m_clearTape);
    input->addWidget(m_rewindTape);


    m_slider->setMinimum(-30);
    m_slider->setMaximum(+30);
    m_slider->setValue(0);


    right->setSpacing(3);

    m_playTime = new QSlider(Qt::Horizontal);
    m_playTime->setFixedWidth(100);
    m_playTime->setToolTip("Velocidade da execução");

    m_playTime->setMinimum(10);
    m_playTime->setMaximum(1000);
    m_playTime->setTickInterval(10);
    m_playTime->setValue(400);
    QToolBar *qt = addToolBar("máquina");
    qt->addAction(m_actionNew);
    qt->addAction(m_actionLoad);
    qt->addAction(m_actionSave);
    qt->addAction(m_actionFormalize);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // toolBar is a pointer to an existing toolbar
    qt->addWidget(spacer);

    //qt->addSeparator();
    qt->addWidget(m_counterLabel);
    qt->addAction(m_actionReset);
    qt->addAction(m_actionNext);
    qt->addAction(m_actionPlayPause);

    qt->addWidget(m_playTime );
    qt->setIconSize(QSize(18, 18));
    qt->setMovable(false);
    qt->setFloatable(false);

    m_slider->setTickPosition(QSlider::NoTicks);

    //right->addWidget(m_automataView);
    right->addWidget(m_splitter);
    right->addWidget(m_slider);
    right->addWidget(m_tapeView);
    right->addLayout(input);
    left->addWidget(m_toolbox);
    left->addLayout(right);
    right->setSpacing(0);

    left->setMargin(2);
    m_automataView->setScene(m_machScene);
    frame->setLayout(left);
    frame->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(frame);

    setContentsMargins(0, 0, 0, 0);
    statusBar();
    resize(600, 500);
}

void MainWindow::createActions()
{
    m_actionExit = new QAction(QIcon(":/images/close.png"), tr("Sair"), this);
    m_actionNew = new QAction(QIcon(":/images/new.png"), tr("Novo"), this);
    m_actionLoad = new QAction(QIcon(":/images/open.png"), tr("Abrir"), this);
    m_actionSave = new QAction(QIcon(":/images/save.png"), tr("Salvar"), this);

    m_actionFormalize = new QAction(QIcon(":/images/formalize.png"), tr("Formalizar"), this);
    m_actionAbout = new QAction(tr("Sobre..."), this);

    m_actionSave->setShortcut(QKeySequence("Ctrl+S"));
    m_actionLoad->setShortcut(QKeySequence("Ctrl+O"));
    m_actionNew->setShortcut(QKeySequence("Ctrl+N"));
    m_actionFormalize->setShortcut(QKeySequence("Ctrl+F"));


    m_actionReset = new QAction("reset", this);
    m_actionReset->setShortcut(QKeySequence("Ctrl+R"));
    m_actionReset->setIcon(QIcon(tr(":/images/reset.png")));
    m_actionReset->setToolTip("Reinicia a máquina");

    m_actionNext = new QAction("Próximo", this);
    m_actionNext->setIcon(QIcon(tr(":/images/next.png")));
    m_actionNext->setShortcut(QKeySequence("N"));
    m_actionNext->setToolTip("Avança um estado na máquina");
    m_actionPlayPause = new QAction("Executar", this);
    m_actionPlayPause->setIcon(QIcon(tr(":/images/play.png")));
    m_actionPlayPause->setCheckable(true);
    m_actionPlayPause->setShortcut(QKeySequence("P"));
    m_actionPlayPause->setToolTip("Rodar a máquina");

    m_actionAboutQt = new QAction(tr("Sobre o Qt."), this);
}

void MainWindow::connectActions()
{
    connect(m_toolbox, SIGNAL(toolChanged(int)), m_machScene, SLOT(setMode(int)));
    connect(m_actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(saveMachine()));
    connect(m_actionLoad, SIGNAL(triggered()), this, SLOT(loadMachine()));
    connect(m_slider, SIGNAL(valueChanged(int)), m_tapeView, SLOT(seekTo(int)));
    connect(m_sendToTape, SIGNAL(clicked()), this, SLOT(writeToTape()));
    connect(m_rewindTape, SIGNAL(clicked()), this, SLOT(rewindTape()));
    connect(m_clearTape, SIGNAL(clicked()), this, SLOT(clearTape()));
    connect(m_actionPlayPause, SIGNAL(toggled(bool)), this, SLOT(playOrPause(bool)));
    connect(m_actionReset, SIGNAL(triggered()), this, SLOT(resetMachine()));
    connect(m_actionNext, SIGNAL(triggered()), this, SLOT(stepIn()));
    connect(m_actionNew, SIGNAL(triggered()), this, SLOT(newMachine()));
    connect(m_actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(m_tapeView, SIGNAL(valueChanged(int)), m_slider, SLOT(setValue(int)));
    connect(m_automataView, SIGNAL(stepCounterChanged(int)), this, SLOT(updateCounter(int)));
    connect(m_automataView, SIGNAL(playStatusChanged(bool)), this, SLOT(playChanged(bool)));
    connect(m_playTime, SIGNAL(valueChanged(int)), this, SLOT(playTimeChanged(int)));
    connect(m_actionFormalize, SIGNAL(triggered()), this, SLOT(formalize()));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    m_machineMenu = menuBar()->addMenu(tr("Máquina"));
    m_machineMenu->addAction(m_actionFormalize);
    m_machineMenu->addSeparator();
    m_machineMenu->addAction(m_actionNew);
    m_machineMenu->addAction(m_actionLoad);
    m_machineMenu->addAction(m_actionSave);
    m_machineMenu->addSeparator();
    m_machineMenu->addAction(m_actionExit);
    m_aboutMenu = menuBar()->addMenu(tr("Sobre"));
    m_aboutMenu->addAction(m_actionAbout);
    m_aboutMenu->addAction(m_actionAboutQt);
}
