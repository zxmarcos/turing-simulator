#include <QtWidgets>
#include <QtCore>
#include <QtPrintSupport>
#include "machinedescriptor.h"

MachineDescriptor::MachineDescriptor(QWidget *parent) :
    QDialog(parent)
{
    m_export = new QAction(QIcon(tr(":/images/export-pdf")), "Exportar para PDF", this);
    connect(m_export, SIGNAL(triggered()), this, SLOT(print()));
    m_toolbar = new QToolBar("Opções");

    m_toolbar->setIconSize(QSize(18, 18));
    m_toolbar->addAction(m_export);
    m_editor = new QTextBrowser;
    QStatusBar *bar = new QStatusBar;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);

    layout->addWidget(m_toolbar);
    layout->addWidget(m_editor);
    layout->addWidget(bar);
    setLayout(layout);
    layout->setMargin(0);
    resize(400, 500);
    setContentsMargins(0, 0, 0, 0);
    setWindowTitle(tr("Descrição Formal da Máquina"));

}


struct StateComparator
{
    bool operator()(const StateItem *a, const StateItem *b) const {
        if (a->getNumber() < b->getNumber())
            return true;
        return false;
    }
};

void MachineDescriptor::setMachineScene(MachineScene *scene)
{
    m_editor->clear();
    QTextCursor cursor = m_editor->textCursor();

    QTextCharFormat format = cursor.charFormat();
    format.setFontPointSize(10);
    format.setFontItalic(true);
    format.setFontFamily("Serif");

    QTextCharFormat format2 = cursor.charFormat();
    format2.setFontPointSize(10);
    format2.setFontItalic(true);
    format2.setFontFamily("Arial");


    QList<TransitionItem *> transitions;
    QList<StateItem *> states;
    QList<StateItem *> finalStates;
    StateItem *q0 = nullptr;

    // primeiro vamos popular nossa lista de estados e transições.
    foreach (QGraphicsItem *item, scene->items()) {
        if (dynamic_cast<StateItem*>(item)) {
            StateItem *i = static_cast<StateItem *>(item);
            states.push_back(i);
            // verifica se esse estado é inicial
            if (i->getType() == StateItem::Initial)
                q0 = i;
            // verifica se esse estado é final
            else if (i->getType() == StateItem::Final)
                finalStates.push_back(i);
        } else if (dynamic_cast<TransitionItem*>(item)) {
            transitions.push_back(static_cast<TransitionItem *>(item));
        }
    }

    // vamos definir nosso alfabeto
    QList<QString> tapeAlphabet;
    foreach (TransitionItem *item, transitions) {
        foreach (AcceptState ast, item->getAcceptList()) {
            tapeAlphabet.push_front(ast.accept);
            tapeAlphabet.push_front(ast.write);
        }
    }
    if (tapeAlphabet.indexOf("#") == -1)
        tapeAlphabet.push_front(tr("#"));
    QList<QString> alphabet(tapeAlphabet);
    // removemos o símbolo branco do alfabeto de entrada
    alphabet.removeAll("#");

    // vamos remover os duplicados e ordenar
    {
        alphabet = QSet<QString>::fromList(alphabet).values();
        tapeAlphabet = QSet<QString>::fromList(tapeAlphabet).values();
        qSort(alphabet);
        qSort(tapeAlphabet);
    }

    qSort(states.begin(), states.end(), StateComparator());
    qSort(finalStates.begin(), finalStates.end(), StateComparator());

    QTextTableFormat tableFormat;
    tableFormat.setBackground(QColor("#e0e0e0"));
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(8);

    cursor.insertText(m_comments, format2);
    cursor.insertText("\n\n", format);

    cursor.insertText(tr("M = (Q, \u2211, \u0393, q0, \u03B2, F, \u03B4)\n"), format);

    cursor.insertText(tr("Q = { "), format);
    {
        for (int i = 0; i < states.size(); i++) {
            cursor.insertText(QString::number(states[i]->getNumber()));
            if (i != (states.size() - 1))
                cursor.insertText(", ", format);
        }
    }
    cursor.insertText(tr(" }\n"), format);

    cursor.insertText(tr("\u2211 = { "), format);
    {
        for (int i = 0; i < alphabet.size(); i++) {
            cursor.insertText(alphabet.at(i));
            if (i != (alphabet.size() - 1))
                cursor.insertText(", ", format);
        }
    }
    cursor.insertText(tr(" }\n"), format);

    cursor.insertText(tr("\u0393 = { "), format);
    {
        for (int i = 0; i < tapeAlphabet.size(); i++) {
            cursor.insertText(tapeAlphabet.at(i));
            if (i != (tapeAlphabet.size() - 1))
                cursor.insertText(", ", format);
        }
    }
    cursor.insertText(tr(" }\n"), format);

    if (q0 != nullptr)
        cursor.insertText(tr("q0 = { %1 }\n").arg(QString::number(q0->getNumber())), format);
    else cursor.insertText(tr("q0 = { }\n"));
    cursor.insertText(tr("\u03B2 = { # }\n"), format);

    cursor.insertText(tr("F = { "), format);
    {
        for (int i = 0; i < finalStates.size(); i++) {
            cursor.insertText(QString::number(finalStates[i]->getNumber()));
            if (i != (finalStates.size() - 1))
                cursor.insertText(", ", format);
        }
    }
    cursor.insertText(tr(" }\n"), format);



    cursor.insertText(tr("\n"), format);
    QTextTable *table = cursor.insertTable(states.size() + 1, tapeAlphabet.size() + 1, tableFormat);

    QTextTableCell cell = table->cellAt(0, 0);
    QTextCursor cellCursor = cell.firstCursorPosition();
    cellCursor.insertText(QString("\u03B4"), format);

    // escreve os estados
    for (int i = 1; i <= states.size(); i++) {
        QTextTableCell cell = table->cellAt(i, 0);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(QString::number(states[i - 1]->getNumber()), format);
    }

    // escreve o alfabeto
    for (int i = 1; i <= tapeAlphabet.size(); i++) {
        QTextTableCell cell = table->cellAt(0, i);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(tapeAlphabet.at(i - 1), format);
    }

    for (int i = 0; i < states.size(); i++) {
        StateItem *st = states[i];
        for (int k = 0; k < tapeAlphabet.size(); k++) {
            TransitionAccept tac = st->next(tapeAlphabet[k]);
            if (tac.next == nullptr)
                continue;
            QTextTableCell cell = table->cellAt(i + 1, k + 1);
            QTextCursor cellCursor = cell.firstCursorPosition();

            QString s = "(";
            s += QString::number(tac.next->getNumber());
            s += ", ";
            s += tac.write;
            s += ", ";
            if (tac.direction == AcceptState::Left)
                s += "E)";
            else s += "D)";
            cellCursor.insertText(s, format);
        }
    }
    m_editor->adjustSize();
    adjustSize();
}

void MachineDescriptor::setMachineComments(QString str)
{
    m_comments = str;
}

void MachineDescriptor::print()
{

    QString name = QFileDialog::getSaveFileName(this, tr("Salvar PDF"), ".",
                                                tr("Arquivos PDF (*.pdf)"));
    if (name.isEmpty())
        return;
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setFullPage(true);
    printer.setOutputFileName(name);

    QTextDocument *document = m_editor->document();
    document->print(&printer);
}
