 float a; // variable décimal
volatile int comptageImpulsion=0; // variable de type int dont l'état est enregistré dans la RAM afin d'être changé
void setup()   
{
Serial.begin(115200); // initialise connexion série à 115200 bauds
attachInterrupt(0, gestionINT0, RISING); // interruption sur la pin 2 (INT0), 
                                        //appelle de la fonction gestionInto
                                       // interruption a chaque front montant (RISING)
}
void loop ()
{}
void gestionINT0() 
{
const float n = 0.116875406; // const décimal de hauteur d'eau
comptageImpulsion=comptageImpulsion+1; // incrémentation de (+1) à chaque front montant
a = comptageImpulsion*n;

Serial.print("hauteur d'eau = ");
Serial.println (String(a)+" mm"); 
} 
