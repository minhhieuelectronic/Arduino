#include "hx711.h"

long HX711_Buffer = 0;
long Weight_Maopi = 0, Weight_Shiwu = 0;

//****************************************************
//Khoi tao HX711
//****************************************************
void Init_Hx711()
{
  pinMode(HX711_SCK, OUTPUT);
  pinMode(HX711_DT, INPUT);
}


//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
  HX711_Buffer = HX711_Read();
  Weight_Maopi = HX711_Buffer / 100;
  Serial.println(Weight_Maopi);
}

//****************************************************
//称重
//****************************************************
unsigned int Get_Weight()
{
  HX711_Buffer = HX711_Read();
  HX711_Buffer = HX711_Buffer / 100;

  Weight_Shiwu = HX711_Buffer;
  Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//lay gia tri mau

  Weight_Shiwu = (unsigned int)((float)Weight_Shiwu / 4.11);
  //tinh trong luong thu te cua vat
  //Bởi vì các đường cong đặc tính cảm biến khác nhau không giống nhau, do đó, mỗi bộ cảm biến cần phải chỉnh lại số chia này là 4,30.
  //Khi trọng lượng của thử nghiệm được tìm thấy là quá lớn, tăng giá trị.
  //trọng lượng của phép thử quá nhỏ, hãy giảm giá trị.
  //Giá trị nói chung khoảng 7,16. Tùy thuộc vào cảm biến.
  //+0.05 được làm tròn thành phần trăm phần trăm

  return Weight_Shiwu;
}

//****************************************************
//读取HX711
//****************************************************
unsigned long HX711_Read(void)	//增益128
{
  unsigned long count;
  unsigned char i;
  bool Flag = 0;

  digitalWrite(HX711_DT, HIGH);
  delayMicroseconds(1);

  digitalWrite(HX711_SCK, LOW);
  delayMicroseconds(1);

  count = 0;
  while (digitalRead(HX711_DT));
  for (i = 0; i < 24; i++)
  {
    digitalWrite(HX711_SCK, HIGH);
    delayMicroseconds(1);
    count = count << 1;
    digitalWrite(HX711_SCK, LOW);
    delayMicroseconds(1);
    if (digitalRead(HX711_DT))
      count++;
  }
  digitalWrite(HX711_SCK, HIGH);
  count ^= 0x800000;
  delayMicroseconds(1);
  digitalWrite(HX711_SCK, LOW);
  delayMicroseconds(1);

  return (count);
}
