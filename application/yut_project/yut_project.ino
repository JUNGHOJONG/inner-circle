#include <Arduino.h>
#include <Streaming.h>
#include <Vector.h>
#include <Wire.h>

const long BAUD = 115200; // before -> 230400
const int ROOT_COUNT = 4;
const int ROOT_LENGTH = 8;

int root[ROOT_COUNT + 1][ROOT_LENGTH];
int currentPosition = 0;
int movementCount = 0;

// root 결정 변수
boolean zeroPin_rootCheck = false;
boolean twoPin_rootCheck = false;
boolean fourPin_rootCheck = false;

int button = 14;
int flag = 0;
int count = 0; // 이번에 누른 카운
int excount = 1; // 방금 전까지 누적된 카운트

int isPlaying = 0;

void setup()
{
  Serial.begin(BAUD);
  Wire.begin();
  while (!Serial)
  {
  // wait for serial port to connect.
  }
  initRoot();
  currentPosition = 8; // 원점

  pinMode(button, INPUT_PULLUP);
  for (int i=0; i<=ROOT_LENGTH; i++) {
    pinMode(i + 2, OUTPUT); // 0 - 8 ==> 2 - 10 번 핀으로 변경
  }
}

void initRoot() {
  for (int i=1; i<=ROOT_COUNT; i++) {
    for (int j=0; j<ROOT_LENGTH; j++) {
      root[i][j] = -1;
    }
  }
  root[1][0] = 8;
  root[1][1] = 5;
  root[1][2] = 2;
  root[1][3] = 1;
  root[1][4] = 0;
  root[1][5] = 3;
  root[1][6] = 6;
  root[1][7] = 7;
  root[2][0] = 8;
  root[2][1] = 5;
  root[2][2] = 2;
  root[2][3] = 4;
  root[3][0] = 8;
  root[3][1] = 5;
  root[3][2] = 2;
  root[3][3] = 1;
  root[3][4] = 0;
  root[3][5] = 4;
  root[4][0] = 8;
  root[4][1] = 5;
  root[4][2] = 2;
  root[4][3] = 4;
  root[4][4] = 6;
  root[4][5] = 7;
}

/**
  - 말이 원점으로 다시 오면 끝나는 것으로 간주한다.
  - 버튼 누를 때마다 말이 정확히 움직이는지 확
*/
void loop()
{
  // 마스터가 슬레이브에게 데이터 달라고 요청
  Wire.requestFrom(1, 1);

  // 슬레이브에서 전달받은 데이터
  isPlaying = Wire.read();

  // 마스터가 슬레이브에게 데이터 전달(시작)
  Wire.beginTransmission(1);

  int yutCount = 0; // 도, 개, 걸, 윷, 모

  if (digitalRead(button) == 0 && isPlaying == 0) { // 음악이 재생 되는 동안에는 작동되면 안된다
    if (flag == 0) {
      yutCount = throwYut();
      printYutCount(yutCount);
      int rootNumber = getRootNumber(); // root 결정
      currentPosition = movePosition(yutCount, rootNumber);
      updateRootCheck();
      flag = 1;
      Serial.print("현재위치: ");
      Serial.println(currentPosition);

      // 윷 카운트 버퍼에 넣어 전달(마스터 -> 슬레이브)
      Wire.write(yutCount);

      // 마스터가 슬레이브에게 데이터 전달(끝)
      Wire.endTransmission();
    }
  } else {
    flag = 0;
  }

  // 현재 핀에 해당하는 LED에 불을킨다(0 - 8번 핀 사용한)
  turnOnLedOfCurrentPosition();
}

int throwYut() {
  int yutCount = random(1, 101);
  if (yutCount >= 1 && yutCount <= 15) {
    return 1; // 도(15%)
  } else if (yutCount >= 16 && yutCount <= 50) {
    return 2; // 개(35%)
  } else if (yutCount >= 51 && yutCount <= 85) {
    return 3; // 걸(35%)
  } else if (yutCount >= 86 && yutCount <= 97) {
    return 4; // 윷(12%)
  } else {
    return 5; // 모(3%)
  }
}

void printYutCount(int yutCount) {
  if (yutCount == 1) {
    Serial.println("[도]");
  } else if (yutCount == 2) {
    Serial.println("[개]");
  } else if (yutCount == 3) {
    Serial.println("[걸]");
  } else if (yutCount == 4) {
    Serial.println("[윷]");
  } else {
    Serial.println("[모]");
  } 
}

int getRootNumber() {
  if (!zeroPin_rootCheck && twoPin_rootCheck && fourPin_rootCheck) { // 2 root
    return 2;
  } else if (!zeroPin_rootCheck && twoPin_rootCheck && !fourPin_rootCheck) { // 4 root
    return 4;
  } else if (!zeroPin_rootCheck && !twoPin_rootCheck && !fourPin_rootCheck) { // 1 root
    return 1;
  } else { // 3 root
    return 3;  
  }
}

int movePosition(int yutCount, int rootNumber) { // 이동할 핀으로 이
  // 해당 root 에서 해당 핀이 몇 번째 index인지 반환
 int index = getIndexOfRoot(rootNumber);

 int currentRootLength = getCurrentRootLength(rootNumber);

  // 이동할 핀 최신화(index + yutCount)
  if (index + yutCount >= currentRootLength) {
    zeroPin_rootCheck = false;
    twoPin_rootCheck = false;
    fourPin_rootCheck = false;
    return 8; // 원점 반환, reset need
  }
  return root[rootNumber][index + yutCount];
}

int getIndexOfRoot(int rootNumber) {
  for (int j=0; j<ROOT_LENGTH; j++) {
    if (root[rootNumber][j] == currentPosition) {
      return j;
    }
  }
  return -1;
}

int getCurrentRootLength(int rootNumber) {
  int count = 0;
  for (int j=0; j<ROOT_LENGTH; j++) {
    if (root[rootNumber][j] >= 0) {
      count++;
    }
  }
  return count;
}

void updateRootCheck() {
  if (currentPosition == 0) {
    zeroPin_rootCheck = true;
  } else if (currentPosition == 2) {
    twoPin_rootCheck = true;
  } else if (currentPosition == 4) {
    fourPin_rootCheck = true;
  }
}

void turnOnLedOfCurrentPosition() {
  for (int i=0; i<=ROOT_LENGTH; i++) {
    if (currentPosition == i) {
      digitalWrite(i + 2, HIGH); // 0 - 8 ==> 2 - 10 번 핀으로 변경
    } else {
      digitalWrite(i + 2, LOW);   
    }
  }  
}
