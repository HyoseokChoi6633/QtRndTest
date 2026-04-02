#include "widget.h"
#include "ui_widget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QList>

#include <QRandomGenerator>
#include <QByteArray>

unsigned char calculateCRC8(const QByteArray &data);

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_pVBLayout(nullptr)
    , m_pLEMsg(nullptr)
    , m_pLEMsgDec(nullptr)
{
    ui->setupUi(this);

    m_pVBLayout = new QVBoxLayout(this);

    QHBoxLayout* pHBLayout = new QHBoxLayout();

    QLabel* pLblMsg  = new QLabel("테스트 문자열 : ", this);

    m_pLEMsg = new QLineEdit(this);

    if(m_pLEMsg) {
        m_pLEMsg->setPlaceholderText("테스트할 문자를 입력하세요.");
    }

    m_pBtnEnc = new QPushButton("메시지 암호화", this);

    if(pHBLayout) {
        pHBLayout->addWidget(pLblMsg);
        pHBLayout->addWidget(m_pLEMsg);
        pHBLayout->addWidget(m_pBtnEnc);
    }

    if(m_pVBLayout) {
        m_pVBLayout->addLayout(pHBLayout);

        m_pVBLayout->setSpacing(10);        // 위젯 사이의 간격
        m_pVBLayout->setContentsMargins(20, 20, 20, 20);        // 외곽 여백
    }

    QVBoxLayout* pVBLayout = new QVBoxLayout();

    QLabel* pLblEncLabel  = new QLabel("문자열 4바이트 암호화 변환", this);

    m_pTEEncData = new QTextEdit(this);

    if(pVBLayout) {
        pVBLayout->addWidget(pLblEncLabel);
        pVBLayout->addWidget(m_pTEEncData);
    }

    if(m_pVBLayout) {
        m_pVBLayout->addLayout(pVBLayout);
    }

    m_pBtnDec = new QPushButton("메시지 복호화 =>", this);

    m_pLEMsgDec = new QLineEdit(this);

    if(m_pLEMsgDec) {
        m_pLEMsgDec->setReadOnly(true);
    }

    pHBLayout = new QHBoxLayout();

    if(pHBLayout) {
        pHBLayout->addWidget(m_pBtnDec);
        pHBLayout->addWidget(m_pLEMsgDec);
    }

    if(m_pVBLayout) {
        m_pVBLayout->addLayout(pHBLayout);
    }

    // 버튼 connect()
    connect(m_pBtnEnc, &QPushButton::clicked, this, &Widget::onBtnEncClicked);
    connect(m_pBtnDec, &QPushButton::clicked, this, &Widget::onBtnDecClicked);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onBtnEncClicked() {
    // 버튼 클릭시 동작할 내용

    if(m_pLEMsg->text().isEmpty()) {
        return;
    }

    m_liEncData.clear();

    QByteArray dataOri = m_pLEMsg->text().toUtf8();

    // QByteArray의 내부 포인터를 unsigned char* 로 변환
    unsigned char *ptr = reinterpret_cast<unsigned char*>(dataOri.data());

    unsigned char arBit233[3];

    for (int i = 0; i < dataOri.size(); ++i) {

        // 1. 하위 3비트 (C) 추출
        // 마스크: 00000111 (0x07)
        arBit233[2] = ptr[i] & 0x07;

        // 2. 중간 3비트 (B) 추출
        // 오른쪽으로 3번 밀고, 마스크 00000111 (0x07) 적용
        arBit233[1] = (ptr[i] >> 3) & 0x07;

        // 3. 상위 2비트 (A) 추출
        // 오른쪽으로 6번 밀고, 마스크 00000011 (0x03) 적용
        arBit233[0] = (ptr[i] >> 6) & 0x03;

        // 0부터 2까지 (0, 1, 2) 난수 발생
        // bounded(n) 함수는 0부터 n-1까지의 숫자를 반환합니다.
        int iRandomNumber = QRandomGenerator::global()->bounded(3);

        unsigned char binShift = 0;
        unsigned char arrResultEnc[3] = {0,};

        switch(iRandomNumber) {
        case 1:
        {
            // 1개인 경우 (001, 010, 100) 중 하나 선택
            QList<unsigned char> cases = { 0b001, 0b010, 0b100 };

            for(int j=0; j<3; ++j) {
                int iIdx = QRandomGenerator::global()->bounded(cases.size());
                binShift = cases[iIdx];

                arrResultEnc[j] = (binShift << 5) | (arBit233[j] << iRandomNumber);

                // 0번지 난수 (0 또는 1)
                int bit0 = QRandomGenerator::global()->bounded(2);
                // 4번지 난수 (0 또는 1)
                int bit4 = QRandomGenerator::global()->bounded(2);

                arrResultEnc[j] &= ~(1 << 0);          // 0번 비트 클리어
                arrResultEnc[j] |= (bit0 << 0);        // 0번 비트 난수 설정

                arrResultEnc[j] &= ~(1 << 4);          // 4번 비트 클리어
                arrResultEnc[j] |= (bit4 << 4);        // 4번 비트 난수 설정
            }
        }
            break;
        case 2:
        {
            // 2개인 경우 (011, 101, 110) 중 하나 선택
            QList<unsigned char> cases = { 0b011, 0b101, 0b110 };
            for(int j=0; j<3; ++j) {
                int iIdx = QRandomGenerator::global()->bounded(cases.size());
                binShift = cases[iIdx];

                arrResultEnc[j] = (binShift << 5) | (arBit233[j] << iRandomNumber);

                // 0번지 난수 (0 또는 1)
                int bit0 = QRandomGenerator::global()->bounded(2);
                // 1번지 난수 (0 또는 1)
                int bit1 = QRandomGenerator::global()->bounded(2);

                arrResultEnc[j] &= ~(1 << 0);          // 0번 비트 클리어
                arrResultEnc[j] |= (bit0 << 0);        // 0번 비트 난수 설정

                arrResultEnc[j] &= ~(1 << 1);          // 1번 비트 클리어
                arrResultEnc[j] |= (bit1 << 1);        // 1번 비트 난수 설정
            }
        }
            break;
        case 0:
        default:
            binShift = 0x000;

            for(int j=0; j<3; ++j) {
                arrResultEnc[j] = (binShift << 5) | (arBit233[j] << iRandomNumber);

                // 0번지 난수 (0 또는 1)
                int bit3 = QRandomGenerator::global()->bounded(2);
                // 1번지 난수 (0 또는 1)
                int bit4 = QRandomGenerator::global()->bounded(2);

                arrResultEnc[j] &= ~(1 << 3);          // 0번 비트 클리어
                arrResultEnc[j] |= (bit3 << 3);        // 0번 비트 난수 설정

                arrResultEnc[j] &= ~(1 << 4);          // 1번 비트 클리어
                arrResultEnc[j] |= (bit4 << 4);        // 1번 비트 난수 설정
            }
            break;
        }

        // 2. QByteArray에 데이터 3바이트 담기
        QByteArray packet;
        packet.append(arrResultEnc[0]);
        packet.append(arrResultEnc[1]);
        packet.append(arrResultEnc[2]);

        // 3. 3바이트 전체에 대한 CRC8 값 1개 계산
        unsigned char finalCrc = calculateCRC8(packet);

        packet.append(finalCrc);

        // 2. 4바이트 패킷 데이터 준비 (앞서 만든 4바이트 QByteArray)
        // packet[0]: 데이터1, packet[1]: 데이터2, packet[2]: 데이터3, packet[3]: CRC8
        unsigned char b1 = static_cast<unsigned char>(packet[0]);
        unsigned char b2 = static_cast<unsigned char>(packet[1]);
        unsigned char b3 = static_cast<unsigned char>(packet[2]);
        unsigned char crc = static_cast<unsigned char>(packet[3]);

        // 3. 32비트 정수로 병합 (Big-Endian 방식 예시)
        // 앞바이트부터 왼쪽으로 밀어서 합칩니다.
        unsigned int combined = (b1 << 24) | (b2 << 16) | (b3 << 8) | crc;

        m_liEncData.append(combined);
    }

    displayEncDataToTextEdit();
}

void Widget::displayEncDataToTextEdit()
{
    // 1. 기존 내용 초기화
    m_pTEEncData->clear();

    // 2. ReadOnly 설정 (UI 디자인 시 설정했다면 생략 가능)
    m_pTEEncData->setReadOnly(true);

    // 3. 폰트를 고정폭(Fixed Pitch)으로 설정하면 16진수 줄이 예쁘게 맞습니다.
    QFont monoFont("Courier New");
    monoFont.setStyleHint(QFont::Monospace);
    m_pTEEncData->setFont(monoFont);

    if (m_liEncData.isEmpty()) {
        m_pTEEncData->append("No Data Available.");
        return;
    }

    // 4. 리스트 순회하며 출력
    for (int i = 0; i < m_liEncData.size(); ++i) {
        unsigned int val = m_liEncData.at(i);

        // 32비트 -> 8자리 16진수 문자열
        QString hexStr = QString("%1").arg(val, 8, 16, QChar('0')).toUpper();

        // 보기 좋게 2자리씩 끊기 (예: "AA BB CC DD")
        QString formattedHex = QString("%1 %2 %3 %4")
                                   .arg(hexStr.mid(0, 2))
                                   .arg(hexStr.mid(2, 2))
                                   .arg(hexStr.mid(4, 2))
                                   .arg(hexStr.mid(6, 2));

        // 인덱스 번호와 함께 추가 (예: [001] AA BB CC DD)
        QString line = QString("[%1]  %2").arg(i + 1, 3, 10, QChar('0')).arg(formattedHex);

        m_pTEEncData->append(line);
    }
}

void Widget::onBtnDecClicked() {
    if (m_liEncData.isEmpty()) return;
    QByteArray decodedBytes;

    for (unsigned int combined : m_liEncData) {
        // 1. 4바이트 추출
        unsigned char b[3];
        b[0] = (combined >> 24) & 0xFF;
        b[1] = (combined >> 16) & 0xFF;
        b[2] = (combined >> 8)  & 0xFF;
        unsigned char crcReceived = combined & 0xFF;

        // 2. CRC 검증 (생략 시 복호화는 진행되나 무결성 보장 안됨)
        QByteArray checkData;
        checkData.append(reinterpret_cast<char*>(b), 3);
        if (calculateCRC8(checkData) != crcReceived) continue;

        // 3. iRand 판별 (중요: 반드시 b[0] 등 하나의 기준 바이트 상위 3비트만 사용)
        int iRand = 0;
        unsigned char headPattern = (b[0] >> 5) & 0x07;
        if (headPattern == 0b001 || headPattern == 0b010 || headPattern == 0b100) iRand = 1;
        else if (headPattern == 0b011 || headPattern == 0b101 || headPattern == 0b110) iRand = 2;
        else iRand = 0;

        // 4. 비트 추출 및 마스킹 (0x07로 마스킹하여 3비트 데이터만 남김)
        unsigned char bits[3];
        for (int j = 0; j < 3; ++j) {
            // b[j]에서 데이터가 있는 구간만 mask로 추출한 후, iRand만큼 원위치 시킴
            unsigned char mask = 0x07 << iRand;
            bits[j] = (b[j] & mask) >> iRand;
            bits[j] &= 0x07; // 다시 한번 3비트 범위(0~7)로 강제 제한 (안전장치)
        }

        // 5. 원본 바이트 재구성 (암호화 시 arBit233[0]=상위2, [1]=중간3, [2]=하위3)
        // bits[0]은 2비트(0~3)여야 하므로 0x03으로 마스킹
        unsigned char originalByte = ((bits[0] & 0x03) << 6) |
                                     ((bits[1] & 0x07) << 3) |
                                     (bits[2] & 0x07);

        decodedBytes.append(originalByte);
        qDebug() << "Rand:" << iRand << "bits:" << bits[0] << bits[1] << bits[2] << "Byte:" << (int)originalByte;
    }

    // 6. 한글(UTF-8) 대응을 위해 fromUtf8 사용
    m_pLEMsgDec->setText(QString::fromUtf8(decodedBytes));
}

// CRC-8 계산 함수 (0x07 다항식 기준)
unsigned char calculateCRC8(const QByteArray &data) {
    unsigned char crc = 0x00;
    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<unsigned char>(data.at(i));
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80) crc = (crc << 1) ^ 0x07;
            else crc <<= 1;
        }
    }
    return crc;
}
