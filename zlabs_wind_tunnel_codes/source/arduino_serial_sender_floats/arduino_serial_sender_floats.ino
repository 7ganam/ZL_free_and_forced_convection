
void setup() {
  // Begin the Serial at 9600 Baud
  Serial2.begin(9600);
  delay(200);
//    Serial2.println("111.11");
//    Serial2.println("*");
//    Serial2.println("9");
//    Serial2.println("*"); 
}

void loop() 
{

    Serial2.println("111.11");
    Serial2.println("*");
    Serial2.println("4000");
    Serial2.println("*"); 
delay(1000);

}
