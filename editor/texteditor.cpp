#include "texteditor.h"

#include <QPainter>
#include <QTextBlock>
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfuture.h>
#include <qtimer.h>

TextEditor::TextEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    // m_debounceTimer = new QTimer(this);
    // m_debounceTimer->setSingleShot(true);
    // connect(m_debounceTimer, &QTimer::timeout, this, &TextEditor::processContentChanges);


    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
    connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setCursorWidth(2);
}

int TextEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + QFontMetrics(document()->defaultFont()).horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}


void TextEditor::findReplace()
{
    if (m_findReplace)
        return;

    m_findReplace = new FindReplaceDialog(this);

    connect(m_findReplace, &FindReplaceDialog::message, this, &TextEditor::message);
    connect(m_findReplace, &FindReplaceDialog::destroyed, this, [&]() {m_findReplace = nullptr;});

    m_findReplace->setAttribute(Qt::WA_DeleteOnClose);
    m_findReplace->show();
}

void TextEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void TextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

// Debounce text changes
// void TextEditor::contentChanged(int position, int charsRemoved, int charsAdded)
// {
//     Q_UNUSED(position);
//     Q_UNUSED(charsRemoved);
//     Q_UNUSED(charsAdded);

//     m_debounceTimer->start(100); // Adjust the timeout as needed
// }

// Process content changes after debounce
// void TextEditor::processContentChanges()
// {
//     // Perform the actual content change processing here
//     // This function will be called after the debounce timer expires
//     highlightSyntaxInBackground();
// }

// // Asynchronous syntax highlighting
// void TextEditor::highlightSyntaxInBackground()
// {
//     QFuture<void> future = QtConcurrent::run([this]() {
//         // Perform syntax highlighting here
//         // Ensure thread-safe access to the document
//     });
// }

void TextEditor::highlightCurrentLine()
{
    if (m_cachedSelection.cursor == textCursor()) {
        return; // No need to recalculate if the selection hasn't changed
    }

    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(150, 150, 150);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);

        m_cachedSelection = selection;
    }

    setExtraSelections(extraSelections);
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());


    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), QFontMetrics(document()->defaultFont()).height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F && event->modifiers() == Qt::ControlModifier)
        findReplace();
    else if (event->key() == Qt::Key_Equal && event->modifiers() == Qt::ControlModifier)
        zoomIn();
    else if (event->key() == Qt::Key_Minus && event->modifiers() == Qt::ControlModifier)
        zoomOut();

    QPlainTextEdit::keyPressEvent(event);
}
