#include <Arduino.h>
#include <Streaming.h>
#include <Vector.h>

const long BAUD = 230400;
const int ROOT_COUNT = 4;
const int ROOT_LENGTH = 8;

int Elements[9][5];
int root[ROOT_COUNT + 1][ROOT_LENGTH];
int currentPosition = 0;
int movementCount = 0;

// root 결정 변수
boolean zeroPin_rootCheck = false;
boolean twoPin_rootCheck = false;
boolean fourPin_rootCheck = false;

int button = 9;
int flag = 0;
int count = 0; // 이번에 누른 카운
int excount = 1; // 방금 전까지 누적된 카운트

void setup()
{
  Serial.begin(BAUD);
  while (!Serial)
  {
  // wait for serial port to connect.
  }
  initYutBoard();
  printBoard();
  initRoot();
  currentPosition = 8; // 원점

  pinMode(button, INPUT_PULLUP);
  for (int i=0; i<=ROOT_LENGTH; i++) {
    pinMode(i, OUTPUT);
  }
}

void printBoard() {
  for (int i=0; i<9; i++) {
    for (int j=0; j<5; j++) {
          Serial.print(Elements[i][j]);
      }  
      Serial.println();
  }
}

void initYutBoard() {
  Elements[0][0] = 4; // 0번 위치, distance = 4
  Elements[1][0] = 3;
  Elements[2][0] = 2;
  Elements[3][0] = 5;
  Elements[4][0] = 3; // 4번 위치, distance = 3, 5 -> 0을 초과하는 대상이 2
  Elements[4][1] = 5;
  Elements[5][0] = 1;
  Elements[6][0] = 6;
  Elements[7][0] = 7;
  Elements[8][0] = 0; // 8번 위치, distance = 0, 4, 8개 -> 0을 초과하는 대상이 3개
  Elements[8][1] = 4;
  Elements[8][2] = 8;
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
  int yutCount = 0; // 도, 개, 걸, 윷, 모

  if (digitalRead(button) == 0) {
    if (flag == 0) {
      yutCount = random(1, 3);
      Serial.print("yutCount: ");
      Serial.println(yutCount);
      int rootNumber = getRootNumber(); // root 결정
      currentPosition = movePosition(yutCount, rootNumber);
      updateRootCheck();
      flag = 1;
      Serial.print("currentPosition: ");
      Serial.println(currentPosition);
    }
  } else {
    flag = 0;
  }

  // 현재 핀에 해당하는 LED에 불을킨다(0 - 8번 핀 사용한)
  turnOnLedOfCurrentPosition();
}

/*
void BUTTON() {
  if (digitalRead(button) == LOW) {
    if (flag == 0) {
      flag = 1;
      count++;
      Serial.println("PUSHED=============");
    }
  } else {
    flag = 0;
    Serial.println("NO PUSHED=============");
  }
}
*/

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
      digitalWrite(i, HIGH);
    } else {
      digitalWrite(i, LOW);   
    }
  }  
}
