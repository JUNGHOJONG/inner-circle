#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <FreeStack.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

#include <Wire.h>

/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;
int16_t last_ms_char; // milliseconds of last recieved character from Serial port.
int8_t buffer_pos; // next position to recieve character from Serial port.

//------------------------------------------------------------------------------
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 *
 * \note returned Error codes are typically passed up from MP3player.
 * Whicn in turns creates and initializes the SdCard objects.
 *
 * \see
 * \ref Error_Codes
 */
  char buffer[6]; // 0-35K+null

int button = 4;

void setup() {

  uint8_t result; //result code from some function as to be tested at later time.

  Serial.begin(115200);

  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();

  last_ms_char = millis(); // stroke the inter character timeout.
  buffer_pos = 0; // start the command string at zero length.

  pinMode(button, INPUT_PULLUP);

  Wire.begin(1);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

int count = 0;
int flag = 0;
int bitStage = 0;

int themeOfSong = 0;

void loop() {
  
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

String makeMP3FileName() {
  // 숫자 + 문자 + 숫자 -> 1 + - + 5
  return String(themeOfSong) + "-" + String(bitStage) + ".mp3";
}

void receiveEvent() {
  MP3player.available();
  boolean bitReset = false;

  if (Wire.available() && !MP3player.isPlaying()) {

    if (bitStage == 0) {
      themeOfSong = random(1, 2); // 추 후, 테마 개수가 늘어나면 변경 가능
    }

    int yutCount = Wire.read();
    Serial.print("yutCount: ");
    Serial.println(yutCount);
    bitStage += yutCount;

    if (bitStage >= 5) {
      bitReset = true;
      bitStage = 5;
    }

    // mp3 file name 정하기
    String MP3FileName = makeMP3FileName();
    char temp[8]; // 파일이름이 길어지면 버퍼길이를 늘리면 된
    MP3FileName.toCharArray(temp, 8); // 1-5.mp3 -> size: 8

    printYutCount(yutCount);
    Serial.println(String(themeOfSong) + "(음악 종류) - " + String(bitStage) + "(비트 개수)");
    MP3player.playMP3(temp); // sceneNumber(yutStack 이 6 이상이 될때까지 유) - musicNumber(yutStack)

    // bitStage 가 6 이상일 때, 0으로 초기화하는 작
    if (bitReset) {
      bitStage = 0;
    }
  }
}

void requestEvent() {
  Wire.write(MP3player.isPlaying());
}
