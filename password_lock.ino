#include <Keypad.h>            //引用键盘库
#include <Servo.h>             //引用舵机库

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
byte rowpin[row] = {10,9,8,7};  //设置行数引脚
byte colpin[col] = {6,5,4,3};   //设置列数引脚

//利用上述定义实例化一个键盘
Keypad keypad = Keypad(makeKeymap(keys),rowpin,colpin,row,col);

Servo servo;                    //实例化舵机对象

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

int servopin = 2;               //设置舵机引脚

void check();
void led(char color);

void setup() {
  Serial.begin(9600);

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  servo.attach(servopin);
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
          }
        }
        if (setpassword){
          if (num < 4){
            temppassword[num] = key;
            num++;
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
