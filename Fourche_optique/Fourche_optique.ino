//===Détection de pluie avec une fourche optique

//
// Branchement du capteur sur pin5 digital
//==========================================================

// Pins utilisées
const int fourchePin = 5;     //Sortie de la fourche pin5
const int ledPin =  9;        // LED témoin sur pin

int EtatFourche =0 ;
int n = 1 ;

// Initialisation
void setup() {
  Serial.begin(9600);   //connection au port série 
  pinMode(ledPin, OUTPUT);     //LED en sortie
  pinMode(fourchePin, INPUT);  //en entrée
  Serial.println("Fourche optique - detection de pluie");
}

//=== Boucle de mesures
void loop() {
  //Vérifie si un objet obtcure la fourche optique
  EtatFourche = digitalRead(fourchePin);
  while (EtatFourche==HIGH)
   {
    Serial.println (EtatFourche + n);      

    
  } 
  if (EtatFourche==LOW)
  {
    Serial.println("Vide"); 
  }
  delay(2000);
}
