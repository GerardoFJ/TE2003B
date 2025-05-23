//Agregamos ls librerias de cristal liquido
#include <LiquidCrystal.h>
//Inicializamos los pines conectados de la pantalla de cristal liquido al esp32
LiquidCrystal lcd(22,23,5,18,19,21);
 
void setup() {
  // Inicializamos el display de 16 x 2
  lcd.begin(16, 2);
}
 
void loop() {
  //Nos posicionamos en la posiciocion 0,0 del display
  lcd.setCursor(0, 0);
  //Escribimos la palabra Programador
  lcd.print("Programador");
  
  //Nos posicionamos en la posiciocion 0,2 del display
  lcd.setCursor(0, 2);
  //Escribimos la palabra Novato
  lcd.print("Novato");

  //Movemos el texto de derecha a izquierda
  lcd.scrollDisplayLeft();
  
  delay(200);
}