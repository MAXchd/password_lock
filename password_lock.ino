#include <Keypad.h>            //引用键盘库
#include <Servo.h>             //引用舵机库
#include <SevSeg.h>            //引用数码管库

const byte row = 4;            //按键矩阵行数
const byte col = 4;            //按键矩阵列数
const int passwordnum = 4;     //密码位数

//定义按键矩阵对应字符
char keys[row][col] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowpin[row] = {14,15,16,17};   //设置行数引脚
byte colpin[col] = {18,19,20,21};   //设置列数引脚

//利用上述定义实例化一个键盘
Keypad keypad = Keypad(makeKeymap(keys),rowpin,colpin,row,col);

int num = 0;                    //记录当前输入位数
int wrongnum = 0;               //记录错误次数
bool unlock = false;            //密码锁状态
bool setpassword = false;       //密码设置模式状态

char defpassword[passwordnum] = {'1','2','3','4'};    //存储密码，出厂默认1234
char password[passwordnum];                           //存储输入密码
char temppassword[passwordnum];                       //临时存储新密码

//设置REBled灯引脚
int redpin = 13;
int greenpin = 12;
int bluepin = 11;

int buzzerpin = 6;              //设置蜂鸣器引脚

Servo servo;                    //实例化舵机对象
int servopin = 2;               //设置舵机引脚

SevSeg sevseg;                  //实例化数码管
byte dignum = 4;                              //数码管位数
byte digpins[] = {33,27,25,53};               //位引脚
byte segpins[] = {31,23,49,43,45,29,51,47};   //段引脚
byte hardwareConfig = COMMON_CATHODE;         //共阴极数码管



void check();
void led(char color);
void setled();
int resentpassword();
int resentsetpassword();

void setup() {
  Serial.begin(9600);

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);      //设置led灯引脚为输出模式

  servo.attach(servopin);
  servo.write(0);                //初始化舵机

  sevseg.begin(hardwareConfig, dignum, digpins, segpins); //初始化数码管
}

void loop() {
  setled();

  //读取按键输入
  char key = keypad.getKey();

  if (key != NO_KEY){
    switch (key){

      //密码输入功能
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        if (!unlock){
          if (num < 4){
            password[num] = key;
            num++;
            sevseg.setNumber(resentpassword());
          }
        }
        if (setpassword){
          if (num < 4){
            temppassword[num] = key;
            num++;
            sevseg.setNumber(resentsetpassword());
          }
        }
        break;

      //确定键
      case '#':
        if (num == 4){

          //验证密码正确性并解锁
          if (!unlock){
            check();
            num = 0;
            for (int i=0;i<4;i++){
              Serial.print(password[i]);
            }
          }

          //设置密码
          if (setpassword){
            for (int i=0;i<4;i++){
              defpassword[i] = temppassword[i];
            }
            num = 0;
            setpassword = false;
            Serial.println("设置完成");
          }
          sevseg.blank();
        }
        break;

      //密码设置功能
      case '*':
        if (unlock && !setpassword){
          setpassword = true;
          Serial.println("请设置密码");
        }
        break;

      //输入回退功能
      case 'A':
        if (num > 0){
          num--;
          if (!unlock){
            sevseg.setNumber(resentpassword());
          }
          if (setpassword){
            sevseg.setNumber(resentsetpassword());
          }
        }
        break;

      //开锁功能
      case 'D':
        if (unlock && !setpassword){
          servo.write(90);
          Serial.println("door open");
          delay(5000);
          servo.write(0);
          unlock = false;
        }
        break;
      default:;
    };
  }
  sevseg.refreshDisplay();
}

//检查输入的四位密码是否正确，将密码输出
void check(){
  for (int i = 0;i < 4;i++){
    if (password[i] != defpassword[i]){
      wrongnum++;               //记录错误次数
      Serial.println(wrongnum);
      break;
    }
    if (i == 3){
      unlock = true;
      wrongnum = 0;             //正确输入后将错误数清零
      Serial.println("unlock");
    }
  }
  if  (wrongnum>=5){
    tone(buzzerpin,494,5000*wrongnum);
  }
}

//RGBled灯颜色控制
void led(char color){
  switch(color){
    case 'R':
      analogWrite(redpin,255);
      analogWrite(greenpin,0);
      analogWrite(bluepin,0);
      break;
    case 'G':
      analogWrite(redpin,0);
      analogWrite(greenpin,255);
      analogWrite(bluepin,0);
      break;
    case 'B':
      analogWrite(redpin,0);
      analogWrite(greenpin,0);
      analogWrite(bluepin,255);
      break;
    case 'Y':
      analogWrite(redpin,255);
      analogWrite(greenpin,255);
      analogWrite(bluepin,0);
      break;
    default:;
  }
}

//REBled灯在不同状态下的颜色显示
void setled(){
  if (wrongnum>=5){
    led('R');
  }
  else if (setpassword){
    led('B');
  }
  else if (unlock){
    led('G');
  }
  else {
    led('Y');
  }
}

//将输入数字字符合并为整数便于数码管输出
int resentpassword(){
  int resentpassword = 0;
  for (int i=0;i<num;i++){
    resentpassword = resentpassword*10 + password[i] - 48;
  }
  return resentpassword;
}

int resentsetpassword(){
  int resentsetpassword = 0;
  for (int i=0;i<num;i++){
    resentsetpassword = resentsetpassword*10 + temppassword[i] - 48;
  }
  return resentsetpassword;
}