#pragma once

#include <qmainwindow.h>
#include "about.h"
#include "editor/utilities/keyboardshortcutguide.h"
#include "git/git.h"
#include "mediaplayer/mediaplayer.h"
#include "editor/texteditor.h"
#include<QThread>
#include<QtConcurrent/QtConcurrent>
#include "./transcriptgenerator.h"
#include "mediaplayer/utilities/mediasplitter.h"
#include "qmainwindow.h"

#include"audiowaveform.h"
#include "qmediadevices.h"
#include "qtablewidget.h"
#include "tts/ttsrow.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Tool; }
QT_END_NAMESPACE

class Tool final : public QMainWindow
{
    Q_OBJECT

public:
    explicit Tool(QWidget *parent = nullptr);
    ~Tool() final;

public slots:

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    // bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;


private slots:
    void handleMediaPlayerError();
    void createKeyboardShortcutGuide();
    void changeFont();
    void changeFontSize(int change);
    void transliterationSelected(QAction* action);
    void createMediaSplitter();

    void on_Upload_and_generate_Transcript_triggered();
    //    void on_editor_openTranscript_triggered();

    void on_btn_translate_clicked();

    void on_editor_openTranscript_triggered();



    void on_add_video_clicked();

    void on_open_transcript_clicked();

    void on_new_transcript_clicked();

    void on_save_transcript_clicked();

    void on_save_as_transcript_clicked();

    void on_load_a_custom_dict_clicked();

    void on_get_PDF_clicked();

    void on_decreseFontSize_clicked();

    void on_increaseFontSize_clicked();

    void on_toggleWordEditor_clicked();

    void on_keyboard_shortcuts_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_actionAbout_triggered();

    void on_actionInit_triggered();

    void on_actionAdd_All_triggered();

    void on_actionCommit_triggered();

    void on_actionAdd_Remote_URL_triggered();

    void on_actionPush_triggered();

    void on_actionPull_triggered();

    void on_actionShow_Waveform_triggered();


    void on_actionSave_Timestamps_triggered();
    void connectWaveformAndMediaplayer();
    void on_actionUpdate_Timestamps_triggered();

    void on_actionShow_Endlines_triggered();

    void on_actionOpen_triggered();

private:
    void setFontForElements();
    void setTransliterationLangCodes();
    bool isDroppedOnLayout(const QPoint &pos, QVBoxLayout *layout);
    QVector<TTSRow> parseXML(const QUrl& fileUrl);
    void setDefaultAudioOutputDevice();
    void onTabChanged(int index);

    MediaPlayer *player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    Ui::Tool *ui;
    QFont font;
    QMap<QString, QString> m_transliterationLang;
    Git* git = nullptr;
    // QTableWidget* tableWidget;
    bool isSendingSampleRateSuccess = false;
    bool isSamplingSuccess = false;
    QSettings* settings;
    KeyboardShortcutGuide* help_keyshortcuts = nullptr;
    About* about = nullptr;
    QMediaDevices m_mediaDevices;

};
