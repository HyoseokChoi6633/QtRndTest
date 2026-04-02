#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;

    QVBoxLayout* m_pVBLayout;

    QLineEdit* m_pLEMsg;
    QLineEdit* m_pLEMsgDec;
    QPushButton* m_pBtnEnc;
    QPushButton* m_pBtnDec;

    QList<unsigned int> m_liEncData;

    QTextEdit* m_pTEEncData;

    void displayEncDataToTextEdit();

private slots:
    void onBtnEncClicked();     // 슬롯 함수 선언
    void onBtnDecClicked();
};
#endif // WIDGET_H
